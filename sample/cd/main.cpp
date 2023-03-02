#include "noise/noise.h"
#include "cd.h"
#include "noise/log.h"

int32_t main(int32_t argc, char** argv)
{
    noise::Timelapsed time_performance(__FUNCTION__);
    log_init();

    LOGI(fmt::format("{}", "Go..."));
    noise::Backtest bt([](){
        return std::make_shared<StrategyCD>();
    });

    bt.run();
}
