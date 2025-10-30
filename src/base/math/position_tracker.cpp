#define LOG_TAG "histo"

#include "noise/def.h"
#include "noise/noise.h"

#include "noise/math/position_tracker.h"

namespace noise {
namespace math {

PositionTracker::PositionTracker(void)
{

}

PositionTracker::~PositionTracker(void)
{

}

void PositionTracker::set(int date)
{
    auto it = position_map_.find(date);
    if (it != position_map_.end()) {
        it->second = (it->second+1);
    } else {
        position_map_.emplace(date, 1); //init
    }
}

int  PositionTracker::get(int date)
{
    auto it = position_map_.find(date);
    if (it != position_map_.end()) {
        return it->second;
    } else {
        return 0;
    }
}

int  PositionTracker::max(void)
{
    int max = 1;
    auto iter = position_map_.begin();
    while (iter != position_map_.end()) {
        max = std::max(max, iter->second);
        iter++;
    }
    return max;
}

float PositionTracker::average(void)
{
    int total = 0;
    auto iter = position_map_.begin();
    while (iter != position_map_.end()) {
        total += iter->second;
        iter++;
    }
    return (float) total / position_map_.size();
}

void PositionTracker::set_pnl(int date, float pnl)
{
    auto it = pnl_map_.find(date);
    if (it != pnl_map_.end()) {
        it->second = (it->second+pnl);
    } else {
        pnl_map_.emplace(date, pnl);
    }
}

float PositionTracker::get_pnl(int date)
{
    auto it = pnl_map_.find(date);
    if (it != pnl_map_.end()) {
        return it->second;
    } else {
        return 0;
    }
}


} //math
} //noise