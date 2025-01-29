set(ENGINE_SOURCES
    engine.c
    figures.c
    slidingMove.c
    moves.c
    types.c
)

add_library(engine STATIC ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})