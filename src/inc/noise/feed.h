#pragma once

#include "noise/def.h"

namespace noise
{

struct bar;

struct feed_config {
    std::string code;
    std::string root_path;   //path to backtest root dir 
    std::string filename;
};

class Feed {

public:
    Feed(void);
    ~Feed(void);

    virtual void init(const struct feed_config &cfg) = 0;
    virtual bool next(struct bar &bar) = 0;
    virtual bool is_support_preload(void) = 0;
    virtual std::vector<struct bar>& get_bars(void) = 0;

    struct feed_config m_config;
};


class FeedCSV : public Feed {

public:
    FeedCSV(void);
    ~FeedCSV(void);

    void init(const struct feed_config &cfg) override;
    bool next(struct bar &bar) override;
    bool is_support_preload(void);
    std::vector<struct bar>& get_bars(void) override;

private:
    void load_bars(void);

    uint32_t m_idx;
    std::vector<struct bar> m_vec_bars;
};

} // namespace name

