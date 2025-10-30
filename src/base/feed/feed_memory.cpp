#define LOG_TAG "feed.mem"

#include "noise/def.h"
#include "noise/feed/feed_memory.h"
#include "noise/util.h"
#include "noise/log.h"

#include <cmath>

namespace noise {

FeedMemory::FeedMemory(void)
{
}

FeedMemory::~FeedMemory(void)
{
}

bool FeedMemory::is_support_preload(void)
{
#if 1
    //faster and data processing
    return true;
#else
    return false;
#endif
}

std::vector<struct bar>& FeedMemory::get_bars(void)
{
    return bars_.data;
}

void FeedMemory::init(const struct feed_config &cfg)
{
    config_ = cfg;
    index_ = 0;
    code_ = config_.code;
}

bool FeedMemory::next(struct bar &bar)
{
    //if (index_ >= 10) return false; //debug
    if (index_ >= bars_.data.size()) {
        return false;
    }
    bar = bars_.data.at(index_++);
    return true;
}

#define DECIBEL  (6.0205999132796f)

void FeedMemory::set_bars(struct bars &bars)
{
#if 0
    bars_ = std::move(bars);
#else
    bars_.code = bars.code;
    //dB = 20*LOG10(x)
    for (int i = 0; i < bars.data.size(); i++) {
        struct bar b = bars.data[i];
        b.open      = 20*std::log10(b.open)      / DECIBEL * 10;
        b.high      = 20*std::log10(b.high)      / DECIBEL * 10;
        b.low       = 20*std::log10(b.low)       / DECIBEL * 10;
        b.close     = 20*std::log10(b.close)     / DECIBEL * 10;
        b.pre_close = 20*std::log10(b.pre_close) / DECIBEL * 10;
        bars_.data.push_back(b);
    }
#endif
    return;
}


}