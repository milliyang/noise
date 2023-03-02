#include "qfigure.h"
#include "qdef.h"

#define ARRAY_SIZE(x)           (sizeof(x) / sizeof(x[0]))
#define MIN_BAR                 (10)

#define Y_MIN                   (10000000)

namespace qplot
{

QFigure::QFigure(QWidget *parent) : AutoGrid(parent)
{
    //setMarginLeft(120); //bug: not works for ALL figure
    QWidget::setMouseTracking(true);

    m_is_key_down = false;
    m_show_cross = true;
    m_is_mouse_left_btn_hold = false;
    series_empty_warnning_ = false;
}

QFigure::~QFigure()
{
}

void QFigure::init_figure(void)
{
    m_y_min = Y_MIN;
    m_y_max = 0;

    CHK_NULL_INDI(m_indi_date);

    int num = m_indi_date->data.size();

    start_idx_ = cur_idx_ = 0;
    end_idx_ = num-1;
    total_idx_len_ = num;

    if (num <= 2) {
        return;
    }
    num = MIN(num, DEFAULT_NUM);

    end_idx_ = total_idx_len_-1;
    start_idx_ = end_idx_ - num + 1;
    cur_idx_ = (start_idx_ + end_idx_)/2;
    total_idx_len_ = num;
}

void QFigure::update_figure(void)
{
    CHK_NULL_INDI(m_indi_date);

    if (start_idx_ < 0) {
        start_idx_ = 0;
    }

    m_y_min = Y_MIN;
    m_y_max = 0;

    if (m_default_figure) {
        CHK_NULL(m_indi_high);
        CHK_NULL(m_indi_low);
        //find min max
        for (int i= start_idx_; i< end_idx_; ++i) {
            if (m_indi_high->data[i] > m_y_max ) {
                m_y_max = m_indi_high->data[i];
            }
            if (m_indi_low->data[i] < m_y_min ) {
                m_y_min = m_indi_low->data[i];
            }
        }
    } else {
        // slow
        for (auto it = seriess_.begin(); it != seriess_.end(); it++) {
            const auto &indi = *it;
            if (indi->name == INDICATOR_DATE) {
                continue;
            }

            for (int i= start_idx_; i < end_idx_; ++i) {
                if (i >= indi->data.size()) {
                    if (!series_empty_warnning_) {
                        LOGW("series:%s is EMPTY !!!\n", indi->name.c_str());
                        series_empty_warnning_ = true;
                    }
                    break;  //bugfix empty
                }
                if (indi->data[i] > m_y_max ) {
                    m_y_max = indi->data[i];
                }
                if (indi->data[i] < m_y_min ) {
                    m_y_min = indi->data[i];
                }
            }
        }
    }

    m_y_scaler = getGridHeight() / (m_y_max -m_y_min);
    m_bin_width = getGridWidth() / total_idx_len_;
}

void QFigure::paintEvent(QPaintEvent *event)
{
    AutoGrid::paintEvent(event);
    draw_figure();
}

void QFigure::draw_figure(void)
{
    CHK_NULL_INDI(m_indi_date);

    update_figure();
    draw_right_y_coordinate_info();
    draw_left_tips();

    if (m_default_figure) {
        draw_candle_bar();
    }

    if (m_show_cross) {
        draw_mouse_cross_line();
        draw_right_tips();
    }

    for (int i = 0; i < m_user_indi_st.size(); i++) {
        auto st = m_user_indi_st[i];
        if (st.shown) {
            draw_user_series_by_idx(i);
        }
    }
    draw_user_series_name();
}

void QFigure::init_user_seriess(void)
{
    if (m_default_figure) {
        for (int i = 0; i < seriess_.size(); i++) {
            auto indi = seriess_.at(i);
            if (QUtil::is_system_series(indi->name)) {
                continue;
            }
            if (indi->figure != FIGURE_DEFAULT) {
                continue;
            }
            m_user_indi.push_back(indi);
        }
    } else {
        for (int i = 0; i < seriess_.size(); i++) {
            auto indi = seriess_.at(i);
            if (indi->name == INDICATOR_DATE) {
                continue;
            }
            m_user_indi.push_back(indi);
        }
    }

    int x = getMarginLeft()+10;
    const int x_gap = 60;
    for (int i = 0; i < m_user_indi.size(); i++) {
        auto indi = m_user_indi.at(i);

        struct user_series_st st;
        st.idx = i;
        st.shown = 1;
        if (indi->color.length() <= 0 || indi->color == AUTO) {
            st.color = get_color(i);
        } else {
            st.color = get_color(indi->color);
        }

        if (indi->extra.short_name.length() > 0 && 0) {
            st.short_name = indi->extra.short_name;
        } else {
            int len = MIN(6, (int)(indi->name.length()));
            st.short_name = indi->name.substr(0, len);
        }
        st.rect = QRect(x, getMarginTop(), x_gap, 20);
        x+=x_gap;
        m_user_indi_st.push_back(st);
    }
}

void QFigure::draw_right_y_coordinate_info(void)
{
    CHK_NULL_INDI(m_indi_date);

    QPainter painter(this);
    QPen     pen;
    pen.setColor(Qt::red);
    painter.setPen(pen);

    float step = (m_y_max - m_y_min)/ getHGridNum();
    QString str;

    float x = getWidgetWidth() - getMarginRight() + DEFAULT_TIPS_GAP;

    if (getHGridNum() == 0) {
        str.sprintf("%.2f",m_y_min);
        painter.drawText(QPoint(x, getWidgetHeight() - getMarginBottom() ), str);
        str.sprintf("%.2f",m_y_max);
        painter.drawText(QPoint(x, getMarginTop() ), str);
        return;
    }

    for (int i=0; i <= getHGridNum(); ++i) {
        str.sprintf("%.2f",m_y_min + i* step);
        painter.drawText(QPoint(x, getWidgetHeight() - getMarginBottom() - i*getAtomGridHeight()), str);
    }
}

void QFigure::draw_left_tips(void)
{
    CHK_NULL_INDI(m_indi_date);

    if (cur_idx_ < 0 || cur_idx_ >=  m_indi_date->data.size()) {
        return;
    }

    QPainter painter(this);
    QPen pen;
    pen.setColor(QWHITE);
    painter.setPen(pen);
    QFont font = painter.font();
    //QFontDatabase db; qDebug() << db.families();
    //font.setFamily("DejaVu Sans Mono");
    font.setFamily("FreeMono");
    painter.setFont(font);

    int x = 10;
    int y = 10;
    QString str;
    time_t time = m_indi_date->extra.ptime->time[cur_idx_];
    std::string stime = noise::utime::fmt_time_long(time);
    {
        QRect rect(x,y,1000,30);
        painter.drawText(rect, m_figure_name);
        y+=20;
    }
    {
        QRect rect(x,y,1000,30);
        str.sprintf("%s", stime.substr(0,8).c_str());
        painter.drawText(rect, str);
        y+=20;
    }
    {
        QRect rect(x,y,1000,30);
        str.sprintf(" / %s", stime.substr(9).c_str());
        painter.drawText(rect, str);
        y+=20;
    }

    if (m_figure_name == FIGURE_DEFAULT) {
        {
            QRect rect(x,y,1000,30);
            str.sprintf("O: %.2f", m_indi_open->data[cur_idx_]);
            painter.drawText(rect, str);
            y+=20;
        }
        {
            QRect rect(x,y,1000,30);
            str.sprintf("H: %.2f", m_indi_high->data[cur_idx_]);
            painter.drawText(rect, str);
            y+=20;
        }
        {
            QRect rect(x,y,1000,30);
            str.sprintf("L: %.2f", m_indi_low->data[cur_idx_]);
            painter.drawText(rect, str);
            y+=20;
        }
        {
            QRect rect(x,y,1000,30);
            str.sprintf("C: %.2f", m_indi_close->data[cur_idx_]);
            painter.drawText(rect, str);
            y+=20;
        }
        if (m_indi_chg) {
            QRect rect(x,y,1000,30);
            str.sprintf("Chg:%+0.2f%%", m_indi_chg->data[cur_idx_]);
            painter.drawText(rect, str);
            y+=20;
        }
        if (m_indi_volume) {
            QRect rect(x,y,1000,30);
            str.sprintf("V: %0.1fK", m_indi_volume->data[cur_idx_] / 1000.0f);
            painter.drawText(rect, str);
            y+=20;
        }
        if (0) {
            QRect rect(getMarginLeft(), getMarginBottom(),100,30);
            painter.drawText(rect,tr("xxxx111111111"));
        }

        y+=6;
        for (int i = 0; i < seriess_.size(); i++) {
            auto indi = *seriess_.at(i);
            if (cur_idx_ >= indi.data.size()) {
                continue;
            }
            if (QUtil::is_system_series(indi.name)) {
                continue;
            }

            QRect rect(x,y,1000,30);
            str.sprintf("%s:%0.1f", indi.name.c_str(), indi.data[cur_idx_]);
            painter.drawText(rect, str);
            y+=20;
        }
    } else {
        for (int i = 0; i < seriess_.size(); i++) {
            auto indi = *seriess_.at(i);
            if (indi.name == INDICATOR_DATE) {
                continue;
            }
            if (cur_idx_ >= indi.data.size()) {
                continue;
            }
            QRect rect(x,y,1000,30);
            str.sprintf("%s:%0.1f", indi.name.c_str(), indi.data[cur_idx_]);
            painter.drawText(rect, str);
            y+=20;
        }
    }
}

int QFigure::value_to_y_pos(float value)
{
    return getWidgetHeight() - getMarginBottom() - (value - m_y_min) *m_y_scaler;
}

int QFigure::index_to_x_pos(int idx)
{
    return getMarginLeft() + m_bin_width * (idx - start_idx_ + 0.5f);
}

void QFigure::draw_candle_bar()
{
    CHK_NULL_INDI(m_indi_date);
    if (start_idx_ < 0) {
        return;
    }

    QPainter painter(this);
    QPen     pen;
    pen.setColor(Qt::red);
    pen.setWidth(1);
    painter.setPen(pen);
    QPoint p1;
    QPoint p2;
    QPoint p3;
    QPoint p4;

    float bar_width = m_bin_width * DEFAULT_BAR_WIDTH;
    bar_width = MAX(bar_width , 3.0f);

    for (int i = start_idx_; i <= end_idx_; ++i) {
        float the_open  = m_indi_open->data[i];
        float the_high  = m_indi_high->data[i];
        float the_low   = m_indi_low->data[i];
        float the_close = m_indi_close->data[i];
        if (the_open > the_close) {
            pen.setColor(QGREEN);
        } else {
            pen.setColor(Qt::red);
        }
        painter.setPen(pen);

        int bar_center = index_to_x_pos(i);
        int bar_left = bar_center - (bar_width / 2.0);
        int bar_h = std::abs(value_to_y_pos(the_close) - value_to_y_pos(the_open));
        //int gap = (m_bin_width - bar_width) / 2.0f;

        if (the_open > the_close) {
            //draw candle with color filled
            QRect rect = QRect(bar_left, value_to_y_pos(the_open), bar_width, bar_h);
            painter.fillRect(rect, QBrush(QGREEN));

            // draw bar_center line in bar (high-low)
            p1.setX(bar_center);
            p1.setY(value_to_y_pos(the_high));
            p2.setX(bar_center);
            p2.setY(value_to_y_pos(the_low));
            painter.drawLine(p1,p2);
        } else {
            //draw candle with hollow effect
            painter.drawRect(bar_left, value_to_y_pos(the_close), bar_width, bar_h);

            // draw center line in bar (high-low)
            p1.setX(bar_center);
            p1.setY(value_to_y_pos(the_high));

            float y1,y2;
            if (the_open > the_close) {
                y1 = the_open;
                y2 = the_close;
            } else {
                y1 = the_close;
                y2 = the_open;
            }
            p2.setX(bar_center);
            p2.setY(value_to_y_pos(y1));
            p3.setX(bar_center);
            p3.setY(value_to_y_pos(y2));
            p4.setX(bar_center);
            p4.setY(value_to_y_pos(the_low));

            painter.drawLine(p1,p2);
            painter.drawLine(p3,p4);
        }
    }
}

void QFigure::sync_pos_on_index(bool sync)
{
    //LOGD("[sync mouse] cur_idx:%d size:%ld\n", cur_idx_, m_indi_date->data.size());
    cur_idx_ = CLAMP(cur_idx_, start_idx_, end_idx_);
    float special_shift = m_bin_width/8.0f;
    mouse_position_.setX(index_to_x_pos(cur_idx_) + special_shift);
    if (sync) {
        update();
    }
}

void QFigure::sync_index_on_mouse_pos(bool sync)
{
    int idx = (double)( mouse_position_.x() - getMarginLeft() ) / m_bin_width + start_idx_;
    cur_idx_ = CLAMP(idx, start_idx_, end_idx_);
    //LOGD("cur_idx:%d size:%ld\n", cur_idx_, m_indi_date->data.size());
    if (sync) {
        update();
    }
}

void QFigure::keyPressEvent(QKeyEvent *event)
{
    m_is_key_down = true;

    int bar_num = m_indi_date->data.size();

    switch(event->key()) {
    case Qt::Key_Home:
    {
        if (cur_idx_ <= 0) {
            return;
        }
        start_idx_ = 0;
        end_idx_ = total_idx_len_-1;        
        cur_idx_= 0;
        sync_pos_on_index();
        break;
    }
    case Qt::Key_End:
    {
        if (cur_idx_ >= bar_num -1) {
            return;
        }
        end_idx_ = bar_num-1;
        start_idx_ = end_idx_ - total_idx_len_  + 1;
        cur_idx_= end_idx_;
        sync_pos_on_index();
        break;
    }
    case Qt::Key_Left: {
        if (cur_idx_ <= 0) {
            return;
        }
        //move window left
        if (cur_idx_ == start_idx_) {
            if (total_idx_len_ < m_indi_date->data.size()) {
                end_idx_-=1;
                start_idx_-=1;
            }
        }
        cur_idx_-=1;
        sync_pos_on_index();
        break;
    }
    case Qt::Key_Right: {
        if (cur_idx_ >= bar_num -1) {
            return;
        }
        //move window right
        if (cur_idx_ == end_idx_) {
            if (total_idx_len_ < m_indi_date->data.size()) {
                end_idx_+=1;
                start_idx_+=1;
            }
        }
        cur_idx_+=1;
        sync_pos_on_index();
        break;
    }
    case Qt::Key_Up: {
        update_index(0.5f);
        break;
    }
    case Qt::Key_Down: {
        update_index(2.0f);
        break;
    }
    case Qt::Key_Escape: {
        QCoreApplication::exit(0);
        return;
    }
    default:
        return;
    }

    //only on focus
    //LOGD("%s emit\n", get_id().toString().toStdString().c_str());
    emit_stat_changed();
}

void QFigure::emit_stat_changed(void)
{
    // LOGD("%s emit idx:%3d-%3d-%3d len:%d\n",
    //     get_id().toString().toStdString().c_str(),
    //     start_idx_, cur_idx_, end_idx_, total_idx_len_);
    struct figure_stat stat;
    stat.start_idx = start_idx_;
    stat.end_idx = end_idx_;
    stat.cur_idx = cur_idx_;
    stat.mouse_x_pos = mouse_position_.x();
    stat.uuid_from = get_id();
    stat.show_cross = m_show_cross;
    emit signal_stat(stat);
}

void QFigure::update_index(float factor)
{
    CHK_NULL_INDI(m_indi_date);
    int bar_num = m_indi_date->data.size();

    if (factor < 1.0f) {
        if (total_idx_len_ < MIN_BAR * 2) {
            return;
        }
    } else {
        if (total_idx_len_ >= bar_num -1 ) {
            return;
        }
    }
    int bar_left = (cur_idx_ - start_idx_) * factor;

    int total = total_idx_len_*factor;
    total = MIN(total, bar_num);

    start_idx_ = cur_idx_ - bar_left;
    start_idx_ = MAX(start_idx_, 0);

    end_idx_ = start_idx_ + total - 1;
    end_idx_ = MIN(end_idx_, bar_num-1);

    total_idx_len_ = end_idx_ - start_idx_+1;
    //cur_idx_ = CLAMP(cur_idx_, start_idx_, end_idx_);
    sync_index_on_mouse_pos(true);
}

void QFigure::update_stat(const struct figure_stat &stat)
{
    //LOGD("%s emit recv\n", get_id().toString().toStdString().c_str());

    CHK_NULL_INDI(m_indi_date);
    int bar_num = m_indi_date->data.size();

    //update stat
    start_idx_ = stat.start_idx;
    end_idx_ = stat.end_idx;
    cur_idx_ = stat.cur_idx;
    mouse_position_.setX(stat.mouse_x_pos);
    m_show_cross = stat.show_cross;

    if (end_idx_ > bar_num - 1) {
        end_idx_ = bar_num - 1;
    }
    if (start_idx_ < 0) {
        start_idx_ = 0;
    }
    total_idx_len_ = end_idx_ - start_idx_ + 1;
    update();
}

void QFigure::setup_series(std::vector<PtrSeries> &seriess, bool custom)
{
    m_default_figure = !custom;
    seriess_ = seriess;

    for (auto it = seriess.begin(); it != seriess.end(); it++) {
        const auto indi = *it;
        //LOGD("name:%s len:%ld\n", indi->name.c_str(), indi->data.size());
        if (indi->name == INDICATOR_OPEN) {
            m_indi_open = indi;
        } else if (indi->name == INDICATOR_HIGH) {
            m_indi_high = indi;
        } else if (indi->name == INDICATOR_LOW) {
            m_indi_low = indi;
        } else if (indi->name == INDICATOR_CLOSE) {
            m_indi_close = indi;
        } else if (indi->name == INDICATOR_DATE) {
            m_indi_date = indi;
        } else if (indi->name == INDICATOR_CHANGES) {
            m_indi_chg = indi;
        } else if (indi->name == INDICATOR_VOLUME) {
            m_indi_volume = indi;
        } else {
            m_indi_key = indi;
        }
        m_figure_name = QString::fromStdString(indi->figure);
    }
    if (m_default_figure) {
        assert(m_indi_open.get() != nullptr);
        assert(m_indi_high.get() != nullptr);
        assert(m_indi_low.get() != nullptr);
        assert(m_indi_close.get() != nullptr);
        m_figure_name = FIGURE_DEFAULT;
        m_indi_key = m_indi_close;
    }

    if (m_indi_date == nullptr) {
        m_indi_date = std::make_shared<struct noise::series>();
        m_indi_date->figure = FIGURE_DEFAULT;
        for (int i = 0; i < m_indi_open->data.size(); i++) {
            m_indi_date->data.push_back(i);
        }
    }
    assert(m_indi_date != nullptr);

    init_figure();
    init_user_seriess();
}

void QFigure::draw_mouse_cross_line(void)
{
    QPainter painter(this);
    QPen     pen;
    pen.setColor(QWHITE);
    pen.setWidth(1);
    painter.setPen(pen);

    painter.drawLine(mouse_position_.x(),getMarginTop(),
                     mouse_position_.x(),getWidgetHeight() - getMarginBottom());

    if (is_mouse_inside_figure()) {
        painter.drawLine(getMarginLeft(),mouse_position_.y(),
                     getWidgetWidth()-getMarginRight(),mouse_position_.y());
    } else {
        if (m_indi_key && (m_indi_key->size() > cur_idx_)) {
            float y = value_to_y_pos(m_indi_key->data[cur_idx_]);
            painter.drawLine(getMarginLeft(),y, getWidgetWidth()-getMarginRight(),y);
        }
    }
}

void QFigure::draw_right_tips()
{
    if (!is_mouse_inside_figure() && m_indi_key == nullptr) {
        return;
    }

    QPainter painter(this);
    QPen     pen;
    QBrush brush(Q_TIPS_BG);
    painter.setBrush(brush);
    pen.setColor(QWHITE);
    pen.setWidth(1);
    painter.setPen(pen);

    int tips_width = 60;
    int tips_height = 30;

    float y_value, y_pos;
    float x = getWidgetWidth() - getMarginRight();

    if (is_mouse_inside_figure()) {
        y_value =  m_y_max - ( mouse_position_.y() - getMarginTop() ) / m_y_scaler;
        y_pos = mouse_position_.y();
    } else {
        if (cur_idx_ < m_indi_key->size()) {
            y_value = m_indi_key->data[cur_idx_];
        } else {
            y_value = 0;
        }
        y_pos = value_to_y_pos(y_value);
    }
    {
        QRect rect(x, y_pos - tips_height/2, tips_width, tips_height);
        painter.drawRect(rect);
    }
    {
        QRect rect(x + tips_width/4, y_pos - tips_height/4, tips_width, tips_height);
        QString str;
        painter.drawText(rect, str.sprintf("%.2f",y_value));
    }
}

void QFigure::draw_user_series_name(void)
{
    QPainter painter(this);
    QPen     pen;
    pen.setWidth(1);

    QBrush brush(Q_TIPS_BG);
    painter.setBrush(brush);

    for (int i = 0; i < m_user_indi_st.size(); i++) {
        auto st = m_user_indi_st.at(i);
        // painter.drawRect(st.rect);
        pen.setColor(st.color);
        painter.setPen(pen);
        painter.drawText(st.rect, QString::fromStdString(st.short_name));
    }
}

void QFigure::draw_user_series_by_idx(int idx)
{
    auto series = m_user_indi.at(idx);
    auto st = m_user_indi_st.at(idx);

    if (series->data.size() <= 0) {
        return;
    }
    if (start_idx_ < 0) {
        return;
    }

    QVector<QPoint> points;
    for( int i= start_idx_; i < end_idx_; ++i) {
        if (isnanf(series->data[i])) {
            continue;
        }
        QPoint temp;
        temp.setX(index_to_x_pos(i));
        temp.setY(value_to_y_pos(series->data[i]));
        points.push_back(temp);
    }

    QPainter painter(this);
    QPen     pen;
    pen.setColor(st.color);
    painter.setPen(pen);
    QPolygon polykline(points);
    painter.drawPolyline(polykline);
}

void QFigure::mouseMoveEvent(QMouseEvent *event)
{
    m_is_key_down = false;

    bool move_right = mouse_position_.x() > event->pos().x() ? true : false;
    mouse_position_ = event->pos();

    if (m_is_mouse_left_btn_hold) {
        //move window
        if (move_right) {
            if (end_idx_ < m_indi_date->data.size()-1) {
                end_idx_+=1;
                start_idx_+=1;
                cur_idx_+=1;
            }
        } else {
            if (start_idx_ > 0) {
                end_idx_-=1;
                start_idx_-=1;
                cur_idx_-=1;
            }
        }
        update();
    } else {
        sync_index_on_mouse_pos(true);
    }
    emit_stat_changed();
}

void QFigure::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_show_cross = !m_show_cross;
        m_is_mouse_left_btn_hold = true;
        update();
    }
}

void QFigure::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_is_mouse_left_btn_hold = false;
        update();
    }
}

void QFigure::resizeEvent(QResizeEvent* event)
{
    AutoGrid::resizeEvent(event);
}

QColor QFigure::get_color(int idx)
{
    const static QColor s_colors[] = {
        Qt::yellow,
        Qt::magenta,
        Qt::green,
        Qt::cyan,
        Qt::white,
    };
    idx %= ARRAY_SIZE(s_colors);
    return s_colors[idx];
}

QColor QFigure::get_color(const std::string &color_name)
{
    QColor cc;
    if (QUtil::get_color(cc, color_name)) {
        return cc;
    } else {
        return get_color(0);
    }
}

bool QFigure::is_mouse_inside_figure(void)
{
    if (mouse_position_.x() < getMarginLeft() || mouse_position_.x() > getWidgetWidth() - getMarginRight()) {
        return false;
    }
    if (mouse_position_.y() < getMarginTop() || mouse_position_.y() > getWidgetHeight() - getMarginBottom()) {
        return false;
    }
    return true;
}

void QFigure::wheelEvent(QWheelEvent *event)
{
    //printf("QFigure deita:%d\n", event->delta());
    if (event->delta() > 0) {
        update_index(0.8f);
    } else if (event->delta() < 0) {
        update_index(1.3f);
    }
    event->accept();
}

}