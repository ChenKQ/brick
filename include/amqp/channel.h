/************************************
 * Created by ChenKQ                *
 * in Feb. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "amqp/connectionstate.h"
#include "amqp/connection.h"
#include "amqp/table.h"
#include "amqp/queue.h"
#include "amqp/exchange.h"
#include "amqp/property.h"

#include <string>
#include <memory>

namespace brick
{
namespace amqp
{

/**
 * @brief The Channel class is an important class during the communication with the remote rabbitmq server.
 * It can be copied but all the copies share the same channelstate object.
 */
class Channel final
{
public:
    /**
     * @brief Channel: constructure
     * @param con : a connection connected to the remote rabbitmq server
     * @param channelIndex : the channel index
     */
    Channel (const Connection &con, uint16_t channelIndex=1);


    Queue queueDeclare(const std::string &queueName="", const int passive=0, const int durable=0, const int exclusive=0,
                      const int auto_delete=1, const Table &arguments=Table());
    Exchange exchangeDeclare(const std::string &exchangeName, const std::string &exchangeType,
                         const int passive=0, const int durable=0, const int auto_delete=0, const int internal=0,
                         const Table &arguments=Table());
    void bind(const std::string &queueName, const std::string &exchangeName, const std::string &bindingKey,
              const Table &arguments=Table());
    void unbind(const std::string &queueName, const std::string &exchangeName, const std::string &bindingKey,
                const Table &arguments=Table());
    void basic_publish(const std::string &exchangeName, const std::string &routing_key, const std::string &content,
                       const int mandatory=0, const int immediate=0, const BasicProperites& property = BasicProperites());
    void basic_consume(const std::string &queueName, const int no_local=0, const int no_ack=1,
                       const int exclusive=0, const Table &arguments=Table());
    std::string consume_message();

    inline bool isOpened() const
    {
        if(this->p_channel_state != nullptr)
            return this->p_channel_state->opened;
        else
            return false;
    }


private:
    class ChannelState;
    std::shared_ptr<ChannelState> p_channel_state = nullptr;

    /**
     * @brief The ChannelState class: an internal class used by the Channel.
     * It maintains the switch of the channel through constructotr and deconstructor.
     * The channel will be opened when it is constructed and be closed when it is released.
     * It cannot be copied and moved. Therefore, use std::share_ptr to point to the object.
     */
    class ChannelState final
    {
    public:
        ChannelState(const Connection &con, uint16_t idx);

        ChannelState(const ChannelState&) = delete;
        ChannelState(Channel&&) = delete;
        ChannelState& operator=(const ChannelState&) = delete;
        ChannelState& operator=(ChannelState &&) = delete;
        ~ChannelState();

        /**
         * @brief open: open the channel
         */
        void open();
        /**
         * @brief close: close the channel
         */
        void close();
    public:
        Connection conn;
        const uint16_t channelIndex;
        bool opened = false;
    };
};

}   // namespace amqp
}   // namespace brick


#endif // CHANNEL_H
