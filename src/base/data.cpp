#include "noise/noise.h"

namespace noise {

Data::Data(void)
{
    TRACE_LINE

    m_times = std::make_shared<struct timeseries>();
}

Data::~Data(void)
{
    TRACE_LINE
}

PtrIndicator Data::create_indicator(const std::string &name)
{
    TRACE_LINE
    auto indi = std::make_shared<struct indicator>();
    indi->name = name;
    indi->figure = FIGURE_DEFAULT;
    indi->color = AUTO;
    indi->gamma = "linear";
    indi->shown_on_mouse = 1;
    indi->extra.ptime = m_times;
    m_indicators.push_back(indi);
    return indi;
}

void Data::init(std::shared_ptr<Broker> broker, std::shared_ptr<Feed> feed, const struct plot_config &config)
{
    m_broker = broker;
    m_feed = feed;
    m_config = config;

    //do preload
    if (feed->is_support_preload()) {
        auto bars = feed->get_bars();
        init_default_indicators(bars);
    }
}

void Data::init_default_indicators(std::vector<struct bar> &bars)
{
    auto date   = create_indicator(INDICATOR_DATE);
    auto open   = create_indicator(INDICATOR_OPEN);
    auto high   = create_indicator(INDICATOR_HIGH);
    auto low    = create_indicator(INDICATOR_LOW);
    auto close  = create_indicator(INDICATOR_CLOSE);
    auto v      = create_indicator(INDICATOR_VOLUME);
    auto chg    = create_indicator(INDICATOR_CHANGES);
    auto pnl    = create_indicator(INDICATOR_PNL);

    for (auto it = bars.begin(); it != bars.end(); it++) {
        const auto &bar = *it;
        m_times->time.push_back(bar.time);

        date->data.push_back(bar.time);
        open->data.push_back(bar.open);
        high->data.push_back(bar.high);
        low->data.push_back(bar.low);
        close->data.push_back(bar.close);
        v->data.push_back(bar.volumn);
        chg->data.push_back(bar.pct_chg);

        LOGD("--xohlc:%f %f %f %f\n", bar.open, bar.high, bar.low, bar.close);
    }
    LOGD("bar_len:%ld\n", date->data.size());
}

PtrIndicator Data::find_indicator(const std::string &name)
{
    for (int i = 0; i < m_indicators.size(); i++) {
        if (m_indicators.at(i)->name == name) {
            return m_indicators.at(i);
        }
    }
    return nullptr;
}

bool Data::is_support_preload(void)
{
    return m_feed->is_support_preload();
}

void Data::plot(void)
{
    TRACE_LINE

    if (m_func_plot) {
        auto ploter = m_func_plot();
        ploter->plot("default", m_indicators);
    }
}


}