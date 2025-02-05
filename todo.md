ToDo:
1. Make pgn view and forking(ok if linear forks)

Additional:
1. Rework folders, make it better and formolize common

Add to consider project finished:
1. PGN exporter and importer
2. Add material difference
3. Check the correctness of program with possible move counter

/* cmake --build . && gdb -batch -ex "run" -ex "bt" cchess.exe */

https://en.wikipedia.org/wiki/Portable_Game_Notation
https://www.chess.com/terms/chess-pgn
https://www.chess.com/terms/chess-notation

https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
https://analog-hors.github.io/site/magic-bitboards/


1.c4 e5 2.Nc3 Nf6 3.Nf3 Nc6 4.e3 Bb4 5.Qc2 Bxc3 6.bxc3 O-O 7.e4 d6 8.Be2 Ne7 9.O-O Ng6 10.Re1 Re8 11.a4 c6 12.d3 d5 13.h3 h6 14.Bf1 dxe4 15.dxe4 Be6 16.Be3 Nd7 17.a5 b6 18.Qa4 Rc8 19.Red1 Qc7 20.Ne1 Ngf8 21.Nd3 c5 22.Ne1 Nb8 23.Nc2 Nc6 24.axb6 axb6 25.f3 Qb7 26.Bf2 Ra8 27.Qb5 Nd7 28.Ne3 Rxa1 29.Rxa1 Ra8 30.Rxa8+ Qxa8 31.Nd5 Na5 32.g3 Kh8 33.f4 exf4 34.gxf4 Qd8 35.Bg2 f6 36.Bg3 Kg8 37.Kh2 Kf7 38.Bf3 Kg8 39.Kg2 f5 40.e5 Kf7 41.h4 g6 42.Kf2 h5 43.Kg2 Kg7 44.Bf2 Kf7 45.Ne3 Kg7 46.Nd5 Kf7 47.Kh2 Kg7 48.Kg3 Qb8 49.Kh2 Qd8 50.Ne3 Nf8 51.Kg3 Qb8 52.Bg2 Bd7 53.Qb1 Be6 54.Bd5 Bd7 55.Nf1 Qd8 56.Nh2 Be6 57.Qa2 Bf7 58.Nf3 Ne6 59.Qb1 Qc7 60.Qb5 Nb7 61.Nd2 Nbd8 62.Qa4 Kf8 63.Nf1 Be8 64.Qa1 Bc6 65.Ne3 Qb7 66.Qb1 Ke8 67.Bxc6+ Qxc6 68.Nd5 Nc7 69.Nxb6 Nde6 70.Nc8 Kd7 71.Nd6 Ne8 72.Nb5 Qe4 73.Qd1+ Ke7 74.Qd2 Qxc4 75.Na3 Qb3 76.Nc2 Qa4 77.Be3 Qe4 78.Ne1 N8c7 79.Nd3 Qd5 80.c4 Qxc4 81.Nxc5 Nd5 82.Qa5 Qe2 83.Qa7+ Nec7 84.Bf2 Qg4+ 85.Kh2 Qxf4+ 86.Kg1 Qg4+ 87.Kh2 Qe2 88.Kg1 Qg4+ 89.Kh2 Qf4+ 90.Kg1 Qxe5 91.Qa4 Kf7 92.Nd3 Qd6 93.Qd4 Ne6 94.Ne5+ Kf6 95.Ng4+ Ke7 96.Qa7+ Qc7 97.Qa3+ Kf7 98.Ne3 Nxe3 99.Qxe3 Qf4 100.Qa7+ Qc7 101.Qe3 Qd7 102.Qb3 Qc6 103.Qa2 Qb7 104.Be3 Qe7 105.Qc4 Kg7 106.Qc3+ Qf6 107.Qb4 f4 108.Bf2 Kh6 109.Qd6 g5 110.hxg5+ Qxg5+ 111.Kh2 Qf5 112.Qb8 Qc2 113.Kg1 Qd1+ 114.Kh2 Qc2 115.Kg1 Qd1+ 116.Kh2 Qe2 117.Kg1 Qg4+ 118.Kh2 f3 119.Bg3 Qd4 120.Qe5 Qxe5 121.Bxe5 h4