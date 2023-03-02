#pragma once

#include "noise/def.h"
#include "noise/log.h"
#include "noise/indicator/indicator.h"

namespace noise
{

namespace ta
{
class ISys : public Indicator {

public:
    ISys(void);
    ~ISys(void);

public:
    void init(void) override;                           //called for user setup
    void update_with_preload(void) override;            //called once       (when       support preload)
    void next(const struct noise::bar &bar) override;   //called every bar  (when Don't support preload)
};

} // namespace name

} // namespace name

