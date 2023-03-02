#include "noise/def.h"
#include "noise/feed.h"
#include "noise/util.h"
#include "noise/log.h"


namespace noise {

#ifdef TRACE_LINE
#undef TRACE_LINE
#define TRACE_LINE
#endif

Feed::Feed(void)
{
    TRACE_LINE
}

Feed::~Feed(void)
{
    TRACE_LINE
}

FeedCSV::FeedCSV(void)
{
    TRACE_LINE
}

FeedCSV::~FeedCSV(void)
{
    TRACE_LINE
}

void FeedCSV::load_bars(void)
{
    TRACE_LINE

    std::vector<struct bar> vec_bars;
    util::parse_csv_bars_file(vec_bars, config_.filename);

    if (config_.begin_date.length() > 0) {
        int begin = std::stoi(config_.begin_date);
        for (auto it = vec_bars.begin(); it != vec_bars.end(); it++) {
            if (it->date >= begin) {
                m_vec_bars.push_back(*it);
            }
        }
    } else {
        m_vec_bars = std::move(vec_bars);
    }
    if (m_vec_bars.size() <= 0) {
        LOGW("no bars data match");
    }
}

bool FeedCSV::is_support_preload(void)
{
    return true;
}

std::vector<struct bar>& FeedCSV::get_bars(void)
{
    return m_vec_bars;
}

void FeedCSV::init(const struct feed_config &cfg)
{
    TRACE_LINE
    config_ = cfg;
    m_idx = 0;

    //config_.filename = "/home/leo/work/root/bars/000001.SZ.csv"
    if (config_.filename.length() <= 0) {
        config_.filename = config_.root_path + "/bars/" + config_.code + ".csv";
    }
    LOGI("read file: {}", config_.filename);
    load_bars();
}

bool FeedCSV::next(struct bar &bar)
{
    TRACE_LINE

    //if (m_idx >= 10) return false; //debug

    if (m_idx >= m_vec_bars.size()) {
        return false;
    }
    bar = m_vec_bars.at(m_idx++);
    return true;
}

}