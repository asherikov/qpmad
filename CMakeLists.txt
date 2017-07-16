# General parameters
cmake_minimum_required (VERSION 2.8.12)
project (qpmad CXX)

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
set (CMAKE_VERBOSE_MAKEFILE     ON)


option(QPMAD_BUILD_TESTS        "Build tests"                                           ON)
option(QPMAD_ENABLE_TRACING     "Enable tracing"                                        OFF)
option(QPMAD_USE_HOUSEHOLDER    "Use Housholder matrices for updating factorizations"   OFF)


if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

set (CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -fPIC -pedantic -Wall")
set (CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -Wempty-body -Wignored-qualifiers -Wmissing-field-initializers")
set (CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -Wsign-compare -Wtype-limits -Wuninitialized -Woverloaded-virtual")
set (CMAKE_CXX_FLAGS            "${CMAKE_CXX_FLAGS} -std=c++98")


find_package(Eigen3 REQUIRED)
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})


set(QPMAD_SOURCE_DIR   "${PROJECT_SOURCE_DIR}/src")
include_directories("${QPMAD_SOURCE_DIR}")

configure_file("cmake/config.h.in"        "${QPMAD_SOURCE_DIR}/config.h")


if (QPMAD_BUILD_TESTS)
    enable_testing()
    include(qpmad_add_test)
    add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
endif(QPMAD_BUILD_TESTS)


install (DIRECTORY "${QPMAD_SOURCE_DIR}/"
         DESTINATION "${CMAKE_INSTALL_PREFIX}/include/qpmad/")
