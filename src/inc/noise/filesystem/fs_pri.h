#pragma once
#include <highfive/H5File.hpp>

#include "noise/def.h"

namespace noise {
namespace fs {

VecS hdf_read_codes(const HighFive::File &file);
struct h5_bars hdf_read_dataset(const HighFive::File &file, const std::string &code);

void hdf_write_dataset(HighFive::File &file, const struct h5_bars &src);
void hdf_write_codes(HighFive::File &file, const VecS &src);

} // namespace fs

} // namespace noise