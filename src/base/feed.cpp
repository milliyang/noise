#include "noise/def.h"
#include "noise/feed.h"
#include "noise/util.h"

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
    util::parse_csv_bars_file(m_vec_bars, m_config.filename);
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
    m_config = cfg;
    m_idx = 0;

    //m_config.filename = "/home/leo/work/root/bars/000001.SZ.csv"
    m_config.filename = m_config.root_path + "/bars/" + m_config.code + ".csv";
    printf("filename:%s\n", m_config.filename.c_str());
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