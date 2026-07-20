FIND_SOURCES=find ./matlab_octave ./test/ ./include/ -iname "*.h" -or -iname "*.cpp"

OPTIONS?=default
ROOT_DIR=../../
MAKE_FLAGS?=-j5
VERSION?="XXX__version_not_set__XXX"

TYPE?=Debug

PKG=qpmad
REPO=https://github.com/asherikov/${PKG}.git
DEBIAN_SYSTEM_DEPENDENCIES=libeigen3-dev

CATKIN_PKG=${PKG}
CATKIN_DEPENDENCY_TEST_PKG=${PKG}_catkin_dependency_test
CATKIN_ARGS=
CATKIN_TARGETS=all


build:
	${MAKE}	cmake OPTIONS=default


test: clean
	${MAKE} cppcheck
	${MAKE} spell
	${MAKE}	cmake OPTIONS=license
	${MAKE}	unit_tests OPTIONS=householder
	${MAKE}	unit_tests OPTIONS=testdefault
	${MAKE} unit_tests OPTIONS=debug
	${MAKE}	test_octave

test_octave:
	cd matlab_octave; ${MAKE} octave octave_test

test_dependency: clean
	mkdir -p build/dependency/old
	cd build/dependency/old; cmake ../../../test/dependency/old
	cd build/dependency/old; ${MAKE} ${MAKE_FLAGS} VERBOSE=1
	mkdir -p build/dependency/new
	cd build/dependency/new; cmake ../../../test/dependency/new
	cd build/dependency/new; ${MAKE} ${MAKE_FLAGS} VERBOSE=1

install_latest_eigen:
	mkdir -p ${BUILD_DIR};
	cd ${BUILD_DIR}; git clone https://gitlab.com/libeigen/eigen.git
	mkdir -p ${BUILD_DIR}/eigen/build
	cd ${BUILD_DIR}/eigen/build; \
		cmake -DEIGEN_BUILD_BTL=OFF -DEIGEN_BUILD_DOC=OFF  \
        	-DEIGEN_BUILD_BLAS=OFF -DEIGEN_BUILD_LAPACK=OFF \
            -DEIGEN_BUILD_PKGCONFIG=ON -DEIGEN_BUILD_DOC=OFF \
            -DEIGEN_BUILD_TESTING=OFF \
			-DCMAKE_INSTALL_PREFIX:PATH=/usr ..; sudo make install


format:
	${FIND_SOURCES} | xargs ${CLANG_FORMAT} -verbose -i


# utils
#----------------------------------------------

addutils:
	-git remote add cmakeut https://github.com/asherikov/cmakeut --no-tags

updateutils:
	git fetch --all
	git show remotes/cmakeut/master:cmake/FindEigen3.cmake                  > cmake/FindEigen3.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_compiler_flags.cmake      > cmake/cmakeut_compiler_flags.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_detect_func_macro.cmake   > cmake/cmakeut_detect_func_macro.cmake


# doxygen
#----------------------------------------------

dox:
	git submodule update --init
	${MAKE} doxclean clean
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
	rm -Rf debian/ obj-*/
	cd matlab_octave; ${MAKE} clean

forceclean: clean
	cd matlab_octave; ${MAKE} forceclean
