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
        set (CXX_WARNINGS "")
        set (CXX_OTHER "/bigobj")

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
        find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy clang-tidy18 clang-tidy15 clang-tidy-14 clang-tidy-12 REQUIRED)

        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};-warnings-as-errors=*;-checks=*")

        # see ccws scan_build profile
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-llvm-include-order,-google-readability-todo,-readability-static-accessed-through-instance,-google-default-arguments,-readability-identifier-length")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-modernize-use-trailing-return-type,-hicpp-use-auto,-modernize-use-auto,-modernize-use-trailing-return-type")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-readability-uppercase-literal-suffix,-hicpp-uppercase-literal-suffix")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-fuchsia-default-arguments,-fuchsia-default-arguments-calls,-fuchsia-default-arguments-declarations")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-non-private-member-variables-in-classes,-misc-non-private-member-variables-in-classes")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-pro-type-member-init,-hicpp-member-init,-cppcoreguidelines-prefer-member-initializer")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-modernize-use-default-member-init")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-clang-analyzer-optin.cplusplus.VirtualCall")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-readability-redundant-access-specifiers")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-pro-type-vararg,-hicpp-vararg")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-boost-use-to-string")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-pro-bounds-array-to-pointer-decay,-hicpp-no-array-decay")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-google-runtime-references,-readability-convert-member-functions-to-static")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-fuchsia-overloaded-operator,-fuchsia-multiple-inheritance,-fuchsia-statically-constructed-objects")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-macro-usage")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-llvmlibc-*")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-hicpp-special-member-functions,-cppcoreguidelines-special-member-functions,-bugprone-suspicious-include")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-owning-memory")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cert-err58-cpp")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-readability-function-cognitive-complexity")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-misc-no-recursion")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-llvm-header-guard")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-readability-avoid-const-params-in-decls")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-altera-unroll-loops,-altera-struct-pack-align,-altera-id-dependent-backward-branch")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-modernize-avoid-bind")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-cppcoreguidelines-explicit-virtual-functions,-hicpp-use-override,-modernize-use-override")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-bugprone-reserved-identifier,-bugprone-infinite-loop,-cert-dcl51-cpp,-cert-dcl37-c,-readability-identifier-naming")
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},-abseil-*")

        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY},${CMAKEUT_CLANG_TIDY_EXTRA_IGNORES}")

        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY}" PARENT_SCOPE)
    endif()


    set(CXX_GENERIC "-std=${STANDARD} ${CXX_WARNINGS} ${CXX_OTHER} ${CXX_SANITIZERS}")


    set (CMAKEUT_CXX_FLAGS "${CXX_GENERIC}" PARENT_SCOPE)
endfunction()
