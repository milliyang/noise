#pragma once

#include "noise/def.h"
#include <armadillo>

namespace noise
{

namespace uarma {

arma::frowvec linear_resize(const arma::frowvec &src, int size);
arma::frowvec get_closes(std::vector<struct bar> &bars);

arma::frowvec ma(const arma::frowvec &src, int win = 30);
arma::frowvec max(const arma::frowvec &src, int win = 30);
arma::frowvec stddev(const arma::frowvec &src, int win = 30);

void boll(const arma::frowvec &src, int win,
            arma::frowvec &high,
            arma::frowvec &mid,
            arma::frowvec &low);

}

} // namespace name

