#include "ffmpegpriv.h"
#include "media/ffmpegreader.h"
#include "log/log.h"

namespace brick
{
namespace media
{

namespace
{
    AVPacket m_pkt;
}


FFMpegReader::~FFMpegReader()
{
    destroy();
}

int FFMpegReader::init(const std::string &url)
{   
    m_url = url;

    /* open input file, and allocate format context */
    if(avformat_open_input(&m_fmt_ctx, url.c_str(), nullptr, nullptr) < 0)
    {
        return ErrorCode::AVFORMAT_OPEN_INPUT_ERROR;
    }

    /* retrive stream information */
    if(avformat_find_stream_info(m_fmt_ctx, nullptr) < 0)
    {
        return ErrorCode::AVFORMAT_FIND_STREAM_INFOR_ERROR;
    }

    if(open_codec_context(&m_video_stream_idx, &m_video_codec_ctx, m_fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0)
    {
        m_video_stream = m_fmt_ctx->streams[m_video_stream_idx];

        /* allocate image where the decoded image will be put */
        m_video_width = m_video_codec_ctx->width;
        m_video_height = m_video_codec_ctx->height;
        m_pixel_format = m_video_codec_ctx->pix_fmt;
    }

    if(open_codec_context(&m_audio_stream_idx, &m_audio_codec_ctx, m_fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0)
    {
        m_audio_stream = m_fmt_ctx->streams[m_audio_stream_idx];
    }

    /* dump input information to stderr */
//    av_dump_format(m_fmt_ctx, 0, url, 0);

    if(!m_video_stream && !m_audio_stream)
    {
        destroy();
        return ErrorCode::NO_VIDEO_AUDIO;
    }

    m_frame = av_frame_alloc();
    if(!m_frame)
    {
        return ErrorCode::AVFRAME_ALLOC_ERROR;
    }
//    AVPacket* tmp = new AVPacket();
//    m_pkt.reset(tmp);

    /* initialize packet, set data to NULL, let the demuxer fill it */
    av_init_packet(&m_pkt);
    m_pkt.data = nullptr;
    m_pkt.size = 0;

    return 0;
}

int FFMpegReader::reset()
{
    flush_rest();
    destroy();
    return init(m_url);
}


void FFMpegReader::installActionsOnFetchedVideoFrame(const std::function<FFMpegReader::FrameProcess>& func)
{
    this->m_action_on_video_frame = func;
}


int FFMpegReader::open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, int type)
{
    int ret = av_find_best_stream(m_fmt_ctx, static_cast<AVMediaType>(type), -1, -1, nullptr, 0);
    if(ret < 0)
    {
        return ErrorCode::AVFORMAT_FIND_BEST_STREAM_ERROR;
    }

    int stream_index = ret;
    AVStream* st = fmt_ctx->streams[stream_index];

    /* find decoder for the stream */
    AVCodec* dec = avcodec_find_decoder(st->codecpar->codec_id);
    if(!dec)
    {
        return ErrorCode::AVCODEC_FIND_DECODER_ERROR;
    }

    /* Allocate a codec context for the decoder */
    *dec_ctx = avcodec_alloc_context3(dec);
    if(!*dec_ctx)
    {
        return ErrorCode::AVCODEC_ALLOC_CONTEXT3_ERROR;
    }

    /* Copy codec parameters from input stream to output codec context */
    ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar);
    if(ret < 0)
    {
        return AVCODEC_COPY_PARAMETERS_TO_CONTEXT_ERROR;
    }

    /* Init the decoders */
    AVDictionary *opts = nullptr;
    ret = avcodec_open2(*dec_ctx, dec, &opts);
    if ( ret < 0)
    {
        return AVCODEC_OPEN_ERROR;
    }
    *stream_idx = stream_index;

    return 0;
}

int FFMpegReader::decode_packet(AVCodecContext *dec, const AVPacket *pkt)
{
    int ret = 0;

    // submit the packet to the decoder
    ret = avcodec_send_packet(dec, pkt);
    if(ret < 0)
    {
        return ErrorCode::AVCODEC_SEND_PACKET_ERROR;
    }

    // get all the available frames from the decoder
    while(ret>=0)
    {
        ret = avcodec_receive_frame(dec, m_frame);
        if(ret<0)
        {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if(ret == AVERROR_EOF || ret==AVERROR(EAGAIN))
            {
                return 0;
            }
            return ErrorCode::AVCODEC_RECEIVE_FRAME_ERROR;
        }

        // write the frame data to output file
        if(dec->codec->type == AVMEDIA_TYPE_VIDEO)
        {
            // process on the video frame
            ret = on_video_frame(m_frame);
        }
        else if(dec->codec->type == AVMEDIA_TYPE_AUDIO)
        {
            // process on the audio frame
            ret = on_audio_frame(m_frame);
        }

        av_frame_unref(m_frame);
        if(ret < 0)
            return ret;
    }

    return 0;
}

int FFMpegReader::on_video_frame(AVFrame *frame)
{
    if(frame->width != m_video_width)
    {
        return ErrorCode::VIDEO_WIDTH_DISMATCH;
    }

    if(frame->height != m_video_height)
    {
        return ErrorCode::VIDEO_HEIGHT_DISMATCH;
    }

    if(frame->format != m_pixel_format)
    {
        return ErrorCode::VIDEO_FORMAT_DISMATCH;
    }

//    printf("video_frame n:%d coded_n:%d\n",
//               m_video_frame_count++, frame->coded_picture_number);

     if(m_action_on_video_frame != nullptr)
     {
         m_action_on_video_frame(frame->data, frame->linesize ,
                                 m_video_width, m_video_height, m_pixel_format);
     }
     return 0;
}

int FFMpegReader::on_audio_frame(AVFrame *frame)
{
    size_t unpadded_linesize = frame->nb_samples * av_get_bytes_per_sample(static_cast<AVSampleFormat>(frame->format));
//    printf("count: %d\n", m_audio_frame_count++);
    return 0;
}

void FFMpegReader::destroy()
{
    if(m_video_codec_ctx != nullptr)
    {
        avcodec_free_context(&m_video_codec_ctx);
        m_video_codec_ctx = nullptr;
    }

    if(m_audio_codec_ctx != nullptr)
    {
        avcodec_free_context(&m_audio_codec_ctx);
        m_audio_codec_ctx = nullptr;
    }

    if(m_fmt_ctx != nullptr)
    {
         avformat_close_input(&m_fmt_ctx);
         m_fmt_ctx = nullptr;
    }

    if(m_frame != nullptr)
    {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

}

void FFMpegReader::fetching_frames()
{
    int ret = 0;
    /* read frames */
    while(av_read_frame(m_fmt_ctx, &m_pkt) >= 0)
    {
        if(m_pkt.stream_index == m_video_stream_idx)
        {
            ret = decode_packet(m_video_codec_ctx, &m_pkt);
        }
        else if(m_pkt.stream_index == m_audio_stream_idx)
        {
            ret = decode_packet(m_audio_codec_ctx, &m_pkt);
        }
        av_packet_unref(&m_pkt);

        if(ret < 0)
        {
            break;
        }
    }

    flush_rest();
}

void FFMpegReader::flush_rest()
{
    /* flush the decoders */
    if(m_video_codec_ctx)
    {
        decode_packet(m_video_codec_ctx, &m_pkt);
    }
    if(m_audio_codec_ctx)
    {
        decode_packet(m_audio_codec_ctx, &m_pkt);
    }
}

int FFMpegReader::fetch()
{
    int ret = 0;
    if(av_read_frame(m_fmt_ctx, &m_pkt) >= 0)
    {
        if(m_pkt.stream_index == m_video_stream_idx)
        {
            ret = decode_packet(m_video_codec_ctx, &m_pkt);
        }
        else if(m_pkt.stream_index == m_audio_stream_idx)
        {
            ret = decode_packet(m_audio_codec_ctx, &m_pkt);
        }
        av_packet_unref(&m_pkt);

        if(ret < 0)
        {
//                can_stop = true;
            return ret;
        }
    }
    else
    {
//            can_stop = true;
        LOGINFO << "Fail to read frame";
        return ErrorCode::AVFRAME_READ_FRAME_ERROR;
    }
    return ErrorCode::SUCCESS;
}

}   //namespace media
}   //namespace brick
