#pragma once

#include "noise/def.h"
#include "noise/log.h"
#include "noise/indicator/indicator.h"

#include<vector>
#include<map>

namespace noise
{
struct series;
class Broker;
class Feed;
class Stat;

using PtrSeries = std::shared_ptr<struct series>;
using MapSeries = std::map<std::string, PtrSeries>;

using PtrIndicator = ta::Indicator*;
using MapIndicator = std::map<std::string, PtrIndicator>;

struct plot_config {
    int save_png;
    int save_svg;
};

class Data {

public:
    Data(void);
    ~Data(void);

    PtrSeries create_series(const std::string &name);
    PtrSeries find_series(const std::string &name);

    void push_indicator(const std::string &name, PtrIndicator indicator);
    PtrIndicator find_indicator(const std::string &name);

    bool is_support_preload(void);
    void plot(void);

    int  debug_peek_bars_size(void);

protected:
    //framework used only:
    void setup(std::shared_ptr<Broker> broker, std::shared_ptr<Feed> feed, const struct plot_config &cofnig);
    void setup_plot(FuncPlot func_plot) { plot_func_ = func_plot; };
    void process_preload(void);
    void update_data_series(struct bar &bars);

private:
    void process_preload_bars(std::vector<struct bar> &bars);

private:
    std::shared_ptr<Broker> broker_;
    std::shared_ptr<Feed>   feed_;
    std::shared_ptr<struct chart_info>   info_;
    struct plot_config      config_;
    MapSeries               series_map_;
    MapIndicator            indicator_map_;
    FuncPlot                plot_func_;
    std::shared_ptr<struct timeseries> time_series_;
    int                     time_index_;                //increased 1 on bar
    bool                    data_pre_processed_;

    PtrSeries date_;
    PtrSeries open_;
    PtrSeries high_;
    PtrSeries low_;
    PtrSeries close_;
    PtrSeries vol_;
    PtrSeries chg_;
    PtrSeries pnl_;

    friend class Backtest;
    friend class Stat;
};


} // namespace name

