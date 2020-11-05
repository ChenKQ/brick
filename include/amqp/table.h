#ifndef BRICK_AMQP_TABLE_H
#define BRICK_AMQP_TABLE_H

#include <memory>

typedef struct amqp_table_t_ amqp_table_t;

namespace brick
{
namespace amqp
{

class Table
{
public:
    explicit Table(amqp_table_t* pTable = nullptr);

    void reset(amqp_table_t* pTable);
    const amqp_table_t& to_amqp_table_t() const;

private:
    std::shared_ptr<amqp_table_t> pImp_table = nullptr;
    static void destroyTable(amqp_table_t* table);
};

}   // namespace amqp
}   // namespace brick

#endif // BRICK_AMQP_TABLE_H
