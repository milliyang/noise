#pragma once

#include "noise/def.h"
#include "noise/feed/feed.h"
#include <string>

namespace noise
{

class FeedCSV : public Feed {

public:
    FeedCSV(void) {};
    ~FeedCSV() {};

    void init(const struct feed_config &cfg) override;
    bool next(struct bar &bar) override;
    bool is_support_preload(void);
    std::vector<struct bar>& get_bars(void) override;

private:
    void load_bars(void);

    uint32_t index_;
    std::vector<struct bar> bars_;
    std::string code_;
};

} // namespace name

