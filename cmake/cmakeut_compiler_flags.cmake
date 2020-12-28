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


    if (CMAKEUT_CLANG_TIDY)
        find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy clang-tidy-9 clang-tidy11 clang-tidy-8 REQUIRED)

        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};-warnings-as-errors=*;-checks=*")

        # too annoying
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-llvm-include-order")
        # do not enforce auto
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-modernize-use-trailing-return-type,-hicpp-use-auto,-modernize-use-auto")
        # do not enforce capitalization of literal suffix, e.g., x = 1u -> x = 1U.
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-readability-uppercase-literal-suffix,-hicpp-uppercase-literal-suffix")
        # allow function arguments with default values
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-fuchsia-default-arguments,-fuchsia-default-arguments-calls,-fuchsia-default-arguments-declarations")
        # member variables can be public/protected
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-non-private-member-variables-in-classes,-misc-non-private-member-variables-in-classes")
        # member initialization in constructors -- false positives
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-pro-type-member-init,-hicpp-member-init")
        # default member initialization scatters initializations -- initialization must be done via constructors
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-modernize-use-default-member-init")
        # calling virtual functions from desctructors is well defined and generally safe
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-clang-analyzer-optin.cplusplus.VirtualCall")
        # these checks require values to be assigned to const variables, which is inconvenient
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers")
        # I use access specifiers (public/protected/private) to group members
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-readability-redundant-access-specifiers")
        # issues on many macro
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-pro-type-vararg,-hicpp-vararg")
        # there is no from_string
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-boost-use-to-string")
        # too common
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-hicpp-no-array-decay")

        # overly restrictive fuchsia stuff
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-fuchsia-overloaded-operator,-fuchsia-multiple-inheritance,-fuchsia-statically-constructed-objects")
        # overly restrictive cppcoreguidelines stuff
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-macro-usage")
        # llvmlibc stuff
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-llvmlibc-*")

        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},${CMAKEUT_CLANG_TIDY_EXTRA_IGNORES}")

        # might be useful too
        #,-cert-env33-c
        #,-modernize-avoid-c-arrays
        #,-cppcoreguidelines-pro-type-union-access
        #,-readability-simplify-boolean-expr

        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY}" PARENT_SCOPE)
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
