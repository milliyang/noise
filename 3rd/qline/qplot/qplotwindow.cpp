#include "qdef.h"

#include "qplotwindow.h"
#include "ui_qplotwindow.h"
#include "qfigure.h"
#include "qvolume.h"

namespace qplot
{

QPlotWindow::QPlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("QPlot"));
    resize(1200,800);
}

QPlotWindow::~QPlotWindow()
{
    delete ui;
}

void QPlotWindow::keyPressEvent(QKeyEvent *event)
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

PtrIndicator find_indicator(const std::string &name, std::vector<noise::PtrIndicator> &indicators)
{
    for (int i = 0; i < indicators.size(); i++) {
        if (indicators.at(i)->name == name) {
            return indicators.at(i);
        }
    }
    return nullptr;
}

void QPlotWindow::q_on_stat_changed(struct figure_stat stat)
{
    for (auto it = m_figures.begin(); it != m_figures.end(); it++) {
        auto fig = *it;
        if (fig->get_id() == stat.uuid_from) {
            continue;
        }
        fig->update_stat(stat);
    }
}

void QPlotWindow::plot(std::vector<noise::PtrIndicator> &indicators)
{
    auto dates = find_indicator(INDICATOR_DATE, indicators);
    if (dates.get() == nullptr) {
        // no date indicator
        QMessageBox::warning(this, tr("Warn"), tr("not data"), QMessageBox::Yes);
        return;
    }
    QSplitter *main_splitter = new QSplitter(Qt::Vertical, 0);
    main_splitter->setHandleWidth(1);

    {
        auto figure = new QFigure(this);
        figure->setFocusPolicy(Qt::StrongFocus);
        figure->setup_indicator(indicators, false);
        QSplitter *sp = new QSplitter(Qt::Vertical, main_splitter);
        sp->addWidget(figure);
        m_figures.push_back(figure);
    }

    auto volume = find_indicator(INDICATOR_VOLUME, indicators);
    if (volume) {
        QSplitter *sp1 = new QSplitter(Qt::Vertical, main_splitter);
        auto figure = new QVolume(this);
        figure->setup_indicators(indicators);
        figure->setFocusPolicy(Qt::StrongFocus);
        sp1->addWidget(figure);
        figure->setMaximumHeight(150);
        figure->setMinimumHeight(130);
        m_figures.push_back(figure);
    }

    //
    GroupIndicator groups;
    for (int i = 0; i < indicators.size(); i++) {
        const auto indi = indicators.at(i);
        if (indi->figure == FIGURE_DEFAULT) {
            continue;
        }
        auto it = groups.find(indi->figure);
        if (it == groups.end()) {
            std::vector<noise::PtrIndicator> v_indi;
            v_indi.push_back(dates);
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
        figure->setup_indicator(it->second);
        sp->addWidget(figure);
        m_figures.push_back(figure);
    }

#if 0
    {
        QSplitter *sp = new QSplitter(Qt::Vertical, main_splitter);
        auto figure = new QFigure(this);
        figure->setFocusPolicy(Qt::StrongFocus);
        figure->setup_indicator(indicators, false);
        sp->addWidget(figure);
    }
#endif

    this->setCentralWidget(main_splitter);

    //connect
    for (auto it = m_figures.begin(); it != m_figures.end(); it++) {
        auto fig = *it;
        connect(fig, &AutoGrid::signal_stat, this, &QPlotWindow::q_on_stat_changed);
    }
}


}