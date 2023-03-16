#include "qdef.h"
#include "qvolume.h"

namespace qplot
{

QVolume::QVolume(QWidget* parent) : AutoGrid( parent)
{
    setAtomGridHeightMin(40);
    m_figure_name = "volume";
}

void QVolume::paintEvent(QPaintEvent *event)
{
    AutoGrid::paintEvent(event);

    CHK_NULL_INDI(m_volume);

    update_figure();
    draw_y_coordinate_info();
    draw_volume_bin();
}

void QVolume::init_figure(void)
{
    CHK_NULL_INDI(m_volume);

    int num = (int) m_volume->data.size();

    start_idx_ = cur_idx_ = 0;
    end_idx_ = num-1;
    total_idx_len_ = num;

    if (num <= 2) {
        return;
    }
    num = MAX(num, DEFAULT_NUM);

    end_idx_ = total_idx_len_-1;
    start_idx_ = end_idx_ - num + 1;
    cur_idx_ = end_idx_ - 1;
    total_idx_len_ = num;
}

void QVolume::update_figure(void)
{
    max_volume = 1;
    if (m_volume->data.size() <= 0) {
        return;
    }
    //bugfix:
    if (start_idx_ < 0) {
        start_idx_ = 0;
    }
    for (int i=start_idx_; i < end_idx_; ++i) {
        auto vol = m_volume->data.at(i);
        if (isnanf(vol)) {
            continue;
        }
        if (vol > max_volume) {
            max_volume = vol;
        }
    }

    m_y_scaler = getGridHeight() / max_volume;
    m_bin_width = getGridWidth() / total_idx_len_;
}

void QVolume::draw_y_coordinate_info(void)
{
    QPainter painter(this);
    QPen     pen;
    pen.setColor(Qt::red);
    painter.setPen(pen);
    float ystep = max_volume / getHGridNum();
    QString str;
    float x = getWidgetWidth() - getMarginRight() + DEFAULT_TIPS_GAP;
    for (int i=0; i <= getHGridNum(); ++i) {
        str = QString("%1K").arg((i*ystep) / 1000.0f, 2, 'f', 1);
        painter.drawText(QPoint(x, getWidgetHeight() - getMarginBottom() - i*getAtomGridHeight()), str);
    }
}

int QVolume::value_to_y_pos(float value)
{
    return getWidgetHeight() - getMarginBottom() - (value - 0) *m_y_scaler;
}

int QVolume::index_to_x_pos(int idx)
{
    return getMarginLeft() + m_bin_width * (idx - start_idx_ + 0.5f);
}

void QVolume::draw_volume_bin()
{
    QPainter painter(this);
    QPen     pen;
    pen.setWidth(1);

    float width = m_bin_width * DEFAULT_BAR_WIDTH;
    width = MAX(width , 3.0f);

    bool is_open_gt_close = false;

    for( int i= start_idx_; i < end_idx_; ++i) {
        if (m_open && m_close) {
            is_open_gt_close = m_open->data[i] > m_close->data[i] ? true : false;
        }
        float vol = m_volume->data[i];

        if (is_open_gt_close) {
            pen.setColor(QGREEN);
        } else {
            pen.setColor(Qt::red);
        }
        painter.setPen(pen);

        int bar_center = index_to_x_pos(i);
        int bar_left = bar_center - (width / 2.0f);
        int bar_h = std::abs(value_to_y_pos(vol) - value_to_y_pos(0));

        int volume = m_volume->data.at(i);
        QRect rect = QRect(bar_left, value_to_y_pos(vol), width, bar_h);
        if (is_open_gt_close) {
            painter.fillRect(rect, QBrush(QGREEN));
        } else {
            painter.drawRect(rect);
        }
    }
}

void QVolume::update_stat(const struct figure_stat &stat)
{
    CHK_NULL_INDI(m_volume);
    int bar_num = (int) m_volume->data.size();

    start_idx_ = stat.start_idx;
    end_idx_ = stat.end_idx;
    cur_idx_ = stat.cur_idx;
    mouse_position_.setX(stat.mouse_x_pos);

    if (end_idx_ > bar_num - 1) {
        end_idx_ = bar_num - 1;
    }
    if (start_idx_ < 0) {
        start_idx_ = 0;
    }
    total_idx_len_ = end_idx_ - start_idx_ + 1;
    update();
    // LOGD("%s recv idx:%3d-%3d-%3d len:%d\n",
    //     get_id().toString().toStdString().c_str(),
    //     start_idx_, cur_idx_, end_idx_, total_idx_len_);
}

void QVolume::setup_series(PtrSeries series)
{
    m_volume = series;
    init_figure();
}

void QVolume::setup_seriess(std::vector<PtrSeries> &seriess)
{
    seriess_ = seriess;
    
    for (auto it = seriess.begin(); it != seriess.end(); it++) {
        const auto indi = *it;
        if (indi->name == INDICATOR_VOLUME) {
            setup_series(indi);
        } else if (indi->name == INDICATOR_OPEN) {
            m_open = indi;
        } else if (indi->name == INDICATOR_CLOSE) {
            m_close = indi;
        }
    }
}

}