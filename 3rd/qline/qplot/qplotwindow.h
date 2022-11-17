#pragma once

#include <QMainWindow>
#include <QKeyEvent>

#include <functional>
#include <memory>
#include <vector>

#include "qdef.h"

namespace Ui {
    class MainWindow;
}

namespace qplot {

class QPlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QPlotWindow(QWidget *parent = 0);
    ~QPlotWindow();
    void virtual keyPressEvent(QKeyEvent* event);

public:
    void plot(std::vector<noise::PtrIndicator> &indicators);

public slots:
    void q_on_stat_changed(struct figure_stat stat);

private:
    Ui::MainWindow *ui;
    AutoGrid* pgrid;
    std::vector<AutoGrid*> m_figures;
};






} // namespace qplot