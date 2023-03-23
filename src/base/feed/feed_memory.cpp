#define LOG_TAG "feed.mem"

#include "noise/def.h"
#include "noise/feed/feed_memory.h"
#include "noise/util.h"
#include "noise/log.h"

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

void FeedMemory::set_bars(struct bars &bars)
{
    bars_ = std::move(bars);
    return;
}


}