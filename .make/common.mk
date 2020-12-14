FIND_QPMAD_SOURCES=find ./matlab_octave ./test/ ./include/ -iname "*.h" -or -iname "*.cpp" | grep -v "cpput_" | grep -v "eigenut_"

help:
	-@grep --color -Ev "(^	)|(^$$)" Makefile
	-@grep --color -Ev "(^	)|(^$$)" GNUmakefile
	-@grep --color -Ev "(^	)|(^$$)" make/Makefile*


# release
#----------------------------------------------

update_version_cmake:
	sed -i -e "s=\(project([ a-zA-Z0-9_-]* VERSION\) [0-9]*\.[0-9]*\.[0-9]*)=\1 ${VERSION})=" CMakeLists.txt


cppcheck:
	# --inconclusive
	cppcheck \
	    ./ \
	    --relative-paths \
	    --quiet --verbose --force \
	    --template='[{file}:{line}]  {severity}  {id}  {message}' \
	    --language=c++ --std=c++03 \
	    --enable=warning \
	    --enable=style \
	    --enable=performance \
	    --enable=portability \
	    --suppress=uninitMemberVar \
	    --suppress=syntaxError \
	    --suppress=useInitializationList \
	    --suppress=functionStatic \
	    --suppress=constStatement \
	    -i ./build \
	3>&1 1>&2 2>&3 | tee cppcheck.err
	test 0 -eq `cat cppcheck.err | wc -l && rm cppcheck.err`

spell_interactive:
	${MAKE} spell SPELL_XARGS_ARG=-o

# https://github.com/myint/scspell
spell:
	${FIND_QPMAD_SOURCES} \
	    | xargs ${SPELL_XARGS_ARG} scspell --use-builtin-base-dict --override-dictionary ./qa/scspell.dict

format:
	${FIND_QPMAD_SOURCES} | grep -v "cpput" | grep -v "eigenut" | xargs clang-format80 -verbose -i


clangcheck:
	${SCANBUILD} \
        -o ./scanbuild_results \
        --status-bugs \
        --exclude ./build \
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
        ${MAKE} test

