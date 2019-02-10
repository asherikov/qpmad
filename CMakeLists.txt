# General parameters
cmake_minimum_required (VERSION 2.8.12)
project (qpmad CXX)


# --------------
# Options
# --------------
set(CMAKE_VERBOSE_MAKEFILE          ON)
option(QPMAD_BUILD_TESTS        "Build tests"                                           ON)
option(QPMAD_ENABLE_TRACING     "Enable tracing"                                        OFF)
option(QPMAD_USE_HOUSEHOLDER    "Use Housholder matrices for updating factorizations"   OFF)
# --------------


# --------------
# Process options
# --------------
if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

# Detect catkin
if(CATKIN_DEVEL_PREFIX)
    message(STATUS "Building with catkin.")
    set(QPMAD_CATKIN_BUILD      ON)
    set(QPMAD_ENABLE_TRACING    OFF)

    if(CATKIN_ENABLE_TESTING)
        set(QPMAD_BUILD_TESTS   ON)
    else()
        set(QPMAD_BUILD_TESTS   OFF)
    endif()
endif()
# --------------


# --------------
# Packaging
# --------------
if (QPMAD_CATKIN_BUILD)
    find_package(catkin REQUIRED)
    catkin_package()
else()
    set(PROJECT_VERSION_MAJOR 1)
    set(PROJECT_VERSION_MINOR 0)
    set(PROJECT_VERSION_PATCH 0)

    if (NOT QPMAD_EMBEDDED)
        include(CMakePackageConfigHelpers)
        set(QPMAD_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/share/qpmad/")
        set(QPMAD_INCLUDES "${CMAKE_INSTALL_PREFIX}/include")

        configure_package_config_file(  "cmake/qpmadConfig.cmake.in"
                                        "${PROJECT_BINARY_DIR}/qpmadConfig.cmake"
                                        INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/share/qpmad/"
                                        NO_SET_AND_CHECK_MACRO
                                        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

        write_basic_package_version_file(
            ${PROJECT_BINARY_DIR}/qpmadConfigVersion.cmake
            VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
            COMPATIBILITY SameMajorVersion)
    endif()
endif()

if (QPMAD_CATKIN_BUILD)
    set(QPMAD_INCLUDE_INSTALL_DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
else()
    if (QPMAD_EMBEDDED)
        set(QPMAD_INCLUDE_INSTALL_DESTINATION ${QPMAD_EMBEDDED_INSTALL_DIR})
    else()
        set(QPMAD_INCLUDE_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${QPMAD_ID_LOWER_CASE})
    endif()
endif()
# --------------


# --------------
# Load cmake modules
# --------------
include(CMakeParseArguments)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
# --------------


# --------------
# Dependencies
# --------------
# Eigen
find_package(Eigen3 REQUIRED)
include_directories (SYSTEM "${EIGEN3_INCLUDE_DIR}")

# eigenut
option(EIGENUT_EMBEDDED "" ON)
set(EIGENUT_EMBEDDED_ID "QPMAD"  CACHE STRING "")
set(EIGENUT_EMBEDDED_INSTALL_DIR "${QPMAD_INCLUDE_INSTALL_DESTINATION}"  CACHE STRING "")
set(EIGENUT_SELECT_HEADERS "eigenut_config.h;eigenut_types.h;eigenut_misc.h"  CACHE STRING "")
add_subdirectory(eigenut)
get_property(EIGENUT_INCLUDE_DIRECTORIES DIRECTORY "eigenut" PROPERTY INCLUDE_DIRECTORIES)
include_directories(${EIGENUT_INCLUDE_DIRECTORIES})
# --------------


# --------------
# Configure headers
# --------------
set(QPMAD_SOURCE_DIR   "${PROJECT_SOURCE_DIR}/include/")
include_directories("${QPMAD_SOURCE_DIR}")

configure_file( "${QPMAD_SOURCE_DIR}/qpmad/config.h.in"
                "${QPMAD_SOURCE_DIR}/qpmad/config.h")
# --------------


# --------------
# tests
# --------------
if (QPMAD_BUILD_TESTS)
    enable_testing()
    add_subdirectory("${PROJECT_SOURCE_DIR}/test")
endif(QPMAD_BUILD_TESTS)
# --------------


# --------------
# Install
# --------------
if (NOT QPMAD_CATKIN_BUILD)
    install (FILES "${PROJECT_BINARY_DIR}/qpmadConfig.cmake"
             DESTINATION "${QPMAD_INSTALL_PATH}/")

    install (FILES "${PROJECT_BINARY_DIR}/qpmadConfigVersion.cmake"
             DESTINATION "${QPMAD_INSTALL_PATH}/")
endif()

install (DIRECTORY "${QPMAD_SOURCE_DIR}/qpmad"
         DESTINATION "${CMAKE_INSTALL_PREFIX}/include/qpmad/"
         FILES_MATCHING PATTERN "*.h")
# --------------
