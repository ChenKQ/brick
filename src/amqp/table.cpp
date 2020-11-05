#include "amqppriv.h"
#include "amqp/table.h"

namespace brick
{
namespace amqp
{

Table::Table(amqp_table_t *pTable)
{
    pImp_table = std::shared_ptr<amqp_table_t>(pTable);
}

void Table::reset(amqp_table_t *pTable)
{
    pImp_table.reset(pTable);
}

const amqp_table_t &Table::to_amqp_table_t() const
{
    if(pImp_table == nullptr)
    {
        return amqp_empty_table;
    }
    return *pImp_table;
}

void Table::destroyTable(amqp_table_t *table)
{
    delete table;
}



}   // namespace amqp
}   // namespace brick
