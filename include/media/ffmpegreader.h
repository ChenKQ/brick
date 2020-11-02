#ifndef BRICK_FFMPEGREADER_H
#define BRICK_FFMPEGREADER_H

#include "ffmpeg.h"

#include <string>
#include <functional>

namespace brick
{
namespace media
{
/**
 * @brief The FFMpegReader class : use ffmpeg as the backend to read image frames from
 * video files (mp4, flv, avi, etc.) or network video streams (rtmp, rtsp, http, etc.)
 */
class FFMpegReader final
{
public:
    /**
     * @brief FrameProcess : the function signature to be called after fetching a frame
     * @param data : a pointer referring to the source image datas.
     * sometimes data refers to different image planes (access by data[0], data[1], ...)
     * while sometimes data refers to a packed image (access by data[0])
     * according to the pixel format
     * @param linesize : the steps between image rows for alignment
     * @param width : the width of an image, it should be smaller than the step
     * @param height : the height of an image
     * @param pixelFormat : the pixel Format the image, it can be converted to
     * enum PixelFormat (in ffmpeg.h) and enum AVPixelFormat (in ffmpeg enum),
     * whose int values are defined equal
     */
    using FrameProcess = void (const unsigned char* const* data, const int* linesize,
                              int width, int height, int pixelFormat);

public:

    /**
     * @brief FFMpegReader : constructor
     */
    FFMpegReader() = default;

    /**
     * @brief ~FFMpegReader : deconstructor, destroy ffmpeg resources
     * by calling destroy()
     */
    ~FFMpegReader();

    // copy and move are forbidden
    FFMpegReader(const FFMpegReader&) = delete;
    FFMpegReader(FFMpegReader &&) = delete;
    FFMpegReader& operator=(const FFMpegReader&) = delete;
    FFMpegReader& operator=(FFMpegReader&&) = delete;

    /**
     * @brief init : create and initial ffmpe resources accroding to the url
     * @param url : the video file or url to open
     * @return : the error code (enum ErrorCode)
     */
    int init(const std::string& url);

    /**
     * @brief reset : reset the ffmpeg configurations
     * @return the error code (enum ErrorCode)
     */
    int reset();

    /**
     * @brief installActionsOnFetchedVideoFrame : install the function called after
     * fetching a frame
     * @param func : the function to be called
     */
    void installActionsOnFetchedVideoFrame(const std::function<FrameProcess>& func);

    /**
     * @brief fetching_frame : keep fetching frames until EOF
     */
    void fetching_frames();

    /**
     * @brief flush_rest : flush the decoders
     */
    void flush_rest();

    /**
     * @brief fetch : fetch one frame
     * @return  : error code (enum ErrorCode)
     */
    int fetch();

    /**
     * @brief destroy : release the ffmpeg resources. it is called by the deconstructor
     */
    void destroy();

    inline int getVideoWidth() const {return m_video_width; }
    inline int getVideoHeight() const {return m_video_height; }
    inline PixelFormat getPixelFormat() const
    {
        return static_cast<PixelFormat>(m_pixel_format);
    }

private:
    std::string m_url;

    AVFormatContext *m_fmt_ctx = nullptr;

    AVStream *m_video_stream = nullptr;
    AVStream *m_audio_stream = nullptr;
    int m_video_stream_idx = -1;
    int m_audio_stream_idx = -1;

    AVCodecContext *m_video_codec_ctx = nullptr;
    AVCodecContext *m_audio_codec_ctx = nullptr;

    AVPacket m_pkt;
    AVFrame *m_frame = nullptr;

    int m_video_width;
    int m_video_height;
    AVPixelFormat m_pixel_format;

    int m_video_frame_count = 0;
    int m_audio_frame_count = 0;

    std::function<FrameProcess> m_action_on_video_frame = nullptr;

private:
    /**
     * @brief open_codec_context: Open the ffmpeg codec context
     * @param [out] stream_idx : the index of the finded stream
     * @param [out] dec_ctx : the decoder context created accoring to
     * the format contex (AVFormatContext) and media type (AVMediaType)
     * @param [in] fmt_ctx : the format context
     * @param [in] type : the media type to open (video, audio, data, subtitle, etc.)
     * @return the error code (enum ErrorCode)
     */
    int open_codec_context(int *stream_idx,
                           AVCodecContext **dec_ctx,
                           AVFormatContext *fmt_ctx,
                           enum AVMediaType type);

    /**
     * @brief decode_packet: get AVPackets and then get the
     * @param dec : pointer to AVCodecContext
     * @param pkt : pointer to AVPacket
     * @return : the error code (enum ErrorCode)
     */
    int decode_packet(AVCodecContext* dec, const AVPacket* pkt);

    /**
     * @brief on_video_frame : actions on the got video frame
     * @param frame : pointer to video AVFrame
     * @return : the error code (enum ErrorCode)
     */
    int on_video_frame(AVFrame* frame);

    /**
     * @brief on_audio_frame : actions on the got audio frame
     * @param frame : point to the audio AVFrame
     * @return : the error code (enum ErrorCode)
     */
    int on_audio_frame(AVFrame* frame);
};

} //namespace media
} //namespace brick

#endif // BRICK_FFMPEGREADER_H
