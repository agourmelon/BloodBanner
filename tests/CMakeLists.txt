# ── Google Test ───────────────────────────────────────────────────────────────

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# ── Helper pour déclarer un test ──────────────────────────────────────────────

function(add_bloodbanner_test target)
    add_executable(${target} ${ARGN})
    target_link_libraries(${target} PRIVATE common GTest::gtest_main)
    gtest_discover_tests(${target})
endfunction()

# ── Tests unitaires ───────────────────────────────────────────────────────────

add_bloodbanner_test(test_houses     common/test_houses.cpp)
add_bloodbanner_test(test_units      common/test_units.cpp)
add_bloodbanner_test(test_orders     common/test_orders.cpp)
add_bloodbanner_test(test_provinces  common/test_provinces.cpp)
add_bloodbanner_test(test_game_state common/test_game_state.cpp)

# ── Tests d'intégration ───────────────────────────────────────────────────────

add_bloodbanner_test(test_map         integration/test_map.cpp)
add_bloodbanner_test(test_recruitment integration/test_recruitment.cpp)
add_bloodbanner_test(test_combat      integration/test_combat.cpp)
