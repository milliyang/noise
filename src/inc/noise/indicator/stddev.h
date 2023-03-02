#pragma once

#include "noise/def.h"
#include "noise/log.h"
#include "noise/indicator/indicator.h"

namespace noise
{

class Strategy;

namespace ta
{
/**
 * Stddev could be use With:
 *  1. Strategy, auto register, process, plot on chart
 *  2. indepant class, to calc mean average analize
 */
class Stddev : public Indicator {

public:
    Stddev(Strategy *stgy, std::string name, int period = 5, bool divided_by_unit = true, std::string ref = INDICATOR_CLOSE);
    Stddev(std::string name, PtrSeries& series, int period = 5, bool divided_by_unit = true);
    ~Stddev(void);

    float get(int pos) override;
    float get_by_index(int idx = 0);

public:
    void init(void) override;                           //called once for setup
    void update_with_preload(void) override;            //called once       (when       support preload)
    void next(const struct noise::bar &bar) override;   //called every bar  (when Don't support preload)

public:
    int          period_;
    PtrSeries    series_;
    PtrSeries    series_ref_;
    std::string  ref_name_;
    bool         divided_by_unit_;
};


} // namespace ta
} // namespace noise

