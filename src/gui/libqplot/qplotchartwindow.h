#ifndef QPLOTCHARTWINDOW_H
#define QPLOTCHARTWINDOW_H

#include <QMainWindow>
#include <functional>
#include <memory>
#include <vector>
#include "qdef.h"

namespace Ui {
class QPlotChartWindow;
}

namespace qplot {

class QPlotChartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QPlotChartWindow(QWidget *parent = nullptr);
    ~QPlotChartWindow();

public:
    void virtual keyPressEvent(QKeyEvent* event);
    void plot(std::vector<noise::PtrSeries> &seriess);

public slots:
    void q_on_stat_changed(struct figure_stat stat);

private:
    Ui::QPlotChartWindow *ui;
    AutoGrid* pgrid;
    std::vector<AutoGrid*> m_figures;
    std::shared_ptr<struct noise::timeseries> time_;
    std::shared_ptr<struct noise::series> dates_;
};

}

#endif // QPLOTCHARTWINDOW_H
