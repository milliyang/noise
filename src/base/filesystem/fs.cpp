#define LOG_TAG "fs.hdf"

#include "noise/filesystem/fs.h"
#include "spdlog/fmt/fmt.h"

namespace noise {

namespace fs {

struct h5_bars fs_get_h5_bars(const struct bars &src)
{
    struct h5_bars out;
    out.code = src.code;
    for (int i = 0; i < src.data.size(); i++) {
        const auto &bar = src.data[i];
        out.data[FS_F_OPEN].push_back(bar.open);
        out.data[FS_F_HIGH].push_back(bar.high);
        out.data[FS_F_LOW].push_back(bar.low);
        out.data[FS_F_CLOSE].push_back(bar.close);
        out.data[FS_F_PRE_CLOSE].push_back(bar.pre_close);
        out.data[FS_F_CHANGE].push_back(bar.change);
        out.data[FS_F_PCT_CHANGE].push_back(bar.pct_chg);
        out.data[FS_F_VOLUMN].push_back(bar.volumn);
        out.data[FS_F_AMOUNT].push_back(bar.amount);

        out.udata[FS_U_DATE].push_back((uint32_t)bar.date);
        out.udata[FS_U_TIME].push_back((uint32_t)bar.time);
    }
    return std::move(out);
}

struct bars fs_get_bars(const struct h5_bars &src)
{
    struct bars out;
    out.code = src.code;
    for (int i = 0; i < src.data[FS_U_DATE].size(); i++) {
        struct bar b;
        //
        b.code      = src.code;
        b.open      = src.data[FS_F_OPEN].at(i);
        b.high      = src.data[FS_F_HIGH].at(i);
        b.low       = src.data[FS_F_LOW].at(i);
        b.close     = src.data[FS_F_CLOSE].at(i);
        b.pre_close = src.data[FS_F_PRE_CLOSE].at(i);
        b.change    = src.data[FS_F_CHANGE].at(i);
        b.pct_chg   = src.data[FS_F_PCT_CHANGE].at(i);
        b.volumn    = src.data[FS_F_VOLUMN].at(i);
        b.amount    = src.data[FS_F_AMOUNT].at(i);
        //
        b.date      = src.udata[FS_U_DATE].at(i);
        b.time      = (time_t) src.udata[FS_U_TIME].at(i);
        out.data.push_back(b);
    }
    return std::move(out);
}

struct h5_basic fs_get_basic(const std::vector<struct code_info> &infos)
{
    struct h5_basic out;
    for (int i = 0; i < infos.size(); i++) {
        const auto &info = infos[i];
        out.data[FS_B_CODE].push_back(info.code);
        out.data[FS_B_SYMBOL].push_back(info.symbol);
        out.data[FS_B_NAME].push_back(info.name);
        out.data[FS_B_AREA].push_back(info.area);
        out.data[FS_B_INDUSTRY].push_back(info.industry);
        out.data[FS_B_MARKET].push_back(info.market);
        out.data[FS_B_LIST_DATE].push_back(info.list_date);
        out.data[FS_B_SEQ].push_back(info.seq);
    }
    return std::move(out);
}

std::vector<struct code_info> fs_get_basic(const struct h5_basic &src)
{
    std::vector<struct code_info> out;

    for (int i = 0; i < src.data[0].size(); i++) {
        struct code_info info;
        info.code       = src.data[FS_B_CODE].at(i);
        info.symbol     = src.data[FS_B_SYMBOL].at(i);
        info.name       = src.data[FS_B_NAME].at(i);
        info.area       = src.data[FS_B_AREA].at(i);
        info.industry   = src.data[FS_B_INDUSTRY].at(i);
        info.market     = src.data[FS_B_MARKET].at(i);
        info.list_date  = src.data[FS_B_LIST_DATE].at(i);
        info.seq        = src.data[FS_B_SEQ].at(i);
        out.push_back(info);
    }

    return std::move(out);
}

} // namespace fs
} // namespace noise