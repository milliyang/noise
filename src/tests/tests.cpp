#include "noise/noise.h"
#include "cd.h"
#include <random>

bool test_talib_ma(void)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 9);
    std::vector<float> values;
    for (int i = 0; i < 1000; i++) {
        values.push_back((float)dist(rd));
    }
    std::vector<float> ma0;
    std::vector<float> ma1;
    {
        noise::Timelapsed time_performance("ma0");
        noise::utalib::ma(ma0, values, 25);
    }
    {
        noise::Timelapsed time_performance("ma1");
        noise::utalib::ma2(ma1, values, 25);
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

int32_t main(int32_t argc, char** argv)
{
    test_talib_ma();

    LOGI("@%s [done]\n", __FUNCTION__);
}

