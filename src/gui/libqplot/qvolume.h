#pragma once

#include "qdef.h"
#include "autogrid.h"

namespace qplot
{

class QVolume : public AutoGrid
{
public:
    explicit QVolume(QWidget* parent);
    void virtual paintEvent(QPaintEvent* event);

public:
    void update_stat(const struct figure_stat &stat);
    void setup_series(PtrSeries series);
    void setup_seriess(std::vector<PtrSeries> &seriess);

private:
    void init_figure(void);
    void update_figure(void);
    void draw_y_coordinate_info(void);
    void draw_volume_bin(void);

private:
    int value_to_y_pos(float value);
    int index_to_x_pos(int idx);


private:
    int start_idx_;     // m_indi_date.at(idx)
    int end_idx_;
    int cur_idx_;
    int total_idx_len_;

    QPoint mouse_position_;

    float max_volume;
    float m_bin_width;
    float m_y_scaler;

    PtrSeries m_volume;
    PtrSeries m_open;
    PtrSeries m_close;
    std::vector<PtrSeries> seriess_;
    QString m_figure_name;
};

} // namespace qplot
