cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_VERBOSE_MAKEFILE ON)


project(qpmad_cmake_dependency_test)

find_package(qpmad REQUIRED)

get_target_property(QPMAD_EIGEN_INCLUDES qpmad::qpmad INTERFACE_INCLUDE_DIRECTORIES)
message(WARNING "includes: ${qpmad_INCLUDE_DIRS}")

add_executable(demo ../catkin/demo.cpp)
target_link_libraries(demo qpmad::qpmad)
