# List filenames (all intermediate directories are stripped) in the given
# directory.
# This is equivalent to 'file(GLOB ... RELATIVE ... ...)', but behaves more
# consistently with different versions of cmake.
#
function(cmakeut_list_filenames  DIR     LISTNAME)
    set(options DIRS_WITH_CMAKELISTS)
    cmake_parse_arguments("CMAKEUT" "${options}" "" "" ${ARGN})

    file(GLOB FILENAMES_TMP "${DIR}/*")

    set (FILENAMES "")
    foreach(FILENAME_TMP ${FILENAMES_TMP})
        if(CMAKEUT_DIRS_WITH_CMAKELISTS)
            if (NOT EXISTS "${FILENAME_TMP}/CMakeLists.txt")
                continue()
            endif()
        endif()
        get_filename_component(FILENAME_TMP ${FILENAME_TMP} NAME)
        list(APPEND FILENAMES ${FILENAME_TMP})
    endforeach(FILENAME_TMP)

    set(${LISTNAME}   "${FILENAMES}"  PARENT_SCOPE)
endfunction()
