BUILD_DIR?=build
MAKE_FLAGS?=-j5

TYPE?=Debug
TRACING?=OFF
EXTRA_CMAKE_PARAM?=
#EXTRA_CMAKE_PARAM?="-DCMAKE_CXX_COMPILER=g++"


default:
	@grep -v "^	" Makefile | grep -v "^$$"


test: clean unit_tests_without_tracing unit_tests_with_tracing unit_tests_with_householder test_octave

test_octave:
	cd matlab_octave; ${MAKE} octave octave_test

build: build_without_tracing


build_with_householder:
	${MAKE} cmake TYPE=Debug TRACING=OFF EXTRA_CMAKE_PARAM="-DQPMAD_USE_HOUSEHOLDER=ON"

unit_tests_with_householder: build_with_householder
	cd ${BUILD_DIR}; ${MAKE} test


build_without_tracing:
	${MAKE}	cmake TYPE=Debug TRACING=OFF

unit_tests_without_tracing: build_without_tracing
	cd ${BUILD_DIR}; ${MAKE} test


build_with_tracing:
	${MAKE}	cmake TYPE=Debug TRACING=ON

unit_tests_with_tracing: build_with_tracing
	cd ${BUILD_DIR}; ${MAKE} test


release:
	${MAKE}	cmake TYPE=Release TRACING=OFF


cmake:
	mkdir -p ${BUILD_DIR};
	cd ${BUILD_DIR}; cmake .. 	-DCMAKE_BUILD_TYPE=${TYPE} \
						-DQPMAD_ENABLE_TRACING=${TRACING} \
						${EXTRA_CMAKE_PARAM}
	cd ${BUILD_DIR}; ${MAKE} ${MAKE_FLAGS}


clean:
	rm -Rf ${BUILD_DIR}
	rm -Rf include/qpmad/config.h
	rm -Rf include/qpmad/cpput*.h
	rm -Rf include/qpmad/eigenut*.h
	cd matlab_octave; ${MAKE} clean
	cd eigenut; ${MAKE} clean

forceclean: clean
	cd matlab_octave; ${MAKE} forceclean


addutils:
	git remote add cmakeut https://github.com/asherikov/cmakeut
	git remote add eigenut https://github.com/asherikov/eigenut

updateutils:
	git fetch --all
	git rm --ignore-unmatch -rf eigenut
	git read-tree --prefix=eigenut -u eigenut/master
	git show remotes/cmakeut/master:cmake/FindEigen3.cmake                  > cmake/FindEigen3.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_add_cpp_test.cmake        > cmake/cmakeut_add_cpp_test.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_compiler_flags.cmake      > cmake/cmakeut_compiler_flags.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_detect_func_macro.cmake   > cmake/cmakeut_detect_func_macro.cmake

.PHONY: build cmake test
