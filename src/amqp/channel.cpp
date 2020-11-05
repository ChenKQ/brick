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
#include "amqp/channel.h"
#include "amqp/connection.h"
#include "amqp/amqpexception.h"
#include "amqp/queue.h"
#include "amqp/exchange.h"
#include "exceptionpriv.h"

#include <iostream>

namespace brick
{
namespace amqp
{

Channel::Channel(const Connection &con, uint16_t channelIndex):
    p_channel_state(std::make_shared<ChannelState>(con, channelIndex))
{
}

Queue Channel::queueDeclare(const std::string& queueName, const int passive, const int durable, const int exclusive,
                           const int auto_delete, const Table& arguments)
{
    Queue q(queueName, passive, durable, exclusive, auto_delete);
    q.declare(this->p_channel_state->channelIndex, this->p_channel_state->conn, arguments);
    return q;
}

Exchange Channel::exchangeDeclare(const std::string &exchangeName, const std::string &exchangeType, const int passive,
                              const int durable, const int auto_delete, const int internal, const Table &arguments)
{
    Exchange x(exchangeName, exchangeType, passive, durable, auto_delete, internal);
    x.declare(this->p_channel_state->channelIndex, this->p_channel_state->conn, arguments);
    return x;
}

void Channel::bind(const std::string &queueName, const std::string &exchangeName, const std::string &bindingKey,
                   const Table& arguments)
{
    amqp_queue_bind(this->p_channel_state->conn.get_c_connection_state(), this->p_channel_state->channelIndex, amqp_cstring_bytes(queueName.c_str()),
                    amqp_cstring_bytes(exchangeName.c_str()), amqp_cstring_bytes(bindingKey.c_str()), arguments.to_amqp_table_t());
    amqp_rpc_reply_t ret = amqp_get_rpc_reply(this->p_channel_state->conn.get_c_connection_state());
    throwAMQPException("fail to bind", ret);
}

void Channel::unbind(const std::string &queueName, const std::string &exchangeName, const std::string &bindingKey,
                     const Table& arguments)
{
    amqp_queue_unbind(this->p_channel_state->conn.get_c_connection_state(), this->p_channel_state->channelIndex, amqp_cstring_bytes(queueName.c_str()),
                      amqp_cstring_bytes(exchangeName.c_str()), amqp_cstring_bytes(bindingKey.c_str()),
                       arguments.to_amqp_table_t());
    amqp_rpc_reply_t ret = amqp_get_rpc_reply(this->p_channel_state->conn.get_c_connection_state());
    throwAMQPException("fail to unbind", ret);
}

void Channel::basic_publish(const std::string &exchangeName, const std::string &routing_key, const std::string &content,
                            const int mandatory, const int immediate, const BasicProperites& properties)
{
    int status = amqp_basic_publish(this->p_channel_state->conn.get_c_connection_state(), this->p_channel_state->channelIndex, amqp_cstring_bytes(exchangeName.c_str()),
                       amqp_cstring_bytes(routing_key.c_str()), mandatory, immediate, properties.amqp_ptr(),
                       amqp_cstring_bytes(content.c_str()));
    throwException("fail to publish", status);
}

void Channel::basic_consume(const std::string &queueName, const int no_local, const int no_ack, const int exclusive,
                            const Table &arguments)
{
    amqp_basic_consume(this->p_channel_state->conn.get_c_connection_state(), this->p_channel_state->channelIndex, amqp_cstring_bytes(queueName.c_str()), amqp_empty_bytes,
                       no_local, no_ack, exclusive, arguments.to_amqp_table_t());
    amqp_rpc_reply_t ret = amqp_get_rpc_reply(this->p_channel_state->conn.get_c_connection_state());
    throwAMQPException("Consuming fail", ret);
}

std::string Channel::consume_message()
{
    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;

    amqp_maybe_release_buffers(this->p_channel_state->conn.get_c_connection_state());
    res = amqp_consume_message(this->p_channel_state->conn.get_c_connection_state(), &envelope, nullptr, 0);

    AmqpBytes body = AmqpBytes(envelope.message.body);
    std::string content = body.toString();
    amqp_destroy_envelope(&envelope);
    return content;
}

Channel::ChannelState::ChannelState(const Connection &con, uint16_t idx):conn(con), channelIndex(idx)
{
    this->open();
}

Channel::ChannelState::~ChannelState()
{
    if(this->opened)
    {
        try
        {
            this->close();
        }
        catch (const AMQPExcept &ex)
        {
            std::cerr << ex.what() << std::endl;
        }
    }
}

void Channel::ChannelState::open()
{
    amqp_channel_open(this->conn.get_c_connection_state(), this->channelIndex);
    amqp_rpc_reply_t ret = amqp_get_rpc_reply(this->conn.get_c_connection_state());
    throwAMQPException("fail to create channel", ret);
    this->opened = true;
}

void Channel::ChannelState::close()
{
    amqp_rpc_reply_t ret = amqp_channel_close(this->conn.get_c_connection_state(), this->channelIndex, AMQP_REPLY_SUCCESS);
    throwAMQPException("fail to close channel", ret);
    this->opened = false;
}

}   // namespace amqp
}   // namespace brick

