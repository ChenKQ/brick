#ifndef BRICK_IMAGE_H
#define BRICK_IMAGE_H

struct SwsContext;
struct AVFrame;

namespace brick
{
namespace media
{

class Image;
void swap(Image& lhs, Image &rhs);

/**
 * @brief The Image class : it is a basic class for image data storage,
 * which are stored in ```m_pData``` where each pointer refers to a
 * image plane. It cannot be copied constructed or copied assigned,
 * but instead, move semantics are supported.
 */
class Image
{
    friend void swap(Image& lhs, Image &rhs);
public:
    static constexpr size_t NumberPlanes = 4;
public:
    Image() = default;
    ~Image();

    /** copy constructor and copy assignment are forbidden **/
    Image(const Image& rhs) = delete;
    Image& operator=(const Image& rhs) = delete;

    /** move constructor and move assignment are allowd**/
    Image(Image &&rhs) noexcept;
    Image& operator=(Image && rhs);

    /**
     * @brief Image : constructor, it allocate memory buffer for the image accordingly
     * @param width : the allocated image width
     * @param height : the allocated image height
     * @param pixelFormat : the pixel format of the image.
     * Refer to ```brick::media::PixelFormat``` in "media/ffmpeg.h" for more details.
     */
    Image(int width, int height, int pixelFormat, int align=0);

    /**
     * @brief Image : it alloc the memory buffer and then copy the data into buffer
     * @param data : the data pointer array to the source image
     * @param linesize : the line sizes of the source image
     * @param width : the width of the source image
     * @param height : the height of the source image
     * @param pixelFormat : the pixel format of the source image
     */
    Image(const unsigned char* const* data, const int* linesize,
          int width, int height, int pixelFormat, int align=0);

    /**
     * @brief fillBuffer : it fills the buffer with the image in ```data```
     * If the buffer is not allocated, it will allocate the momory buffer first.
     * If the ```fit``` is set ```true```, the original image will be rescaled first
     * and then fill the buffer when the dimensions are not match.
     * On the contray, is the ```fit`` is set false (by default), the buffer will not
     * be filled and return false.
     * @param data : the data pointer array to the source image
     * @param linesize : the linesize of the the original image
     * @param width : the width of the source image
     * @param height : the height of the source image
     * @param pixelFormat : the pixel format of the source image
     * @param fit : whether to scale the original image by first when the dimensions
     * are not match.
     * @return whether filled the buffer successfully
     */
    bool fillBuffer(const unsigned char* const* data, const int* linesize,
                    int width, int height, int pixelFormat, bool fit = false);

    /**
     * @brief rescale : recale this image and return a new Image.
     * It will return empty image if this image is empty.
     * @param dst_width : the desired width
     * @param dst_height : the desired height
     * @param dst_pix_fmt : the disired pixel format
     * @return : the returned image
     */
    Image rescale(int dst_width, int dst_height, int dst_pix_fmt);

    /**
     * @brief rescale : recale this image and fill ```dstImg```.
     * @param dstImg : the image to be filled. It cannot be empty.
     * @return : whether filled the dstImage successfully
     */
    bool rescale(Image& dstImg);

    /**
     * @brief empty : whether the image is empty
     * @return : whether the image is empty
     */
    inline bool empty() const
    {
        return m_frame==nullptr;
    }

    /**
     * @brief getWidth : the width of the image
     * @return : image width
     */
    int getWidth() const;

    /**
     * @brief getHeight : the height of the image
     * @return : image height
     */
    int getHeight() const;

    /**
     * @brief getPixelFormat : the pixel format of the image
     * @return : the pixel format of the image
     */
    int getPixelFormat() const;

    /**
     * @brief getLineSize : the linesize array of the image
     * @return : the linesize array of the image
     */
    const int* getLineSize() const;

    /**
     * @brief getBufferSize : get the required buffer size for the image
     * @return : the buffer size required for the image
     */
    int getBufferSize() const ;

    /**
     * @brief getDataPtr : get the pointer to the image raw data
     * @return : the pointer to the image raw data
     */
    unsigned char** getDataPtr() const;

    AVFrame* getAVFrame() const;

private:
    int alloc(int width, int height, int pixFormat, int align=0);
    void dealloc();

    int rescaleFrom(const unsigned char* const* data, const int* linesize,
                 int width, int height, int pixelFormat);
    int copyFrom(const unsigned char* const* data, const int* linesize,
              int width, int height, int pixelFormat);

private:
    AVFrame* m_frame = nullptr;
    SwsContext* m_sws_context = nullptr;

    int m_align = 32;
};

}   // namespace media
}   // namespace brick

#endif // BRICK_IMAGE_H
