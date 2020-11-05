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
#include "amqp/queue.h"
#include "amqp/connection.h"
#include "amqp/amqpexception.h"
#include "exceptionpriv.h"

#include <stdexcept>

namespace brick
{
namespace amqp
{

Queue::Queue(const std::string& queueName, const int passive, const int durable, const int exclusive,
             const int auto_delete): queueName(queueName), passive(passive),
                durable(durable), exclusive(exclusive), auto_delete(auto_delete)
{

}

void Queue::declare(const uint16_t channelIdx, const Connection &conn, const Table &arguments)
{
    amqp_queue_declare_ok_t *r = nullptr;
    r = amqp_queue_declare(conn.get_c_connection_state(), channelIdx, amqp_cstring_bytes(queueName.c_str()),
                       passive, durable, exclusive, auto_delete, arguments.to_amqp_table_t());
    amqp_rpc_reply_t ret =  amqp_get_rpc_reply(conn.get_c_connection_state());
    throwAMQPException("fail to declare an amqp queue", ret);
    this->data = AmqpBytes(r->queue);

    this->queueName = this->data.toString();
}

//void Queue::declare(const uint16_t channelIdx, const amqp_connection_state_t_ *con, const amqp_table_t &arguments)
//{
//    amqp_queue_declare_ok_t *r = nullptr;
//    amqp_connection_state_t_ *conn = const_cast<amqp_connection_state_t_*>(con);
//    r = amqp_queue_declare(conn, channelIdx, amqp_cstring_bytes(queueName.c_str()),
//                       passive, durable, exclusive, auto_delete, arguments);
//    amqp_rpc_reply_t ret =  amqp_get_rpc_reply(conn);
//    throwAMQPException("fail to declare an amqp queue", ret);
//    this->data = AmqpBytes(r->queue);

//    this->queueName = this->data.toString();
//}

}   // namespace amqp
}   // namespace brick
