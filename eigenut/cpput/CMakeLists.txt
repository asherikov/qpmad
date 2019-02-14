# General parameters
cmake_minimum_required (VERSION 3.0.0)
project (cpput CXX)


# --------------
# Options
# --------------
set(CMAKE_VERBOSE_MAKEFILE      ON)
option(CPPUT_EMBEDDED           "Embedded in another project" OFF)
option(CPPUT_BUILD_TESTS        "Build tests" ON)
set(CPPUT_EMBEDDED_ID           "" CACHE STRING "Overrides header guards, namespace (does not apply to 3rdparty headers).")
set(CPPUT_EMBEDDED_COPY_TO_DIR  "" CACHE STRING "Installation destination.")
set(CPPUT_SELECT_HEADERS        "" CACHE STRING "Selection a subset of headers for installation.")
# --------------


# --------------
# Process options
# --------------
set(CPPUT_ID "CPPUT")
if (CPPUT_EMBEDDED)
    set(CPPUT_BUILD_TESTS          OFF)
    if(CPPUT_EMBEDDED_ID)
        set(CPPUT_ID "${CPPUT_EMBEDDED_ID}")
    endif()
    if(NOT CPPUT_EMBEDDED_COPY_TO_DIR)
        message(FATAL_ERROR "CPPUT_EMBEDDED_COPY_TO_DIR must be set with CPPUT_EMBEDDED=ON")
    endif()
else()
    if (CPPUT_EMBEDDED_ID OR CPPUT_EMBEDDED_COPY_TO_DIR)
        message(WARNING "CPPUT_EMBEDDED_ID and CPPUT_EMBEDDED_COPY_TO_DIR ignored since CPPUT_EMBEDDED=OFF")
    endif()

    # Detect catkin
    if(CATKIN_DEVEL_PREFIX)
        message(STATUS "Building with catkin.")
        set(CPPUT_CATKIN_BUILD ON)

        if(CATKIN_ENABLE_TESTING)
            set(CPPUT_BUILD_TESTS ON)
        else()
            set(CPPUT_BUILD_TESTS OFF)
        endif()
    endif()
endif()

string(TOLOWER "${CPPUT_ID}" CPPUT_ID_LOWER_CASE)

set(CPPUT_HEADER_DIR "${PROJECT_SOURCE_DIR}/include/cpput")
# --------------


# --------------
# Packaging & installation paths
# --------------
if (CPPUT_CATKIN_BUILD)
    find_package(catkin REQUIRED)
    catkin_package(INCLUDE_DIRS include)
else()
    set(PROJECT_VERSION_MAJOR 1)
    set(PROJECT_VERSION_MINOR 0)
    set(PROJECT_VERSION_PATCH 0)

    if (NOT CPPUT_EMBEDDED)
        include(CMakePackageConfigHelpers)
        set(CPPUT_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/share/cpput/")
        set(CPPUT_INCLUDES "${CMAKE_INSTALL_PREFIX}/include")

        configure_package_config_file(  "cmake/cpputConfig.cmake.in"
                                        "${PROJECT_BINARY_DIR}/cpputConfig.cmake"
                                        INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cpput/"
                                        NO_SET_AND_CHECK_MACRO
                                        NO_CHECK_REQUIRED_COMPONENTS_MACRO)

        write_basic_package_version_file(
            ${PROJECT_BINARY_DIR}/cpputConfigVersion.cmake
            VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
            COMPATIBILITY SameMajorVersion)
    endif()
endif()


if (CPPUT_CATKIN_BUILD)
    set(CPPUT_INCLUDE_INSTALL_DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
else()
    if (NOT CPPUT_EMBEDDED)
        set(CPPUT_INCLUDE_INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${CPPUT_ID_LOWER_CASE})
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
# Configure headers
# --------------
include(cmakeut_list_filenames)
include(cmakeut_detect_func_macro)
cmakeut_detect_func_macro()

cmakeut_list_filenames("${CPPUT_HEADER_DIR}/" "CPPUT_HEADERS")
foreach (CPPUT_HEADER ${CPPUT_HEADERS})
    if (${CPPUT_HEADER} MATCHES ".h.in\$")
        string(REPLACE ".h.in" ".h" CPPUT_HEADER_NO_SUFFIX "${CPPUT_HEADER}")
        message("Configuring header ${CPPUT_HEADER} -> ${CPPUT_HEADER_NO_SUFFIX}")
        configure_file( "${CPPUT_HEADER_DIR}/${CPPUT_HEADER}"
                        "${CPPUT_HEADER_DIR}/${CPPUT_HEADER_NO_SUFFIX}")
    endif()
endforeach()

include_directories ("${PROJECT_SOURCE_DIR}/include")
# --------------


# --------------
# tests
# --------------
if(CPPUT_BUILD_TESTS)
    enable_testing()
    add_subdirectory("${PROJECT_SOURCE_DIR}/test")
endif()
# --------------


# --------------
# Install
# --------------
if (CPPUT_EMBEDDED)
    if (NOT CPPUT_SELECT_HEADERS)
        cmakeut_list_filenames("${CPPUT_HEADER_DIR}/" "CPPUT_HEADERS")
        foreach (CPPUT_HEADER ${CPPUT_HEADERS})
            if (NOT ${CPPUT_HEADER} MATCHES ".h.in\$" AND NOT ${CPPUT_HEADER} MATCHES "all.h")
                list(APPEND CPPUT_SELECT_HEADERS "${CPPUT_HEADER}")
            endif()
        endforeach()
    endif()

    if (CPPUT_SELECT_HEADERS)
        foreach (CPPUT_HEADER ${CPPUT_SELECT_HEADERS})
            if (CPPUT_HEADER)
                file(   COPY        "${CPPUT_HEADER_DIR}/${CPPUT_HEADER}"
                        DESTINATION "${PROJECT_BINARY_DIR}/")
                file(   RENAME      "${PROJECT_BINARY_DIR}/${CPPUT_HEADER}"
                                    "${CPPUT_EMBEDDED_COPY_TO_DIR}/cpput_${CPPUT_HEADER}")
            endif()
        endforeach()
    endif()
else()
    if (NOT CPPUT_CATKIN_BUILD)
        install (FILES "${PROJECT_BINARY_DIR}/cpputConfig.cmake"
                 DESTINATION "${CPPUT_INSTALL_PATH}/")

        install (FILES "${PROJECT_BINARY_DIR}/cpputConfigVersion.cmake"
                 DESTINATION "${CPPUT_INSTALL_PATH}/")
    endif()

    if (CPPUT_SELECT_HEADERS)
        foreach (CPPUT_HEADER ${CPPUT_SELECT_HEADERS})
            if (CPPUT_HEADER)
                install (FILES "${CPPUT_HEADER_DIR}/${CPPUT_HEADER}"
                         DESTINATION "${CPPUT_INCLUDE_INSTALL_DESTINATION}/")
            endif()
        endforeach()
    else()
        install (DIRECTORY "${CPPUT_HEADER_DIR}/"
                 DESTINATION "${CPPUT_INCLUDE_INSTALL_DESTINATION}"
                 FILES_MATCHING PATTERN "*.h")
    endif()
endif()
# --------------
