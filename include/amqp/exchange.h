/************************************
 * Created by ChenKQ                *
 * in Feb. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "amqp/connection.h"
#include "amqp/table.h"

#include <string>

namespace brick
{
namespace amqp
{

class Exchange final
{
public:
    Exchange(const std::string &exchangeName, const std::string &exchangeType,
             const int passive=0, const int durable=0, const int auto_delete=0, const int internal=0);

    //    void declare(const uint16_t channelIdx, const amqp_connection_state_t_ *con, const amqp_table_t &arguments=amqp_empty_table);
    void declare(const uint16_t channelIdx, const Connection &con, const Table &arguments=Table());

    inline std::string getExchangeName() const {return this->exchangeName;}
    inline std::string getExchangeType() const {return this->exchangeType;}
    inline int getPassive() const {return this->passive;}
    inline int getDurable() const {return  this->durable;}
    inline int getAutoDelete() const {return this->auto_delete;}
    inline int getInternal() const {return  this->internal;}
//    inline AmqpBytes& getExchangeBytes() {return this->data;}
private:
    std::string exchangeName;
    std::string exchangeType;
    int passive;
    int durable;
    int auto_delete;
    int internal;

//    AmqpBytes data;
};


}   // namespace amqp
}   // namespace brick

#endif // EXCHANGE_H
