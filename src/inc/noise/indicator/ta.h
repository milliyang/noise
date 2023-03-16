#pragma once

#include "noise/def.h"
#include "noise/log.h"
#include "noise/indicator/indicator.h"

namespace noise
{

namespace ta
{

/**
 * compare two series golden cross or deadth cross
 */
bool crossover(Indicator*inditor0, Indicator*inditor1, bool golden = true);
bool crossover(PtrSeries& series0, PtrSeries& series1, int idx, bool golden = true);

bool above(Indicator*inditor0, Indicator*inditor1);
bool below(Indicator*inditor0, Indicator*inditor1);

} // namespace ta
} // namespace noise

