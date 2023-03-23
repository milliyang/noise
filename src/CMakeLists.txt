cmake_minimum_required (VERSION 3.8.0)
project (noise)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(NOISE_DIR "${CMAKE_SOURCE_DIR}/../")
set(NOISE_INC "${CMAKE_SOURCE_DIR}/../src/inc")
set(PROJ_DIR  "${CMAKE_SOURCE_DIR}")

set(CONFIG_SPDLOG ON)
set(CONFIG_ARMADILLO ON)
set(CONFIG_HDF5 ON)

if(CMAKE_HOST_UNIX)
	set(SUPPORT_OPENMP_TASK ON)
	set(LinuxLib pthread)
	add_compile_options(-fPIE)
	add_compile_options("-std=c++17")
else()
	set(SUPPORT_OPENMP_TASK OFF)
	set(LinuxLib "")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (DEFINED CMAKE_HOST_UNIX AND NOT DEFINED ENV{DISPLAY})
	message(STATUS "noise:: No X Display")
endif()

if(SUPPORT_OPENMP_TASK)
	add_definitions(-D__SUPPORT_OPENMP_TASK__)
	find_package(OpenMP)
	if(OpenMP_CXX_FOUND)
		set(OPENMP_LIB OpenMP::OpenMP_CXX)
		add_compile_options(-fopenmp)
	endif()
endif()

include_directories("${NOISE_DIR}/src/inc")
include_directories("${NOISE_DIR}/3rd/spdlog/include")
include_directories("${NOISE_DIR}/3rd/glob/inc")

file(GLOB NOISE_SRC "${NOISE_DIR}/src/base/*.cpp"
			  		"${NOISE_DIR}/src/base/util/*.cpp"
					"${NOISE_DIR}/src/base/indicator/*.cpp"
					"${NOISE_DIR}/src/base/filesystem/*.cpp"
					"${NOISE_DIR}/src/base/feed/*.cpp"
					"${NOISE_DIR}/src/base/math/*.cpp"
					"${NOISE_DIR}/src/3rd/glob/*.cpp"
)

######################################## highfive
# include_directories("${NOISE_DIR}/3rd/highfive/include")
# add_subdirectory("${NOISE_DIR}/3rd/highfive")
# target_link_libraries(bar HighFive)

######################################## spdlog
if(CONFIG_SPDLOG)
	add_definitions(-DCONFIG_SPDLOG)
	SET(spdlog_DIR "${NOISE_DIR}/3rd/spdlog/lib/cmake/spdlog")

    # Stand-alone build
    find_package(spdlog REQUIRED)
	set(SPDLIB_LIB spdlog::spdlog)
else()
	# set(SPDLIB_LIB spdlog::spdlog_header_only)
	set(SPDLIB_LIB)
endif()

######################################### armadillo
if(CONFIG_ARMADILLO)
	if(CMAKE_HOST_UNIX)
		#sudo apt-get install libopenblas-dev liblapack-dev libarpack2-dev libsuperlu-dev
		#sudo apt-get install libarmadillo-dev
		find_package(Armadillo REQUIRED)
		include_directories(${ARMADILLO_INCLUDE_DIRS})
		set(INNER_ARMA_LIB ${ARMADILLO_LIBRARIES})
	else()
		include_directories("${NOISE_DIR}/3rd/arma")
		LINK_DIRECTORIES("${NOISE_DIR}/3rd/arma/lib_win64/")
		message(STATUS "noise:: libopenblas")
		set(INNER_ARMA_LIB libopenblas)
	endif()
endif()

if(CONFIG_HDF5)
	include_directories("${NOISE_DIR}/3rd/HighFive/include")
	SET(HighFive_DIR "${NOISE_DIR}/3rd/HighFive/share/HighFive/CMake")

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

# message(${SRC})
add_library(libnoise ${NOISE_SRC} )

# bugfix: library dependance
# app.exe -> dep(libnoise) -> dep(hdf5)
target_link_libraries(libnoise ${HDF5_LIBRARIES} ${INNER_ARMA_LIB} )
