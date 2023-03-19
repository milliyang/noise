function(copy_interface_properties target source)
  foreach(prop
          INTERFACE_COMPILE_DEFINITIONS
          INTERFACE_COMPILE_FEATURES
          INTERFACE_COMPILE_OPTIONS
          INTERFACE_INCLUDE_DIRECTORIES
          INTERFACE_LINK_LIBRARIES
          INTERFACE_SOURCES
          INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
    set_property(TARGET ${target} APPEND PROPERTY ${prop} $<TARGET_PROPERTY:${source},${prop}>)
  endforeach()
endfunction()

if(TARGET HighFive)
    return()
endif()

# Get HighFive targets
include("${CMAKE_CURRENT_LIST_DIR}/HighFiveTargets.cmake")

# Recreate combined HighFive
add_library(HighFive INTERFACE IMPORTED)
set_property(TARGET HighFive APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS MPI_NO_CPPBIND)  # No c++ bindings

# Ensure we activate required C++ std
if(NOT DEFINED CMAKE_CXX_STANDARD)
  if(CMAKE_VERSION VERSION_LESS 3.8)
    message(WARNING "HighFive requires minimum C++11. (C++14 for XTensor) \
        You may need to set CMAKE_CXX_STANDARD in you project")
  else()
    # A client request for a higher std overrides this
    target_compile_features(HighFive INTERFACE cxx_std_11)
  endif()
endif()

# If the user sets this flag, all dependencies are preserved.
# Useful in central deployments where dependencies are not prepared later
set(HIGHFIVE_USE_INSTALL_DEPS OFF CACHE BOOL "Use original Highfive dependencies")
if(HIGHFIVE_USE_INSTALL_DEPS)
  # If enabled in the deploy config, request c++14
  if(OFF AND NOT CMAKE_VERSION VERSION_LESS 3.8)
    set_property(TARGET HighFive APPEND PROPERTY INTERFACE_COMPILE_FEATURES cxx_std_14)
  endif()
  message(STATUS "HIGHFIVE 2.6.2: Using original dependencies (HIGHFIVE_USE_INSTALL_DEPS=YES)")
  copy_interface_properties(HighFive HighFive_HighFive)
  return()
endif()

# When not using the pre-built dependencies, give user options
if(DEFINED HIGHFIVE_USE_BOOST)
  set(HIGHFIVE_USE_BOOST ${HIGHFIVE_USE_BOOST} CACHE BOOL "Enable Boost Support")
else()
  set(HIGHFIVE_USE_BOOST OFF CACHE BOOL "Enable Boost Support")
endif()
set(HIGHFIVE_USE_EIGEN "${HIGHFIVE_USE_EIGEN}" CACHE BOOL "Enable Eigen testing")
set(HIGHFIVE_USE_XTENSOR "${HIGHFIVE_USE_XTENSOR}" CACHE BOOL "Enable xtensor testing")
set(HIGHFIVE_PARALLEL_HDF5 OFF CACHE BOOL "Enable Parallel HDF5 support")
option(HIGHFIVE_VERBOSE "Enable verbose logging" OFF)

if(HIGHFIVE_USE_XTENSOR AND NOT CMAKE_VERSION VERSION_LESS 3.8)
  set_property(TARGET HighFive APPEND PROPERTY INTERFACE_COMPILE_FEATURES cxx_std_14)
endif()

message(STATUS "HIGHFIVE 2.6.2: (Re)Detecting Highfive dependencies (HIGHFIVE_USE_INSTALL_DEPS=NO)")
include("${CMAKE_CURRENT_LIST_DIR}/HighFiveTargetDeps.cmake")
foreach(dependency HighFive_libheaders libdeps)
    copy_interface_properties(HighFive ${dependency})
endforeach()
