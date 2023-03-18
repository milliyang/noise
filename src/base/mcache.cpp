#define LOG_TAG "mcache"

#include "noise/def.h"
#include "noise/noise.h"
#include "noise/util.h"
#include "noise/mcache.h"

namespace noise {

namespace mcache {

using CodeMap = std::map<std::string, struct code_info>;
using CodeVec = std::vector<struct code_info>;

CodeMap *_get_code_map(void)
{
    static CodeMap *s_map = nullptr;
    if (s_map == nullptr) {
        s_map = new CodeMap();
    }
    return s_map;
}

CodeVec *_get_code_vec(void)
{
    static CodeVec *s_vec = nullptr;
    if (s_vec == nullptr) {
        s_vec = new CodeVec();
    }
    return s_vec;
}

void init(const char *path)
{
    CodeMap *map_ = _get_code_map();
    CodeVec *vec_ = _get_code_vec();

    // std::vector<struct code_info> infos;
    // util::parse_code_file(infos, ".config.ini");
    // util::parse_code_file(infos, ".code.csv");
    util::parse_code_file(*vec_, path);

    for(auto it = vec_->begin(); it != vec_->end(); it++) {
        map_->emplace(it->code, *it);
    }
}

bool has_code(std::string &code)
{
    CodeMap *map_ = _get_code_map();
    auto it = map_->find(code);
    if (it != map_->end()) {
        return true;
    } else {
        return false;
    }
    //return (it != map_->end()) ? true : false;
}

// void get_code_info(const std::string &code, struct code_info &info)
// {
//     CodeMap *map_ = _get_code_map();
//     auto it = map_->find(code);
//     if (it != map_->end()) {
//         info = it->second;
//     }
// }

struct code_info get_code_info(const std::string &code)
{
    CodeMap *map_ = _get_code_map();
    auto it = map_->find(code);
    if (it != map_->end()) {
        return it->second;
    } else {
        return {};
    }
}

void get_all_code_info(std::vector<code_info> &codes)
{
    CodeVec *vec_ = _get_code_vec();
    codes = *vec_;
    return;
}


} //namespace mcache

} //namespace noise