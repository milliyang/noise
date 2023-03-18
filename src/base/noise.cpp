#define LOG_TAG "noise"

#include "noise/def.h"
#include "noise/noise.h"
#include "noise/log.h"
#include "noise/mcache.h"
#include <armadillo>

namespace noise {

void init(void)
{
   log_init();
   LOGI("noise::init");
   LOGI("mcache::init");
   mcache::init();
   LOGI("Armadillo version: {}", arma::arma_version::as_string());
}

void plot(const std::string figure, const std::vector<float> *series, bool show)
{
   if (series) {
      //TODO:
      // add series to figure
   }

   if (show) {
      //TODO:
      // show window;
      // wait qapp
   }
}

}