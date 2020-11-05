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
#include "amqp/amqpexception.h"
#include <sstream>

namespace brick
{

namespace amqp
{

AMQPExcept::AMQPExcept(const std::string &str) noexcept :std::runtime_error(str)
{

}

CreateConnectionError::CreateConnectionError(const std::string &str) noexcept :
    AMQPExcept ("fail to create amqp connection: " + str)
{

}

CreateTCPSocketError::CreateTCPSocketError(const std::string &str) noexcept:
    AMQPExcept ("fail to create amqp tcp socket: " + str)
{

}

OpenTCPSocketError::OpenTCPSocketError(const std::string &str) noexcept:
    AMQPExcept ("fail to open amqp tcp socket: " + str)
{

}

QueueDeclareError::QueueDeclareError(const std::string &str) noexcept :
     AMQPExcept ("fail to declare amqp queue: " + str)
{

}

}   // namespace amqp
}   // namespace brick

