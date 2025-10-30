#pragma once

#include "noise/def.h"
#include <cassert>
#include <armadillo>

namespace noise {

class Broker;
class Posotion;
class Feed;
class Stat;
class Strategy;
class Data;
class Plot;

struct series;

using PtrStrategy = std::shared_ptr<Strategy>;

using PtrSeries  = std::shared_ptr<struct series>;
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
    //std::shared_ptr<Plot>       plot;
};

void init(void);
int  init(int32_t argc, char** argv);

void plot(const std::string figure, const std::vector<float> *series, bool show = false);

}

//include all noise headers here:
#include "noise/util.h"
#include "noise/broker.h"
#include "noise/feed/feed.h"
#include "noise/position.h"
#include "noise/stat.h"
#include "noise/strategy.h"
#include "noise/data.h"
#include "noise/plot.h"
#include "noise/indicator/indicator.h"
#include "noise/mcache.h"

//at last
#include "noise/backtest.h"
