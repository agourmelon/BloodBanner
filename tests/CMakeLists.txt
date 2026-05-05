# ── Google Test ───────────────────────────────────────────────────────────────

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

enable_testing()

# ── Outils ────────────────────────────────────────────────────────────────────

add_library(test_utils INTERFACE)
target_include_directories(test_utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# ── Tests unitaires ───────────────────────────────────────────────────────────

add_executable(test_houses     common/test_houses.cpp)
target_link_libraries(test_houses     PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_houses)

add_executable(test_units      common/test_units.cpp)
target_link_libraries(test_units      PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_units)

add_executable(test_orders     common/test_orders.cpp)
target_link_libraries(test_orders     PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_orders)

add_executable(test_provinces  common/test_provinces.cpp)
target_link_libraries(test_provinces  PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_provinces)

add_executable(test_game_state common/test_game_state.cpp)
target_link_libraries(test_game_state PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_game_state)

add_executable(test_map_generator server/test_map_generator.cpp)
target_link_libraries(test_map_generator PRIVATE map_generator_lib test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_map_generator)

# ── Tests d'intégration ───────────────────────────────────────────────────────

add_executable(test_map         integration/test_map.cpp)
target_link_libraries(test_map         PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_map)

add_executable(test_recruitment integration/test_recruitment.cpp)
target_link_libraries(test_recruitment PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_recruitment)

add_executable(test_combat      integration/test_combat.cpp)
target_link_libraries(test_combat      PRIVATE common test_utils GTest::gtest_main)
gtest_add_tests(TARGET test_combat)
