BUILD_DIR?=build
MAKE_FLAGS?=-j1

INCLUDE_DIR=include/eigenut/

default:
	@grep -v "^	" Makefile | grep -v "^$$"

clean:
	rm -Rf ${BUILD_DIR}
	ls ${INCLUDE_DIR}/*.in | sed 's/\.in$$//g' | xargs rm -f
	ls ${INCLUDE_DIR}/*.in | sed 's/\.in$$//g' | sed 's=${INCLUDE_DIR}/=${INCLUDE_DIR}/eigenut_=' | xargs rm -f
	rm -Rf ${INCLUDE_DIR}/cpput*.h
	cd cpput; ${MAKE} clean

build:
	git submodule update --init
	mkdir -p ${BUILD_DIR}
	cd ${BUILD_DIR}; cmake ..
	cd ${BUILD_DIR}; make ${MAKE_FLAGS}

test: build
	cd build; ${MAKE} ${MAKE_FLAGS} test

gitignore:
	echo "build" > .gitignore
	ls ${INCLUDE_DIR}*.in | sed 's/\.in$$//g' >> .gitignore
	ls ${INCLUDE_DIR}*.in | sed 's/\.in$$//g' | sed 's=${INCLUDE_DIR}=${INCLUDE_DIR}eigenut_=' >> .gitignore

#----------------------------------------------
# utils
#----------------------------------------------

addutils:
	git remote remove cpput
	git remote remove cmakeut
	git remote add cmakeut https://github.com/asherikov/cmakeut
	git remote add cpput https://github.com/asherikov/cpput

updateutils:
	git fetch --all
	git rm --ignore-unmatch -rf cpput
	git read-tree --prefix=cpput -u cpput/master
	rm -Rf cpput/3rdparty/
	git show remotes/cmakeut/master:cmake/FindEigen3.cmake                  > cmake/FindEigen3.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_add_cpp_test.cmake        > cmake/cmakeut_add_cpp_test.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_compiler_flags.cmake      > cmake/cmakeut_compiler_flags.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_detect_func_macro.cmake   > cmake/cmakeut_detect_func_macro.cmake
	git show remotes/cmakeut/master:cmake/cmakeut_list_filenames.cmake      > cmake/cmakeut_list_filenames.cmake

.PHONY: build test
