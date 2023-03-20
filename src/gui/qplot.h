#pragma once

#include "noise/def.h"
#include "noise/noise.h"

#include<vector>

namespace noise
{
class QPlot : public Plot {

public:
    QPlot(void);
    ~QPlot(void);

    void plot(std::string win_title, std::vector<PtrSeries> &seriess);
    void plotChart(std::string win_title, std::vector<PtrSeries> &seriess);

};

}