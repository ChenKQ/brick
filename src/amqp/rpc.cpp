#include "amqppriv.h"
#include "amqp/rpc.h"

namespace brick
{
namespace amqp
{

RpcReply::RpcReply(amqp_rpc_reply_t *pReply)
{
    pImp_reply = std::shared_ptr<amqp_rpc_reply_t>(pReply);
}

void RpcReply::reset(amqp_rpc_reply_t *pReply)
{
    pImp_reply.reset(pReply);
}

RpcReply::ResponseType RpcReply::getResponseType()
{
    return static_cast<ResponseType>(pImp_reply->reply_type);
}

const amqp_rpc_reply_t &RpcReply::to_amqp_rpc_reply_t() const
{
    return *pImp_reply;
}

void RpcReply::destroyAmqpRpcReply(amqp_rpc_reply_t *p)
{
    delete p;
}

}   // namespace amqp
}   // namespace brick
