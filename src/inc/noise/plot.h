#pragma once

#include "noise/def.h"

#include<vector>

namespace noise
{
struct indicator;
class Broker;
class Plot;

using PtrIndicator = std::shared_ptr<struct indicator>;
using PtrPlot = std::shared_ptr<Plot>;
using FuncPlot = std::function<PtrPlot(void)>;

class Plot {

public:
    Plot(void) {};
    ~Plot(void) {};
    virtual void plot(std::string filename, std::vector<PtrIndicator> &indicators) = 0;
};


} // namespace name

