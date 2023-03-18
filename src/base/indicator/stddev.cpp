 #include "noise/noise.h"
#include "noise/indicator/stddev.h"
#include "noise/log.h"
#include <cassert>

namespace noise {

namespace ta {

Stddev::Stddev(Strategy *stgy, std::string name, int period, bool divided_by_unit, std::string ref)
    :Indicator(stgy, name)
{
    period_ = period;
    ref_name_ = ref;
    divided_by_unit_ = divided_by_unit;
}

Stddev::Stddev(std::string name, PtrSeries &series, int period, bool divided_by_unit)
{
    name_ = name;
    period_ = period;
    series_ref_ = series;
    divided_by_unit_ = divided_by_unit;
}

Stddev::~Stddev(void)
{
    TRACE_LINE
}

void Stddev::init(void)
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

    //Plot on another figure
    series_->figure = "stddev";

    //LOGI("{} {}", __FUNCTION__, name_);
}

void Stddev::next(const struct noise::bar &bar)
{
    if (preload_done_) {
        return;
    }

    assert(series_.get() != nullptr);
    assert(series_ref_.get() != nullptr);
    noise::uta::stddev(series_->data, series_ref_->data, period_, divided_by_unit_);
}

float Stddev::get_by_index(int idx)
{
    assert(data_.get() != nullptr);
    if (idx < 0 || idx >= series_->data.size() ) {
        LOGW("invalid index:{}", idx);
        return NAN;
    }
    return series_->data[idx];
}

float Stddev::get(int pos)
{
    return get_by_index(time_index_ + pos);
}

void Stddev::update_with_preload(void)
{
    assert(data_.get() != nullptr);
    assert(series_.get() != nullptr);
    assert(series_ref_.get() != nullptr);
    //LOGI("{} {}", __FUNCTION__, name_);
    noise::uta::stddev(series_->data, series_ref_->data, period_, divided_by_unit_);
}

}

}