cmake_minimum_required (VERSION 3.8.0)
project (noise)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")

set(NOISE_DIR "${CMAKE_SOURCE_DIR}/../../")
set(NOISE_INC "${CMAKE_SOURCE_DIR}/../../src/inc")
set(PROJ_DIR "${CMAKE_SOURCE_DIR}")

set(CONFIG_QPLOT ON)
set(CONFIG_GNUPLOT ON)

if(LINUX)
	set(SUPPORT_OPENMP_TASK ON)
	set(LinuxLib pthread)
else()
	set(SUPPORT_OPENMP_TASK OFF)
	set(LinuxLib "")
endif()

file(GLOB SRC "*.cpp")

if (DEFINED CMAKE_HOST_UNIX AND NOT DEFINED ENV{DISPLAY})
	message(STATUS "No X Display")
	set(CONFIG_QPLOT OFF)
	set(CONFIG_GNUPLOT OFF)
endif()

if(SUPPORT_OPENMP_TASK)
	MESSAGE(STATUS "SUPPORT_OPENMP_TASK=ON")
	add_definitions(-D__SUPPORT_OPENMP_TASK__)
	find_package(OpenMP)
	if(OpenMP_CXX_FOUND)
		set(OPENMP_LIB OpenMP::OpenMP_CXX)
		add_compile_options(-fopenmp)
		message(STATUS "options:-fopenmp")
	endif()
endif()

include_directories("${NOISE_DIR}/src/inc")
file(GLOB NOISE_SRC "${NOISE_DIR}/src/base/*.cpp"
			  		"${NOISE_DIR}/src/base/util/*.cpp"
)

if(CONFIG_QPLOT)
	add_compile_options("-std=c++11")
	message(STATUS "options:-std=c++11")

	add_definitions(-DCONFIG_QPLOT)
	include_directories("${NOISE_DIR}/src/extend/qplot")
	file(GLOB PLOT_SRC "${NOISE_DIR}/src/extend/qplot/*.cpp")

	add_subdirectory("${NOISE_DIR}/3rd/qline/qplot" .qline)        # add subdir(path, as_name)
	include_directories("${NOISE_DIR}/3rd/qline/src")
	include_directories("${NOISE_DIR}/3rd/qline/qplot")

	SET(PLOT_LIB qline)
	set(CONFIG_GNUPLOT OFF)	#disable
endif()

if(CONFIG_GNUPLOT)
	add_compile_options("-std=c++17")
	message(STATUS "options:-std=c++17")

	add_definitions(-DCONFIG_GNUPLOT)
	include_directories("${NOISE_DIR}/src/extend/gnuplot")
	file(GLOB PLOT_SRC "${NOISE_DIR}/src/extend/gnuplot/*.cpp")
	# -- Installing: /home/leo/.local/lib/libmatplot.so
	# -- Installing: /home/leo/.local/include/matplot
	set(PLOT_LIB matplot)
	include_directories("/home/leo/.local/include/")
	LINK_DIRECTORIES ("/home/leo/.local/lib/")
endif()


# message(${SRC})
add_executable(bt  ${SRC} ${NOISE_SRC} ${PLOT_SRC} )
target_link_libraries(bt ${LinuxLib} ${OPENMP_LIB} ${CMAKE_DL_LIBS} ${PLOT_LIB})

# install(TARGETS ${PROJECT_NAME} DESTINATION "${ROOT_DIR}/Bin/${TARGET_OS}")

