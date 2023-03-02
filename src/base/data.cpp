#include "noise/noise.h"

namespace noise {

Data::Data(void)
{
    TRACE_LINE

    time_index_ = -1;
    data_processed_ = false;
    time_series_ = std::make_shared<struct timeseries>();
}

Data::~Data(void)
{
    TRACE_LINE
}

PtrSeries Data::create_series(const std::string &name)
{
    TRACE_LINE
    auto indi = std::make_shared<struct series>();
    indi->name = name;
    indi->figure = FIGURE_DEFAULT;
    indi->color = AUTO;
    indi->gamma = "linear";
    indi->shown_on_mouse = 1;
    indi->extra.ptime = time_series_;

    series_map_.emplace(name, indi);
    return indi;
}

void Data::setup(std::shared_ptr<Broker> broker, std::shared_ptr<Feed> feed, const struct plot_config &config)
{
    broker_ = broker;
    feed_ = feed;
    config_ = config;
}

void Data::process_preload(void)
{
    if (feed_->is_support_preload()) {
        auto bars = feed_->get_bars();
        process_preload(bars);
        data_processed_ = true;
    }
}

void Data::process_preload(std::vector<struct bar> &bars)
{
    //default series
    auto date   = create_series(INDICATOR_DATE);
    auto open   = create_series(INDICATOR_OPEN);
    auto high   = create_series(INDICATOR_HIGH);
    auto low    = create_series(INDICATOR_LOW);
    auto close  = create_series(INDICATOR_CLOSE);
    auto vol    = create_series(INDICATOR_VOLUME);
    auto chg    = create_series(INDICATOR_CHANGES);
    auto pnl    = create_series(INDICATOR_PNL);

    for (auto it = bars.begin(); it != bars.end(); it++) {
        const auto &bar = *it;
        time_series_->time.push_back(bar.time);

        date->data.push_back((float)bar.time);
        open->data.push_back(bar.open);
        high->data.push_back(bar.high);
        low->data.push_back(bar.low);
        close->data.push_back(bar.close);
        vol->data.push_back(bar.volumn);
        chg->data.push_back(bar.pct_chg);

        //LOGD("--xohlc:%f %f %f %f\n", bar.open, bar.high, bar.low, bar.close);
    }
    LOGD("bar_len:{}", (int)date->data.size());

    //use series/Indicator
    auto iter = indicator_map_.begin();
    while (iter != indicator_map_.end()) {
        PtrIndicator ptr = iter->second;
        ptr->init();
        ptr->update_with_preload();
        iter++;
    }
}

/**
 * update series/Indicator one by one
 * 0. increase date index
 * 1. update default series
 * 2. update user Indicator series
 */
void Data::update_data_series(struct bar &bars)
{
    time_index_++;

    //use series/Indicator
    auto iter = indicator_map_.begin();
    while (iter != indicator_map_.end()) {
        PtrIndicator indi = iter->second;
        indi->increase();
        iter++;
    }

    if (data_processed_) {
        return;
    }

    // Todo: update data series on bar, one by one
    return;
}

PtrSeries Data::find_series(const std::string &name)
{
    auto it = series_map_.find(name);
    if (it != series_map_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void Data::push_indicator(const std::string &name, PtrIndicator indi)
{
    indicator_map_.emplace(name, indi);
}

PtrIndicator Data::find_indicator(const std::string &name)
{
    auto it = indicator_map_.find(name);
    if (it != indicator_map_.end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

bool Data::is_support_preload(void)
{
    return feed_->is_support_preload();
}

void Data::plot(void)
{
    TRACE_LINE

    if (m_func_plot) {
        auto ploter = m_func_plot();

        //TODO: use map
        std::vector<PtrSeries> ind;
        for (auto const it : series_map_) {
            ind.push_back(it.second);
        }

        //user
        // for (auto const it : indicator_map_) {
        //     ind.push_back(it.second->);
        // }
        ploter->plot("default", ind);
    }
}

}