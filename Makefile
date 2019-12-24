BUILD_DIR?=build
MAKE_FLAGS?=-j5
VERSION?="XXX__version_not_set__XXX"

TYPE?=Debug
TRACING?=OFF
EXTRA_CMAKE_PARAM?=
#EXTRA_CMAKE_PARAM?="-DCMAKE_CXX_COMPILER=g++"


default:
	@grep -v "^	" Makefile | grep -v "^$$"


build:
	${MAKE}	cmake TYPE=Release TRACING=OFF TESTS=OFF

install:
	cd ${BUILD_DIR}; ${MAKE} install


test: clean unit_tests_without_tracing unit_tests_with_tracing unit_tests_with_householder test_octave

test_octave:
	cd matlab_octave; ${MAKE} octave octave_test

test_dependency: clean
	mkdir -p build/dependency_test
	cd build/dependency_test; cmake ../../test/dependency/
	cd build/dependency_test; ${MAKE} ${MAKE_FLAGS}


# build targets
#----------------------------------------------

build_with_householder:
	${MAKE} cmake TYPE=Debug TRACING=OFF EXTRA_CMAKE_PARAM="-DQPMAD_USE_HOUSEHOLDER=ON"

unit_tests_with_householder: build_with_householder
	cd ${BUILD_DIR}; ${MAKE} test


build_without_tracing:
	${MAKE}	cmake TYPE=Debug TRACING=OFF TESTS=ON

unit_tests_without_tracing: build_without_tracing
	cd ${BUILD_DIR}; ${MAKE} test


build_with_tracing:
	${MAKE}	cmake TYPE=Debug TRACING=ON TESTS=ON

unit_tests_with_tracing: build_with_tracing
	cd ${BUILD_DIR}; ${MAKE} test


cmake:
	mkdir -p ${BUILD_DIR};
	cd ${BUILD_DIR}; cmake .. \
						-DQPMAD_BUILD_TESTS=${TESTS} \
						-DCMAKE_BUILD_TYPE=${TYPE} \
						-DQPMAD_ENABLE_TRACING=${TRACING} \
						${EXTRA_CMAKE_PARAM}
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
	#git show remotes/cmakeut/master:cmake/cmakeut_add_cpp_test.cmake        > cmake/cmakeut_add_cpp_test.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_compiler_flags.cmake      > cmake/cmakeut_compiler_flags.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_detect_func_macro.cmake   > cmake/cmakeut_detect_func_macro.cmake


# release
#----------------------------------------------

update_version:
	sed -i -e "s=<version>[0-9]*\.[0-9]*\.[0-9]*</version>=<version>${VERSION}</version>=" package.xml
	sed -i -e "s=\(project([ a-zA-Z0-9_-]* VERSION\) [0-9]*\.[0-9]*\.[0-9]*)=\1 ${VERSION})=" CMakeLists.txt

ros_release:
	# 0. Add Forthcoming section to the changelog
	${MAKE} update_version VERSION=${VERSION}
	git commit -a
	catkin_prepare_release -t 'ros-' --version "${VERSION}" -y
	# initial release -> https://wiki.ros.org/bloom/Tutorials/FirstTimeRelease
	# subsequent releases -> bloom-release --rosdistro melodic --track melodic qpmad


# catkin
#----------------------------------------------

CATKIN_WORKING_DIR?=./build/catkin_workspace
PKG_PATH?=${CATKIN_WORKING_DIR}/src/qpmad
DEPENDENT_PKG_PATH?=${CATKIN_WORKING_DIR}/src/qpmad_catkin_dependency_test

install-deps:
	apt update
#	apt upgrade -y
	apt install -y \
        libeigen3-dev
	apt install -y \
		python-bloom \
		devscripts \
		debhelper
	apt install -y python-catkin-tools


catkin-build-deb: clean
	cd ${PKG_PATH}; bloom-generate rosdebian --os-name ubuntu --ros-distro ${ROS_DISTRO} ./
	# disable installation of catkin stuff: setup scripts, etc.
	#cd ${PKG_PATH}; sed "s/dh_auto_configure --/dh_auto_configure -- -DCATKIN_BUILD_BINARY_PACKAGE=ON/" -i debian/rules
	cd ${PKG_PATH}; fakeroot debian/rules binary


catkin-test-deb: catkin-build-deb
	sudo dpkg -i ../ros*qpmad*.deb
	mkdir -p build/dependency_test
	bash -c 'source /opt/ros/${ROS_DISTRO}/setup.bash; \
             cd build/dependency_test; \
             cmake ../../test/dependency/; \
             ${MAKE} ${MAKE_FLAGS}'


catkin-prepare-workspace: clean
	mkdir -p ${PKG_PATH}
	mkdir -p ${DEPENDENT_PKG_PATH}
	ls -1 | grep -v build | xargs cp -R -t ${PKG_PATH}
	cp -R test/dependency/* ${DEPENDENT_PKG_PATH}/


catkin-old-build: catkin-prepare-workspace
	cd ${CATKIN_WORKING_DIR}/src; catkin_init_workspace
	cd ${CATKIN_WORKING_DIR}; catkin_make_isolated --pkg qpmad --make-args all # old

catkin-old-build-with-dependent: catkin-prepare-workspace
	cd ${CATKIN_WORKING_DIR}/src; catkin_init_workspace
	cd ${CATKIN_WORKING_DIR}; catkin_make_isolated

catkin-old-deb: catkin-prepare-workspace
	${MAKE} catkin-test-deb
	${MAKE} catkin-prepare-workspace
	rm -Rf ${PKG_PATH}
	cd ${CATKIN_WORKING_DIR}/src; catkin_init_workspace
	cd ${CATKIN_WORKING_DIR}; catkin_make_isolated --pkg qpmad_catkin_dependency_test


catkin-new-build: catkin-prepare-workspace
	cd ${CATKIN_WORKING_DIR}; catkin init
	cd ${CATKIN_WORKING_DIR}; catkin build -i --verbose --summary qpmad --make-args all

catkin-new-build-with-dependent: catkin-prepare-workspace
	cd ${CATKIN_WORKING_DIR}; catkin init
	cd ${CATKIN_WORKING_DIR}; catkin build -i --verbose --summary qpmad_catkin_dependency_test

catkin-new-deb:
	${MAKE} catkin-test-deb
	${MAKE} catkin-prepare-workspace
	rm -Rf ${PKG_PATH}
	cd ${CATKIN_WORKING_DIR}; catkin init
	cd ${CATKIN_WORKING_DIR}; catkin build -i --verbose --summary qpmad_catkin_dependency_test


catkin-test-old: install-deps
	${MAKE} catkin-old-build
	${MAKE} catkin-old-build-with-dependent
	${MAKE} catkin-old-deb

catkin-test-new: install-deps
	${MAKE} catkin-new-build
	${MAKE} catkin-new-build-with-dependent
	${MAKE} catkin-new-deb


# docker
#----------------------------------------------
make-docker:
	docker pull ros:${ROS_DISTRO}-ros-base-${UBUNTU_DISTRO}
	docker run -ti ros:${ROS_DISTRO}-ros-base-${UBUNTU_DISTRO} \
		/bin/bash -c "source /opt/ros/${ROS_DISTRO}/setup.bash \
		&& git clone -b ${BRANCH} https://github.com/asherikov/qpmad.git \
		&& cd qpmad \
		&& make ${TARGET} ROS_DISTRO=${ROS_DISTRO} UBUNTU_DISTRO=${UBUNTU_DISTRO}"

# clean
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
