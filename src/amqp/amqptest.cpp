#include <string>
#include <iostream>

#include "amqp/connection.h"
#include "amqp/amqpexception.h"
#include "amqp/channel.h"
#include "amqp/queue.h"
#include "amqp/exchange.h"
#include "amqppriv.h"

#include <cstring>

using namespace brick::amqp;

void test_amqp_bytes()
{
    amqp_bytes_t src;
    const char *h = "hello world";
    src.len = strlen(h);
    src.bytes = const_cast<char*>(h);
    AmqpBytes t = AmqpBytes::takeOver(src);
    std::string str = t.toString();
}

void test_connection_creation()
{
    std::cout<<"create connection"<<std::endl;
    Connection conn("localhost", 5672, "sabo", "sabo");

    std::string str;
    std::cout<<"connected. press any key to disconnect...";
    std::cin>>str;

}

void test_channel_creation()
{
    std::cout<<"create channel"<<std::endl;
    Connection conn("localhost", 5672, "sabo", "sabo");

    Channel channel(conn);

    std::string str;
    std::cout<<"opened the channel. press any key to close and disconnect...";
    std::cin >> str;

}

void test_multiple_channel_creation()
{
    std::cout<<"create multiple channels"<<std::endl;
    Connection conn("localhost", 5672, "sabo", "sabo");
    Channel c1(conn);


    Connection conn2("localhost", 5672, "sabo", "sabo");
    Channel c5(conn2);
    Channel c7(conn2,6);

    std::string str;
    std::cout<<"opened the channel. press any key to close and disconnect...";
    std::cin >> str;
}

void test_queue_declare()
{
    std::cout<<"declare queues"<<std::endl;
    Connection conn("localhost", 5672, "sabo", "sabo");
    Channel c1(conn);

    c1.queueDeclare("test queue del", 0, 0, 1, 1);
    c1.queueDeclare("test queue no del", 0,0,0,0);
    c1.queueDeclare("", 0, 0, 1, 0);

    std::string str;
    std::cout<<"declared. press any key to close and disconnect...";
    std::cin >> str;

}

void test_exchange_declare()
{
    std::cout<<"declare exchange"<<std::endl;
    Connection conn("localhost", 5672, "sabo", "sabo");
    Channel c1(conn);

    c1.exchangeDeclare("test exchange", "direct", 0, 0, 0, 0);
    std::string str;
    std::cout<<"declared. press any key to close and disconnect...";
    std::cin >> str;

}

void test_bind()
{
    std::cout<<"bind exchange and queue:"<<std::endl;
    Connection conn("localhost", 5672, "sabo", "sabo");
    Channel c1(conn);

    Queue q = c1.queueDeclare("", 0,0,1,1);
    std::cout<<"queue name: "<<q.getQueueName() << "; exchange name: " << "test exchange;" <<std::endl;
    Exchange x = c1.exchangeDeclare("test exchange", "direct");

    std::string str;
    std::cout<<"declared. press any key to bind...";
    std::cin>>str;

    c1.bind(q.getQueueName(), x.getExchangeName(), "test binding");


    std::cout<<"binded. press any key to close and disconnect...";
    std::cin >> str;

}

void test_unbind()
{
    std::cout<<"bind exchange and queue:"<<std::endl;
    Connection conn("localhost", 5672, "sabo", "sabo");
    Channel c1(conn);
    Queue q = c1.queueDeclare("", 0,0,1,1);
    std::cout<<"queue name: "<<q.getQueueName() << "; exchange name: " << "test exchange;" <<std::endl;
    Exchange x = c1.exchangeDeclare("test exchange", "direct");
    c1.bind(q.getQueueName(), x.getExchangeName(), "test binding");

    std::string str;
    std::cout<<"binded. press any key to unbind";
    std::cin>>str;

    c1.unbind(q.getQueueName(), x.getExchangeName(), "test binding");
    std::cout<<"unbinded. press any key to continue...";
    std::cin>>str;
}

void test_publish()
{
    std::cout<<"publishing ..."<<std::endl;
    std::string str;
    Connection conn("localhost", 5672, "sabo", "sabo");
    Channel c1 (conn);
    Exchange x = c1.exchangeDeclare("test exchange", "direct");
    std::cout<<"declared exchange \"test exchange\" ..."<<std::endl;
    amqp_basic_properties_t *props = new amqp_basic_properties_t;
    props->_flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
    props->content_type = amqp_cstring_bytes("text/plain");
    props->delivery_mode = 2; /* persistent delivery mode */
    BasicProperites property{props};

    c1.basic_publish("test exchange", "test binding", "hello rabbitmq", 0, 0, property);

}

void test_consume()
{
    std::cout<<"consuming ..." <<std::endl;
    std::string str;

    Connection conn("localhost", 5672, "sabo", "sabo");
    Channel c1(conn);


    Queue q = c1.queueDeclare("",0,0,1,1);
    Exchange x = c1.exchangeDeclare("test exchange", "direct");
    c1.bind(q.getQueueName(), x.getExchangeName(), "test binding");

    std::cout<<"bind exchange " + x.getExchangeName() + " and queue " + q.getQueueName() << std::endl;

//    c1->basic_publish("test exchange", "test binding");
    std::cout<<"basic consume ..." << std::endl;
    c1.basic_consume(q.getQueueName());

    std::cout<<"consuming message ..."<<std::endl;
    str = c1.consume_message();

    std::cout<<"message: "<<str<<std::endl;

    std::cout<<"received. press any key to exit...";
    std::cin>>str;
}

//void test_publish_queue()
//{
//    std::cout<<"publishing ..."<<std::endl;
//    std::string str;
//    anti::amqp::Connection conn("localhost", 5672, "guest", "guest");
//    auto c1 = conn.createChannel();
//    anti::amqp::Queue q = c1->queueDeclare("test queue", 0,0,0,1);
//    c1->basic_publish("", "", "hello rabbitmq");
//    std::cout<<"end publishing. Press any key to continue..." << std::endl;
//    std::cin>>str;
//}

//void test_listen_queue()
//{
//    std::cout<<"listening ..."<<std::endl;
//    std::string str;
//    anti::amqp::Connection conn("localhost", 5672, "guest", "guest");
//    auto c1 = conn.createChannel();

//    anti::amqp::Queue q = c1->queueDeclare("test queue", 0,0,0,1);
//    c1->basic_consume(q.getQueueName());
//    str = c1->consume_message();
//    std::cout<<"received message: " << str << std::endl;
//    std::cout<<"press any key to exit ...";
//    std::cin >> str;
//}

int main(int argc, char **argv)
{
//    test_connection_creation();
//    test_channel_creation();
//    test_multiple_channel_creation();
//    test_queue_declare();
//    test_exchange_declare();
//    test_bind();
//    test_unbind();

//    test_consume();

    /***test case 7***/
    if(strcmp(argv[1],"send")==0)
    {
        test_publish();
    }
    else if(strcmp(argv[1],"listen")==0)
    {
        test_consume();
    }
    else
    {
        std::cout<<"false option"<<std::endl;
    }
    /*****end test case 7****/

    /***test case 8***/
//    if(strcmp(argv[1],"send")==0)
//    {
//        test_publish_queue();
//    }
//    else if(strcmp(argv[1],"listen")==0)
//    {
//        test_listen_queue();
//    }
//    else
//    {
//        std::cout<<"false option"<<std::endl;
//    }
    /*****end test case 8****/

//    std::cout<<"hello world"<<std::endl;
}
