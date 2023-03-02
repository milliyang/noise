#pragma once

#include "noise/def.h"
#include "noise/noise.h"

#include<vector>

namespace noise
{
class GnuPlot : public Plot {

public:
    GnuPlot(void);
    ~GnuPlot(void);

    void plot(std::string filename, std::vector<PtrSeries> &seriess);

};

}