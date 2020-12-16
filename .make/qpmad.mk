ROOT_DIR=../
BUILD_DIR?=build
MAKE_FLAGS?=-j5
VERSION?="XXX__version_not_set__XXX"

TYPE?=Debug
TRACING?=OFF

PKG=qpmad
REPO=https://github.com/asherikov/${PKG}.git
DEPENDENCY_PATH=test/dependency/
DEBIAN_SYSTEM_DEPENDENCIES=libeigen3-dev

CATKIN_PKG=${PKG}
CATKIN_DEPENDENCY_PATH=${DEPENDENCY_PATH}
CATKIN_DEPENDENCY_TEST_PKG=${PKG}_catkin_dependency_test
CATKIN_ARGS=
CATKIN_TARGETS=all


build:
	${MAKE}	cmake OPTIONS=default

install:
	cd ${BUILD_DIR}; ${MAKE} install


test: clean unit_tests_debug unit_tests_householder unit_tests_default test_octave

test_octave:
	cd matlab_octave; ${MAKE} octave octave_test

test_dependency: clean
	mkdir -p build/dependency_test
	cd build/dependency_test; cmake ../../${DEPENDENCY_PATH}
	cd build/dependency_test; ${MAKE} ${MAKE_FLAGS}


# build targets
#----------------------------------------------

unit_tests_householder:
	${MAKE}	cmake OPTIONS=householder
	cd ${BUILD_DIR}; ${MAKE} test

unit_tests_debug:
	${MAKE}	cmake OPTIONS=debug
	cd ${BUILD_DIR}; ${MAKE} test

unit_tests_default:
	${MAKE}	cmake OPTIONS=testdefault
	cd ${BUILD_DIR}; ${MAKE} test

cmake:
	mkdir -p ${BUILD_DIR};
	cd ${BUILD_DIR}; cmake -C ${ROOT_DIR}/cmake/options_${OPTIONS}.cmake ${ROOT_DIR}
	cd ${BUILD_DIR}; ${MAKE} ${MAKE_FLAGS}


# utils
#----------------------------------------------

addutils:
	git remote add cmakeut https://github.com/asherikov/cmakeut --no-tags
	git remote add eigenut https://github.com/asherikov/eigenut --no-tags

updateutils:
	git fetch --all
	git rm --ignore-unmatch -rf eigenut
	git read-tree --prefix=eigenut -u eigenut/master
	git show remotes/cmakeut/master:cmake/FindEigen3.cmake                  > cmake/FindEigen3.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_compiler_flags.cmake      > cmake/cmakeut_compiler_flags.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_detect_func_macro.cmake   > cmake/cmakeut_detect_func_macro.cmake


# doxygen
#----------------------------------------------

doxclean:
	cd doc/dox; git fetch --all; git checkout gh-pages; git pull
	find ./doc/dox/ -mindepth 1 -not -name "\.git" | xargs rm -Rf

dox: doxclean clean
	${MAKE} build
	cd doc; doxygen


# clean
#----------------------------------------------

clean:
	rm -Rf ${BUILD_DIR}
	rm -Rf include/qpmad/config.h
	rm -Rf include/qpmad/eigenut*.h
	rm -Rf debian/ obj-*/
	cd matlab_octave; ${MAKE} clean
	cd eigenut; ${MAKE} clean

forceclean: clean
	cd matlab_octave; ${MAKE} forceclean

.PHONY: build cmake test
