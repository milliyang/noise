#include "noise/def.h"
#include "noise/util.h"
#include <queue>

namespace noise {


void uta::ma(VecF &ma_values, const VecF &values, int period)
{
    assert(period > 0);
    if (values.size() == 0) {
        return;
    }
    assert(ma_values.size() == 0);

    ma_values.clear();
    ma_values.resize(values.size());

    //bugfix special case
    if (values.size() < period) {
        for (int i = 0; i < (period-1); i++) {
            ma_values[i] = NAN;
        }
        return;
    }

    const int MOD = period + 1;
    VecF fifo;
    fifo.reserve(MOD);

    float sum = 0;
    int idx = 0;
    for (int i = 0; i < MOD; i++) {
        fifo.push_back(0.0f);
    }
    for (const auto it: values) {
        fifo[idx% MOD] = it;    //push
        idx++;                  //advance
        sum += it;
        sum -= fifo[idx % MOD]; //pop
        ma_values[idx-1] = (sum/period);
    }

    for (int i = 0; i < (period-1); i++) {
        ma_values[i] = NAN;
    }
}

void uta::max(VecF &max_values, const VecF &values, int period)
{
    //TODO:
    // need fast version
    max_s(max_values, values, period);
}

/**
 * Version0:
 *  A = sum( (v - mean)^2 ) / n;  result=sqrt(A)
 *
 * Version1:
 *  A = sum( (v - mean)^2 ) / n-1;  result=sqrt(A)     <--------- using, and the same as armadillo
 *
 */
void uta::stddev(VecF &stddev_values, const VecF &values, int period, bool divided_by_mean)
{
    //TODO:
    // need fast version
    stddev_s(stddev_values, values, period, divided_by_mean);
}

/**
 * BOLL (By JOHN BOLLINGER)
 * method:
 *  MID=MA(CLOSE,PERIOD)
 *  UPPER=MID + TIMES*STD(CLOSE,PERIOD)
 *  LOWER=MID - TIMES*STD(CLOSE,PERIOD)
 */
void uta::boll(VecF &high, VecF &mid, VecF &low,
                const VecF &values, int period,
                float ratio)
{
    if (values.size() == 0) {
        return;
    }

    //TODO:
    // need fast version
    boll_s(high, mid, low, values, period, ratio);
}

}