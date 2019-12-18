# General parameters
cmake_minimum_required (VERSION 3.0.0)
project (eigenut CXX)


# --------------
# Options
# --------------
set(CMAKE_VERBOSE_MAKEFILE          ON)
option(EIGENUT_EMBEDDED             "Embedded in another project" OFF)
option(EIGENUT_BUILD_TESTS          "Build tests" ON)
set(EIGENUT_EMBEDDED_ID             "" CACHE STRING "Overrides header guards, namespace.")
set(EIGENUT_EMBEDDED_COPY_TO_DIR    "" CACHE STRING "Installation destination.")
set(EIGENUT_SELECT_HEADERS          "" CACHE STRING "Selection a subset of headers for installation.")
# --------------


# --------------
# Process options
# --------------
set(EIGENUT_ID "EIGENUT")
if (EIGENUT_EMBEDDED)
    set(EIGENUT_BUILD_TESTS          OFF)
    if(EIGENUT_EMBEDDED_ID)
        set(EIGENUT_ID "${EIGENUT_EMBEDDED_ID}")
    endif()
    if(NOT EIGENUT_EMBEDDED_COPY_TO_DIR)
        message(FATAL_ERROR "EIGENUT_EMBEDDED_COPY_TO_DIR must be set with EIGENUT_EMBEDDED=ON")
    endif()
else()
    if (EIGENUT_EMBEDDED_ID OR EIGENUT_EMBEDDED_COPY_TO_DIR)
        message(WARNING "EIGENUT_EMBEDDED_ID and EIGENUT_EMBEDDED_COPY_TO_DIR ignored since EIGENUT_EMBEDDED=OFF")
    endif()

    # Detect catkin
    if(CATKIN_DEVEL_PREFIX)
        message(STATUS "Building with catkin.")
        set(EIGENUT_CATKIN_BUILD ON)

        if(CATKIN_ENABLE_TESTING)
            set(EIGENUT_BUILD_TESTS ON)
        else()
            set(EIGENUT_BUILD_TESTS OFF)
        endif()
    endif()
endif()

string(TOLOWER "${EIGENUT_ID}" EIGENUT_ID_LOWER_CASE)

set(EIGENUT_HEADER_DIR "${PROJECT_SOURCE_DIR}/include/eigenut")
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
if (EIGENUT_CATKIN_BUILD)
    find_package(catkin REQUIRED)
    catkin_package(
        INCLUDE_DIRS include
        DEPENDS Eigen
    )
else()
    set(PROJECT_VERSION_MAJOR 1)
    set(PROJECT_VERSION_MINOR 0)
    set(PROJECT_VERSION_PATCH 1)

    if (NOT EIGENUT_EMBEDDED)
        include(CMakePackageConfigHelpers)
        set(EIGENUT_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/share/eigenut/")
        set(EIGENUT_INCLUDES "${CMAKE_INSTALL_PREFIX}/include")

        configure_package_config_file(  "cmake/eigenutConfig.cmake.in"
                                        "${PROJECT_BINARY_DIR}/eigenutConfig.cmake"
                                        INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/share/eigenut/"
                                        NO_SET_AND_CHECK_MACRO
                                        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

        write_basic_package_version_file(
            ${PROJECT_BINARY_DIR}/eigenutConfigVersion.cmake
            VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
            COMPATIBILITY SameMajorVersion)
    endif()
endif()

if (EIGENUT_CATKIN_BUILD)
    set(EIGENUT_INCLUDE_INSTALL_DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
else()
    if (NOT EIGENUT_EMBEDDED)
        set(EIGENUT_INCLUDE_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${EIGENUT_ID_LOWER_CASE})
    endif()
endif()
# --------------


# --------------
# Dependencies
# --------------
# cpput
option(CPPUT_EMBEDDED "" ON)
set(CPPUT_EMBEDDED_ID "${EIGENUT_ID}"  CACHE STRING "")
if (EIGENUT_EMBEDDED_COPY_TO_DIR)
    set(CPPUT_EMBEDDED_COPY_TO_DIR "${EIGENUT_EMBEDDED_COPY_TO_DIR}"  CACHE STRING "")
else()
    set(CPPUT_EMBEDDED_COPY_TO_DIR "${EIGENUT_HEADER_DIR}"  CACHE STRING "")
endif()
set(CPPUT_SELECT_HEADERS "config.h;exception.h;visibility.h"  CACHE STRING "")
add_subdirectory(cpput)
# --------------


# --------------
# Configure headers
# --------------
include(cmakeut_list_filenames)

cmakeut_list_filenames("${EIGENUT_HEADER_DIR}/" "EIGENUT_HEADERS")
foreach (EIGENUT_HEADER ${EIGENUT_HEADERS})
    if (${EIGENUT_HEADER} MATCHES ".h.in\$")
        string(REPLACE ".h.in" ".h" EIGENUT_HEADER_NO_SUFFIX "${EIGENUT_HEADER}")
        message("Configuring header ${EIGENUT_HEADER} -> ${EIGENUT_HEADER_NO_SUFFIX}")
        configure_file( "${EIGENUT_HEADER_DIR}/${EIGENUT_HEADER}"
                        "${EIGENUT_HEADER_DIR}/${EIGENUT_HEADER_NO_SUFFIX}")
    endif()
endforeach()

include_directories ("${PROJECT_SOURCE_DIR}/include")
# --------------


# --------------
# Tests
# --------------
if(EIGENUT_BUILD_TESTS)
    enable_testing()
    add_subdirectory("${PROJECT_SOURCE_DIR}/test")
endif()
# --------------


# --------------
# Install
# --------------
if (EIGENUT_EMBEDDED)
    if (NOT EIGENUT_SELECT_HEADERS)
        cmakeut_list_filenames("${EIGENUT_HEADER_DIR}/" "EIGENUT_HEADERS")
        foreach (EIGENUT_HEADER ${EIGENUT_HEADERS})
            if (NOT ${EIGENUT_HEADER} MATCHES ".h.in\$" AND NOT ${EIGENUT_HEADER} MATCHES "all.h")
                list(APPEND EIGENUT_SELECT_HEADERS "${EIGENUT_HEADER}")
            endif()
        endforeach()
    endif()

    if (EIGENUT_SELECT_HEADERS)
        foreach (EIGENUT_HEADER ${EIGENUT_SELECT_HEADERS})
            if (EIGENUT_HEADER)
                file(   COPY        "${EIGENUT_HEADER_DIR}/${EIGENUT_HEADER}"
                        DESTINATION "${PROJECT_BINARY_DIR}/")
                file(   RENAME      "${PROJECT_BINARY_DIR}/${EIGENUT_HEADER}"
                                    "${EIGENUT_EMBEDDED_COPY_TO_DIR}/eigenut_${EIGENUT_HEADER}")
            endif()
        endforeach()
    endif()
else()
    if (NOT EIGENUT_CATKIN_BUILD)
        install (FILES "${PROJECT_BINARY_DIR}/eigenutConfig.cmake"
                 DESTINATION "${EIGENUT_INSTALL_PATH}/")

        install (FILES "${PROJECT_BINARY_DIR}/eigenutConfigVersion.cmake"
                 DESTINATION "${EIGENUT_INSTALL_PATH}/")
    endif()

    if (EIGENUT_SELECT_HEADERS)
        foreach (EIGENUT_HEADER ${EIGENUT_SELECT_HEADERS})
            if (EIGENUT_HEADER)
                install (FILES "${EIGENUT_HEADER_DIR}/${EIGENUT_HEADER}"
                         DESTINATION "${EIGENUT_INCLUDE_INSTALL_DESTINATION}/")
            endif()
        endforeach()
    else()
        install (DIRECTORY "${EIGENUT_HEADER_DIR}/"
                 DESTINATION "${EIGENUT_INCLUDE_INSTALL_DESTINATION}"
                 FILES_MATCHING PATTERN "*.h")
    endif()
endif()
# --------------
