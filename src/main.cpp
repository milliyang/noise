
#include "noise/def.h"
#include "noise/backtest.h"
#include "noise/util.h"

int32_t main(int32_t argc, char** argv)
{
    noise::Timelapsed time_performance(__FUNCTION__);

    noise::Backtest bt;
    bt.run();
    TRACE_LINE
}
