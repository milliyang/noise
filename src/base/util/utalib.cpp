#include "noise/def.h"
#include "noise/util.h"
#include <queue>

namespace noise {

void utalib::ma2(std::vector<float> &ma_values, const std::vector<float> &values, int period)
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

void utalib::ma(std::vector<float> &ma_values, const std::vector<float> &values, int period)
{
    assert(period > 0);
    if (values.size() == 0) {
        return;
    }
    assert(ma_values.size() == 0);

    const int MOD = period + 1;
    std::vector<float> fifo;
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

    for (int i = 0; i < (period-1); i++) {
        ma_values[i] = NAN;
    }
}

void utalib::max(std::vector<float> &max_values, const std::vector<float> &values, int period)
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
void utalib::stddev(std::vector<float> &stddev_values, const std::vector<float> &values, int period, bool divided_by_mean)
{
    assert(period > 0);
    if (values.size() == 0) {
        return;
    }

    std::vector<float> means;
    utalib::ma(means, values, period);

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
void utalib::boll(std::vector<float> &high, std::vector<float> &mid, std::vector<float> &low,
                const std::vector<float> &values, int period,
                float ratio)
{
    if (values.size() == 0) {
        return;
    }

    assert(high.size() == mid.size());
    assert(low.size() == mid.size());
    if (ratio <= 0) {ratio = 1.0f;}

    std::vector<float> means;
    std::vector<float> stddev;
    utalib::ma(means, values, period);
    utalib::stddev(stddev, values, period);

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