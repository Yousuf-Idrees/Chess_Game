@echo off
echo Building ChessGame...
g++ -std=c++11 -O3 -o ChessGame.exe main.cpp Board.cpp Piece.cpp Move.cpp AI.cpp Game.cpp Timer.cpp SaveLoad.cpp
if %errorlevel% neq 0 (
    echo Build failed!
) else (
    echo Build successful!
    echo To play in terminal: .\ChessGame.exe
    echo To play GUI: Run python gui/server.py and open http://localhost:8080
)
