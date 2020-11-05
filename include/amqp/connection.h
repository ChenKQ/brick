/************************************
 * Created by ChenKQ                *
 * in Feb. 2020                     *
 * at Beijing.                      *
 *==================================*
 *                                  *
 *                                  *
 *                                  *
 * *********************************/

#ifndef CONNECTION_H
#define CONNECTION_H

#include "amqp/connectionstate.h"
#include "amqp/tcpsocket.h"

#include <string>

namespace brick
{
namespace amqp
{

typedef struct AmqpHost{
    std::string hostName;
    int port=5672;
} Host;

typedef struct AmqpUser{
    std::string userName;
    std::string passwd;
} User;

/**
 * @brief The Connection class minics the Connection type in Java APIs. It is responsible for connecting the remote rabbitmq server.
 * The instances are not allowed to be copied. Therefore, use pointers and references instead.
 */
class Connection final
{
public:
    Connection(const std::string& hostName="localhost", int portNumber=5672,
               const std::string& userName="guest", const std::string& password="guest",
               const std::string& virtualHost="/");

    inline std::string getHostName() const {return this->host.hostName; }
    inline int getPort() const {return this->host.port; }
    inline std::string getUserName() const {return this->user.userName; }
    inline std::string getPassword() const {return this->user.passwd; }
    inline std::string getVirtualHost() const {return  this->vHost; }
    inline const ConnectionState& getConnectionState() const {return this->connection_state;}
    inline ConnectionState& getConnectionState() {return this->connection_state;}
    inline amqp_connection_state_t_* get_c_connection_state() const {return this->connection_state.getPtr().get();}

private:
    Host host;
    User user;
    std::string vHost;

    ConnectionState connection_state;
    TcpSocket tcp_socket;

    /**
     * @brief connect: connect to the remote rabbitmq server
     */
    void connect();
    /**
     * @brief login: amqp login
     */
    void login();
};

}   // namespace amqp
}   // namespace brick


#endif // CONNECTION_H
