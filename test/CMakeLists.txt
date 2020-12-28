include(qpmad_add_test)

find_package(Boost REQUIRED unit_test_framework timer system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

get_cmake_property(VARNAMES VARIABLES)

qpmad_add_test("test_givens" "givens.cpp")
qpmad_add_test("test_inverse" "inverse.cpp")
qpmad_add_test("test_solver" "solver.cpp")
qpmad_add_test("test_resolve" "resolve.cpp")
if(NOT QPMAD_ENABLE_TRACING)
    qpmad_add_test("test_allocation" "allocation.cpp")
endif()
