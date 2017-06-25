MAKE_FLAGS?=-j5

TYPE?=Debug
TRACING?=OFF
EXTRA_CMAKE_PARAM?=
#EXTRA_CMAKE_PARAM?="-DCMAKE_CXX_COMPILER=g++"


test: clean unit_tests_without_tracing unit_tests_with_tracing unit_tests_with_givens
	cd matlab_octave; ${MAKE} octave octave_test


build: build_without_tracing


build_with_givens:
	${MAKE} cmake TYPE=Debug TRACING=OFF EXTRA_CMAKE_PARAM="-DQPMAD_USE_HOUSEHOLDER=OFF"

unit_tests_with_givens: build_with_givens
	cd build; ${MAKE} test


build_without_tracing:
	${MAKE}	cmake TYPE=Debug TRACING=OFF

unit_tests_without_tracing: build_without_tracing
	cd build; ${MAKE} test


build_with_tracing:
	${MAKE}	cmake TYPE=Debug TRACING=ON

unit_tests_with_tracing: build_with_tracing
	cd build; ${MAKE} test


release:
	${MAKE}	cmake TYPE=Release TRACING=OFF


cmake:
	mkdir -p build;
	cd build; cmake .. 	-DCMAKE_BUILD_TYPE=${TYPE} \
						-DQPMAD_ENABLE_TRACING=${TRACING} \
						${EXTRA_CMAKE_PARAM}
	cd build; ${MAKE} ${MAKE_FLAGS}


clean:
	rm -Rf build
	rm -Rf src/config.h
	cd matlab_octave; ${MAKE} clean

forceclean: clean
	cd matlab_octave; ${MAKE} forceclean


.PHONY: build cmake
