#pragma once

#include "noise/def.h"
#include <QColor>
#include <QMap>

namespace qplot
{


class QUtil {

public:
    static bool is_system_indicator(const std::string &name);

    static bool get_color(QColor &color, const std::string &color_name);

private:
    QUtil(void);
    ~QUtil(void);
};


} // namespace qplot

