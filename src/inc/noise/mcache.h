#pragma once

#include "noise/def.h"

namespace noise {

namespace mcache {


/**
 * @brief: load all code into memory cache
 * @path:  filepath
 */
void init(const char *path = ".code.csv");


bool has_code(std::string &code);
struct code_info get_code_info(const std::string &code);
void get_all_code_info(std::vector<code_info> &codes);


} // namespace mcache
} // namespace name

