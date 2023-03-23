#pragma once

#include "noise/def.h"
#include "noise/feed/feed.h"


namespace noise {

class Feed;

namespace mcache {


/**
 * @brief: load all code into memory cache
 * @path:  filepath
 */
void init(const char *path = ".code.csv");


bool has_code(const std::string &code);
struct code_info get_code_info(const std::string &code);
void get_all_code_info(std::vector<code_info> &codes);


FeedPtr get_feed(const struct feed_config &cfg);



} // namespace mcache
} // namespace name

