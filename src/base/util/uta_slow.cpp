#include "noise/def.h"
#include "noise/util.h"
#include <queue>

namespace noise {

void uta::ma_s(VecF &ma_values, const VecF &values, int period)
{
    assert(period > 0);
    if (values.size() == 0) {
        return;
    }

    int idx_ma = (int)ma_values.size();  //resume from this idx
    for (; idx_ma < (int)values.size(); idx_ma++) {
        float sum = 0;
        for (int idx = idx_ma-period+1; idx <= idx_ma; idx++) {
            if (idx < 0) {
                sum = NAN;
                break;
            } else {
                sum += values.at(idx);
            }
        }
        sum /= period;
        ma_values.push_back(sum);
    }
}

void uta::max_s(VecF &max_values, const VecF &values, int period)
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
void uta::stddev_s(VecF &stddev_values, const VecF &values, int period, bool divided_by_mean)
{
    assert(period > 0);
    if (values.size() == 0) {
        return;
    }

    VecF means;
    uta::ma(means, values, period);

    int idx_resume = (int)stddev_values.size();  //resume from this idx
    for (; idx_resume < (int)values.size(); idx_resume++) {
        float sum = 0;
        for (int idx = idx_resume-period+1; idx <= idx_resume; idx++) {
            if (idx < 0) {
                sum = NAN;
            } else {
                float v = values.at(idx) - means.at(idx_resume);
                sum += powf(v,2);
            }
        }
        sum = std::sqrt(sum / period);

        if (divided_by_mean) {
            stddev_values.push_back(sum/means.at(idx_resume));
        } else {
            stddev_values.push_back(sum);
        }
    }
}

/**
 * BOLL (By JOHN BOLLINGER)
 * method:
 *  MID=MA(CLOSE,PERIOD)
 *  UPPER=MID + TIMES*STD(CLOSE,PERIOD)
 *  LOWER=MID - TIMES*STD(CLOSE,PERIOD)
 */
void uta::boll_s(VecF &high, VecF &mid, VecF &low,
                const VecF &values, int period,
                float ratio)
{
    if (values.size() == 0) {
        return;
    }

    assert(high.size() == mid.size());
    assert(low.size() == mid.size());
    if (ratio <= 0) {ratio = 1.0f;}

    VecF means;
    VecF stddev;
    uta::ma(means, values, period);
    uta::stddev(stddev, values, period);

    int idx_resume = (int)high.size();  //resume from this idx
    for (; idx_resume < (int)values.size(); idx_resume++) {
        float mm = means.at(idx_resume);
        float std = stddev.at(idx_resume);

        high.push_back(mm+std*ratio);
        mid.push_back(mm);
        low.push_back(mm-std*ratio);
    }
}

}