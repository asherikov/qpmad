function(cmakeut_compiler_flags STANDARD)
    set (CXX_WARNINGS "-Wall -Wextra -Werror=pedantic -pedantic-errors -Wshadow")
    set (CXX_OTHER "-fPIC")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

        set(CXX_WARNINGS "${CXX_WARNINGS} -Werror=extra-tokens")

    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        # using GCC
        set(CXX_WARNINGS "${CXX_WARNINGS}")

    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

        # using Intel C++

    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

        # using Visual Studio C++

    endif()


    set(CXX_GENERIC "-std=${STANDARD} ${CXX_WARNINGS} ${CXX_OTHER}")


    if ("${STANDARD}" STREQUAL "c++11")

        # -Wsuggest-override -Wsuggest-final-methods
        set (CMAKEUT_CXX_FLAGS "${CXX_GENERIC}" PARENT_SCOPE)

    elseif ("${STANDARD}" STREQUAL "c++03")

        set (CMAKEUT_CXX_FLAGS "${CXX_GENERIC}" PARENT_SCOPE)

    elseif ("${STANDARD}" STREQUAL "c++98")

        set (CMAKEUT_CXX_FLAGS "${CXX_GENERIC}" PARENT_SCOPE)

    else()

        message(FATAL_ERROR "Unknown standard")

    endif()
endfunction()
