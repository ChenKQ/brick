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
#include "amqp/exchange.h"
#include "amqp/connection.h"
#include "amqp/amqpexception.h"
#include "exceptionpriv.h"

namespace brick
{
namespace amqp
{

Exchange::Exchange(const std::string &exchangeName, const std::string &exchangeType,
                   const int passive, const int durable, const int auto_delete, const int internal):
    exchangeName(exchangeName), exchangeType(exchangeType), passive(passive), durable(durable),
    auto_delete(auto_delete),internal(internal)
{

}

void Exchange::declare(const uint16_t channelIdx, const Connection &con, const Table &arguments)
{
    amqp_exchange_declare(con.get_c_connection_state(), channelIdx, amqp_cstring_bytes(exchangeName.c_str()),
                          amqp_cstring_bytes(exchangeType.c_str()), passive, durable, auto_delete, internal, arguments.to_amqp_table_t());
    amqp_rpc_reply_t ret = amqp_get_rpc_reply(con.get_c_connection_state());
    throwAMQPException("fail to declare an amqp exchange", ret);
}

}   // namespace amqp
}   // namespace brick
