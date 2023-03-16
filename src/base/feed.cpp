#include "noise/def.h"
#include "noise/feed.h"
#include "noise/util.h"
#include "noise/log.h"


namespace noise {

Feed::Feed(void)
{
}

Feed::~Feed(void)
{
}

FeedCSV::FeedCSV(void)
{
}

FeedCSV::~FeedCSV(void)
{
}

void FeedCSV::load_bars(void)
{
    std::vector<struct bar> tmp_bars;
    util::parse_csv_bars_file(tmp_bars, config_.filename);

    if (config_.begin_date.length() > 0) {
        int begin = std::stoi(config_.begin_date);
        for (auto it = tmp_bars.begin(); it != tmp_bars.end(); it++) {
            if (it->date >= begin) {
                bars_.push_back(*it);
            }
        }
    } else {
        bars_ = std::move(tmp_bars);
    }
    if (bars_.size() <= 0) {
        LOGW("no bars data match");
    }
}

bool FeedCSV::is_support_preload(void)
{
    //return true;
    return false;
}

std::vector<struct bar>& FeedCSV::get_bars(void)
{
    return bars_;
}

void FeedCSV::init(const struct feed_config &cfg)
{
    config_ = cfg;
    index_ = 0;

    //config_.filename = "/home/leo/work/root/bars/000001.SZ.csv"
    if (config_.filename.length() <= 0) {
        config_.filename = config_.root_path + "/bars/" + config_.code + ".csv";
    } else {
        config_.code = util::get_filename_without_extension(config_.filename);
    }
    code_ = config_.code;
    LOGI("read file: {}", config_.filename);
    load_bars();
}

bool FeedCSV::next(struct bar &bar)
{
    LOGD("{} {}", index_, bars_.size());
    //if (index_ >= 10) return false; //debug
    if (index_ >= bars_.size()) {
        return false;
    }
    bar = bars_.at(index_++);
    return true;
}

}