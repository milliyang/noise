#define LOG_TAG "tests"

#include "noise/noise.h"
#include "noise/util2.h"
#include "noise/log.h"

#include <random>
#include <armadillo>

using namespace noise;

#define PERIOD      (25)
#define SAMPLES     (100000)

static void chk_vec_the_same(const VecF &v0, const VecF &v1)
{
    assert(v0.size() == v1.size());
    for (size_t i = 0; i < v0.size(); i++) {
        if (isnanf(v0[i])) {
            assert(isnanf(v1[i]));
        } else {
            //LOGI("{} {} {}", i, v0[i], v1[i]);
            assert(std::abs(v0[i] - v1[i]) < 0.0001f);
        }
    }
}

static void chk_vec_the_same(const arma::frowvec &v0, const arma::frowvec &v1)
{
    assert(v0.n_cols == v1.n_cols);
    for (size_t i = 0; i < v0.n_cols; i++) {
        if (isnanf(v0[i])) {
            assert(isnanf(v1[i]));
        } else {
            //LOGI("{} {} {}", i, v0[i], v1[i]);
            assert(std::abs(v0[i] - v1[i]) < 0.0001f);
        }
    }
}

static VecF get_random_vec(int length = 1000)
{
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 9);
    VecF values;
    for (int i = 0; i < length; i++) {
        values.push_back((float)dist(rd));
    }
    return std::move(values);
}

bool test_ta_ma_speed(void)
{
    VecF ma0;
    VecF ma1;
    VecF values = get_random_vec(SAMPLES);
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->ma");
        uta::ma(ma0, values, PERIOD);
    }
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->ma_s");
        uta::ma_s(ma1, values, PERIOD);
    }
    chk_vec_the_same(ma0, ma1);

    arma::frowvec ma2;
    arma::frowvec src = arma::frowvec(values);
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->arma::ma");
        ma2 = uarma::ma(src, PERIOD);
    }
    chk_vec_the_same(ma0, ma2);

    LOGI(" ->@{} [OK]", __FUNCTION__);
    return true;
}

bool test_ta_max(void)
{
    VecF ma0;
    VecF ma1;
    VecF values = get_random_vec(SAMPLES);
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->max");
        uta::max(ma0, values, PERIOD);
    }
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->max_s");
        uta::max_s(ma1, values, PERIOD);
    }
    chk_vec_the_same(ma0, ma1);

    arma::frowvec ma2;
    arma::frowvec src = arma::frowvec(values);
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->arma::max");
        ma2 = uarma::max(src, PERIOD);
    }
    chk_vec_the_same(ma0, ma2);

    LOGI(" ->@{} [OK]", __FUNCTION__);
    return true;
}

bool test_ta_stddev(void)
{
    VecF ma0;
    VecF ma1;
    VecF values = get_random_vec(SAMPLES);
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->stddev");
        uta::stddev(ma0, values, PERIOD);
    }
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->stddev_s");
        uta::stddev_s(ma1, values, PERIOD);
    }
    chk_vec_the_same(ma0, ma1);

    arma::frowvec ma2;
    arma::frowvec src = arma::frowvec(values);
    {
        noise::Timelapsed time_performance(__FUNCTION__ "->arma::stddev");
        ma2 = uarma::stddev(src, PERIOD);
    }
    chk_vec_the_same(ma0, ma2);

    LOGI(" ->@{} [OK]", __FUNCTION__);
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

    LOGI(" ->@{} [NG]", __FUNCTION__);
    return true;
}

int32_t main(int32_t argc, char** argv)
{
    log_init();

    test_ta_ma_speed();
    test_ta_max();
    test_ta_stddev();

    test_ta_preload_version();

    LOGI(" ->@{} [done]", __FUNCTION__);
}

