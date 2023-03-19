#define LOG_TAG "fs.hdf"

#include "noise/filesystem/fs.h"

#include "spdlog/fmt/fmt.h"
#include "noise/def.h"

namespace noise {

namespace fs {

#define H5_KEY_CODE_LIST     "code_list"

#define H5_KEY_FMT_BASIC     "basic/s{}"
#define H5_KEY_FMT_BAR_F     "{}/f{}"
#define H5_KEY_FMT_BAR_U     "{}/u{}"

VecS hdf_read_codes(const HighFive::File &file)
{
    auto dataset = file.getDataSet(H5_KEY_CODE_LIST);
    auto codes = dataset.read<VecS>();
    dataset.read(codes);
    return std::move(codes);
}

struct h5_bars hdf_read_dataset(const HighFive::File &file, const std::string &code)
{
    struct h5_bars out;
    out.code = code;

    for (int i = 0; i < FS_F_NUM; i++) {
        std::string key = fmt::format(H5_KEY_FMT_BAR_F,  code, i);
        auto dataset = file.getDataSet(key);
        auto data = dataset.read<noise::VecF>();
        dataset.read(data);
        out.data[i] = std::move(data);
    }
    for (int i = 0; i < FS_U_NUM; i++) {
        std::string key = fmt::format(H5_KEY_FMT_BAR_U, code, i);
        auto dataset = file.getDataSet(key);
        auto data = dataset.read<noise::VecU>();
        dataset.read(data);
        out.udata[i] = std::move(data);
    }

#if 0
    //DEBUG:
    for (int i = 0; i < date_.size(); i++) {
        struct bar bar;
        bar.date = (int)date_[i];
        bar.open = open_[i];
        bar.high = high_[i];
        bar.low  = low_[i];
        bar.close  = close_[i];
        bars.push_back(bar);
        noise::uprint::print(code.c_str(), bar);
    }
#endif
    return std::move(out);
}

void hdf_write_dataset(HighFive::File &file, const struct h5_bars &src)
{
    for (int i = 0; i < FS_F_NUM; i++) {
        std::string key = fmt::format(H5_KEY_FMT_BAR_F,  src.code, i);
        file.createDataSet(key, src.data[i]);
    }
    for (int i = 0; i < FS_U_NUM; i++) {
        std::string key = fmt::format(H5_KEY_FMT_BAR_U, src.code, i);
        file.createDataSet(key, src.udata[i]);
    }
}

void hdf_write_basic(HighFive::File &file, const struct h5_basic &src)
{
    for (int i = 0; i < FS_B_NUM; i++) {
        std::string key = fmt::format(H5_KEY_FMT_BASIC, i);
        file.createDataSet(key, src.data[i]);
    }
}

struct h5_basic hdf_read_basic(const HighFive::File &file)
{
    struct h5_basic out;
    for (int i = 0; i < FS_B_NUM; i++) {
        std::string key = fmt::format(H5_KEY_FMT_BASIC, i);
        auto dataset = file.getDataSet(key);
        auto data = dataset.read<noise::VecS>();
        dataset.read(data);
        out.data[i] = std::move(data);
    }
    return std::move(out);
}

void hdf_write_codes(HighFive::File &file, const VecS &src)
{
    file.createDataSet(H5_KEY_CODE_LIST, src);
}




} // namespace fs
} // namespace noise