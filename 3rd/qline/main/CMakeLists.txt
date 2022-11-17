# ┌──────────────────────────────────────────────────────────────────┐
# │  Projects Settings                                               │
# └──────────────────────────────────────────────────────────────────┘
cmake_minimum_required (VERSION 3.8.0)
project(QLine)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Qt                                                              │
# └──────────────────────────────────────────────────────────────────┘
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
find_package(Qt5Widgets REQUIRED)

set(QLINE_DIR  "${CMAKE_SOURCE_DIR}/..")

# ┌──────────────────────────────────────────────────────────────────┐
# │  Source file                                                     │
# └──────────────────────────────────────────────────────────────────┘
file(GLOB MAIN_SOURCES "*.cpp" "*.ui")
include_directories("${QLINE_DIR}/src")


add_subdirectory("${QLINE_DIR}/src" .qline)        # add subdir(path, as_name)

add_executable(QLine ${MAIN_SOURCES} )
target_link_libraries(QLine qline)
qt5_use_modules(QLine Widgets)

