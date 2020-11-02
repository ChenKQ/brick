#include "media/imageformatconverter.h"

namespace brick
{
namespace media
{

ImageFormatConverter::~ImageFormatConverter()
{
    destroy();
}

int ImageFormatConverter::create(int src_width, int src_height, PixelFormat src_pix_format,
                                 int dst_width, int dst_height, PixelFormat dst_pix_format)

{
    m_src_width = src_width;
    m_src_height = src_height;
    m_src_pix_format = src_pix_format;

    m_dst_width = dst_width;
    m_dst_height = dst_height;
    m_dst_pix_format = dst_pix_format;

    m_sws_context = sws_getContext(m_src_width, m_src_height, static_cast<AVPixelFormat>(m_src_pix_format),
                                      m_dst_width, m_dst_height, static_cast<AVPixelFormat>(m_dst_pix_format),
                                   SWS_BILINEAR, nullptr, nullptr, nullptr);
    if(!m_sws_context)
    {
        return AVSWS_GET_CONTEXT_ERROR;
    }

    /* buffer is going to be written to rawvideo file, no alignment */
    int ret = av_image_alloc(m_dst_data, m_dst_linesize, m_dst_width, m_dst_height,
                             static_cast<AVPixelFormat>(m_dst_pix_format), 1);
    if(ret < 0)
    {
        return AVUTIL_IMAGE_ALLOC_ERROR;
    }
    m_dst_bufsize = ret;

    return ErrorCode::SUCCESS;
}

void ImageFormatConverter::destroy()
{
    if(!m_dst_data[0])
    {
        av_freep(&m_dst_data[0]);
    }
    if(!m_sws_context)
    {
        sws_freeContext(m_sws_context);
    }
}

unsigned char** ImageFormatConverter::convert(const unsigned char * const* src_data, const int* src_linesize)
{
    sws_scale(m_sws_context, src_data, src_linesize, 0, m_src_height,
              m_dst_data, m_dst_linesize);
    return m_dst_data;
}

unsigned char** ImageFormatConverter::copy(const unsigned char* const* src_data, const int *src_linesize)
{
    /* copy decoded frame to destination buffer:
    * this is required since rawvideo expects non aligned data */
    av_image_copy(m_dst_data, m_dst_linesize, const_cast<const unsigned char **>(src_data), src_linesize,
                  static_cast<AVPixelFormat>(m_src_pix_format), m_src_width, m_src_height);
    return m_dst_data;
}

unsigned char** ImageFormatConverter::getDataPtr()
{
    return m_dst_data;
}

int *ImageFormatConverter::getLinesize()
{
    return m_dst_linesize;
}


}   //namespace media
}   //namespace brick
