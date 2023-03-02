#pragma once

#include "noise/def.h"

#include<vector>

namespace noise
{
struct series;
class Broker;
class Plot;

using PtrSeries = std::shared_ptr<struct series>;
using PtrPlot = std::shared_ptr<Plot>;
using FuncPlot = std::function<PtrPlot(void)>;

class Plot {

public:
    Plot(void) {};
    ~Plot(void) {};
    virtual void plot(std::string filename, std::vector<PtrSeries> &seriess) = 0;
};


} // namespace name

