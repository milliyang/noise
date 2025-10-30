#pragma once

#include "noise/def.h"

namespace noise {
namespace math {

using DateMap = std::map<int, int>;
using PnlMap  = std::map<int, float>;

class PositionTracker {

public:
    PositionTracker(void);
    ~PositionTracker();

    //in position
    void    set(int date);
    int     get(int date);
    int     max(void);
    float   average(void);

    //pnl
    void    set_pnl(int date, float pnl);
    float   get_pnl(int date);

private:
    DateMap     position_map_;
    PnlMap      pnl_map_;
};

} // namespace math
} // namespace noise

