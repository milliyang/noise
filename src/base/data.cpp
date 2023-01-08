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

    m_indicators.emplace(name, indi);
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
    auto vol    = create_indicator(INDICATOR_VOLUME);
    auto chg    = create_indicator(INDICATOR_CHANGES);
    auto pnl    = create_indicator(INDICATOR_PNL);

    for (auto it = bars.begin(); it != bars.end(); it++) {
        const auto &bar = *it;
        m_times->time.push_back(bar.time);

        date->data.push_back((float)bar.time);
        open->data.push_back(bar.open);
        high->data.push_back(bar.high);
        low->data.push_back(bar.low);
        close->data.push_back(bar.close);
        vol->data.push_back(bar.volumn);
        chg->data.push_back(bar.pct_chg);

        //LOGD("--xohlc:%f %f %f %f\n", bar.open, bar.high, bar.low, bar.close);
    }
    LOGD("bar_len:%d\n", (int)date->data.size());
}

PtrIndicator Data::find_indicator(const std::string &name)
{
    auto it = m_indicators.find(name);
    if (it != m_indicators.end()) {
        return it->second;
    } else {
        return nullptr;
    }
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

        //TODO: use map
        std::vector<PtrIndicator> ind;
        for (auto const it : m_indicators) {
            ind.push_back(it.second);
        }
        ploter->plot("default", ind);
    }
}

}