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
        ma_values.push_back(sum/period);
    }

    //bugfix special case
    if (ma_values.size() < period) {
        period = (int) ma_values.size() + 1;
    }

    for (int i = 0; i < (period-1); i++) {
        ma_values[i] = NAN;
    }
}

void uta::max(VecF &max_values, const VecF &values, int period)
{
    assert(period > 0);
    if (values.size() == 0) {
        return;
    }

    int idx_resume = (int)max_values.size();  //resume from this idx
    for (; idx_resume < (int)values.size(); idx_resume++) {
        float sum = 0;
        for (int idx = idx_resume-period+1; idx <= idx_resume; idx++) {
            if (idx < 0) {
                sum = 0;
            } else {
                if (sum < values.at(idx)) {
                    sum = values.at(idx);
                }
            }
        }
        max_values.push_back(sum);
    }
}

/* A = sum( (v - mean)^2 ) / n;  result=sqrt(A) */
void uta::stddev(VecF &stddev_values, const VecF &values, int period, bool divided_by_mean)
{
    assert(period > 0);
    if (values.size() == 0) {
        return;
    }

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