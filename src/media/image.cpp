#include "media/image.h"

#include "media/ffmpeg.h"
#include "ffmpegpriv.h"
#include <assert.h>

namespace brick
{
namespace media
{
Image::Image(Image &&rhs) noexcept : m_frame(rhs.m_frame), m_sws_context(rhs.m_sws_context)
{
    rhs.m_frame = nullptr;
    rhs.m_sws_context = nullptr;
}

Image::~Image()
{
    dealloc();
}

Image &Image::operator=(Image &&rhs)
{
    swap(*this, rhs);
    return *this;
}

Image::Image(int width, int height, int pixelFormat, int align)
{
    m_align = align;
    alloc(width, height, pixelFormat, align);
}

Image::Image(const unsigned char * const *data, const int *linesize, int width, int height, int pixelFormat, int align)
{
    m_align = align;
    int flag = alloc(width, height, pixelFormat, align);
    if(flag < 0)
        return;
    av_image_copy(m_frame->data, m_frame->linesize,
                  const_cast<const unsigned char **>(data), linesize,
                  static_cast<AVPixelFormat>(pixelFormat), width, height);
}

bool Image::fillBuffer(const unsigned char * const *data, const int *linesize, int width, int height, int pixelFormat,bool fit)
{
    if(empty())
    {
        int flag = alloc(width, height, pixelFormat);
        if(ErrorCode::SUCCESS != flag)
        {
            return false;
        }
    }

    if((width!=m_frame->width) || (height!=m_frame->height) ||
            (pixelFormat!=m_frame->format))
    {
        if(fit)
        {
            if(ErrorCode::SUCCESS != rescaleFrom(data, linesize, width, height, pixelFormat))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return copyFrom(data, linesize, width, height, pixelFormat);
    }

}

int Image::rescaleFrom(const unsigned char * const *data, const int *linesize, int width, int height, int pixelFormat)
{
    if(m_sws_context == nullptr)
    {
        m_sws_context = sws_getContext(width, height, static_cast<AVPixelFormat>(pixelFormat),
                                       m_frame->width, m_frame->height, static_cast<AVPixelFormat>(m_frame->format),
                                       SWS_BICUBIC, nullptr, nullptr, nullptr);
    }
    if(m_sws_context == nullptr)
    {
        return AVSWS_GET_CONTEXT_ERROR;
    }

    int flag = sws_scale(m_sws_context, data, linesize, 0, height,
              m_frame->data, m_frame->linesize);
    if(flag !=height)
    {
        return ErrorCode::AVSWS_SCALE_ERROR;
    }
    return ErrorCode::SUCCESS;
}

int Image::copyFrom(const unsigned char * const *data, const int *linesize, int width, int height, int pixelFormat)
{
    av_image_copy(m_frame->data, m_frame->linesize,
                  const_cast<const unsigned char **>(data), linesize,
                  static_cast<AVPixelFormat>(pixelFormat), width, height);
    return ErrorCode::SUCCESS;

}

Image Image::rescale(int dst_width, int dst_height, int dst_pix_fmt)
{
    if(empty())
    {
        return {};
    }

    Image img;
    img.alloc(dst_width, dst_height, dst_pix_fmt);
    img.rescaleFrom(m_frame->data, m_frame->linesize,
                    m_frame->width, m_frame->height, m_frame->format);
    return img;
}

bool Image::rescale(Image &dstImg)
{
    assert(!dstImg.empty());
    if(empty())
    {
        return false;
    }
    dstImg.rescaleFrom(m_frame->data, m_frame->linesize,
                       m_frame->width, m_frame->height, m_frame->format);
    return true;
}

int Image::getWidth() const
{
    if(m_frame!=nullptr)
    {
        return m_frame->width;
    }
    return 0;
}

int Image::getHeight() const
{
    if(m_frame != nullptr)
    {
        return m_frame->height;
    }
    return 0;
}

int Image::getPixelFormat() const
{
    if(m_frame!=nullptr)
    {
        return m_frame->format;
    }
    return PixelFormat::UNKNOWN;
}

const int *Image::getLineSize() const
{
    if(m_frame!=nullptr)
    {
        return m_frame->linesize;
    }
    return nullptr;
}

int Image::getBufferSize() const
{
    int bufferSize = av_image_get_buffer_size(static_cast<AVPixelFormat>(m_frame->format),
                                              m_frame->width, m_frame->height, m_align);
    return bufferSize;
}

unsigned char** Image::getDataPtr() const
{
    if(m_frame!=nullptr)
    {
        return m_frame->data;
    }
    return nullptr;
}

AVFrame *Image::getAVFrame() const
{
    return m_frame;
}

int Image::alloc(int width, int height, int pixFormat, int align)
{
    return FFMpeg::AllocPicture(&m_frame, pixFormat, width, height, align);
}

void Image::dealloc()
{
    if(m_frame!=nullptr)
    {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
}

void swap(Image &lhs, Image &rhs)
{
    using std::swap;
    swap(lhs.m_frame, rhs.m_frame);
    swap(lhs.m_sws_context, rhs.m_sws_context);
    swap(lhs.m_align, rhs.m_align);
}

}   // namespace media
}   // namespace brick
