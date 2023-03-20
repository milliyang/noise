#include "qplotchartwindow.h"
#include "ui_qplotchartwindow.h"

#include "qdef.h"
#include "qfigure.h"
#include "qvolume.h"

namespace qplot
{

QPlotChartWindow::QPlotChartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QPlotChartWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("QPlotChart"));
    resize(1920,1080);;
}

QPlotChartWindow::~QPlotChartWindow()
{
    delete ui;
}


void QPlotChartWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Escape: {
        QCoreApplication::exit(0);
        break;
    }
    default:
        break;
    }
}

static PtrSeries find_series(const std::string &name, std::vector<noise::PtrSeries> &seriess)
{
    for (int i = 0; i < seriess.size(); i++) {
        if (seriess.at(i)->name == name) {
            return seriess.at(i);
        }
    }
    return nullptr;
}

void QPlotChartWindow::q_on_stat_changed(struct figure_stat stat)
{
    for (auto it = m_figures.begin(); it != m_figures.end(); it++) {
        auto fig = *it;
        if (fig->get_id() == stat.uuid_from) {
            continue;
        }
        fig->update_stat(stat);
    }
}

void QPlotChartWindow::plot(std::vector<noise::PtrSeries> &seriess)
{
    QSplitter *main_splitter = new QSplitter(Qt::Vertical, 0);
    main_splitter->setHandleWidth(1);

    dates_ = find_series(INDICATOR_DATE, seriess);
    if (dates_.get() == nullptr) {
        // no date series, auto generate one
        dates_ = std::make_shared<struct noise::series>();
        time_  = std::make_shared<struct noise::timeseries>();
        dates_->name = INDICATOR_DATE;
        dates_->figure = FIGURE_DEFAULT;
        dates_->extra.ptime = time_;

        for (int i = 0; i < seriess[0]->size(); i++) {
            dates_->data.push_back(i);
            time_->time.push_back(i);
        }
    }

    GroupIndicator groups;
    for (int i = 0; i < seriess.size(); i++) {
        const auto indi = seriess.at(i);
        auto it = groups.find(indi->figure);
        if (it == groups.end()) {
            std::vector<noise::PtrSeries> v_indi;
            v_indi.push_back(dates_);
            v_indi.push_back(indi);
            groups[indi->figure] = v_indi;
        } else {
            it->second.push_back(indi);
        }
    }
    for (auto it = groups.begin(); it != groups.end(); it++) {
        QSplitter *sp = new QSplitter(Qt::Vertical, main_splitter);
        auto figure = new QFigure(this);
        figure->setFocusPolicy(Qt::StrongFocus);
        figure->setup_series(it->second);
        sp->addWidget(figure);
        m_figures.push_back(figure);
    }

    this->setCentralWidget(main_splitter);

    //connect
    for (auto it = m_figures.begin(); it != m_figures.end(); it++) {
        auto fig = *it;
        connect(fig, &AutoGrid::signal_stat, this, &QPlotChartWindow::q_on_stat_changed);
    }
}


}