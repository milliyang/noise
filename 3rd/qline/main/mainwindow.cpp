#include <QSplitter>
#include <QDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "linegrid.h"
#include "volumegrid.h"

MyMainWindow::MyMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pgrid = new VolumeGrid(this);
    pgrid->setObjectName(tr("kline"));
    pgrid->setFocusPolicy(Qt::StrongFocus);

    auto p2 = new LineGrid(this);
    p2->setFocusPolicy(Qt::StrongFocus);

    // setCentralWidget(pgrid);

    QSplitter *splitterMain = new QSplitter(Qt::Vertical, 0); //新建主分割窗口，水平分割
    QSplitter *sp0 = new QSplitter(Qt::Vertical, splitterMain);
    QSplitter *sp1 = new QSplitter(Qt::Vertical, splitterMain);

    splitterMain->setHandleWidth(1);

    sp0->addWidget(p2);
    sp1->addWidget(pgrid);

    {
        QSplitter *sp2 = new QSplitter(Qt::Vertical, splitterMain);
        auto p3 = new LineGrid(this);
        p3->setFocusPolicy(Qt::StrongFocus);
        sp2->addWidget(p3);
    }
    {
        QSplitter *sp2 = new QSplitter(Qt::Vertical, splitterMain);
        auto p3 = new LineGrid(this);
        p3->setFocusPolicy(Qt::StrongFocus);
        sp2->addWidget(p3);
    }
    {
        QSplitter *sp2 = new QSplitter(Qt::Vertical, splitterMain);
        auto p3 = new LineGrid(this);
        p3->setFocusPolicy(Qt::StrongFocus);
        sp2->addWidget(p3);
    }
    this->setCentralWidget(splitterMain);

    resize(1200,800);
}

MyMainWindow::~MyMainWindow()
{
    delete ui;
}

void MyMainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Escape: {
        qDebug() << __FUNCTION__ << "escape";
        exit(0);
        break;
    }
    default:
        break;
    }
}
