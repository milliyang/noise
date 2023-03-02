# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.8.0)

# 项目信息
project (noise)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fPIE")

set(ROOT_DIR "${CMAKE_SOURCE_DIR}/src")

include_directories("${ROOT_DIR}/inc")
include_directories("${ROOT_DIR}/../3rd/spdlog/include")

add_definitions(-DCONFIG_SPDLOG)

file(GLOB SRC "${ROOT_DIR}/*.cpp"
			  "${ROOT_DIR}/base/*.cpp"
			  "${ROOT_DIR}/base/util/*.cpp"
			  "${ROOT_DIR}/base/indicator/*.cpp"
)

if(LINUX)
	set(XLIB pthread)
else()
	set(XLIB "")
endif()

# message(${SRC})
add_executable(bt ${SRC})
target_link_libraries(bt ${XLIB} ${CMAKE_DL_LIBS})


