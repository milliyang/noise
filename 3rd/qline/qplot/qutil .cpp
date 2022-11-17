#include "noise/def.h"
#include "qutil.h"
#include "qdef.h"

namespace qplot
{

const static QMap<std::string, QColor> c_color_map = {
    {"r",       Qt::red},
    {"g",       Qt::green},
    {"b",       Qt::blue},
    {"w",       Qt::white},
    {"y",       Qt::yellow},
    {"black",   Qt::black},
    {"cyan",    Qt::cyan},
    {"magenta", Qt::magenta},
    {"qgeeen",  QGREEN},
};

bool QUtil::is_system_indicator(const std::string &name)
{
    if (name.length() >0 && name.substr(0,1) == "_") {
        return true;
    }
    return false;
}

bool QUtil::get_color(QColor &color, const std::string &color_name)
{
    auto it = c_color_map.find(color_name);
    if (it == c_color_map.end()) {
        return false;
    }

    color = *it;
    return true;
}



} // namespace qplot

