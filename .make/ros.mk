PROJECT=qpmad

CATKIN_PKGS=qpmad

CATKIN_ARGS=
CATKIN_TARGETS=all

CATKIN_WORKING_DIR=./build/catkin_workspace
CATKIN_PKGS_PATH=${CATKIN_WORKING_DIR}/src/

APT_INSTALL=apt install -y --no-install-recommends

# release
#----------------------------------------------

ros_release:
	# 0. Add Forthcoming section to the changelog
	${MAKE} update_version VERSION=${VERSION}
	git commit -a
	catkin_prepare_release -t 'catkin-' --version "${VERSION}" -y
	# initial release -> https://wiki.ros.org/bloom/Tutorials/FirstTimeRelease
	# subsequent releases -> bloom-release --rosdistro melodic --track melodic ${PKG}


# catkin
#----------------------------------------------

ros_add_repos:
	sh -c "echo \"deb http://packages.ros.org/ros/ubuntu ${UBUNTU_DISTRO} main\" > /etc/apt/sources.list.d/ros-latest.list"
	sh -c "apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654 \
		|| apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654 \
		|| apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654"
	sh -c "apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key 6B05F25D762E3157 \
		|| apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key 6B05F25D762E3157 \
		|| apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key 6B05F25D762E3157"
	apt-get update -qq
	${APT_INSTALL} dpkg
	apt update -y

ros_install:
	${APT_INSTALL} -y ros-${ROS_DISTRO}-ros-base
	bash -c 'source /opt/ros/${ROS_DISTRO}/setup.bash; rosdep init'

ros_install_deps:
	apt update
	#apt upgrade -y
	${APT_INSTALL} ${DEBIAN_SYSTEM_DEPENDENCIES}
	${APT_INSTALL} \
		python3-bloom \
		devscripts \
		debhelper \
		fakeroot
	${APT_INSTALL} python3-catkin-tools


catkin_test_deb_pkg:
	cd ${CATKIN_PKGS_PATH}/${PKG}; bloom-generate rosdebian --os-name ubuntu --ros-distro ${ROS_DISTRO} ./
	cd ${CATKIN_PKGS_PATH}/${PKG}; fakeroot debian/rules binary
	echo ${PKG} | tr "_" "-" | xargs -I {} sudo /bin/sh -c 'dpkg -i ${CATKIN_PKGS_PATH}/ros*{}*.deb'

catkin_test_deb: clean
	${MAKE} catkin_prepare_workspace
	${MAKE} catkin_fake_rosdep
	mkdir -p build/dependency_test
	echo ${CATKIN_PKGS} | tr " " "\n" | xargs -I {} ${MAKE} catkin_test_deb_pkg PKG="{}" ROS_DISTRO=${ROS_DISTRO}
	bash -c 'source /opt/ros/${ROS_DISTRO}/setup.bash; \
		cd build/dependency_test; \
		cmake ../../${DEPENDENCY_PATH}/; \
		${MAKE} ${MAKE_FLAGS}'


catkin_prepare_workspace: clean
	mkdir -p ${CATKIN_PKGS_PATH}/${PROJECT}
	ls -1A | grep -v build | xargs cp -R -t ${CATKIN_PKGS_PATH}/${PROJECT}
	cp -R ${CATKIN_DEPENDENCY_PATH} ${CATKIN_WORKING_DIR}/src/${CATKIN_DEPENDENCY_TEST_PKG}

catkin_fake_rosdep:
	# https://answers.ros.org/question/280213/generate-deb-from-dependent-res-package-locally/#280235
	sudo /bin/sh -c 'echo "yaml file:///tmp/rosdep.yaml" > /etc/ros/rosdep/sources.list.d/50-qpmad.list'
	sudo rm -Rf /tmp/rosdep.yaml
	echo ${CATKIN_PKGS} | tr " " "\n" | \
		xargs -I {} sudo /bin/sh -c 'echo "{}:" >> /tmp/rosdep.yaml; echo "  ubuntu: [ros-${ROS_DISTRO}-{}]" | tr "_" "-" >> /tmp/rosdep.yaml'
	rosdep update


catkin_old_build: catkin_prepare_workspace
	cd ${CATKIN_WORKING_DIR}/src; catkin_init_workspace
	cd ${CATKIN_WORKING_DIR}; catkin_make_isolated --pkg ${CATKIN_PKGS} ${CATKIN_ARGS} --make-args ${CATKIN_TARGETS} # old

catkin_old_build_with_dependent: catkin_prepare_workspace
	cd ${CATKIN_WORKING_DIR}/src; catkin_init_workspace
	cd ${CATKIN_WORKING_DIR}; catkin_make_isolated

catkin_old_deb: catkin_prepare_workspace
	${MAKE} catkin_test_deb
	${MAKE} catkin_prepare_workspace
	cd ${CATKIN_PKGS_PATH}; ls -1A | grep -v "dependency" | xargs rm -Rf
	cd ${CATKIN_WORKING_DIR}/src; catkin_init_workspace
	cd ${CATKIN_WORKING_DIR}; catkin_make_isolated --pkg ${CATKIN_DEPENDENCY_TEST_PKG}
	sudo ${MAKE} clean_deb clean_rosdep


catkin_new_build: catkin_prepare_workspace
	cd ${CATKIN_WORKING_DIR}; catkin init
	cd ${CATKIN_WORKING_DIR}; catkin build -i --verbose --summary ${CATKIN_PKGS} --make-args ${CATKIN_TARGETS} ${CATKIN_ARGS}

catkin_new_build_with_dependent: catkin_prepare_workspace
	cd ${CATKIN_WORKING_DIR}; catkin init
	cd ${CATKIN_WORKING_DIR}; catkin build -i --verbose --summary ${CATKIN_DEPENDENCY_TEST_PKG}

catkin_new_deb:
	${MAKE} catkin_test_deb
	${MAKE} catkin_prepare_workspace
	cd ${CATKIN_PKGS_PATH}; ls -1A | grep -v "dependency" | xargs rm -Rf
	cd ${CATKIN_WORKING_DIR}; catkin init
	cd ${CATKIN_WORKING_DIR}; catkin build -i --verbose --summary ${CATKIN_DEPENDENCY_TEST_PKG}
	sudo ${MAKE} clean_deb


catkin_test_old: ros_install_deps
	${MAKE} catkin_old_build
	${MAKE} catkin_old_build_with_dependent
	${MAKE} catkin_old_deb

catkin_test_new: ros_install_deps
	${MAKE} catkin_new_build
	${MAKE} catkin_new_build_with_dependent
	${MAKE} catkin_new_deb


ros_prerelease_deps:
	sudo ${MAKE} ros_add_repos UBUNTU_DISTRO=${UBUNTU_DISTRO}
	pip3 install ros_buildfarm

ros_prerelease: ros_prerelease_deps
	# sudo apt install docker.io
	# sudo adduser username docker
	generate_prerelease_script.py \
		https://raw.githubusercontent.com/ros-infrastructure/ros_buildfarm_config/production/index.yaml \
		${ROS_DISTRO} default ubuntu ${UBUNTU_DISTRO} amd64 \
		--level 0 --custom-repo ${PROJECT}:git:${REPO}:${BRANCH} \
		--output-dir ./build/ros_prerelease
	# dirty fix to break interactive part of the script
	cd ./build/ros_prerelease; sed -i -e "/_ls_prerelease_scripts=/d" prerelease.sh
	# shell script debug
	cd ./build/ros_prerelease; sed -i 's=\(#!.*sh\)=#!/bin/sh -x=' *.sh
	# dirty workaround for ccache permissions issue
	cd ./build/ros_prerelease; sed -i "s|\(-e=TRAVIS\)|-eCCACHE_DIR=./.ccache/ \1|" *.sh
	cd ./build/ros_prerelease; env ABORT_ON_TEST_FAILURE=1 CCACHE_DIR=`pwd`/.ccache ./prerelease.sh


# docker
#----------------------------------------------
ros_make_docker:
	docker pull ros:${ROS_DISTRO}-ros-base-${UBUNTU_DISTRO}
	docker run -ti ros:${ROS_DISTRO}-ros-base-${UBUNTU_DISTRO} \
		/bin/bash -c "source /opt/ros/${ROS_DISTRO}/setup.bash \
		&& git clone -b ${BRANCH} ${REPO} ${PROJECT} \
		&& cd ${PROJECT} \
		&& make ${TARGET} ROS_DISTRO=${ROS_DISTRO} UBUNTU_DISTRO=${UBUNTU_DISTRO}"


# other
#----------------------------------------------
clean_deb:
	dpkg --get-selections | grep qpmad | cut -f 1 | xargs sudo dpkg -r

clean_rosdep:
	rm -Rf /tmp/rosdep.yaml /etc/ros/rosdep/sources.list.d/50-qpmad.list
	rosdep update
