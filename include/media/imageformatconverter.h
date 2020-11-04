#ifndef BRICK_IMAGEFORMATCONVERTER_H
#define BRICK_IMAGEFORMATCONVERTER_H

#include "ffmpeg.h"

namespace brick
{
namespace media
{

/**
 * @brief The ImageFormatConverter class : the image format converter that transfers
 * the original ffmpeg frame into aligned and specified image
 */
class ImageFormatConverter
{
public:

    /**
     * @brief ImageFormatConverter : The default constructor
     */
    ImageFormatConverter() = default;

    /**
     * @brief ~ImageFormatConverter : deconstructor, it release the ffmpeg resources
     */
    ~ImageFormatConverter();

    // copy and move are forbidden
    ImageFormatConverter(const ImageFormatConverter &) = delete;
    ImageFormatConverter(ImageFormatConverter &&) = delete;
    ImageFormatConverter& operator=(const ImageFormatConverter &) = delete;
    ImageFormatConverter& operator=(ImageFormatConverter &&) = delete;

    /**
     * @brief create : create and initial the ffmpeg resources
     * @param src_width : the original image width
     * @param src_height : the original image height
     * @param src_pix_format : the original image pixel format
     * @param dst_width : the destination image width
     * @param dst_height : the destination image height
     * @param dst_pix_format : the destination image pixel format
     * @return : the error code (enum ErrorCode)
     */
    int create(int src_width, int src_height, int src_pix_format,
               int dst_width, int dst_height, int dst_pix_format);

    /**
     * @brief convert : convert the source data to the buffer and return the buffer pointer
     * @param src_data : a pointer refering to the original image planes
     * @param src_linesize : the linesize of each frame
     * @return a pointer referring to the converted image plane addresses
     */
    unsigned char** convert(const unsigned char* const* src_data, const int* src_linesize);

    /**
     * @brief copy : copy the unaligned source data to the specified buffer (aligned data)
     * @param src_data : a pointer refering to the original image planes
     * @param src_linesize : the linesize of each frame
     * @return a pointer referring to the copied image plane addresses
     */
    unsigned char** copy(const unsigned char* const* src_data, const int* src_linesize);

    /**
     * @brief getDataPtr : get the pointer referring to the copied image plane addresses
     * @return a pointer referring to the copied image plane addresses
     */
    unsigned char** getDataPtr();

    /**
     * @brief getLinesize : get the linesizes of the image planes
     * @return a pointer referring to the linesize array
     */
    int* getLinesize();

private:
    int m_src_width;
    int m_src_height;
    int m_src_linesize[4];
    int m_src_pix_format;

    int m_dst_width;
    int m_dst_height;
    int m_dst_linesize[4];
    int m_dst_pix_format;

    unsigned char* m_dst_data[4] = {nullptr};
    int m_dst_bufsize;

    SwsContext *m_sws_context;

    /**
     * @brief destroy : release the ffmpeg resources. it is called by the deconstructor
     */
    void destroy();
};

}   //namespace media
}   //namespace brick

#endif // BRICK_IMAGEFORMATCONVERTER_H
