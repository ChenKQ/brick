#ifndef BRICK_AMQP_PROPERTY_H
#define BRICK_AMQP_PROPERTY_H

#include <memory>

struct amqp_basic_properties_t_;

namespace brick
{
namespace amqp
{

class BasicProperites
{
public:
    explicit BasicProperites(amqp_basic_properties_t_ *property = nullptr);

    const amqp_basic_properties_t_& to_amqp_basic_properties_t() const;
    const amqp_basic_properties_t_* amqp_ptr() const;

private:
    std::shared_ptr<amqp_basic_properties_t_> pImp_properties = nullptr;

    void destroyBasicProperties(amqp_basic_properties_t_ *property);
};

}   // namespace amqp
}   // namespace brick

#endif // BRICK_AMQP_PROPERTY_H
