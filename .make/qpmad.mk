FIND_SOURCES=find ./matlab_octave ./test/ ./include/ -iname "*.h" -or -iname "*.cpp" | grep -v "cpput_"

OPTIONS?=default
ROOT_DIR=../../
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

install_latest_eigen:
	mkdir -p ${BUILD_DIR};
	cd ${BUILD_DIR}; git clone https://gitlab.com/libeigen/eigen.git
	mkdir -p ${BUILD_DIR}/eigen/build
	cd ${BUILD_DIR}/eigen/build; cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ..; sudo make install


format:
	${FIND_SOURCES} | grep -v "cpput" | grep -v "eigenut" | xargs ${CLANG_FORMAT} -verbose -i


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

dox: doxclean clean
	${MAKE} build
	cd doc; doxygen


# deb
#----------------------------------------------

deb-cloudsmith: deb
	ls qpmad-*-any.deb | xargs --no-run-if-empty -I {} cloudsmith push deb asherikov-aV7/qpmad/any-distro/any-version {}


# clean
#----------------------------------------------

clean: clean_common
	rm -Rf include/qpmad/config.h
	rm -Rf include/qpmad/cpput*.h
	rm -Rf debian/ obj-*/
	cd matlab_octave; ${MAKE} clean
	cd cpput; ${MAKE} clean

forceclean: clean
	cd matlab_octave; ${MAKE} forceclean
