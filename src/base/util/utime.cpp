#include "noise/def.h"
#include "noise/util.h"

#include <iostream>
#include <iomanip>
#include <ctime>

namespace noise {

//00:00:00 GMT, Jan 1, 1970, TimeStamp (TS)

#define FUCK_YEAR       (1900)
#define TS_YEAR         (1970)

#define FUCK_MONTH_1      (1)

#define FUCK_MONTH(m)     ((m)+1)
#define UNFUCK_MONTH(m)   ((m)-1)


time_t utime::get_time(int date /*YYYYMMMDD*/)
{
    std::tm tm;
    memset(&tm, 0, sizeof(tm));
    tm.tm_mday = date % 100;
    date = (date - tm.tm_mday) / 100;
    tm.tm_mon  = date % 100;
    date = (date - tm.tm_mon) / 100;
    tm.tm_year = date % 10000 - FUCK_YEAR;

    //bugfix: 20220531->20220601
    tm.tm_mon = UNFUCK_MONTH(tm.tm_mon);

    //printf("bar_time xxxxx: %d %d %d\n", tm.tm_year, tm.tm_mon, tm.tm_mday);
    return std::mktime(&tm);
}

int utime::get_date(time_t time)
{
    struct tm tm;
    tm = *std::localtime(&time);
    tm.tm_year += FUCK_YEAR;
    return tm.tm_year * 10000 + FUCK_MONTH(tm.tm_mon) * 100 + tm.tm_mday;
}

std::string utime::fmt_time(time_t time)
{
    struct tm tm;
    tm = *std::localtime(&time);
    tm.tm_year += FUCK_YEAR;
#if 1
    char buffer[32];
    sprintf(buffer,"%02d%02d%02d %02d:%02d:%02d", tm.tm_year, FUCK_MONTH(tm.tm_mon), tm.tm_mday,
                        tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(buffer);
#else
    //buggy:
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y%m%d %H%M%S");
    return ss.str();
#endif
}

//YYYY-MM-DD HH:mm:ss
std::string utime::fmt_time_long(time_t time)
{
    struct tm tm;
    tm = *std::localtime(&time);
    tm.tm_year += FUCK_YEAR;
    char buffer[32];
    sprintf(buffer,"%04d%02d%02d %02d:%02d:%02d", tm.tm_year % 10000, FUCK_MONTH(tm.tm_mon), tm.tm_mday,
                        tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(buffer);
}

int utime::get_duration(time_t time)
{
    //bugfix: 1 second
    //[time start] time:2018-01-02 00:00:00
    //[time end]   time:2018-01-04 00:00:00
    time++;
    float duration = time / (float) (60*60*24);
    return (int) std::ceil(duration);
}

int utime::get_duration(time_t start, time_t end)
{
#if 0
    struct tm tm_start;
    struct tm tm_end;
    localtime_r(&start, &tm_start);
    localtime_r(&end,   &tm_end);
    //note: tm_yday is calculated from month 12
    int duration = (tm_end.tm_year - tm_start.tm_year) * 365 + tm_end.tm_yday - tm_start.tm_yday;
    duration++;
    return duration;
#else
    return utime::get_duration(end - start);
#endif
}

void utime::print_time(const char *tag, time_t time)
{
    struct tm tm;
    tm = *std::localtime(&time);
    tm.tm_year += FUCK_YEAR;
    printf("[%s] time:%4d-%02d-%02d %02d:%02d:%02d\n",
        tag,
        tm.tm_year+1900,
        FUCK_MONTH(tm.tm_mon),
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec);
}

}