#pragma once

#include "noise/def.h"
#include "noise/log.h"

namespace noise
{

class Data;
class Strategy;

namespace ta
{

//common indicators:
class Ma;
class Stddev;
class Boll;

class Indicator {

public:
    Indicator(Strategy*stgy, std::string name);
    Indicator(void);
    ~Indicator(void);

public:
    virtual void init(void) = 0;                            //[must] called for user setup
    virtual void next(const struct noise::bar &bar) = 0;    //[must] called once       (when       support preload)
    virtual void update_with_preload(void) = 0;             //[must] called every bar  (when Don't support preload)

    virtual float get(int pos) = 0;                         //[must]

    int       get_series_size(void);                        //[must]
    PtrSeries get_series(int idx);                          //[must]
    std::string name(void);                                 //[must]
    bool is_ready(void);
    int get_index(void);

protected:
    void setup(std::shared_ptr<Data> &data);
    void increase(void);

protected:
    std::shared_ptr<Data>       data_;       //ptr to system wide data
    int                         time_index_; //increase every bar
    std::string                 name_;
    Strategy                    *strategy_;
    bool                        preload_done_;

    friend class noise::Strategy;
    friend class Data;              //buggy
    friend class noise::Data;
};

} // namespace name

} // namespace name

