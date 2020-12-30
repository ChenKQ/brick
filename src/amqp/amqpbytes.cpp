/************************************
 * Created by ChenKQ                *
 * on Feb. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/

#include "amqppriv.h"
#include "amqp/amqpbytes.h"

#include <cstring>

namespace brick
{
namespace amqp
{

void swap(AmqpBytes &lhs, AmqpBytes &rhs)
{
    using std::swap;
    swap(lhs.len, rhs.len);
    swap(lhs.bytes, rhs.bytes);
}

AmqpBytes::AmqpBytes(const amqp_bytes_t &src):len(src.len)
{
    this->bytes = malloc(src.len);
    if (this->bytes != nullptr)
    {
        memcpy(this->bytes, src.bytes, src.len);

    }
    else
    {
        throw std::bad_alloc();
    }
}

AmqpBytes::AmqpBytes(size_t len, void *bytes):len(len)
{
    this->bytes = malloc(len);
    if (this->bytes != nullptr)
    {
        memcpy(this->bytes, bytes, len);
    }
    else
    {
        throw std::bad_alloc();
    }
}

AmqpBytes::AmqpBytes(const AmqpBytes &another): len(another.len)
{
    this->bytes = malloc(len);
    if (this->bytes != nullptr)
    {
        memcpy(this->bytes, bytes, len);
    }
    else
    {
        throw std::bad_alloc();
    }
}

AmqpBytes::AmqpBytes(AmqpBytes &&another) noexcept:len(another.len), bytes(another.bytes)
{
    another.bytes = nullptr;
}

AmqpBytes &AmqpBytes::operator=(AmqpBytes rhs)
{
    swap(*this, rhs);
    return *this;
}

AmqpBytes::~AmqpBytes()
{
    if(this->bytes!=nullptr)
        free(this->bytes);
}

AmqpBytes AmqpBytes::takeOver(const amqp_bytes_t &src)
{
    AmqpBytes t;
    t.len = src.len;
    t.bytes = src.bytes;
    return t;
}

AmqpBytes AmqpBytes::takeOver(size_t len, void *bytes)
{
    AmqpBytes t;
    t.len = len;
    t.bytes = bytes;
    return t;
}

std::string AmqpBytes::toString()
{
    char* str = static_cast<char*>(this->bytes);
    return {str, len};
}

amqp_bytes_t AmqpBytes::to_amqp_bytes()
{
    amqp_bytes_t res;
    res.len = this->len;
    res.bytes = this->bytes;
    return res;
}

}   // namespace amqp
}   // namespace brick



