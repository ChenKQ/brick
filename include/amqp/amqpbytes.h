/************************************
 * Created by ChenKQ                *
 * in Feb. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/

#ifndef BRICK_AMQPBYTES_H
#define BRICK_AMQPBYTES_H

#include <string>

typedef struct amqp_bytes_t_ amqp_bytes_t;

namespace brick
{
namespace amqp
{

/**
 * @brief The AmqpBytes class: It is a simple wrapper of the structure "amqp_bytes_t" in rabbitmq-c
 * for a simple usage. The memory is automatic managed.
 */
class AmqpBytes final
{
    friend void swap(AmqpBytes &lhs, AmqpBytes &rhs);
public:
    explicit AmqpBytes(const amqp_bytes_t &src);
    AmqpBytes(size_t len, void *bytes);

    AmqpBytes() = default;
    AmqpBytes(const AmqpBytes &another);
    AmqpBytes(AmqpBytes &&another) noexcept;
    AmqpBytes& operator=(AmqpBytes rhs);
    ~AmqpBytes();

    /**
     * @brief takeOver: it takes over the content in the src. After taken over, do not manually free the content in src.
     * @param src: the data to be taken over.
     * @return: the constructed instance created from src.
     */
    static AmqpBytes takeOver(const amqp_bytes_t &src);
    /**
     * @brief takeOver: it takes over the data in bytes. After taken over, do not manually free the data in bytes.
     * @param len: the length of the data.
     * @param bytes: the pointer pointing to the data.
     * @return
     */
    static AmqpBytes takeOver(size_t len, void *bytes);

    /**
     * @brief toString: convert to the string type of content.
     * @return: converted string
     */
    std::string toString();

    /**
     * @brief to_amqp_bytes: convert to amqp_bytes_t type. the memory are shared. It is just for a temporary usage.
     * @return: returned amqp_bytes_t instance.
     */
    amqp_bytes_t to_amqp_bytes();

private:
    size_t len=0;
    void *bytes = nullptr;
};

}   // namespace amqp
}   // namespace brick

#endif // BRICK_AMQPBYTES_H
