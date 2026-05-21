cmake_minimum_required(VERSION 3.28)
project(BloodBanner CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ── Flags de compilation ──────────────────────────────────────────────────────

if(MSVC)
    add_compile_options(/W4 /WX /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# ── Sous-projets ──────────────────────────────────────────────────────────────

add_subdirectory(src)
add_subdirectory(tests)
