cmake_minimum_required(VERSION 3.10)
project(trading_system)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set policy for download timestamp behavior
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()


# Add Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    #URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
    #DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    GIT_REPOSITORY https://github.com/google/googletest.git
    #GIT_TAG release-1.10.0
    GIT_TAG release-1.11.0
)
FetchContent_MakeAvailable(googletest)

set (ALL_SOURCE_FILES 
    src/trading_system.cpp 
    src/trading_records.cpp 
    src/utils.cpp
)  

# Main executable
add_executable(trading_system 
    src/main.cpp
    ${ALL_SOURCE_FILES}
)

target_include_directories(trading_system PRIVATE ${CMAKE_SOURCE_DIR}/src)

# Test executable
add_executable(trading_system_test
    tests/trading_system_test.cpp
    ${ALL_SOURCE_FILES}
)

target_include_directories(trading_system_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(trading_system_test PRIVATE gtest gtest_main)

# Enable testing
enable_testing()
add_test(NAME trading_system_test COMMAND trading_system_test)