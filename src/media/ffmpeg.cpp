#include "ffmpegpriv.h"
#include "media/ffmpeg.h"

namespace brick
{
namespace media
{
int FFMpeg::WriteFrame(AVFormatContext *fmt_ctx, AVCodecContext *codec_ctx, AVStream *stream, AVFrame *frame)
{
    int ret = 0;

    // send the frame to the encoder
    ret = avcodec_send_frame(codec_ctx, frame);
    if(ret < 0)
    {
        return AVCODEC_SEND_FRAME_ERROR;
    }

    while(ret >= 0)
    {
        AVPacket pkt = {nullptr};

        ret = avcodec_receive_packet(codec_ctx, &pkt);
        if(ret==AVERROR(EAGAIN) || ret==AVERROR_EOF)
        {
            break;
        }
        else if(ret < 0)
        {
            return AVCODEC_RECEIVE_PACKET_ERROR;
        }

        /* rescale output packet timestamp values from codec to stream timebase */
        av_packet_rescale_ts(&pkt, codec_ctx->time_base, stream->time_base);
        pkt.stream_index = stream->index;

        /* Write the compressed frame to the media file. */
        ret = av_interleaved_write_frame(fmt_ctx, &pkt);
        av_packet_unref(&pkt);
        if(ret < 0)
        {
            return AVFORMAT_INTERLEAVED_WRITE_FRAME_ERROR;
        }
    }

    return ret == AVERROR_EOF ? 1 : 0;
}

int FFMpeg::AllocPicture(AVFrame **new_frame, int pix_fmt, int width, int height, int align)
{
    AVFrame* picture = nullptr;

    picture = av_frame_alloc();
    if(!picture)
    {
        return AVFRAME_ALLOC_ERROR;
    }

    picture->format = pix_fmt;
    picture->width = width;
    picture->height = height;

    int ret = av_frame_get_buffer(picture, align);
    if(ret < 0 )
    {
        return AVFRAME_GET_BUFFER_ERROR;
    }

    *new_frame = picture;
    return 0;
}

}   // namespace media
}   //namespace brick

