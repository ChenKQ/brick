/************************************
 * Created by ChenKQ                *
 * in May. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/


#ifndef AMQP_CONNECTIONSTATE_H
#define AMQP_CONNECTIONSTATE_H

#include "amqp/rpc.h"

#include <memory>

struct amqp_connection_state_t_;

namespace brick
{
namespace amqp
{

class ConnectionState
{
public:
    void init();

    inline bool isConnected() const {return this->conn != nullptr;}
    inline std::shared_ptr<amqp_connection_state_t_> getPtr() const {return this->conn;}

private:
    std::shared_ptr<amqp_connection_state_t_> conn = nullptr;  

    static RpcReply::ResponseType close(amqp_connection_state_t_ *c);
    static int destroy(amqp_connection_state_t_ *c);
    static int closeAndDestroy(amqp_connection_state_t_ *c);
};

}   // namespace amqp
}   // namespace brick

#endif // CONNECTIONSTATE_H
