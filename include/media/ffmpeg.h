#ifndef BRICK_FFMPEG_H
#define BRICK_FFMPEG_H

#ifdef __cplusplus

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C"{
#endif

#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#ifdef __cplusplus
}
#endif

#include <memory>

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
    BGR24 = AV_PIX_FMT_BGR24,
    RGB24 = AV_PIX_FMT_RGB24,
    YUV420P = AV_PIX_FMT_YUV420P
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
    static int AllocPicture(AVFrame** new_frame, enum PixelFormat pix_fmt,
                            int width, int height);

} FFMpeg;

class VideoFrame;
void swap(VideoFrame& lhs, VideoFrame& rhs);

/**
 * @brief The VideoFrame class : wrap the AVFrame for automatic memory management
 * The instance of this class cannot be copied but can be moved.
 * example:
 *      VideoFrame frame;
 *      frame.init(PixelFormat::YUV420P, 1920, 1080);
 *      AVFrame* f = frame.getAVFrame();
 *      // some actions on the AVFrame
 */
class VideoFrame
{
    friend void swap(VideoFrame& lhs, VideoFrame& rhs);
public:
    /**
     * @brief VideoFrame : default constructor.
     * call init method if the instance is created by this way
     */
    VideoFrame() = default;

    /**
     * @brief VideoFrame : constructor
     * @param pix_fmt : pixel format of the frame
     * @param width : width of the picture
     * @param height : height of the picture
     */
    VideoFrame(PixelFormat pix_fmt, int width, int height);

    /**
     * @brief ~VideoFrame : deconstructor which will free the memory
     */
    ~VideoFrame();

    /* copy constructor and copy assignment are forbidden
       move constructor and move assignment are allowed */
    VideoFrame(const VideoFrame&) = delete;
    VideoFrame(VideoFrame&&) noexcept;
    VideoFrame& operator= (const VideoFrame&) = delete;
    VideoFrame &operator= (VideoFrame&&);

    /**
     * @brief init : allocate the memory according to the parameters
     * @param pix_fmt : pixel format of the frame
     * @param width : width of the picture
     * @param height : height of the picture
     * @return : ErrorCode
     */
    int init(PixelFormat pix_fmt, int width, int height);

    /**
     * @brief getAVFrame : get the AVFrame pointer
     * @return : pointer to AVFrame created
     */
    inline AVFrame* getAVFrame() {return m_frame;}

private:
    AVFrame* m_frame = nullptr;
    PixelFormat m_pixal_format;
    int m_width;
    int m_height;

    int allocMemory();
    int deallocMemory();
};



}   //namespace media
}   //namespace brick

#endif // FFMPEG_H
