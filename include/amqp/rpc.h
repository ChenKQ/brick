#ifndef BRICK_AMQP_RPC_H
#define BRICK_AMQP_RPC_H

#include <memory>

typedef struct amqp_rpc_reply_t_ amqp_rpc_reply_t;

namespace brick
{
namespace amqp
{

class RpcReply
{
public:
    enum ResponseType
    {
        NONE = 0,
        NORMAL,
        LIBRARY_EXCEPTION,
        SERVER_EXCEPTION
    };

public:
    explicit RpcReply(amqp_rpc_reply_t* pReply);

    void reset(amqp_rpc_reply_t* pReply);
    ResponseType getResponseType();
    const amqp_rpc_reply_t& to_amqp_rpc_reply_t() const;

private:
    std::shared_ptr<amqp_rpc_reply_t> pImp_reply;

    static void destroyAmqpRpcReply(amqp_rpc_reply_t* p);
};



}   // namespace amqp
}   // namespace brick

#endif // BRICK_AMQP_RPC_H
