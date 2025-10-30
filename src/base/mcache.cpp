#define LOG_TAG "mcache"

#include "noise/filesystem/fs.h"
#include "glob.h"

#include "noise/def.h"
#include "noise/noise.h"
#include "noise/util.h"
#include "noise/mcache.h"

#include "noise/feed/feed_csv.h"
#include "noise/feed/feed_memory.h"

namespace noise {

namespace mcache {

using CodeMap = std::map<std::string, struct code_info>;
using CodeVec = std::vector<struct code_info>;

struct context {
    bool     inited;
    CodeMap *code_map;
    CodeVec *code_vec;
    fs::H5  *h5;
};

static bool    s_inited = false;
static context s_ctx;

void init(const std::string &filepath)
{
    memset(&s_ctx, 0, sizeof(s_ctx));
    s_ctx.code_map = new CodeMap();
    s_ctx.code_vec =  new CodeVec();

    if (util::is_end_with(filepath, ".h5")) {
        gfs::path h5_path{filepath};
        if (gfs::exists(h5_path)) {
            LOGI("using dataset:{}", filepath);
            s_ctx.h5 = new fs::H5(filepath);
            auto basic = s_ctx.h5->read_basic();
            auto infos = fs::fs_get_basic(basic);
            for (auto it = infos.begin(); it != infos.end(); it++) {
                s_ctx.code_map->emplace(it->code, *it);
                s_ctx.code_vec->push_back(*it);
            }
        }
    } else {
        // std::vector<struct code_info> infos;
        // util::parse_code_file(infos, ".config.ini");
        // util::parse_code_file(infos, ".code.csv");
        util::parse_code_file(*s_ctx.code_vec, filepath);
        for(auto it = s_ctx.code_vec->begin(); it != s_ctx.code_vec->end(); it++) {
            s_ctx.code_map->emplace(it->code, *it);
        }
    }
    s_inited = true;
}

bool has_code(const std::string &code)
{
    CodeMap* map_ = s_ctx.code_map;
    auto it = map_->find(code);
    if (it != map_->end()) {
        return true;
    } else {
        return false;
    }
    //return (it != map_->end()) ? true : false;
}

struct code_info get_code_info(const std::string &code)
{
    CodeMap *map_ = s_ctx.code_map;
    auto it = map_->find(code);
    if (it != map_->end()) {
        return it->second;
    } else {
        return {};
    }
}

void get_all_code_info(std::vector<code_info> &codes)
{
    CodeVec* vec_ = s_ctx.code_vec;
    codes = *vec_;
    return;
}

struct bars get_bars(const std::string &code)
{
    if (s_ctx.h5) {
        if (has_code(code)) {
            auto h5bar = s_ctx.h5->read_bars(code);
            auto bars = fs::fs_get_bars(h5bar);
            return std::move(bars);
        }
    }

    //not found
    struct bars bars;
    return std::move(bars);
}

FeedPtr get_feed(const struct feed_config &cfg)
{
    if (s_ctx.h5) {
        auto feed = std::make_shared<FeedMemory>();
        feed->init(cfg);

        if (has_code(cfg.code)) {
            auto h5bar = s_ctx.h5->read_bars(cfg.code);
            auto bars = fs::fs_get_bars(h5bar);
            feed->set_bars(bars);
        } else {
            LOGW("h5 not code {} data", cfg.code);
        }
        return feed;
    } else {
        auto feed = std::make_shared<FeedCSV>();
        feed->init(cfg);
        return feed;
    }
}


} //namespace mcache

} //namespace noise