#pragma once

#include "noise/def.h"
#include "noise/feed/feed.h"
#include "string.h"

namespace noise
{

struct bars;
struct feed_config;
class Feed;

class FeedMemory : public Feed {

public:
    FeedMemory(void);
    ~FeedMemory();

    void init(const struct feed_config &cfg) override;
    bool next(struct bar &bar) override;
    bool is_support_preload(void) override;
    std::vector<struct bar>& get_bars(void) override;

public:
    void set_bars(struct bars &bars);

private:
    uint32_t index_;
    struct bars bars_;
    std::string code_;
};

} // namespace name

