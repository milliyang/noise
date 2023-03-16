#include "noise/noise.h"
#include "noise/indicator/indicator.h"
#include "noise/strategy.h"

namespace noise {

namespace ta {

Indicator::Indicator(Strategy*stgy, std::string name)
{
    assert(stgy != nullptr);
    strategy_ = stgy;
    name_ = name;
    preload_done_ = false;
    stgy->register_indicator(this);
}

Indicator::Indicator(void)
{
    preload_done_ = false;
}

Indicator::~Indicator(void)
{

}

void Indicator::setup(std::shared_ptr<Data> &data)
{
    time_index_ = -1;
    data_ = data;
}

std::string Indicator::name(void)
{
    return name_;
}

bool Indicator::is_ready(void)
{
    float value = get(0);
    return !isnanf(value);
}

int Indicator::get_index(void)
{
    return time_index_;
}

void Indicator::increase(void)
{
    time_index_++;
}

}

}