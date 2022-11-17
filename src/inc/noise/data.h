#pragma once

#include "noise/def.h"

#include<vector>

namespace noise
{
struct indicator;
class Broker;
class Feed;
using PtrIndicator = std::shared_ptr<struct indicator>;

struct plot_config {
    int show_on_finish;
    int save_png;
    int save_svg;
};


class Data {

public:
    Data(void);
    ~Data(void);

    void init(std::shared_ptr<Broker> broker, std::shared_ptr<Feed> feed, const struct plot_config &cofnig);

    std::shared_ptr<struct indicator> create_indicator(const std::string &name);
    std::shared_ptr<struct indicator> find_indicator(const std::string &name);
    bool is_support_preload(void);

    void setup_plot(FuncPlot func_plot) { m_func_plot = func_plot; };
    void plot(void);

protected:
    void update_default_indicator(struct bar &bars);

private:
    void init_default_indicators(std::vector<struct bar> &bars);

private:
    std::shared_ptr<Broker> m_broker;
    std::shared_ptr<Feed> m_feed;
    struct plot_config m_config;
    std::vector<PtrIndicator> m_indicators;
    FuncPlot m_func_plot;

    std::shared_ptr<struct timeseries> m_times;
};


} // namespace name

