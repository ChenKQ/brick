/************************************
 * Created by ChenKQ                *
 * in May 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/

#ifndef AMQP_TCPSOCKET_H
#define AMQP_TCPSOCKET_H

#include "connectionstate.h"

#include <memory>
#include <string>

typedef struct amqp_socket_t_ amqp_socket_t;

namespace brick
{
namespace amqp
{

class TcpSocket
{
public:
    TcpSocket(const std::string &hostName, int tcpPort);
    void init(ConnectionState &cstate);

    inline std::shared_ptr<amqp_socket_t_> getPtr() const {return this->socket;}

private:
    std::string hostName;
    int tcpPort;

    std::shared_ptr<amqp_socket_t_> socket = nullptr;

    void create(ConnectionState &cstate);
    void open();
};

}   // namespace amqp
}   // namespace brick


#endif // AMQP_TCPSOCKET_H
