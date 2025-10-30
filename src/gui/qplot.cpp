#include "qplot.h"
//
#include <QApplication>
#include <QTranslator>
// 3rd/qline/src
#include "qplotwindow.h"
#include "qplotchartwindow.h"

namespace noise {

PtrSeries find_series(const std::string &name, std::vector<PtrSeries> &seriess)
{
    for (int i = 0; i < seriess.size(); i++) {
        if (seriess.at(i)->name == name) {
            return seriess.at(i);
        }
    }
    return nullptr;
}

QPlot::QPlot(void)
{

}

QPlot::~QPlot(void)
{

}

void QPlot::plot(std::string filename, std::vector<PtrSeries> &seriess)
{
    auto dates = find_series(INDICATOR_DATE, seriess);
    if (dates.get() == nullptr) {
        return; // no date series
    }

    int argc = 0;
    QApplication app(argc, NULL);

    QTranslator translator;
    translator.load("myI18N_zh_CN.qm");
    app.installTranslator(&translator);

    qplot::QPlotWindow window;
    window.plot(seriess);
    //window.setWindowState(window.windowStatus() ^ Qt::WindowFullScreen);
    //window.show();
    //window.showFullScreen();
    window.showMaximized();
    app.exec();
}

void QPlot::plotChart(std::string win_title, std::vector<PtrSeries> &seriess)
{
    int argc = 0;
    QApplication app(argc, NULL);

    QTranslator translator;
    translator.load("myI18N_zh_CN.qm");
    app.installTranslator(&translator);

    qplot::QPlotChartWindow window;
    window.plot(seriess);
    window.show();

    app.exec();
}

#if 0
    //sample code
void QPlot::test_plot0(void)
{
    std::vector<double> x;
    for(int i = 0; i < m_close.size(); i++) {
        x.push_back(i);
    }
    auto f = matplot::figure();
    f->name("Backtest");
    f->position({100, 100, 1400, 1400});

    matplot::hold(true);
    // auto p1 = matplot::plot(x, m_ma5->data,         "-");
    auto p1 = matplot::plot(x, m_close_ma5,     "-");
    auto p2 = matplot::plot(x, m_close_ma10,    "_");
    auto p3 = matplot::plot(x, m_boll_h,        "--");
    auto p4 = matplot::plot(x, m_boll_m,        "--");
    auto p5 = matplot::plot(x, m_boll_l,        "--");
    matplot::legend({p1,p2,p3,p4,p5}, {"close", "ma10", "boll_h", "boll_m", "boll_l"});
    matplot::hold(false);
    matplot::show();
    // matplot::save("xxxx.svg");
}
#endif


}