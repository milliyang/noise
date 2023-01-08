#pragma once

#include "noise/def.h"

namespace noise
{

namespace util {
    void parse_csv_bars_file(std::vector<struct bar> &v_bars, const std::string &filepath);
}

namespace uprint {
    void print(const char*tag, const struct stat &stat);
    void print(const char*tag, const struct bar &bar);
    void print(const char*tag, const struct order &order);
    void print(const char*tag, const struct trade &trade);
    void print(const char*tag, const std::list<struct trade> &trades);
    void print(const char*tag, const std::vector<struct trade> &trades);
    void print(const char*tag, const std::vector<std::string> &strings);
    void print(const char*tag, const std::vector<float> &prices);

    void print(const char*tag, time_t time);
    void print_time(const char*tag, time_t time);
}

namespace umath {
    float sum(const std::vector<float> &values);
    float mean(const std::vector<float> &values);
    float stddev(const std::vector<float> &values);

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

namespace utalib {
    void ma(std::vector<float> &ma_values,          const std::vector<float> &values, int period);
    void ma2(std::vector<float> &ma_values,         const std::vector<float> &values, int period);
    void max(std::vector<float> &max_values,        const std::vector<float> &values, int period);
    void stddev(std::vector<float> &stddev_values,  const std::vector<float> &values, int period); /*stddev.p*/

    void boll(std::vector<float> &high, std::vector<float> &mid, std::vector<float> &low,
                const std::vector<float> &values, int period,
                float ratio);
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
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::string a_tag;
};


} // namespace name

