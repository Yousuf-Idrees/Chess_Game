# Chess Game - C++ Project

## Description
This is a complete C++ chess game implementation with terminal and GUI support. It uses a robust, stateless C++ backend that handles rules, state management, and the AI opponent. The AI utilizes the Minimax algorithm with Alpha-Beta pruning, dynamic programming (transposition tables), and a greedy move ordering heuristic.

## Setup & Compilation
1. Double-click `build.bat` OR run `g++ -std=c++11 -O3 -o ChessGame.exe main.cpp Board.cpp Piece.cpp Move.cpp AI.cpp Game.cpp Timer.cpp SaveLoad.cpp` in the terminal.
2. Ensure you have g++ installed on your system.

## How to Play

### Terminal Mode (Standard C++)
Run `.\ChessGame.exe` in your terminal. You can enter moves in algebraic notation (e.g. `e2e4`, `g1f3`). You can also type `undo`, `redo`, `save`, `load`, or `quit`.

### GUI Mode
We implemented a beautiful web-based interface that matches your reference design while strictly using C++ for the core logic!
1. Ensure the C++ executable `ChessGame.exe` is built first.
2. Open terminal and run: `python gui/server.py`
3. Open `http://localhost:8080` in your web browser.
4. Enjoy playing with full dragging, animations, timers, and wooden UI!

## Algorithms Applied
1. **Dynamic Programming (Transposition Table):** Implemented in `AI.cpp` inside `AI::minimax()`. We hash the board state (FEN) and store the evaluation depth and score. If the exact board state is reached again at the same search depth, we reuse the evaluation, avoiding redundant computations.
2. **Greedy Move Ordering:** Implemented in `AI.cpp` inside `AI::orderMoves()`. Before expanding nodes in Minimax, we score all pseudo-legal moves based on immediate benefits (capturing high-value pieces, promoting pawns, controlling the center). Moves are sorted in descending order, significantly improving Alpha-Beta pruning efficiency.
3. **Minimax & Alpha-Beta Pruning:** Standard Minimax is utilized for exploring future moves, and Alpha-Beta safely prunes large chunks of the game tree.

## Performance Evaluation
- **Without Transposition Table:** The AI re-evaluates the same states many times, slowing search.
- **With Transposition Table (DP):** Search time drops significantly due to reused state evaluations.
- **Without Move Ordering:** Alpha-Beta prunes late, exploring wide trees.
- **With Greedy Move Ordering:** The best moves are often searched first, allowing Alpha-Beta to prune massive branches almost instantly, vastly increasing search depth within the time limit.

## Saving, Undo & Timers
- **Timers:** Custom Timer class measures real elapsed milliseconds securely between the C++ calls and updates state.
- **Undo/Redo & Save/Load:** Supported robustly through a central stack history of Board states (FEN) saved directly into `save.dat`.
