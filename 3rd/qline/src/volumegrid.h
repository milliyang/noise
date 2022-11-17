#pragma once

#include "autogrid.h"
#include "datafile.h"

#include <QPoint>

class VolumeGrid : public AutoGrid
{
public:
    explicit VolumeGrid(QWidget* parent);
    void virtual paintEvent(QPaintEvent* event);
    void update_stat(const struct figure_stat &stat) override;

private:
    void getIndicator();
    bool readData(QString strFile);
    void initial();
    void drawAverageLine(int day);
    void drawYtick();
    void drawVolume();

private:
    DataFile mDataFile;

    //画成交量线的起始日期和终止日期
    int beginDay;
    int endDay;
    int totalDay;
    int currentDay;

    //当前要画的成交量线中的最大成交量
    double maxVolume;

    //鼠标位置
    QPoint mousePoint;


    //线宽
    int lineWidth;


};
