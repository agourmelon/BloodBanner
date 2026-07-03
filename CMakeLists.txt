cmake_minimum_required(VERSION 3.28)
project(BloodBanner CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Flags de compilation ──────────────────────────────────────────────────────

if(MSVC) # meh would we use msvc ?
    add_compile_options(/W4 /WX /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang") #REFACTO need better gestion here soz
        add_compile_options(-stdlib=libc++)
        add_link_options(-stdlib=libc++)
    endif()
endif()

# ── Sous-projets ──────────────────────────────────────────────────────────────

add_subdirectory(src)
add_subdirectory(tests)
