# Chess Game - C++ Project

A complete C++ chess game featuring both Terminal and Web GUI interfaces, full rule validation, game persistence, and an AI opponent built with optimized minimax search heuristics.

## Features

* **Dual Interface Support:** Play via standard terminal or an interactive web GUI.
* **Minimax AI Opponent:** Features customizable depth searching with state memoization and intelligent pruning.
* **Game Management:** Full support for `undo`, `redo`, `save`, `load`, and move timers.
* **Algorithmic Optimizations:** Dynamic programming (transposition tables) and greedy move ordering for rapid move evaluation.

```markdown
## System Architecture

```mermaid
graph TD
    A[Chess Game System] --> B[Core C++ Backend]
    A --> C[User Interfaces]

    B --> B1[Board & Rules Engine]
    B --> B2[AI Engine - Minimax / Pruning]
    B --> B3[State Stack - FEN / save.dat]

    C --> C1[Terminal Interface CLI]
    C --> C2[Web GUI - Python + Browser]

## Setup & Compilation

### Prerequisites

* A C++ compiler supporting C++11 or higher (`g++`, `clang++`, or MSVC).
* Python 3.x (only required for running the optional Web GUI).

### Building the Project

Run the build script or compile manually using `g++`:

```bash
# Option A: Automatic build script (Windows)
build.bat

# Option B: Manual compilation via terminal
g++ -std=c++11 -O3 -o ChessGame main.cpp Board.cpp Piece.cpp Move.cpp AI.cpp Game.cpp Timer.cpp SaveLoad.cpp
