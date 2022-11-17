#pragma once

#include "qdef.h"

namespace qplot
{

struct user_indicator_st {
    int         idx;
    std::string short_name;
    QRect       rect;
    QColor      color;
    uint8_t     shown;
};

class QFigure : public AutoGrid
{
    Q_OBJECT

public:
    explicit QFigure(QWidget *parent = 0);
    void virtual paintEvent(QPaintEvent* event);
    void virtual keyPressEvent(QKeyEvent* event);
    void virtual mouseMoveEvent(QMouseEvent* event);
    void virtual mousePressEvent(QMouseEvent* event);
    void virtual mouseReleaseEvent(QMouseEvent* event);
    void virtual resizeEvent(QResizeEvent* event);
    ~QFigure();

    void setup_indicator(std::vector<PtrIndicator> &indicators, bool custom=true);
    void add_indicator(noise::indicator &indicator);
    void update_stat(const struct figure_stat &stat);

protected:
    void wheelEvent(QWheelEvent *event);

    //figure drawing
private:
    void init_figure(void);
    void init_user_indicators(void);

    void update_figure(void);
    void draw_figure(void);
    void draw_left_tips(void);
    void draw_right_y_coordinate_info(void);
    void draw_candle_bar(void);
    //
    void draw_mouse_cross_line(void);
    void draw_right_tips(void);
    //    
    void draw_user_indicator_by_idx(int idx);
    void draw_user_indicator_name(void);

    void update_index(float factor = 1.0f);

    //utils
private:
    bool is_system_indicator(const std::string &name);
    bool is_mouse_inside_figure(void);
    QColor get_color(int idx);
    QColor get_color(const std::string &color_name);
    //
    int value_to_y_pos(float value);
    int index_to_x_pos(int idx);
    //
    void sync_pos_on_index(bool sync=true);
    void sync_index_on_mouse_pos(bool sync=true);

    void emit_stat_changed(void);


private:

    int start_idx_;         // m_indi_date.at(idx)
    int end_idx_;
    int cur_idx_;
    int total_idx_len_;

    float m_y_max;          //right y coordinate max
    float m_y_min;          //right y coordinate min
    float m_y_scaler;       // y coordinate scale factor
    float m_bin_width;
    QPoint mouse_position_;

    bool m_is_key_down;
    bool m_show_cross;

    bool m_is_mouse_left_btn_hold;

    std::vector<PtrIndicator> m_indicators;
    PtrIndicator m_indi_open;
    PtrIndicator m_indi_high;
    PtrIndicator m_indi_low;
    PtrIndicator m_indi_close;
    PtrIndicator m_indi_date;
    PtrIndicator m_indi_chg;
    PtrIndicator m_indi_volume;
    PtrIndicator m_indi_key;

    bool m_default_figure;
    QString m_figure_name;


    std::vector<PtrIndicator> m_user_indi;
    std::vector<struct user_indicator_st> m_user_indi_st;
};


} // namespace qplot

