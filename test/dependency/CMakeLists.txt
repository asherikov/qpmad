cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_VERBOSE_MAKEFILE ON)


if (DEFINED CATKIN_DEVEL_PREFIX)
    project(qpmad_catkin_dependency_test)

    find_package(catkin REQUIRED qpmad)

    catkin_package(DEPENDS qpmad)

    include_directories(
        include
        SYSTEM ${catkin_INCLUDE_DIRS}
    )

    add_executable(demo demo.cpp)

    target_link_libraries(demo ${catkin_LIBRARIES})

    install(TARGETS demo
        ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
        RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
    )
else()
    project(qpmad_cmake_dependency_test)

    find_package(qpmad REQUIRED)

    include_directories(${qpmad_INCLUDE_DIRS})

    add_executable(demo demo.cpp)
    target_link_libraries(demo ${qpmad_LIBRARIES})
endif()
