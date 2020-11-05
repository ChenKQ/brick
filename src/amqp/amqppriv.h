#ifndef BRICK_AMQP_AMQPPRIV_H
#define BRICK_AMQP_AMQPPRIV_H

extern "C"
{
#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_tcp_socket.h>
}

#include <string>
#include <ctime>

namespace brick
{
namespace amqp
{

inline std::string getTime()
{
    time_t timep;
    time (&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

}   // namespace amqp
}   // namespace brick


#endif // BRICK_AMQP_AMQPPRIV_H
