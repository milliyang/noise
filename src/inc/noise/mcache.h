#pragma once

#include "noise/def.h"
#include "noise/feed/feed.h"


namespace noise {

class Feed;

namespace mcache {


/**
 * @brief: load all code into memory cache
 * @path:  filepath
 *          - ".code.csv"
 *          - ".chs.code.h5"
 */
void init(const std::string &filepath);
bool has_code(const std::string &code);

void get_all_code_info(std::vector<code_info> &codes);

FeedPtr          get_feed(const struct feed_config &cfg);
struct code_info get_code_info(const std::string &code);
struct bars      get_bars(const std::string &code);


} // namespace mcache
} // namespace name

