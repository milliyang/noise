cmake_minimum_required (VERSION 3.8.0)
project (noise)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(BT_NOISE_DIR "${CMAKE_SOURCE_DIR}/../")
set(PROJ_DIR "${CMAKE_SOURCE_DIR}")

set(CONFIG_QPLOT ON)
set(CONFIG_SPDLOG ON)
set(CONFIG_ARMADILLO ON)
set(CONFIG_HDF5 ON)
set(CONFIG_GNUPLOT OFF)	#disable

if(CMAKE_HOST_UNIX)
	set(SUPPORT_OPENMP_TASK ON)
	set(LinuxLib pthread)
	add_compile_options(-fPIE)
else()
	set(SUPPORT_OPENMP_TASK OFF)
	set(LinuxLib "")
endif()


add_definitions(-DCONFIG_SPDLOG)
if (DEFINED CMAKE_HOST_UNIX AND NOT DEFINED ENV{DISPLAY})
	message(STATUS "noise:: No X Display")
	set(CONFIG_QPLOT OFF)
endif()

if(SUPPORT_OPENMP_TASK)
	add_definitions(-D__SUPPORT_OPENMP_TASK__)
	find_package(OpenMP)
	if(OpenMP_CXX_FOUND)
		set(OPENMP_LIB OpenMP::OpenMP_CXX)
		add_compile_options(-fopenmp)
	endif()
endif()

include_directories("${BT_NOISE_DIR}/src/inc")
include_directories("${BT_NOISE_DIR}/3rd/spdlog/include")
include_directories("${BT_NOISE_DIR}/3rd/glob/inc")
include_directories("${BT_NOISE_DIR}/3rd/filesystem/inc")

#spdlog
if(CONFIG_SPDLOG)
	SET(spdlog_DIR "${BT_NOISE_DIR}/3rd/spdlog/lib/cmake/spdlog")

    # Stand-alone build
    find_package(spdlog REQUIRED)
	#set(SPDLIB_LIB spdlog::spdlog)
else()
	# set(SPDLIB_LIB spdlog::spdlog_header_only)
	set(SPDLIB_LIB )
endif()

################################ Qt Plot
if(CONFIG_QPLOT)
	if(LINUX)
		add_compile_options("-std=c++11")
		message(STATUS "noise:: options:-std=c++11")
	endif()
	add_definitions(-DCONFIG_QPLOT)
	include_directories("${BT_NOISE_DIR}/src/gui")
	include_directories("${BT_NOISE_DIR}/src/gui/libqplot")

	file(GLOB PLOT_SRC "${BT_NOISE_DIR}/src/gui/*.cpp")

	add_subdirectory("${BT_NOISE_DIR}/src/gui/libqplot" .libqplot)        # add subdir(path, as_name)
	SET(PLOT_LIB libqplot)
	set(CONFIG_GNUPLOT OFF)	#disable
endif()

################################ Gnu Plot
if(CONFIG_GNUPLOT)
	add_compile_options("-std=c++17")
	message(STATUS "noise:: options:-std=c++17")

	add_definitions(-DCONFIG_GNUPLOT)
	include_directories("${BT_NOISE_DIR}/src/extend/gnuplot")
	file(GLOB PLOT_SRC "${BT_NOISE_DIR}/src/extend/gnuplot/*.cpp")
	# -- Installing: /home/leo/.local/lib/libmatplot.so
	# -- Installing: /home/leo/.local/include/matplot
	set(PLOT_LIB matplot)
	include_directories("/home/leo/.local/include/")
	LINK_DIRECTORIES ("/home/leo/.local/lib/")
endif()

################################ armadillo
if(CONFIG_ARMADILLO)
	if(CMAKE_HOST_UNIX)
		#sudo apt-get install libopenblas-dev liblapack-dev libarpack2-dev libsuperlu-dev
		#sudo apt-get install libarmadillo-dev
		find_package(Armadillo REQUIRED)
		include_directories(${ARMADILLO_INCLUDE_DIRS})
		set(ARMA_LIB ${ARMADILLO_LIBRARIES})
	else()
		include_directories("${BT_NOISE_DIR}/3rd/arma")
		LINK_DIRECTORIES("${BT_NOISE_DIR}/3rd/arma/lib_win64/")
		set(ARMA_LIB libopenblas)
	endif()
endif()

################################ HighFive && HDF5
if(CONFIG_HDF5)
	include_directories("${BT_NOISE_DIR}/3rd/HighFive/include")
	SET(HighFive_DIR "${BT_NOISE_DIR}/3rd/HighFive/share/HighFive/CMake")

	if(CMAKE_HOST_UNIX)
		# download https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.14/hdf5-1.14.0/bin/unix/hdf5-1.14.0-Std-ubuntu2204_64.tar.gz	[NG] require GLIBC_2.33 in ubuntu 20.04
		# download https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.14/hdf5-1.14.0/src/CMake-hdf5-1.14.0.tar.gz
		# install at : ~/bin/hdf5/HDF5-1.14.0-Linux/HDF_Group/HDF5/1.14.0/
		SET(HDF5_DIR "~/bin/hdf5/HDF5-1.14.0-Linux/HDF_Group/HDF5/1.14.0/cmake")
	else()
		# download https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-1.14/hdf5-1.14.0/bin/windows/hdf5-1.14.0-Std-win10_64-vs17.zip
		# install at: C:\Program Files\HDF_Group\HDF5\1.14.0\bin
		include_directories("C:/Program Files/HDF_Group/HDF5/1.14.0/include")
		find_package(HighFive REQUIRED)		# find in HighFive_DIR
	endif()

	find_package(HDF5 REQUIRED)
endif()

################################ glob require
if(CMAKE_HOST_UNIX)
	add_definitions(-DGLOB_USE_GHC_FILESYSTEM)
	add_compile_options("-std=c++17")
else()
	# Fxxk:
	#  - HighFive vector<string> need c++17 std::filesystem
	#  - but, glob use private ghc::filesystem
	add_compile_options("/std:c++17")
	add_compile_options("/utf-8")

	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

################################ libnoise
add_subdirectory("${BT_NOISE_DIR}/src" .libnoise)        # add subdir(path, as_name)
SET(NOISE_LIB libnoise)



# for each sample folder, build one program
file(GLOB_RECURSE SAMPLE_FOLDERS LIST_DIRECTORIES true RELATIVE "${CMAKE_SOURCE_DIR}" sample)

foreach(SAMPLE IN ITEMS ${SAMPLE_FOLDERS})
    message(STATUS "build: ${SAMPLE}")
	file(GLOB SAMPLE_SRC "${SAMPLE}/*.cpp")

	add_executable(${SAMPLE}  ${SAMPLE_SRC} ${PLOT_SRC} )
	target_link_libraries(${SAMPLE} ${SPDLIB_LIB} ${LinuxLib} ${OPENMP_LIB} ${CMAKE_DL_LIBS} ${PLOT_LIB} ${ARMA_LIB}  ${NOISE_LIB} ${HDF5_LIBRARIES} )
endforeach()
