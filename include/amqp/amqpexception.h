/************************************
 * Created by ChenKQ                *
 * in Feb. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/


#ifndef AMQPEXCEPTION_H
#define AMQPEXCEPTION_H

#include "amqp/rpc.h"

#include <stdexcept>
#include <string>


namespace  brick
{

namespace amqp
{

class AMQPExcept: public std::runtime_error
{
public:
    explicit AMQPExcept(const std::string& str) noexcept;

};

class CreateConnectionError:public AMQPExcept
{
public:
    explicit CreateConnectionError(const std::string& str) noexcept;
};

class CreateTCPSocketError: public AMQPExcept
{
public:
    explicit CreateTCPSocketError(const std::string& str) noexcept;
};

class OpenTCPSocketError: public AMQPExcept
{
public:
    explicit OpenTCPSocketError(const std::string& str) noexcept;
};

class QueueDeclareError:public AMQPExcept
{
public:
    explicit QueueDeclareError(const std::string& str) noexcept;
};

}   // namespace amqp
}   // namespace brick

#endif // AMQPEXCEPTION_H
