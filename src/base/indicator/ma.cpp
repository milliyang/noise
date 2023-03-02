#include "noise/noise.h"
#include "noise/indicator/ma.h"
#include "noise/log.h"
#include <cassert>

namespace noise {

namespace ta {

Ma::Ma(Strategy *stgy, std::string name, int period, std::string ref)
    :Indicator(stgy, name)
{
    period_ = period;
    ref_name_ = ref;
}

Ma::Ma(std::string name, PtrSeries &series, int period)
{
    name_ = name;
    period_ = period;
    series_ref_ = series;
}

Ma::~Ma(void)
{
    TRACE_LINE
}

void Ma::init(void)
{
    assert(data_.get() != nullptr);

    std::string name;
    if (name_.length() <= 0) {
        name = fmt::format("ma{}", period_);
    } else {
        name = name_;
    }
    series_     = data_->create_series(name);
    series_ref_ = data_->find_series(ref_name_);
}

void Ma::next(const struct noise::bar &bar)
{
    LOGW("TODO:");
}

float Ma::get_by_index(int idx)
{
    assert(data_.get() != nullptr);
    if (idx < 0 || idx >= series_->data.size() ) {
        LOGW("invalid index:{}", idx);
        return NAN;
    }
    return series_->data[idx];
}

float Ma::get(int pos)
{
    return get_by_index(time_index_ + pos);
}

void Ma::update_with_preload(void)
{
    assert(series_.get() != nullptr);
    assert(series_ref_.get() != nullptr);
    noise::utalib::ma(series_->data, series_ref_->data, period_);
}


}

}