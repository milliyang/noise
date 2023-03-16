#include "noise/noise.h"
#include <random>

using namespace noise;

bool test_ta_ma_speed(void)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 9);
    VecF values;
    for (int i = 0; i < 1000; i++) {
        values.push_back((float)dist(rd));
    }
    VecF ma0;
    VecF ma1;
    {
        noise::Timelapsed time_performance("ma0");
        uta::ma(ma0, values, 25);
    }
    {
        noise::Timelapsed time_performance("ma1");
        uta::ma_s(ma1, values, 25);
    }

    assert(ma0.size() == ma1.size());
    for (size_t i = 0; i < ma0.size(); i++) {
        if (isnanf(ma0[i])) {
            assert(isnanf(ma1[i]));
        } else {
            assert(std::abs(ma0[i] - ma1[i]) < 0.0001f);
        }
    }
    LOGI("@%s [OK]\n", __FUNCTION__);
    return true;
}

/**
 * compare the results: preload vs. not preload
 *
 * ma     vs. ma_s
 * stddev vs. stddev_s
 * ...
 */
bool test_ta_preload_version(void)
{
    //TODO:

    LOGI("@%s [NG]\n", __FUNCTION__);
    return true;
}


int32_t main(int32_t argc, char** argv)
{
    test_ta_ma_speed();
    test_ta_preload_version();

    LOGI("@%s [done]\n", __FUNCTION__);
}

