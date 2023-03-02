#include "noise/noise.h"
#include "noise/indicator/indicator.h"
#include "noise/indicator/isys.h"
#include <cassert>

namespace noise {

namespace ta {

ISys::ISys(void)
{
    TRACE_LINE
}

ISys::~ISys(void)
{
    TRACE_LINE
}

void ISys::init(void)
{
    TRACE_LINE

    assert(data_.get() != nullptr);

    //data_->create_series("name"xxx);
    TRACE_LINE
}

void ISys::next(const struct noise::bar &bar)
{
    TRACE_LINE
}

void ISys::update_with_preload(void)
{
    TRACE_LINE
}

}

}