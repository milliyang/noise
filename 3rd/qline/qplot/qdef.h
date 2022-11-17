#pragma once

#include <QString>
#include <QPoint>
#include <QMessageBox>
#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QVector>
#include <QDockWidget>
#include <QWidget>
#include <QMainWindow>
#include <QSplitter>
#include <QDebug>
#include <QUuid>
#include <QCoreApplication>
#include <QFontDatabase>
#include <QWheelEvent>

#include "autogrid.h"
#include "qutil.h"
#include "noise/def.h"
#include "noise/util.h"

#include <map>

#define LOGD  printf
#define LOGW  printf

#define CHK_NULL_INDI(indi)             \
{                                       \
    if (indi == nullptr) {              \
        LOGD("nullptr\n");              \
        return;                         \
    }                                   \
    if (indi->data.size() <= 0) {       \
        LOGD("size==0\n");              \
        return;                         \
    }                                   \
}

#define CHK_NULL(indi)                  \
{                                       \
    if (indi == nullptr) {              \
        return;                         \
    }                                   \
}

#define QGREEN              QColor(85,252,252)
#define QWHITE              QColor("#FFFFFF")
#define Q_TIPS_BG           QColor(64,0,128)


#define DEFAULT_NUM             (150)
#define DEFAULT_BAR_WIDTH       (0.8f)
#define DEFAULT_TIPS_GAP        (4)

namespace qplot
{

using PtrIndicator  = noise::PtrIndicator;
using GroupIndicator = std::map<std::string, std::vector<noise::PtrIndicator>>;

} // namespace qplot

