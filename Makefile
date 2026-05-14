run_app:
	@g++ \
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

	@./app.exe
