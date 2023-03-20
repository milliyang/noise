#define LOG_TAG "fs.h5"

#include "noise/filesystem/fs.h"
#include "noise/filesystem/fs_private.h"

namespace noise {

namespace fs {

H5::H5(std::string filename, bool read)
{
    filename_ = filename;
    op_read_ = read;
    opened_ = false;
    file_ = nullptr;
}

H5::~H5(void)
{

}

void H5::init(void)
{
    if (!opened_) {
        if (op_read_) {
            file_ = std::make_unique<HighFive::File>(filename_, HighFive::File::ReadOnly);
        } else {
            file_ = std::make_unique<HighFive::File>(filename_, HighFive::File::Truncate);
        }
        opened_ = true;
    }
}

bool H5::has_code(const std::string &code)
{
    init();

    //TODO:

    return false;
}

VecS H5::read_codes(void)
{
    init();
    assert(op_read_ == true);
    return hdf_read_codes(*file_);
}

struct h5_bars H5::read_bars(const std::string &code)
{
    init();
    assert(op_read_ == true);
    return hdf_read_dataset(*file_, code);
}

struct h5_basic H5::read_basic(void)
{
    init();
    assert(op_read_ == true);
    return hdf_read_basic(*file_);
}

void H5::write_bars(const struct h5_bars &bars)
{
    init();
    assert(op_read_ == false);
    return hdf_write_dataset(*file_, bars);
}

void H5::write_basic(const struct h5_basic &basic)
{
    init();
    assert(op_read_ == false);
    return hdf_write_basic(*file_, basic);
}

void H5::write_codes(const VecS &codes)
{
    init();
    assert(op_read_ == false);
    return hdf_write_codes(*file_, codes);
}


} // namespace fs
} // namespace noise