#include "ffmpegpriv.h"
#include "media/ffmpegwriter.h"

#include "log/log.h"

namespace brick
{
namespace media
{

FFMpegWriter::FFMpegWriter():m_pixel_format(AV_PIX_FMT_RGB24)
{

}

FFMpegWriter::~FFMpegWriter()
{
    destroy();
}

FFMpegWriter::FFMpegWriter(FFMpegWriter &&rhs) noexcept:
    m_format(rhs.m_format),m_format_ctx(rhs.m_format_ctx),
    m_video_stream(rhs.m_video_stream),
    m_video_encode_ctx(rhs.m_video_encode_ctx), m_video_encoder(rhs.m_video_encoder),
    m_option(rhs.m_option),
    m_output_name(rhs.m_output_name), m_next_pts(rhs.m_next_pts),
    m_video_width(rhs.m_video_width), m_video_height(rhs.m_video_height),
    m_fps(rhs.m_fps), m_pixel_format(rhs.m_pixel_format),
    m_have_video(rhs.m_have_video), m_encode_video(rhs.m_encode_video)
{
    rhs.m_format = nullptr;
    rhs.m_format_ctx = nullptr;

    rhs.m_video_stream = nullptr;

    rhs.m_video_encode_ctx = nullptr;
    rhs.m_video_encoder = nullptr;

    rhs.m_option = nullptr;
}

FFMpegWriter &FFMpegWriter::operator=(FFMpegWriter &&rhs)
{
    swap(*this, rhs);
    return *this;
}

int FFMpegWriter::init(const std::string& url, int width, int height, int frameRate,
                       int pixelFormat)
{
    m_output_name = url;
    m_video_width= width;
    m_video_height = height;
    m_fps = frameRate;
    m_pixel_format = pixelFormat;

    int ret = 0;

    avformat_alloc_output_context2(&m_format_ctx, nullptr,
                                   nullptr, m_output_name.c_str());
    if(!m_format_ctx)
    {
        return AVFORMAT_ALLOC_OUTPUT_CONTEXT_ERROR;
    }

    m_format = m_format_ctx->oformat; 

    if(m_format->video_codec != AV_CODEC_ID_NONE)
    {
        m_format->video_codec = AVCodecID::AV_CODEC_ID_H264;
//        m_format->video_codec = AVCodecID::AV_CODEC_ID_FFV1;
        add_video_stream(m_format->video_codec);
        m_have_video = 1;
        m_encode_video = 1;
    }

    /* Now that all the parameters are set, we can open the audio and
         * video codecs and allocate the necessary encode buffers. */
    if(m_have_video)
    {
        open_video();
    }

    /* open the output file, if needed */
    if(!(m_format->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&m_format_ctx->pb, m_output_name.c_str(), AVIO_FLAG_WRITE);
        if(ret < 0 )
        {
            return AVIO_OPEN_ERROR;
        }
    }

    /* Write the stream header, if any. */
    ret = avformat_write_header(m_format_ctx, nullptr);
    if(ret < 0 )
    {
        return AVFORMAT_WRITE_HEADER_ERROR;
    }
    return ErrorCode::SUCCESS;
}

void FFMpegWriter::reset()
{
    destroy();
    init(m_output_name, m_video_width, m_video_height, m_fps, m_pixel_format);
}

void FFMpegWriter::destroy()
{
    /* Write the trailer, if any. The trailer must be written before you
     * close the CodecContexts open when you wrote the header; otherwise
     * av_write_trailer() may try to use memory that was freed on
     * av_codec_close(). */
    if(!m_format)
        return;

    av_write_trailer(m_format_ctx);

    /* Close each codec. */
    if (m_have_video)
    {
        close_stream();
    }

    if (!(m_format->flags & AVFMT_NOFILE))
        /* Close the output file. */
        avio_closep(&m_format_ctx->pb);

    /* free the stream */
    avformat_free_context(m_format_ctx);
}

int FFMpegWriter::write(AVFrame* frame)
{
    int ret = FFMpeg::WriteFrame(m_format_ctx, m_video_encode_ctx, m_video_stream, frame);
    if(ret < 0 )
    {
        return ret;
    }
    return ErrorCode::SUCCESS;
}

int FFMpegWriter::add_video_stream(int codec_id)
{
    m_video_encoder = avcodec_find_encoder(static_cast<AVCodecID>(codec_id));
    if(!m_video_encoder)
    {
        return AVCODEC_FIND_ENCODER_ERROR;
    }

    m_video_stream = avformat_new_stream(m_format_ctx, nullptr);
    if(!m_video_stream)
    {
        return AVFORMAT_NEW_STREAM_ERROR;
    }
    m_video_stream->id = static_cast<int>(m_format_ctx->nb_streams-1);

    AVCodecContext* c = nullptr;
    c = avcodec_alloc_context3(m_video_encoder);
    if(!c)
    {
        return AVCODEC_ALLOC_CONTEXT3_ERROR;
    }
    m_video_encode_ctx = c;

    c->codec_id = static_cast<AVCodecID>(codec_id);
    c->bit_rate = 4000000;
    c->width = m_video_width;
    c->height = m_video_height;

    m_video_stream->time_base = {1, m_fps};
    c->time_base = m_video_stream->time_base;

    c->gop_size = 12;
    c->pix_fmt = static_cast<AVPixelFormat>(m_pixel_format);

    if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO)
    {
        /* just for testing, we also add B-frames */
        c->max_b_frames = 2;
    }
    if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO)
    {
        /* Needed to avoid using macroblocks in which some coeffs overflow.
         * This does not happen with normal video, it just happens here as
         * the motion of the chroma plane does not match the luma plane. */
        c->mb_decision = 2;
    }

    if(m_format_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    return ErrorCode::SUCCESS;
}

int FFMpegWriter::open_video()
{
    AVCodecContext* c = m_video_encode_ctx;
    AVDictionary* opt = nullptr;

    av_dict_copy(&opt, m_option, 0);

    int ret = avcodec_open2(c, m_video_encoder, &opt);
    av_dict_free(&opt);
    if(ret < 0)
    {
        return AVCODEC_OPEN_ERROR;
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(m_video_stream->codecpar, c);
    if (ret < 0)
    {
        return AVCODEC_COPY_PARAMETERS_FROM_CONTEXT_ERROR;
    }
    return ErrorCode::SUCCESS;
}


int FFMpegWriter::close_stream()
{
    avcodec_free_context(&m_video_encode_ctx);
    return 0;
}


WriteManager::WriteManager(const FFMpegWriter& writer)
{
    m_fill_count = 0;
    m_is_writting = false;
    m_flag_stop = false;
    m_buffer.reserve(static_cast<size_t>(m_buffer_size));
    allocBuffer(writer.getPixelFormat(), writer.getWidth(), writer.getHeight());
}

WriteManager::~WriteManager()
{
    stop();

    m_mtx.lock();
    if(!m_buffer.empty())
    {
        deallocBuffer();
    }
    m_mtx.unlock();
}

int WriteManager::init(const FFMpegWriter& writer)
{
    int ret = 0;
    if(!m_buffer.empty())
    {
        ret = deallocBuffer();
        if(ret < 0 )
        {
            return ret;
        }
    }

//    m_pixel_format = pix_fmt;
//    m_width = width;
//    m_height = height;
    m_fill_count = 0;
    m_is_writting = false;
    m_flag_stop = false;

    m_buffer.reserve(static_cast<size_t>(m_buffer_size));
    return allocBuffer(writer.getPixelFormat(), writer.getWidth(), writer.getHeight());
}

void WriteManager::fillPicture(const unsigned char * const *data,
                              const int *linesize, int width, int height, int pixelFormat)
{
    if(!m_is_writting)
    {
        ++m_fill_count;
    }

    Image& frame = m_buffer[m_fill_count & 1];
    frame.fillBuffer(data, linesize, width, height, pixelFormat, true);
    m_cv_write.notify_one();
}

void WriteManager::keepWritingFrames(FFMpegWriter& writer)
{
    std::unique_lock<std::mutex> locker(m_mtx);
    while(true)
    {
        if(m_flag_stop)
        {
            return;
        }

        while(m_fill_count<2)
        {
            m_cv_write.wait(locker);
        }

        m_is_writting = true;
        Image& frame = m_buffer[(m_fill_count-1) & 1];
        AVFrame* f = frame.getAVFrame();
        if((m_fill_count - m_pts_index) >= 2)
        {
            f->pts = m_pts_index++;
            writer.write(f);
        }
        m_is_writting = false;
        m_cv_write.wait(locker);
    }
}


void WriteManager::stop()
{
    m_flag_stop = true;
    m_cv_write.notify_one();
}

void WriteManager::start(FFMpegWriter& writer)
{
    m_thread = std::thread([&writer, obj = this]()
    {
        obj->keepWritingFrames(writer);
    });
    m_thread.detach();
}

int WriteManager::allocBuffer(int pix_fmt, int width, int height)
{
    for(int i = 0; i < m_buffer_size; ++i)
    {
        m_buffer.push_back({width, height, pix_fmt});
    }

    return ErrorCode::SUCCESS;
}

int WriteManager::deallocBuffer()
{
    m_buffer.clear();

    return ErrorCode::SUCCESS;
}

void swap(FFMpegWriter &lhs, FFMpegWriter &rhs)
{
    using std::swap;
    swap(lhs.m_format, rhs.m_format);
    swap(lhs.m_format_ctx, rhs.m_format_ctx);
    swap(lhs.m_video_stream, rhs.m_video_stream);
    swap(lhs.m_video_encode_ctx, rhs.m_video_encode_ctx);
    swap(lhs.m_video_encoder, rhs.m_video_encoder);
    swap(lhs.m_option, rhs.m_option);
    swap(lhs.m_output_name, rhs.m_output_name);
    swap(lhs.m_next_pts, rhs.m_next_pts);
    swap(lhs.m_video_width, rhs.m_video_width);
    swap(lhs.m_video_height, rhs.m_video_height);
    swap(lhs.m_fps, rhs.m_fps);
    swap(lhs.m_pixel_format, rhs.m_pixel_format);
    swap(lhs.m_have_video, rhs.m_have_video);
    swap(lhs.m_encode_video, rhs.m_encode_video);
}

}   //namespace media
}   //namespace brick
