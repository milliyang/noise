#pragma once

#include "noise/noise.h"
#include "noise/indicator/ma.h"
#include "noise/indicator/stddev.h"
#include "noise/indicator/boll.h"

class StrategyCD : public noise::Strategy {

public:
    StrategyCD(void);
    ~StrategyCD(void);

    void on_create(struct noise::bt_config &cfg) override;
    void on_start() override;
    void on_bar_start() override;
    void on_bar_recv(const struct noise::bar &bar) override;
    void on_bar_stop() override;
    void on_finish() override;
    void on_destroy() override;

private:

    void prepare_data(const struct noise::bar &bar);

private:
    int in_pos_cnt_;

private:
    int index_;
    std::string code_;
    noise::PtrSeries series_close_;

    std::vector<double> m_cash;

    noise::ta::Ma *ma5_;
    noise::ta::Ma *ma10_;
    noise::ta::Stddev *stddev10_;
    noise::ta::Boll *boll_;

    noise::ta::Ma *ma60_;
};
