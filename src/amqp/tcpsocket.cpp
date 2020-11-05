#include "amqppriv.h"
#include "amqp/tcpsocket.h"
#include "amqp/amqpexception.h"

#include <memory>

namespace brick
{

namespace amqp
{

TcpSocket::TcpSocket(const std::string &hostName, int tcpPort):hostName(hostName), tcpPort(tcpPort)
{
//    this->hostName = hostName;
//    this->tcpPort = tcpPort;
}

void TcpSocket::init(ConnectionState &cstate)
{
    this->create(cstate);
    this->open();
}

void TcpSocket::create(ConnectionState &cstate)
{
    auto conn = cstate.getPtr();
    amqp_socket_t_ *tsocket = amqp_tcp_socket_new(conn.get());
    if(tsocket!=nullptr)
    {
        this->socket = std::shared_ptr<amqp_socket_t_>(tsocket,[](amqp_socket_t_ *){});
    }
    else
    {
        throw CreateTCPSocketError("");
    }

}

void TcpSocket::open()
{
    int status = amqp_socket_open(this->socket.get(), this->hostName.c_str(), this->tcpPort);
    if(status)
    {
        throw OpenTCPSocketError("");
    }
}

}   // namespace amqp
}   // namespace brick
