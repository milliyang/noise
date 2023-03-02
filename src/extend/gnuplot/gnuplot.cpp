#include "gnuplot.h"

#include <matplot/matplot.h>

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

GnuPlot::GnuPlot(void)
{

}

GnuPlot::~GnuPlot(void)
{

}

void GnuPlot::plot(std::string filename, std::vector<PtrSeries> &seriess)
{
    auto dates = find_series(INDICATOR_DATE, seriess);
    if (dates.get() == nullptr) {
        //TODO:
        // no date series
        return;
    }

    auto stddev5 = find_series("stddev5", seriess);

    std::vector<double> x;
    std::vector<std::string> s_date;
    for(int i = 0; i < dates->data.size(); i++) {
        x.push_back(i);
        s_date.push_back(std::to_string((int)dates->data.at(i)));
    }

    auto h = matplot::figure(true);
    h->name("Backtest");
    h->number_title(true);
    h->position({100, 100, 1400, 1400});
    h->size(1000, 1000);

    matplot::tiledlayout(2,1);
    auto ax1 = matplot::nexttile();
    matplot::hold(true);

    std::vector<std::string> legends;
    for (int i = 0; i < seriess.size(); i++) {
        auto indi = seriess.at(i);

        //ignore default series 
        if (indi->name.length() > 0 && indi->name.at(0) == '_') {
            continue;
        }
        if (indi->figure != FIGURE_DEFAULT) {
            continue;
        }
        if (indi->sign.length() == 0) {
            indi->sign = "-"; //default value;
        }
        auto p1 = matplot::plot(ax1, x, indi->data, indi->sign);
        legends.push_back(indi->name);
    }
    matplot::legend(ax1, legends);


    if (stddev5.get() != nullptr) {
        auto ax2 = matplot::nexttile();
        auto pstddev = matplot::plot(ax2, x, stddev5->data, "_");
        matplot::grid(ax2, true);
        matplot::legend({pstddev}, {stddev5->name});
    }

    //matplot::scatter(matplot::iota(1, 20), matplot::rand(20, 0, 1));
    matplot::hold(false);
    matplot::grid(ax1, true);

    // ax2->x_axis().tick_values(m_date);
    // ax2->x_axis().tick_values(x);
    // ax2->x_axis().ticklabels(s_date);
    matplot::show();

}


#if 0
    //sample code
void GnuPlot::test_plot0(void)
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