# General parameters
cmake_minimum_required(VERSION 3.0)
project(qpmad VERSION 1.0.2)


# --------------
# Options
# --------------
set(CMAKE_VERBOSE_MAKEFILE          ON)
option(QPMAD_BUILD_TESTS        "Build tests"                                           OFF)
option(QPMAD_ENABLE_TRACING     "Enable tracing"                                        OFF)
option(QPMAD_USE_HOUSEHOLDER    "Use Housholder matrices for updating factorizations"   OFF)
# --------------


# --------------
# Process options
# --------------
if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

set(QPMAD_SOURCE_DIR   "${PROJECT_SOURCE_DIR}/include/")
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
set(Eigen_INCLUDE_DIRS ${EIGEN3_INCLUDE_DIR})
# --------------


# --------------
# Packaging
# --------------
set(QPMAD_INCLUDE_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
set(QPMAD_CMAKE_INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/share/${CMAKE_PROJECT_NAME}/cmake/")

include(CMakePackageConfigHelpers)
configure_package_config_file(  "cmake/${CMAKE_PROJECT_NAME}Config.cmake.in"
                                "${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
                                INSTALL_DESTINATION "${QPMAD_CMAKE_INSTALL_DESTINATION}"
                                NO_SET_AND_CHECK_MACRO
                                NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
    ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    COMPATIBILITY SameMajorVersion)
# --------------


# --------------
# Dependencies
# --------------
# eigenut
option(EIGENUT_EMBEDDED "" ON)
set(EIGENUT_EMBEDDED_ID "QPMAD_UTILS"  CACHE STRING "")
set(EIGENUT_EMBEDDED_COPY_TO_DIR "${QPMAD_SOURCE_DIR}/${CMAKE_PROJECT_NAME}"  CACHE STRING "")
set(EIGENUT_SELECT_HEADERS "config.h;types.h;misc.h"  CACHE STRING "")
add_subdirectory(eigenut)
# --------------


# --------------
# Configure headers
# --------------
include_directories("${QPMAD_SOURCE_DIR}")

configure_file( "${QPMAD_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/config.h.in"
                "${QPMAD_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/config.h")


get_property(QPMAD_OCTAVE_INCLUDES DIRECTORY "${PROJECT_SOURCE_DIR}" PROPERTY INCLUDE_DIRECTORIES)
list(REMOVE_DUPLICATES QPMAD_OCTAVE_INCLUDES)
string(REPLACE ";" " -I" QPMAD_OCTAVE_INCLUDES "${QPMAD_OCTAVE_INCLUDES}")
set(QPMAD_OCTAVE_INCLUDES "-I${QPMAD_OCTAVE_INCLUDES}")

configure_file( "${PROJECT_SOURCE_DIR}/matlab_octave/make.m.in"
                "${PROJECT_SOURCE_DIR}/matlab_octave/make.m")
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
install (FILES "${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}Config.cmake"
         DESTINATION "${QPMAD_CMAKE_INSTALL_DESTINATION}/")

install (FILES "${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION "${QPMAD_CMAKE_INSTALL_DESTINATION}/")

install (DIRECTORY "${QPMAD_SOURCE_DIR}/${CMAKE_PROJECT_NAME}/"
         DESTINATION "${QPMAD_INCLUDE_INSTALL_DESTINATION}"
         FILES_MATCHING PATTERN "*.h")
# --------------
