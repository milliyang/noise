#include "noise/noise.h"
#include "noise/indicator/indicator.h"
#include "noise/indicator/ta.h"
#include "noise/log.h"
#include <cassert>

namespace noise {

namespace ta {

/**
 * compare two series golden cross or deadth cross
 */
bool crossover(Indicator*inditor0, Indicator*inditor1, bool golden)
{
    if (golden) {
        bool last_day_lteq = inditor0->get(-1) <= inditor1->get(-1) ? true : false;
        if (!last_day_lteq) {
            return false;
        }

        bool today_gt = inditor0->get(0) > inditor1->get(0) ? true : false;
        return today_gt;
    } else {
        bool last_day_gteq = inditor0->get(-1) >= inditor1->get(-1) ? true : false;
        if (!last_day_gteq) {
            return false;
        }

        bool today_lt = inditor0->get(0) < inditor1->get(0) ? true : false;
        return today_lt;
    }
}

/**
 * compare two series golden cross or deadth cross
 */
bool crossover(PtrSeries& series0, PtrSeries& series1, int idx, bool golden)
{
    assert(series0->size() == series1->size());
    if (series0->size() <= idx) {
        return false;
    }
    if (golden) {
        bool last_day_lteq = series0->get(idx-1) <= series1->get(idx-1) ? true : false;
        if (!last_day_lteq) {
            return false;
        }

        bool today_gt = series0->get(idx) > series1->get(idx) ? true : false;
        return today_gt;
    } else {
        bool last_day_gteq = series0->get(idx-1) >= series1->get(idx-1) ? true : false;
        if (!last_day_gteq) {
            return false;
        }

        bool today_lt = series0->get(idx) < series1->get(idx) ? true : false;
        return today_lt;
    }
}





} // namespace ta
} // namespace noise
