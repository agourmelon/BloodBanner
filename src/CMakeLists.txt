# ── Bibliothèque commune (header-only) ────────────────────────────────────────

add_library(common INTERFACE)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# ── Bibliothèque map_generator ────────────────────────────────────────────────

add_library(map_generator_lib server/map_generator.cpp)
target_link_libraries(map_generator_lib PUBLIC common)

# ── Serveur ───────────────────────────────────────────────────────────────────

add_executable(server server/main.cpp)
target_link_libraries(server PRIVATE map_generator_lib)

# ── Client ────────────────────────────────────────────────────────────────────

add_executable(client client/main.cpp)
target_link_libraries(client PRIVATE common)
