FIND_QPMAD_SOURCES=find ./test/ ./include/ -iname "*.h" -or -iname "*.cpp" | grep -v "cpput_" | grep -v "eigenut_"

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
