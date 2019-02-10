BUILD_DIR?=build
MAKE_FLAGS?=-j1

INCLUDE_DIR=include/cpput/

default:
	@grep -v "^	" Makefile | grep -v "^$$"

clean:
	rm -Rf ${BUILD_DIR}
	ls ${INCLUDE_DIR}/*.in | sed 's/\.in$$//g' | xargs rm -f
	ls ${INCLUDE_DIR}/*.in | sed 's/\.in$$//g' | sed 's=${INCLUDE_DIR}/=${INCLUDE_DIR}/cpput_=' | xargs rm -f

build:
	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR}; cmake ..
	cd ${BUILD_DIR}; make ${MAKE_FLAGS}

test: build
	cd build; ${MAKE} ${MAKE_FLAGS} test


addutils:
	git remote add cmakeut https://github.com/asherikov/cmakeut
	git remote add better_enums https://github.com/aantron/better-enums
	git remote add popl https://github.com/badaix/popl
	git remote add safe_int https://github.com/dcleblanc/SafeInt

3rdparty:
	git fetch --all
	git show remotes/cmakeut/master:cmake/cmakeut_add_cpp_test.cmake        > cmake/cmakeut_add_cpp_test.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_compiler_flags.cmake      > cmake/cmakeut_compiler_flags.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_detect_func_macro.cmake   > cmake/cmakeut_detect_func_macro.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_list_filenames.cmake      > cmake/cmakeut_list_filenames.cmake
	git show remotes/better_enums/master:enum.h    > ${INCLUDE_DIR}/better_enum.h
	git show remotes/popl/master:include/popl.hpp  > ${INCLUDE_DIR}/popl.h
	git show remotes/safe_int/master:SafeInt.hpp   > ${INCLUDE_DIR}/safe_int.h
	${MAKE} allheader

allheader:
	echo "#ifndef H_CPPUT_ALL" > ${INCLUDE_DIR}/all.h
	echo "#define H_CPPUT_ALL" >> ${INCLUDE_DIR}/all.h
	cd ${INCLUDE_DIR}; ls *.h | grep -v "all.h" | sed 's/^\(.*\)/#include "\1"/' >> all.h
	cd ${INCLUDE_DIR}; ls *.h.in | grep -v "all.h" | sed 's/\.in$$//g' | sed 's/^\(.*\)/#include "\1"/' >> all.h
	echo "#endif" >> ${INCLUDE_DIR}/all.h
	cat ${INCLUDE_DIR}/all.h

gitignore:
	echo "build" > .gitignore
	ls ${INCLUDE_DIR}*.in | sed 's/\.in$$//g' >> .gitignore
	ls ${INCLUDE_DIR}*.in | sed 's/\.in$$//g' | sed 's=${INCLUDE_DIR}=${INCLUDE_DIR}cpput_=' >> .gitignore

.PHONY: build test 3rdparty
