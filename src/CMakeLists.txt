# ── Bibliothèque commune (header-only) ────────────────────────────────────────

add_library(common INTERFACE)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# ── Serveur ───────────────────────────────────────────────────────────────────

add_executable(server
    server/main.cpp
    server/map_generator.cpp
)
target_link_libraries(server PRIVATE common)

# ── Client ────────────────────────────────────────────────────────────────────

add_executable(client client/main.cpp)
target_link_libraries(client PRIVATE common)
