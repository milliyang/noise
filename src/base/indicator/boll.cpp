#include "noise/noise.h"
#include "noise/indicator/boll.h"
#include "noise/log.h"
#include <cassert>

namespace noise {

namespace ta {

Boll::Boll(Strategy *stgy, std::string name, int period, std::string ref)
    :Indicator(stgy, name)
{
    period_ = period;
    ref_name_ = ref;
}

Boll::Boll(std::string name, PtrSeries &series, int period)
{
    name_ = name;
    period_ = period;
    series_ref_ = series;
}

Boll::~Boll(void)
{
    TRACE_LINE
}

void Boll::init(void)
{
    assert(data_.get() != nullptr);

    std::string name;
    if (name_.length() <= 0) {
        name = fmt::format("ma{}", period_);
    } else {
        name = name_;
    }
    series_high_ = data_->create_series("boll_high");
    series_mid_  = data_->create_series("boll_mid ");
    series_low_  = data_->create_series("boll_low ");
    series_ref_  = data_->find_series(ref_name_);
    //LOGI("{} {}", __FUNCTION__, name_);
}

void Boll::next(const struct noise::bar &bar)
{
    LOGW("TODO:");
}

float Boll::get_by_index(int idx)
{
    // assert(data_.get() != nullptr);
    // if (idx < 0 || idx >= series_->data.size() ) {
    //     LOGW("invalid index{}", idx);
    //     return NAN;
    // }
    // return series_->data[idx];
    return 0;
}

float Boll::get(int pos)
{
    return get_by_index(time_index_ + pos);
}

void Boll::update_with_preload(void)
{
    assert(series_high_.get() != nullptr);
    assert(series_mid_.get() != nullptr);
    assert(series_low_.get() != nullptr);
    assert(series_ref_.get() != nullptr);
    //LOGI("{} {}", __FUNCTION__, name_);
    noise::utalib::boll(series_high_->data, series_mid_->data, series_low_->data, series_ref_->data, period_, 2.0f);

}

}

}