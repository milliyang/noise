#include "qplotwindow.h"
#include "ui_qplotwindow.h"

#include "qdef.h"
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
    //resize(3840,1800);
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

PtrSeries find_series(const std::string &name, std::vector<noise::PtrSeries> &seriess)
{
    for (int i = 0; i < seriess.size(); i++) {
        if (seriess.at(i)->name == name) {
            return seriess.at(i);
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

void QPlotWindow::plot(std::vector<noise::PtrSeries> &seriess)
{
    auto dates = find_series(INDICATOR_DATE, seriess);
    if (dates.get() == nullptr) {
        // no date series
        QMessageBox::warning(this, tr("Warn"), tr("not data"), QMessageBox::Yes);
        return;
    }
    QSplitter *main_splitter = new QSplitter(Qt::Vertical, 0);
    main_splitter->setHandleWidth(1);

    if (dates->extra.info != nullptr) {
        QString title = QString("%1 %2")
            .arg(QString::fromStdString(dates->extra.info->code))
            .arg(QString::fromStdString(dates->extra.info->basic.name));
        setWindowTitle(title);
    }

    {
        auto figure = new QFigure(this);
        figure->setFocusPolicy(Qt::StrongFocus);
        figure->setup_series(seriess, false);
        QSplitter *sp = new QSplitter(Qt::Vertical, main_splitter);
        sp->addWidget(figure);
        m_figures.push_back(figure);
    }

    auto volume = find_series(INDICATOR_VOLUME, seriess);
    if (volume) {
        QSplitter *sp1 = new QSplitter(Qt::Vertical, main_splitter);
        auto figure = new QVolume(this);
        figure->setup_seriess(seriess);
        figure->setFocusPolicy(Qt::StrongFocus);
        sp1->addWidget(figure);
        figure->setMaximumHeight(150);
        figure->setMinimumHeight(130);
        m_figures.push_back(figure);
    }

    //
    GroupIndicator groups;
    for (int i = 0; i < seriess.size(); i++) {
        const auto indi = seriess.at(i);
        if (indi->figure == FIGURE_DEFAULT) {
            continue;
        }
        auto it = groups.find(indi->figure);
        if (it == groups.end()) {
            std::vector<noise::PtrSeries> v_indi;
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
        figure->setup_series(it->second);
        sp->addWidget(figure);
        m_figures.push_back(figure);
    }

#if 0
    {
        QSplitter *sp = new QSplitter(Qt::Vertical, main_splitter);
        auto figure = new QFigure(this);
        figure->setFocusPolicy(Qt::StrongFocus);
        figure->setup_series(seriess, false);
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