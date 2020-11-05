#ifndef BRICK_AMQP_EXCEPTIONPRIV_H
#define BRICK_AMQP_EXCEPTIONPRIV_H

#include "amqppriv.h"

namespace brick
{
namespace amqp
{

void throwAMQPException(const std::string& context, const amqp_rpc_reply_t& ret);
void throwException(const std::string& context, int errorCode);

}   // namespace amqp
}   // namespace brick


#endif // BRICK_AMQP_EXCEPTIONPRIV_H
