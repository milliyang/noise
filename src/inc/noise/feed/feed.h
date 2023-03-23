#pragma once

#include "noise/def.h"
#include "string.h"

namespace noise
{

struct bar;

struct feed_config {
    std::string code;
    std::string root_path;      //path to backtest root dir 
    std::string filename;
    std::string begin_date;     //start bt from begin_date
};

class Feed {

public:
    Feed(void) {};
    ~Feed() {};

    virtual void init(const struct feed_config &cfg) = 0;
    virtual bool next(struct bar &bar) = 0;
    virtual bool is_support_preload(void) = 0;
    virtual std::vector<struct bar>& get_bars(void) = 0;

    struct feed_config config_;
};

using FeedPtr = std::shared_ptr<Feed>;

} // namespace name

