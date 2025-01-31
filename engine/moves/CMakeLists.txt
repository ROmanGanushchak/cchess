set(ENGINE_MOVES_SOURCES
    attacks.c
    slidingMove.c
    moves.c
    unsafeMoves.c
)

add_library(engine_moves STATIC ${ENGINE_MOVES_SOURCES})
target_include_directories(engine_moves PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})