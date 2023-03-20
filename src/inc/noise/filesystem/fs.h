#pragma once
#include <highfive/H5File.hpp>
#include "noise/def.h"

namespace noise {
namespace fs {

enum FS_F_FIELD {
    FS_F_OPEN,
    FS_F_HIGH,
    FS_F_LOW,
    FS_F_CLOSE,
    FS_F_PRE_CLOSE,
    FS_F_CHANGE,
    FS_F_PCT_CHANGE,
    FS_F_VOLUMN,
    FS_F_AMOUNT,
    FS_F_NUM,
};

enum FS_U_FIELD {
    FS_U_DATE = 0,
    FS_U_TIME,
    FS_U_NUM,
};

enum FS_B_FIELD {
    FS_B_CODE = 0,
    FS_B_SYMBOL,
    FS_B_NAME,
    FS_B_AREA,
    FS_B_INDUSTRY,
    FS_B_MARKET,
    FS_B_LIST_DATE,
    FS_B_SEQ,
    FS_B_NUM,
};

/**
 * struct h5_bars <-> struct bars
 * struct bars {
 *     std::string code;
 *     std::vector<struct bar> data;
 * };
 *
 */
struct h5_bars {
    std::string code;
    VecF data[FS_F_NUM];
    VecU udata[FS_U_NUM];
};

struct h5_basic {
    VecS data[FS_B_NUM];
};

/*** fs.cpp ***/
struct h5_bars  fs_get_h5_bars(const struct bars &src);
struct h5_basic fs_get_basic(const std::vector<struct code_info> &infos);

struct bars     fs_get_bars(const struct h5_bars &src);


/*** csv.cpp ***/
void csv_read_bars_file(std::vector<struct bar> &bars, const std::string &filepath);
void csv_write_bars_file(const std::vector<struct bar> &bars, const std::string &filepath);

void csv_read_basic_file(h5_basic &basic, const std::string &filepath);
void csv_write_basic_file(const h5_basic &basic, const std::string &filepath);


/*** hdf.cpp ***/
VecS hdf_read_codes(const HighFive::File &file);
struct h5_bars  hdf_read_dataset(const HighFive::File &file, const std::string &code);
struct h5_basic hdf_read_basic(const HighFive::File &file);

void hdf_write_dataset(HighFive::File &file, const struct h5_bars  &src);
void hdf_write_basic(HighFive::File &file, const struct h5_basic &src);
void hdf_write_codes(HighFive::File &file, const VecS &src);


class H5 {

public:
    H5(std::string filename, bool read = true);
    ~H5(void);

public:
    //read
    VecS read_codes(void);
    bool has_code(const std::string &code);
    struct h5_bars read_bars(const std::string &code);
    struct h5_basic read_basic(void);

    //write
    void write_codes(const VecS &codes);
    void write_bars(const struct h5_bars &bars);
    void write_basic(const struct h5_basic &basic);

private:
    void init(void);

private:
    std::string filename_;
    bool op_read_;
    bool opened_;

    std::unique_ptr<HighFive::File> file_;
};

} // namespace fs

} // namespace noise