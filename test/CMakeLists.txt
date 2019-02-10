include(cmakeut_compiler_flags)
cmakeut_compiler_flags("c++98")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKEUT_CXX_FLAGS} -Wno-c99-extensions")

include(qpmad_add_test)

find_package(Boost REQUIRED unit_test_framework timer system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

get_cmake_property(VARNAMES VARIABLES)

qpmad_add_test("test_givens" "givens.cpp")
qpmad_add_test("test_cholesky" "cholesky.cpp")
qpmad_add_test("test_inverse" "inverse.cpp")
qpmad_add_test("test_solver" "solver.cpp")
