OPTIONS?=default
ROOT_DIR=../../
BUILD_ROOT?=build/
BUILD_DIR?=${BUILD_ROOT}/${OPTIONS}
MAKE_FLAGS?=-j5
VERSION?="XXX__version_not_set__XXX"

TYPE?=Debug

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


test: clean
	${MAKE} cppcheck
	${MAKE} spell
	${MAKE}	unit_tests OPTIONS=householder
	${MAKE}	unit_tests OPTIONS=testdefault
	${MAKE} unit_tests OPTIONS=debug
	${MAKE}	test_octave

test_octave:
	cd matlab_octave; ${MAKE} octave octave_test

test_dependency: clean
	mkdir -p build/dependency_test
	cd build/dependency_test; cmake ../../${DEPENDENCY_PATH}
	cd build/dependency_test; ${MAKE} ${MAKE_FLAGS}


# build targets
#----------------------------------------------

unit_tests:
	${MAKE}	cmake OPTIONS=${OPTIONS}
	${MAKE}	ctest OPTIONS=${OPTIONS}

cmake:
	mkdir -p ${BUILD_DIR};
	cd ${BUILD_DIR}; cmake -C ${ROOT_DIR}/cmake/options_${OPTIONS}.cmake ${ROOT_DIR}
	cd ${BUILD_DIR}; ${MAKE} ${MAKE_FLAGS}

ctest:
	cd ${BUILD_DIR}; ctest --schedule-random --output-on-failure


# utils
#----------------------------------------------

addutils:
	-git remote add cmakeut https://github.com/asherikov/cmakeut --no-tags
	-git remote add cpput https://github.com/asherikov/cpput --no-tags

updateutils:
	git fetch --all
	git rm --ignore-unmatch -rf cpput
	git read-tree --prefix=cpput -u cpput/master
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


# deb
#----------------------------------------------
deb:
	${MAKE} cmake OPTIONS=deb
	${MAKE} install OPTIONS=deb
	grep "<version>" package.xml | grep -o "[0-9]*\.[0-9]*\.[0-9]*" > build/version
	fpm -t deb --depends ${DEBIAN_SYSTEM_DEPENDENCIES} --version `cat build/version` --package qpmad-`cat build/version`-any.deb

deb-cloudsmith:
	ls qpmad-*-any.deb | xargs --no-run-if-empty -I {} cloudsmith push deb asherikov-aV7/qpmad/any-distro/any-version {}


# clean
#----------------------------------------------

clean:
	rm -Rf ${BUILD_ROOT}
	rm -Rf include/qpmad/config.h
	rm -Rf include/qpmad/cpput*.h
	rm -Rf debian/ obj-*/
	cd matlab_octave; ${MAKE} clean
	cd cpput; ${MAKE} clean

forceclean: clean
	cd matlab_octave; ${MAKE} forceclean

.PHONY: build cmake test
