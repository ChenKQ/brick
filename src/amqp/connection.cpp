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
#include "amqp/connection.h"
#include "amqp/amqpexception.h"
#include "exceptionpriv.h"

#include <memory>
#include <sstream>

namespace brick
{
namespace amqp
{

Connection::Connection(const std::string& hostName, int portNumber,
                       const std::string& userName, const std::string& password,
                       const std::string& virtualHost): tcp_socket(hostName, portNumber)
{
    this->host.hostName=hostName;
    this->host.port = portNumber;
    this->user.userName = userName;
    this->user.passwd = password;
    this->vHost = virtualHost;
    this->connect();
    this->login();
}


void Connection::connect()
{
    this->connection_state.init();
    this->tcp_socket.init(this->connection_state);
}


void Connection::login()
{
    auto p_conn_state = this->connection_state.getPtr();
    amqp_rpc_reply_t ret = amqp_login(p_conn_state.get(), this->vHost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                      this->user.userName.c_str(), this->user.passwd.c_str());
    throwAMQPException("fail to login", ret);
}

}   // namespcae amqp
}   // namespace brick

