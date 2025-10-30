#pragma once

#include <chrono>
#include <string>
#include <common_types.h>


std::string format_timepoint(const TimePoint& time_point);
void printTimePoint(const TimePoint& tp);
TimePoint get_timepoint_from_string(const std::string& date_str = "2025-01-01 00:00:00");
