#include "amqppriv.h"
#include "amqp/connectionstate.h"
#include "amqp/amqpexception.h"

#include <iostream>
#include <functional>

namespace brick
{
namespace amqp
{

void ConnectionState::init()
{
    amqp_connection_state_t_ *tcon = amqp_new_connection();
    if(tcon !=nullptr)
    {
        this->conn = std::shared_ptr<amqp_connection_state_t_>(tcon, ConnectionState::closeAndDestroy);
    }
    else
    {
        throw CreateTCPSocketError("");
    }
}

RpcReply::ResponseType ConnectionState::close(amqp_connection_state_t_ *c)
{
    amqp_rpc_reply_t ret = amqp_connection_close(c, AMQP_REPLY_SUCCESS);
    return static_cast<RpcReply::ResponseType>(ret.reply_type);
}

int ConnectionState::destroy(amqp_connection_state_t_ *c)
{
    return amqp_destroy_connection(c);
}

int ConnectionState::closeAndDestroy(amqp_connection_state_t_ *c)
{
    RpcReply::ResponseType reply = ConnectionState::close(c);
    amqp_response_type_enum ret = static_cast<amqp_response_type_enum>(reply);
    if(ret != AMQP_RESPONSE_NORMAL)
        return -1;
    return ConnectionState::destroy(c);
}

}   // namespace amqp
}   // namespace brick
