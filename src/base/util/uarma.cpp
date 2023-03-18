#include "noise/def.h"
#include "noise/util.h"
#include "noise/util2.h"
#include "3rd/args.hxx"
#include "noise/args.h"
#include "noise/log.h"

namespace noise {

static float interpolate(float src, float src_l, float src_r, float target_l, float target_r)
{
    float value;
    if (src <= src_l) {
        value = target_l;
    } else if (src >= src_r) {
        value = target_r;
    } else {
        //    a      M - L     x - l
        //  ----- = ------  = -------
        //    b      R - M     r - x
        float a = src - src_l;
        float b = src_r - src;
        value = (a * target_r + b * target_l) / (a + b);
    }
    return value;
}

arma::frowvec uarma::linear_resize(const arma::frowvec &src, int size)
{
    arma::frowvec target = arma::frowvec(size);
    const int SRC_SIZE = (int) src.n_cols-1;
    for (int i = 0; i < size; i++) {
        if (i == 0) {
            target[i] = src[i];
            continue;
        } else if (i == (size-1)) {
            target[i] = src[src.n_cols-1];
            continue;
        }
        float ratio = (float) (i * SRC_SIZE) / (float)(size-1);
        int ii = (int)ratio;
        //   ii   ratio  ii+1
        //   10    ?     12
        target[i] = interpolate(ratio, (float)ii, (float)(ii+1), src[ii], src[ii+1]);
    }
    return target;
}

arma::frowvec uarma::get_closes(std::vector<struct bar> &bars)
{
    arma::frowvec closes = arma::frowvec(bars.size());
    int i = 0;
    for (int i = 0; i < bars.size(); i++) {
        closes[i] = bars[i].close;
    }
    return closes;
}

/* Fucking slow, but logic clear */
arma::frowvec uarma::ma(const arma::frowvec &src, int win)
{
#if 1
    arma::frowvec out = arma::frowvec(src.n_cols);
    const int IDX_NAN = win-1;
    for (int i = IDX_NAN; i < src.n_cols; i++) {
        int start = i-win+1;
        start = std::max(start, 0);
        arma::frowvec slice = src.cols(start, i);
        //assert(slice.n_cols == win);
        out.at(i) = arma::mean(slice);
    }
    for (int i = 0; i < IDX_NAN; i++) {
        out.at(i) = NAN;
    }
    return std::move(out);
#else
    return arma::mean(src, win);    // not working, API not this way
#endif
}

/* Fucking slow, but logic clear */
arma::frowvec uarma::max(const arma::frowvec &src, int win)
{
    arma::frowvec out = arma::frowvec(src.n_cols);
    const int IDX_NAN = win - 1;
    for (int i = IDX_NAN; i < src.n_cols; i++) {
        int start = i - win + 1;
        start = std::max(start, 0);
        arma::frowvec slice = src.cols(start, i);
        //assert(slice.n_cols == win);
        out.at(i) = arma::max(slice);
    }
    for (int i = 0; i < IDX_NAN; i++) {
        out.at(i) = NAN;
    }
    return std::move(out);
}

/**
 * - Fucking not slow and logic clear
 *   why?
 */
arma::frowvec uarma::stddev(const arma::frowvec &src, int win)
{
    arma::frowvec out = arma::frowvec(src.n_cols);
    const int IDX_NAN = win - 1;
    for (int i = IDX_NAN; i < src.n_cols; i++) {
        int start = i - win + 1;
        start = std::max(start, 0);
        arma::frowvec slice = src.cols(start, i);
        //assert(slice.n_cols == win);
        out.at(i) = arma::stddev(slice);
    }
    for (int i = 0; i < IDX_NAN; i++) {
        out.at(i) = NAN;
    }
    return std::move(out);
}

void uarma::boll(const arma::frowvec &src, int win,
                    arma::frowvec &high,
                    arma::frowvec &mid,
                    arma::frowvec &low)
{

}

}