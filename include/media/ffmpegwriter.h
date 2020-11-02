#ifndef BRICK_FFMPEGWRITER_H
#define BRICK_FFMPEGWRITER_H

#include "ffmpeg.h"

#include <string>
#include <mutex>
#include <vector>
#include <atomic>
#include <thread>

namespace brick
{
namespace media
{

/**
 * @brief The FFMpegWriter class : it is used for video writting
 */
class FFMpegWriter final
{
public:
    /* Constructor and Deconstructor */
    FFMpegWriter() = default;
    ~FFMpegWriter();

    /* copy and move are forbidden */
    FFMpegWriter(const FFMpegWriter& ) = delete;
    FFMpegWriter(FFMpegWriter&& ) = delete;
    FFMpegWriter& operator=(const FFMpegWriter&) = delete;
    FFMpegWriter& operator=(FFMpegWriter &&) = delete;

    /**
     * @brief init : configure the ffmpeg objects
     * @param url : the url of the saved video file
     * @param width : the width of each output video frame
     * @param height : the height of each output video frame
     * @param frameRate : the frame rate of the output video
     * @param pixelFormat : pixel format of each output picture
     * @return ErrorCode
     */
    int init(const std::string& url, int width, int height, int frameRate,
             PixelFormat pixelFormat);

    /**
     * @brief transer : transfer the frame data from data to dstFrame
     * @param dstFrame : where to save the frame data, it should be allocated before
     * @param data : the source data ptr
     * @param linesize : the linesize of each plane
     * @param width : the width of each input video frame
     * @param height : the height of each input video frame
     * @param pixelFormat : pixel format of each input picture
     * @return ErrorCode
     */
    int transfer(AVFrame* dstFrame, const unsigned char* const* data, const int* linesize,
                 int width, int height, PixelFormat pixelFormat);

    /**
     * @brief write : transfer the picture in frame to AVPacket and then save to disk
     * @param frame : the frame to be saved
     * @return ErrorCode
     */
    int write(AVFrame* frame);

    inline PixelFormat getPixelFormat() const {return m_pixel_format;}
    inline int getWidth() const {return m_video_width;}
    inline int getHeight() const {return m_video_height;}

private:
    AVOutputFormat* m_format = nullptr;
    AVFormatContext* m_format_ctx = nullptr;

    AVStream* m_video_stream = nullptr;

    AVCodecContext* m_video_encode_ctx = nullptr;
    AVCodec* m_video_encoder = nullptr;

    SwsContext* m_sws_ctx = nullptr;

    AVDictionary *m_option = nullptr;

    std::string m_output_name;
    std::int64_t m_next_pts = 0;

    int m_video_width = 0;
    int m_video_height = 0;
    int m_fps = 25;
    PixelFormat m_pixel_format = PixelFormat::RGB24;
    int m_have_video = 0;
    int m_encode_video = 0;

private:
    int add_video_stream(enum AVCodecID codec_id);

    int open_video();

    int close_stream();

    void destroy();
};

/**
 * @brief The WriteManager class : manage the video saving process.
 * with multiple threads. It should be used with FFmpegWriter together
 */
class WriteManager
{
public:
    /**
     * @brief WriteManager : default constructor.
     * the method init should be called afterwards
     */
    WriteManager() = default;

    /**
     * @brief WriteManager : constructor with specified information
     * @param writer : the FFMpegWriter for writting frames
     */
    WriteManager(const FFMpegWriter& writer);
    ~WriteManager();

    /**
     * @brief init : allocate the buffer
     * @param writer : the FFMpegWriter for writting frames
     * @return ErrorCode
     */
    int init(const FFMpegWriter& writer);

    /**
     * @brief fillPicture : fill the buffer with the source image "data".
     * It is called by a running thread.
     * @param writer : FFMpegWriter that transfers source data to required
     * output format
     * @param data : the source image data
     * @param linesize : the linesize of the source image data
     * @param width : the width of each input video frame
     * @param height : the height of each input video frame
     * @param pixelFormat : pixel format of each input picture
     */
    void fillPicture(FFMpegWriter& writer, const unsigned char* const* data, const int* linesize,
                     int width, int height, PixelFormat pixelFormat);

    /**
     * @brief start : it starts a new saving thread and saving frames in the buffer to disk
     * @param writer : FFMpegWriter responsible for writing to disk
     */
    void start(FFMpegWriter& writer);

    /**
     * @brief stop : stop the saving thread
     */
    void stop();


private:
//    PixelFormat m_pixel_format;
//    int m_width;
//    int m_height;
    const int m_buffer_size = 2;
    std::vector<VideoFrame> m_buffer;
//    std::vector<bool> m_in_use;
//    std::vector<bool> m_filled;
    std::mutex m_mtx;
    std::condition_variable m_cv_write;
    std::atomic<std::int64_t> m_fill_count;
    std::atomic<bool> m_is_writting;
//    size_t m_fetch_count = 0;

    int m_pts_index = 0;

    std::atomic<bool> m_flag_stop;
    std::thread m_thread;

private:
    int allocBuffer(PixelFormat pix_fmt, int width, int height);
    int deallocBuffer();

    void keepWritingFrames(FFMpegWriter& writer);
};

}   // namespace media
}   // namespace brick

#endif // BRICK_FFMPEGWRITER_H
