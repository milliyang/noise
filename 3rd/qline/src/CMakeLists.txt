# ┌──────────────────────────────────────────────────────────────────┐
# │  Projects Settings                                               │
# └──────────────────────────────────────────────────────────────────┘
cmake_minimum_required (VERSION 3.8.0)
# project(QLine)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Qt                                                              │
# └──────────────────────────────────────────────────────────────────┘
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
find_package(Qt5Widgets REQUIRED)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Source file                                                     │
# └──────────────────────────────────────────────────────────────────┘
# !!! Warnning !!!:
#   CMAKE_SOURCE_DIR may have big trouble:
#     file(GLOB LIB_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")
file(GLOB LIB_SOURCES "*.cpp" "*.h" )

# ┌──────────────────────────────────────────────────────────────────┐
# │  UI,RCC file                                                     │
# └──────────────────────────────────────────────────────────────────┘
# FILE(GLOB UI_FILES "*.ui")
# QT5_WRAP_UI(WRAP_FILES ${UI_FILES})
# SET(RCC_FILES rcc.qrc)

# ┌──────────────────────────────────────────────────────────────────┐
# │  output libqline.a                                               │
# └──────────────────────────────────────────────────────────────────┘
MESSAGE("From subdir: 3rd/qline")
add_library(qline STATIC ${LIB_SOURCES} ${WRAP_FILES})
qt5_use_modules(qline Widgets)
