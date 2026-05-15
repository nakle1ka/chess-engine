# Chess Engine
A chess engine developed for educational purposes.

## Features
- ⚡ **Bitboards** — efficient position representation using 64-bit integers.
- 🧠 **Minimax with Alpha-Beta Pruning** — optimal search tree traversal.
- 🎯 **Evaluation Function** — accounts for piece placement on the board.
- 🖥️ **Console Interface** — classic move input and output.

## Architecture
```txt
app/
└── main.cpp
internal/
├── AI/            # Best move calculation and position evaluation
├── attacks/       # Bitboard-based attack generation
├── board/         # Board state and piece bitboards
├── chess_visual/  # Console position visualization
├── rays/          # Sliding rays for bishops, rooks, and queens
└── shared/        # Shared types and constants
Makefile
```

## Build and Run
```bash
make run_app

# Or compile manually
g++ \
app/main.cpp \
internal/rays/rays.cpp \
internal/board/board.cpp \
internal/attacks/attacks.cpp \
internal/chess_visual/chess_visual.cpp \
internal/AI/AI.cpp \
-I internal/AI \
-I internal/board \
-I internal/rays \
-I internal/attacks \
-I internal/chess_visual \
-o app.exe \
-Wall -Wextra

./app.exe
```
