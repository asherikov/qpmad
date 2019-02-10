include(cmakeut_compiler_flags)
cmakeut_compiler_flags("c++03")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKEUT_CXX_FLAGS}")


set(TEST_LIBS "")
set(TEST_TARGETS "")
set(TEST_FLAGS "")
include(cmakeut_add_cpp_test)

# --------------
find_package(Boost REQUIRED unit_test_framework timer system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
set(TEST_LIBS "${TEST_LIBS};${Boost_UNIT_TEST_FRAMEWORK_LIBRARIES};${Boost_SYSTEM_LIBRARIES};${Boost_TIMER_LIBRARIES}")
set(TEST_FLAGS "${TEST_FLAGS};-Wno-c99-extensions")
# --------------


cmakeut_add_cpp_test(inclusion LIBS "${TEST_LIBS}" FLAGS "${TEST_FLAGS}")
cmakeut_add_cpp_test(blockmatrix LIBS "${TEST_LIBS}" FLAGS "${TEST_FLAGS}")
cmakeut_add_cpp_test(misc LIBS "${TEST_LIBS}" FLAGS "${TEST_FLAGS}")
