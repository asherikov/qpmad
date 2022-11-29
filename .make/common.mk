ROOT_DIR=../../
BUILD_ROOT?=./build
BUILD_DIR?=${BUILD_ROOT}/${OPTIONS}

APT_INSTALL=sudo apt install -y --no-install-recommends
PIP_INSTALL=sudo python3 -m pip install
GEM_INSTALL=sudo gem install

CLANG_FORMAT?=clang-format13
SCANBUILD?=scan-build-13


help:
	-@grep --color -Ev "(^	)|(^$$)" Makefile
	-@grep --color -Ev "(^	)|(^$$)" GNUmakefile
	-@grep --color -Ev "(^	)|(^$$)" .make/*.mk


install_deps_common:
	${APT_INSTALL} cmake
	${APT_INSTALL} cppcheck
	${PIP_INSTALL} scspell3k


# release
#----------------------------------------------

update_version_cmake:
	sed -i -e "s=\(project([ a-zA-Z0-9_-]* VERSION\) [0-9]*\.[0-9]*\.[0-9]*)=\1 ${VERSION})=" CMakeLists.txt


# static checks
#----------------------------------------------

cppcheck:
	# --inconclusive
	cppcheck \
	    ./ \
	    --relative-paths \
	    --quiet --verbose --force \
	    --template='[{file}:{line}]  {severity}  {id}  {message}' \
	    --language=c++ --std=c++11 \
	    --enable=warning \
	    --enable=style \
	    --enable=performance \
	    --enable=portability \
	    --suppress=uninitMemberVar \
	    --suppress=syntaxError \
	    --suppress=useInitializationList \
	    --suppress=functionStatic \
	    --suppress=constStatement \
	    --inline-suppr \
	    -i ${BUILD_ROOT} \
	3>&1 1>&2 2>&3 | tee cppcheck.err
	test 0 -eq `cat cppcheck.err | wc -l && rm cppcheck.err`


spell_interactive:
	${MAKE} spell SPELL_XARGS_ARG=-o

# https://github.com/myint/scspell
spell:
	${FIND_SOURCES} \
	    | xargs ${SPELL_XARGS_ARG} scspell --use-builtin-base-dict --override-dictionary ./qa/scspell.dict

clangcheck:
	${SCANBUILD} \
        -o ./scanbuild_results \
        --status-bugs \
        --exclude ${BUILD_ROOT} \
        --exclude /usr/include/ \
        --exclude /usr/local/include/ \
        --exclude /usr/src/ \
        --exclude /opt/ros/ \
        -enable-checker core.CallAndMessage \
        -enable-checker core.DivideZero \
        -enable-checker core.DynamicTypePropagation \
        -enable-checker core.NonNullParamChecker \
        -enable-checker core.NullDereference \
        -enable-checker core.StackAddressEscape \
        -enable-checker core.UndefinedBinaryOperatorResult \
        -enable-checker core.VLASize \
        -enable-checker core.uninitialized.ArraySubscript \
        -enable-checker core.uninitialized.Assign \
        -enable-checker core.uninitialized.Branch \
        -enable-checker core.uninitialized.CapturedBlockVariable \
        -enable-checker core.uninitialized.UndefReturn \
        -enable-checker cplusplus.InnerPointer \
        -enable-checker cplusplus.Move \
        -enable-checker cplusplus.NewDelete \
        -enable-checker cplusplus.NewDeleteLeaks \
        -enable-checker deadcode.DeadStores \
        -enable-checker nullability.NullPassedToNonnull \
        -enable-checker nullability.NullReturnedFromNonnull \
        -enable-checker nullability.NullableDereferenced \
        -enable-checker nullability.NullablePassedToNonnull \
        -enable-checker nullability.NullableReturnedFromNonnull \
        -enable-checker optin.cplusplus.UninitializedObject \
        -enable-checker optin.mpi.MPI-Checker \
        -enable-checker optin.performance.GCDAntipattern \
        -enable-checker optin.performance.Padding \
        -enable-checker optin.portability.UnixAPI \
        -enable-checker security.FloatLoopCounter \
        -enable-checker security.insecureAPI.DeprecatedOrUnsafeBufferHandling \
        -enable-checker security.insecureAPI.UncheckedReturn \
        -enable-checker security.insecureAPI.getpw \
        -enable-checker security.insecureAPI.gets \
        -enable-checker security.insecureAPI.mkstemp \
        -enable-checker security.insecureAPI.mktemp \
        -enable-checker security.insecureAPI.vfork \
        -enable-checker unix.API \
        -enable-checker unix.Malloc \
        -enable-checker unix.MallocSizeof \
        -enable-checker unix.MismatchedDeallocator \
        -enable-checker unix.Vfork \
        -enable-checker unix.cstring.BadSizeArg \
        -enable-checker unix.cstring.NullArg \
        -enable-checker valist.CopyToSelf \
        -enable-checker valist.Uninitialized \
        -enable-checker valist.Unterminated \
		${MAKE} cmake OPTIONS=test


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

install:
	cd ${BUILD_DIR}; ${MAKE} install

clean_common:
	rm -Rf ${BUILD_ROOT}


# deb
#----------------------------------------------
deb_any:
	${MAKE} cmake OPTIONS=deb
	${MAKE} install OPTIONS=deb
	grep "project.*VERSION" CMakeLists.txt | grep -o "[0-9]*\.[0-9]*\.[0-9]*" > ${BUILD_ROOT}/version
	grep "project" CMakeLists.txt | sed 's/project(\([[:graph:]]*\).*/\1/' > ${BUILD_ROOT}/project
	fpm \
		-t deb \
		--depends "${DEBIAN_SYSTEM_DEPENDENCIES}" \
		--version `cat ${BUILD_ROOT}/version` \
		--package ${BUILD_ROOT}/`cat ${BUILD_ROOT}/project`-`cat ${BUILD_ROOT}/version`-any.deb

deb_install_deps:
	${APT_INSTALL} ruby
	${GEM_INSTALL} fpm

deb_install_deps_cloudsmith: deb_install_deps
	${PIP_INSTALL} --upgrade cloudsmith-cli

deb_cloudsmith_any: deb_any
	ls ${BUILD_ROOT}/*-any.deb | xargs --no-run-if-empty -I {} cloudsmith push deb asherikov-aV7/all/any-distro/any-version {}


# documentation
#----------------------------------------------
doxclean:
	cd doc/gh-pages; git fetch --all; git checkout gh-pages; git pull
	find ./doc/gh-pages/ -mindepth 1 -not -name "\.git" | xargs rm -Rf


.PHONY: build cmake test
