#pragma once

#include "noise/def.h"

namespace noise
{

class Broker;



class Stat {

public:
    Stat(void);
    ~Stat(void);

    void init(std::shared_ptr<Broker> broker);
    void process(void);
    void plot(void);


private:
    std::shared_ptr<Broker> m_broker;

    struct stat m_stat;
};


} // namespace name

