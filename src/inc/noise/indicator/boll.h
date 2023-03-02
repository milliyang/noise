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
 * Boll could be use With:
 *  1. Strategy, auto register, process, plot on chart
 *  2. indepant class, to calc mean average analize
 */
class Boll : public Indicator {

public:
    Boll(Strategy *stgy, std::string name, int period = 20, std::string ref = INDICATOR_CLOSE);
    Boll(std::string name, PtrSeries& series, int period = 20);
    ~Boll(void);

    float get(int pos) override;
    float get_by_index(int idx = 0);

public:
    void init(void) override;                           //called once for setup
    void update_with_preload(void) override;            //called once       (when       support preload)
    void next(const struct noise::bar &bar) override;   //called every bar  (when Don't support preload)

public:
    PtrSeries    series_high_;
    PtrSeries    series_mid_;
    PtrSeries    series_low_;

    Strategy    *strategy_;
    int          period_;
    PtrSeries    series_ref_;
    std::string  ref_name_;
};


} // namespace ta
} // namespace noise

