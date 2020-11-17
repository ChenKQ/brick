#ifndef BRICK_FFMPEG_H
#define BRICK_FFMPEG_H

#include <memory>

struct AVFormatContext;
struct AVOutputFormat;
struct AVStream;
struct AVPacket;

struct AVCodecContext;
struct AVCodec;
struct AVFrame;

struct SwsContext;
struct AVDictionary;

namespace brick
{
namespace media
{

enum ErrorCode : int
{
    SUCCESS = 0,

    NO_VIDEO = -99,
    NO_AUDIO,
    NO_VIDEO_AUDIO,
    VIDEO_HEIGHT_DISMATCH,
    VIDEO_WIDTH_DISMATCH,
    VIDEO_FORMAT_DISMATCH,
    VIDEO_EOF,
    AUDIO_EOF,


    AVFORMAT_OPEN_INPUT_ERROR = -199,
    AVFORMAT_FIND_STREAM_INFOR_ERROR,
    AVFORMAT_FIND_BEST_STREAM_ERROR,
    AVFORMAT_ALLOC_OUTPUT_CONTEXT_ERROR,
    AVFORMAT_NEW_STREAM_ERROR,
    AVFORMAT_WRITE_HEADER_ERROR,
    AVFORMAT_INTERLEAVED_WRITE_FRAME_ERROR,

    AVCODEC_FIND_DECODER_ERROR = -299,
    AVCODEC_FIND_ENCODER_ERROR,
    AVCODEC_ALLOC_CONTEXT3_ERROR,
    AVCODEC_COPY_PARAMETERS_TO_CONTEXT_ERROR,
    AVCODEC_COPY_PARAMETERS_FROM_CONTEXT_ERROR,
    AVCODEC_OPEN_ERROR,
    AVCODEC_SEND_PACKET_ERROR,
    AVCODEC_RECEIVE_FRAME_ERROR,
    AVCODEC_SEND_FRAME_ERROR,
    AVCODEC_RECEIVE_PACKET_ERROR,

    AVUTIL_IMAGE_ALLOC_ERROR = -399,

    AVFRAME_ALLOC_ERROR = -499,
    AVFRAME_READ_FRAME_ERROR,
    AVFRAME_GET_BUFFER_ERROR,

    AVSWS_GET_CONTEXT_ERROR = -599,

    AVIO_OPEN_ERROR = -699

};


enum PixelFormat
{
    UNKNOWN = -1,

    // grey
    GRAY8 = 8,

    // rgb 24 bits
    RGB24 = 2,
    BGR24 = 3,

    // rgb 32 bits
    ARGB = 25,
    RGBA = 26,
    ABGR = 27,
    BGRA = 28,

    // yuv 444 24 bits, 32 bits
    YUV444P = 5, // I444, Y444
    NV24 = 191, // YYYY UVUVUVUV
    NV42 = 192, // YYYY VUVUVUVU

    YUVA444P = 81,

    // yuv422 16bits, 24bits
    YUYV422 = 1,
    YVYU422 = 110,
    YUYV422P = 4,
    UYVY422 = 15,
    NV16 = 103, // YYYY UVUV

    YUVA422P = 80,

    // yuv420 12 bits, 20 bits
    YUV420P = 0,
    NV12 = 23,  // YYYY UV
    NV21 = 24, // YYYY VU

    YUVA420P = 33,
};

/**
 * @brief FFMpeg : some useful function for FFMpeg video process
 */
typedef struct FFMpeg
{
    /**
     * @brief WriteFrame: transfer the AVFrame data to AVPacket and then save to disk
     * @param [in] fmt_ctx : format context for the output file
     * @param [in] codec_ctx : codec context for the output
     * @param [in] stream : video stream of the output
     * @param [in] frame : frame to be written
     * @return ErrorCode
     */
    static int WriteFrame(AVFormatContext* fmt_ctx, AVCodecContext* codec_ctx,
                           AVStream* stream, AVFrame* frame);

    /**
     * @brief AllocPicture : allocate the memory for the frame
     * @param [out] new_frame : pointer to the new frame pointer
     * @param [in] pix_fmt : pixel format of the allocated frame
     * @param [in] width : width of the picture
     * @param [in] height : height of the picture
     * @return
     */
    static int AllocPicture(AVFrame** new_frame, int pix_fmt,
                            int width, int height);

} FFMpeg;

}   //namespace media
}   //namespace brick

#endif // FFMPEG_H
