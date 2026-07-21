# General parameters
cmake_minimum_required(VERSION 3.10.0)
project(qpmad VERSION 1.5.0)

# --------------
# Options
# --------------
set(CMAKE_VERBOSE_MAKEFILE          ON)

option(QPMAD_BUILD_TESTS        "Build tests"                                                       OFF)
option(QPMAD_ENABLE_TRACING     "Enable tracing"                                                    OFF)
option(QPMAD_CPP_SANITIZERS     "Enable sanitizers"                                                 OFF)
option(QPMAD_CLANG_TIDY         "Enable clang tidy"                                                 OFF)
option(QPMAD_PEDANTIC_LICENSE   "Be pedantic regarding licensing, some features may be disabled"    OFF)

option(QPMAD_USE_HOUSEHOLDER    "Use Housholder matrices for updating factorizations"               OFF)
# --------------


# --------------
# Process options
# --------------
if(NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE RelWithDebInfo)
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
find_package(Eigen3 REQUIRED NO_MODULE)
# --------------


# --------------
# C++ flags
# --------------
include(ccws_set_cmake_flags OPTIONAL RESULT_VARIABLE CCWS_ON)
if(NOT CCWS_ON)
    include(cmakeut_compiler_flags)
    set (CMAKEUT_CPP_SANITIZERS ${QPMAD_CPP_SANITIZERS})
    set (CMAKEUT_CLANG_TIDY ${QPMAD_CLANG_TIDY})
    # boost test issues
    set (CMAKEUT_CLANG_TIDY_EXTRA_IGNORES "-cert-err58-cpp,-cppcoreguidelines-avoid-non-const-global-variables")
    # Eigen issues
    set (CMAKEUT_CLANG_TIDY_EXTRA_IGNORES "${CMAKEUT_CLANG_TIDY_EXTRA_IGNORES},-clang-analyzer-core.uninitialized.Assign,-clang-analyzer-security.ArrayBound")
    cmakeut_compiler_flags("c++14")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKEUT_CXX_FLAGS}")
endif()
# --------------


# --------------
# Configure headers
# --------------
configure_file( "${QPMAD_SOURCE_DIR}/${PROJECT_NAME}/config.h.in"
                "${QPMAD_SOURCE_DIR}/${PROJECT_NAME}/config.h")
# --------------


# --------------
# library
# --------------
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
    "$<BUILD_INTERFACE:${QPMAD_SOURCE_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)
target_link_libraries(${PROJECT_NAME} INTERFACE Eigen3::Eigen)
set_property(TARGET ${PROJECT_NAME} PROPERTY INTERFACE_${PROJECT_NAME}_MAJOR_VERSION ${PROJECT_VERSION_MAJOR})
set_property(TARGET ${PROJECT_NAME} APPEND PROPERTY COMPATIBLE_INTERFACE_STRING ${PROJECT_VERSION_MAJOR})

install(
    TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
    INCLUDES DESTINATION include
)
install (
    DIRECTORY "${QPMAD_SOURCE_DIR}/${PROJECT_NAME}/"
    DESTINATION "include/${PROJECT_NAME}"
    FILES_MATCHING PATTERN "*.h"
)
# --------------

# --------------
# MATLAB/Octave interface
# --------------
get_target_property(QPMAD_OCTAVE_INCLUDES Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)
list(REMOVE_DUPLICATES QPMAD_OCTAVE_INCLUDES)
string(REPLACE ";" " -I" QPMAD_OCTAVE_INCLUDES "${QPMAD_OCTAVE_INCLUDES}")
set(QPMAD_OCTAVE_INCLUDES "-I${QPMAD_SOURCE_DIR} -I${QPMAD_OCTAVE_INCLUDES}")

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
# Packaging
# --------------
export(EXPORT ${PROJECT_NAME}
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${PROJECT_NAME}::
)

install(EXPORT ${PROJECT_NAME}
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION share/${PROJECT_NAME}/cmake
)


include(CMakePackageConfigHelpers)
configure_package_config_file(
    "cmake/${PROJECT_NAME}Config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION "share/${PROJECT_NAME}/cmake"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION share/${PROJECT_NAME}/cmake
)
# --------------
