#pragma once

#include "noise/noise.h"

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
    void update_indicator(void);

    void prepare_data(const struct noise::bar &bar);


private:
    int m_cnt;
    std::string m_code;

    bool m_preloaded_once;
    noise::PtrIndicator m_close;

    std::vector<double> m_cash;

    noise::PtrIndicator m_date;
    noise::PtrIndicator m_ma5;
    noise::PtrIndicator m_ma10;
    noise::PtrIndicator m_stddev5;
    noise::PtrIndicator m_boll_high;
    noise::PtrIndicator m_boll_mid;
    noise::PtrIndicator m_boll_low;
};
