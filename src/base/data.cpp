#include "noise/noise.h"

namespace noise {

Data::Data(void)
{
    TRACE_LINE

    time_index_ = -1;
    data_pre_processed_ = false;
    time_series_ = std::make_shared<struct timeseries>();
    info_ = std::make_shared<struct chart_info>();
}

Data::~Data(void)
{
    TRACE_LINE
}

PtrSeries Data::create_series(const std::string &name)
{
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

    //setup chart info
    info_->code = feed->config_.code;
    if (mcache::has_code(info_->code)) {
        info_->basic = mcache::get_code_info(info_->code);
    }
}

void Data::process_preload(void)
{
    //default series
    date_  = create_series(INDICATOR_DATE);
    open_  = create_series(INDICATOR_OPEN);
    high_  = create_series(INDICATOR_HIGH);
    low_   = create_series(INDICATOR_LOW);
    close_ = create_series(INDICATOR_CLOSE);
    vol_   = create_series(INDICATOR_VOLUME);
    chg_   = create_series(INDICATOR_CHANGES);
    pnl_   = create_series(INDICATOR_PNL);

    //user series/Indicator
    auto iter = indicator_map_.begin();
    while (iter != indicator_map_.end()) {
        PtrIndicator ptr = iter->second;
        ptr->init();
        iter++;
    }

    if (feed_->is_support_preload()) {
        auto bars = feed_->get_bars();
        process_preload_bars(bars);
        data_pre_processed_ = true;
    }
}

void Data::process_preload_bars(std::vector<struct bar> &bars)
{
    for (auto it = bars.begin(); it != bars.end(); it++) {
        const auto &bar = *it;
        time_series_->time.push_back(bar.time);

        date_->data.push_back((float)bar.time);
        open_->data.push_back(bar.open);
        high_->data.push_back(bar.high);
        low_->data.push_back(bar.low);
        close_->data.push_back(bar.close);
        vol_->data.push_back(bar.volumn);
        chg_->data.push_back(bar.pct_chg);

        //LOGD("--xohlc:%f %f %f %f\n", bar.open, bar.high, bar.low, bar.close);
    }
    LOGD("bar_len:{}", (int)date_->data.size());

    //use series/Indicator
    auto iter = indicator_map_.begin();
    while (iter != indicator_map_.end()) {
        PtrIndicator ptr = iter->second;
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
void Data::update_data_series(struct bar &bar)
{
    time_index_++;

    //use series/Indicator
    {
        auto iter = indicator_map_.begin();
        while (iter != indicator_map_.end()) {
            PtrIndicator indi = iter->second;
            indi->increase();
            iter++;
        }
    }

    if (data_pre_processed_) {
        return;
    }

    // update data series on bar, one by one
    time_series_->time.push_back(bar.time);
    date_->data.push_back((float)bar.time);
    open_->data.push_back(bar.open);
    high_->data.push_back(bar.high);
    low_->data.push_back(bar.low);
    close_->data.push_back(bar.close);
    vol_->data.push_back(bar.volumn);
    chg_->data.push_back(bar.pct_chg);

    // user series
    {
        auto iter = indicator_map_.begin();
        while (iter != indicator_map_.end()) {
            PtrIndicator ptr = iter->second;
            ptr->next(bar);
            iter++;
        }
    }
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
    if (plot_func_) {
        auto ploter = plot_func_();

        //TODO: use map
        std::vector<PtrSeries> ind;
        for (auto const it : series_map_) {
            it.second->extra.info = info_;
            ind.push_back(it.second);
        }

        //user
        // for (auto const it : indicator_map_) {
        //     ind.push_back(it.second->);
        // }
        ploter->plot("default", ind);
    }
}

int Data::debug_peek_bars_size(void)
{
    auto bars = feed_->get_bars();
    return (int) bars.size();
}
}