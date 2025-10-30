#include <chrono>
#include <string>
#include <common_types.h>
#include <iomanip>
#include <sstream>
#include <iostream>

#include "utils.h"

std::string format_timepoint(const TimePoint& time_point)
{
    auto time_t = std::chrono::system_clock::to_time_t(time_point);
    auto tm = *std::localtime(&time_t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void printTimePoint(const TimePoint& tp)
{
    // 将 time_point 转换为 time_t
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    
    // 将 time_t 转换为可读格式
    std::cout << std::ctime(&time); // ctime 会自动添加换行符
}

TimePoint get_timepoint_from_string(const std::string& date_str)
{
    std::tm tm = {};
    std::istringstream ss(date_str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}