#define LOG_TAG "noise"

#include "noise/def.h"
#include "noise/noise.h"
#include "noise/log.h"
#include "noise/mcache.h"
#include <armadillo>

namespace noise {

int init(int32_t argc, char** argv)
{
    int ret;
    log_init();
    LOGI("noise::init");

    if (argc > 0) {
        ret = uargs::args0_parse(__FILE__, argc, argv);
        if (ret != 0) {
            return ret;
        }
    }

    PtrArgs args = uargs::args0_get();

    LOGI("mcache::init");
    if (args->has(ARGS_DATASET)) {
        auto dataset = args->get(ARGS_DATASET);
        mcache::init(dataset);
    } else {
        mcache::init(".code.csv");
    }
    //mcache::init();
    LOGI("Armadillo version: {}", arma::arma_version::as_string());
    return 0;
}

void init(void)
{
    init(0, NULL);
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