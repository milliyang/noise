#pragma once

#include "noise/def.h"
#include <armadillo>

namespace noise
{

namespace uarma {

arma::frowvec linear_resize(const arma::frowvec &src, int size);
arma::frowvec get_closes(std::vector<struct bar> &bars);

}

} // namespace name

