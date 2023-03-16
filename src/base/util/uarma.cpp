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

}