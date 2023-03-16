#pragma once

#include "noise/def.h"

namespace noise
{

namespace util {
    void parse_csv_bars_file(std::vector<struct bar> &v_bars, const std::string &filepath);
    //
    void parse_code_file(std::vector<struct code_info> &codes, const std::string &filepath);
    void parse_ini_code_file(std::vector<struct code_info> &codes, const std::string &filepath);
    void parse_csv_code_file(std::vector<struct code_info> &codes, const std::string &filepath);

    std::string get_filename_with_extension(const std::string& filepath);
    std::string get_filename_without_extension(const std::string& filepath);
}

namespace uprint {
    void print(const char*tag, const struct stat &stat);
    void print(const char*tag, const struct bar &bar);
    void print(const char*tag, const struct order &order);
    void print(const char*tag, const struct trade &trade);
    void print(const char*tag, const std::list<struct trade> &trades);
    void print(const char*tag, const std::vector<struct trade> &trades);
    void print(const char*tag, const std::vector<std::string> &strings);
    void print(const char*tag, const VecF &prices);
    void print(const char *tag, const struct code_info &info);

    void print(const char*tag, time_t time);
    void print_time(const char*tag, time_t time);
}

namespace umath {
    float sum(const VecF &values);
    float mean(const VecF &values);
    float stddev(const VecF &values);

    //don't use template
    int sum(const std::vector<int> &values);
    float mean(const std::vector<int> &values);
    void test_nanf(void);
}

namespace utime {
    time_t get_time(int date);
    int    get_date(time_t time);
    int    get_duration(time_t time);
    int    get_duration(time_t start, time_t end);

    void   print_time(const char *tag, time_t time);

    std::string fmt_time(time_t time);
    std::string fmt_time_long(time_t time);  //YYYY-MM-DD HH:mm:ss
}

namespace uta {
    void ma(VecF &ma_values,          const VecF &values, int period);
    void max(VecF &max_values,        const VecF &values, int period);
    void stddev(VecF &stddev_values,  const VecF &values, int period, bool divided_by_mean = false); /*stddev.p*/

    void boll(VecF &high, VecF &mid, VecF &low,
                const VecF &values, int period,
                float ratio);

    //slow & support resume
    void ma_s(VecF &ma_values,          const VecF &values, int period);
    void max_s(VecF &max_values,        const VecF &values, int period);
    void stddev_s(VecF &stddev_values,  const VecF &values, int period, bool divided_by_mean = false);
    void boll_s(VecF &high, VecF &mid, VecF &low,
                const VecF &values, int period,
                float ratio);
}

struct args0;
using PtrArgs = std::shared_ptr<struct args0>;

namespace uargs {
    int     args0_parse(const char *title, int32_t argc, char** argv);
    PtrArgs args0_get(void);
}

class Timelapsed {
public:
    Timelapsed(std::string tag) { start = std::chrono::high_resolution_clock::now(); a_tag=tag;}
    ~Timelapsed(void) {
        auto nano = std::chrono::high_resolution_clock::now() - start;
        //auto us = std::chrono::duration_cast<std::chrono::microseconds>(nano).count();
        //auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(nano).count();
        //printf("time elapsed: %d(us)  %d(nano sec)\n", (int)us, (int)ns);
        printf("[%s] time elapsed: %f ms\n", a_tag.c_str(), nano.count() / (1000.0f * 1000.0f));
    };
    //std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
    std::string a_tag;
};


} // namespace name

