#include "amqppriv.h"
#include "exceptionpriv.h"
#include "amqp/amqpexception.h"

#include <string>
#include <sstream>

namespace brick
{
namespace amqp
{

void throwAMQPException(const std::string &context, const amqp_rpc_reply_t &ret)
{
    switch (ret.reply_type) {
    case AMQP_RESPONSE_NORMAL:
        return;

    case AMQP_RESPONSE_NONE:{
        std::stringstream errorMsg;
        errorMsg << context + ": missing RPC reply type!\n";
        throw AMQPExcept(errorMsg.str());
    }

    case AMQP_RESPONSE_LIBRARY_EXCEPTION:{
        std::stringstream errorMsg;
        errorMsg << context + ": " << ret.library_error << ".\n";
        throw AMQPExcept(errorMsg.str());
    }

    case AMQP_RESPONSE_SERVER_EXCEPTION:
        switch (ret.reply.id) {
        case AMQP_CONNECTION_CLOSE_METHOD: {
            amqp_connection_close_t *m = static_cast<amqp_connection_close_t *>(ret.reply.decoded);
            std::stringstream errorMsg;
            errorMsg << context + ": server connection error, message: " << m->reply_code << " " << m->reply_text.len
                    << " " << m->reply_text.bytes;
            throw AMQPExcept(errorMsg.str());
        }
        case AMQP_CHANNEL_CLOSE_METHOD: {
            amqp_channel_close_t *m = static_cast<amqp_channel_close_t *>(ret.reply.decoded);
            std::stringstream errorMsg;
            errorMsg << context + ": server channel error, message: " << m->reply_code << " " << m->reply_text.len
                     << m->reply_text.bytes;
            throw AMQPExcept(errorMsg.str());
        }
        default:
            std::stringstream errorMsg;
            errorMsg << context + ": unknown server error, method id: " << ret.reply.id;
            throw AMQPExcept(errorMsg.str());
        }
    default:
        break;
    }
    throw AMQPExcept("unknown login error!");
}

void throwException(const std::string& context, int errorCode)
{
    if (errorCode < 0) {
        std::stringstream errorMsg;
        errorMsg << context << errorCode;
        throw AMQPExcept(errorMsg.str());
    }
}



}   // namespace amqp
}   // namespace brick
