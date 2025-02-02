set(ENGINE_MOVES_SOURCES
    attacks.c
    slidingMove.c
    moves.c
    movesPossible.c
    unsafeMoves.c
)

add_library(engine_moves OBJECT ${ENGINE_MOVES_SOURCES})
target_include_directories(engine_moves PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})