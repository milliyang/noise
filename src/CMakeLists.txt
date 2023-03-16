cmake_minimum_required (VERSION 3.8.0)
project (noise)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(NOISE_DIR "${CMAKE_SOURCE_DIR}/../")
set(NOISE_INC "${CMAKE_SOURCE_DIR}/../src/inc")
set(PROJ_DIR  "${CMAKE_SOURCE_DIR}")

set(CONFIG_SPDLOG ON)
set(CONFIG_ARMADILLO ON)

if(CMAKE_HOST_UNIX)
	set(SUPPORT_OPENMP_TASK ON)
	set(LinuxLib pthread)
	add_compile_options(-fPIE)
else()
	set(SUPPORT_OPENMP_TASK OFF)
	set(LinuxLib "")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if (DEFINED CMAKE_HOST_UNIX AND NOT DEFINED ENV{DISPLAY})
	message(STATUS "noise:: No X Display")
endif()

if(SUPPORT_OPENMP_TASK)
	MESSAGE(STATUS "noise:: SUPPORT_OPENMP_TASK=ON")
	add_definitions(-D__SUPPORT_OPENMP_TASK__)
	find_package(OpenMP)
	if(OpenMP_CXX_FOUND)
		set(OPENMP_LIB OpenMP::OpenMP_CXX)
		add_compile_options(-fopenmp)
		message(STATUS "noise:: options:-fopenmp")
	endif()
endif()

include_directories("${NOISE_DIR}/src/inc")
include_directories("${NOISE_DIR}/3rd/spdlog/include")
include_directories("${NOISE_DIR}/3rd/glob/inc")

file(GLOB NOISE_SRC "${NOISE_DIR}/src/base/*.cpp"
			  		"${NOISE_DIR}/src/base/util/*.cpp"
					"${NOISE_DIR}/src/base/indicator/*.cpp"
					"${NOISE_DIR}/src/3rd/glob/*.cpp"
)

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

# include("${NOISE_DIR}/3rd/spdlog/lib/cmake/spdlog/spdlogConfig.cmake")
# find_package(OpenMP)

######################################### armadillo
if(CONFIG_ARMADILLO)
	if(CMAKE_HOST_UNIX)
		message(STATUS "noise:: armadillo openblas")
		#sudo apt-get install libarmadillo-dev
		#sudo apt-get install libopenblas-dev
		find_package(armadillo)
		# set(ARMA_LIB openblas)
	else()
		include_directories("${NOISE_DIR}/3rd/arma")
		LINK_DIRECTORIES("${NOISE_DIR}/3rd/arma/lib_win64/")
		message(STATUS "noise:: libopenblas")
		# set(ARMA_LIB libopenblas)
	endif()
endif()

# message(${SRC})
add_library(noise ${NOISE_SRC} )
