#pragma once

#include "noise/def.h"

namespace noise {

class Broker;
class Posotion;
class Feed;
class Stat;
class Strategy;
class Data;
class Plot;

struct indicator;

using PtrStrategy = std::shared_ptr<Strategy>;

struct indicator;
using PtrIndicator  = std::shared_ptr<struct indicator>;
using PtrPlot       = std::shared_ptr<Plot>;

using FuncPlot      = std::function<PtrPlot(void)>;
using FuncStrategy  = std::function<PtrStrategy(void)>;

struct BtContext {
    std::shared_ptr<Broker>     broker;
    std::shared_ptr<Posotion>   position;
    std::shared_ptr<Feed>       feed;
    std::shared_ptr<Stat>       stat;
    std::shared_ptr<Strategy>   strategy;
    std::shared_ptr<Data>       data;
    std::shared_ptr<Plot>       plot;
};

}

//include all noise headers here:
#include "noise/util.h"
#include "noise/broker.h"
#include "noise/feed.h"
#include "noise/position.h"
#include "noise/stat.h"
#include "noise/strategy.h"
#include "noise/data.h"
#include "noise/plot.h"

//at last
#include "noise/backtest.h"
