function(cmakeut_compiler_flags STANDARD)
    set (CXX_WARNINGS "-Wall -Wextra -Wshadow -Werror -pedantic-errors")
    # --save-temps
    set (CXX_OTHER "-fPIC")

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

        if (NOT CMAKE_CXX_COMPILER_VERSION OR CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0)
            # workaround for clang50
            set(CXX_WARNINGS "${CXX_WARNINGS} -Wno-error=unused-command-line-argument")
        endif()
        set(CXX_WARNINGS "${CXX_WARNINGS} -Werror=extra-tokens")

    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

        # using GCC
        set(CXX_WARNINGS "${CXX_WARNINGS}")

    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

        # using Intel C++

    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

        # using Visual Studio C++

    endif()


    set(CXX_SANITIZERS "")
    if(CMAKEUT_CPP_SANITIZERS)
        if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

            if ("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
                # -fsanitize=address segfaults on boost UTF.
                set(CXX_SANITIZERS "-fsanitize=undefined")
            else()
                set(CXX_SANITIZERS "-fsanitize=address -fsanitize=undefined")
            endif()

        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")

            if (NOT CMAKE_CXX_COMPILER_VERSION OR CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
                message(WARNING "GCC version is unknown or too old. Disabling sanitizers.")
            else()
                set(CXX_SANITIZERS "-fsanitize=address -fsanitize=undefined -fsanitize-undefined-trap-on-error")
            endif()

        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")

            # using Intel C++

        elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")

            # using Visual Studio C++

        endif()
    endif()


    set(CXX_GENERIC "-std=${STANDARD} ${CXX_WARNINGS} ${CXX_OTHER} ${CXX_SANITIZERS}")


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
