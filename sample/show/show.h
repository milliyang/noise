#pragma once

#include "noise/noise.h"
#include "noise/indicator/ma.h"
#include "noise/indicator/stddev.h"
#include "noise/indicator/boll.h"

class StrategyShow : public noise::Strategy {

public:
    StrategyShow(void);
    ~StrategyShow(void);

    void on_create(struct noise::bt_config &cfg) override;
    void on_start() override;
    void on_bar_start() override;
    void on_bar_recv(const struct noise::bar &bar) override;
    void on_bar_stop() override;
    void on_finish() override;
    void on_destroy() override;

private:

    void prepare_data(const struct noise::bar &bar);
    void override_config(struct noise::bt_config &cfg);

private:
    int in_pos_cnt_;
    float max_profit_;

private:
    int index_;
    noise::PtrSeries series_close_;
    std::vector<double> m_cash;

    noise::ta::Ma *ma5_;
    noise::ta::Ma *ma10_;
    noise::ta::Ma *ma60_;
    noise::ta::Ma *ma120_;

    noise::ta::Stddev *stddev10_;
    noise::ta::Boll *boll_;
    
};
