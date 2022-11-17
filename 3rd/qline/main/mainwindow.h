#pragma once

#include <QMainWindow>
#include <QKeyEvent>

#include <functional>
#include <memory>
#include <vector>

#include "autogrid.h"

namespace Ui
{
class MainWindow;
}

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyMainWindow(QWidget *parent = 0);
    ~MyMainWindow();
    void virtual keyPressEvent(QKeyEvent* event);

private:
    Ui::MainWindow *ui;
    AutoGrid* pgrid;
};
