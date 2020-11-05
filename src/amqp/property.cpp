#include "amqppriv.h"
#include "amqp/property.h"


namespace brick
{
namespace amqp
{

BasicProperites::BasicProperites(amqp_basic_properties_t_ *property)
{
    pImp_properties = std::shared_ptr<amqp_basic_properties_t_>(property);
}

const amqp_basic_properties_t_ &BasicProperites::to_amqp_basic_properties_t() const
{
    return *pImp_properties;
}

const amqp_basic_properties_t_ *BasicProperites::amqp_ptr() const
{
    return pImp_properties.get();
}

void BasicProperites::destroyBasicProperties(amqp_basic_properties_t_ *property)
{
    delete property;
}


}   // namespace amqp
}   // namespace brick
