#include "noise/noise.h"
#include "cd.h"

int32_t main(int32_t argc, char** argv)
{
    noise::Timelapsed time_performance(__FUNCTION__);

    noise::Backtest bt([](){
        return std::make_shared<StrategyCD>();
    });

    bt.run();
}
