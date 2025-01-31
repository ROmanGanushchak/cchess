set(ENGINE_SOURCES
    engine.c
    types.c
    fen.c
    pgn.c
)

add_subdirectory(moves)
add_library(engine STATIC ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})