#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;

    translator.load("myI18N_zh_CN.qm");

    a.installTranslator(&translator);

    MyMainWindow w;
    w.show();

    return a.exec();
}
