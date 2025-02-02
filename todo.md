ToDo:
1. Make invertMove for PGN
    1. Make List and add command for movement
    2. Fix lastMoveBefore inconsistency in invertMove
    3. Test invert move for correctness
2. Add forward and back with MOVE_DETAILS
3. Add forks for vatiations(ok for at most one)
4. Add combined struct for board and hist

Additional:
1. Think about validating only actual move in movePiece, removeInvalid only for the move itself
2. Rework folders, make it better and formolize common

Add to consider project finished:
1. PGN exporter and importer
2. Add material difference
3. Check the correctness of program with possible move counter



https://en.wikipedia.org/wiki/Portable_Game_Notation
https://www.chess.com/terms/chess-pgn
https://www.chess.com/terms/chess-notation

https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
https://analog-hors.github.io/site/magic-bitboards/

