/************************************
 * Created by ChenKQ                *
 * in Feb. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/

#ifndef QUEUE_H
#define QUEUE_H

#include "amqpbytes.h"
#include "table.h"

#include <string>

namespace brick
{
namespace amqp
{

class Connection;
class Channel;

class Queue final
{
public:
    Queue(const std::string& queueName="", const int passive=0, const int durable=0, const int exclusive=0,
          const int auto_delete=1);

//    void declare(const uint16_t channelIdx, const amqp_connection_state_t_ *con, const amqp_table_t &arguments=amqp_empty_table);
    void declare(const uint16_t channelIdx, const Connection &conn, const Table &arguments = Table());

    inline std::string getQueueName() const {return this->queueName; }
    inline int getPassive() const {return this->passive; }
    inline int getDurable() const {return this->durable; }
    inline int getExclusive() const {return this->exclusive; }
    inline int getAutoDelete() const {return this->auto_delete; }

private:
    std::string queueName;
    int passive;
    int durable;
    int exclusive;
    int auto_delete;

    AmqpBytes data;
};

}   // namespace amqp
}   // namespace brick


#endif // QUEUE_H
