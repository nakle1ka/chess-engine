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

run_tests: run_attacks_tests run_rays_tests

run_attacks_tests:
	@g++ \
	tests/attacks_test.cpp \
	internal/attacks/attacks.cpp \
	internal/rays/rays.cpp \
	-I internal/attacks \
	-I tests \
	-o app.exe \
	-Wall -Wextra

	@./app.exe

run_rays_tests:
	@g++ \
	tests/rays_test.cpp \
	internal/rays/rays.cpp \
	-I internal/rays \
	-I tests \
	-o app_rays.exe \
	-Wall -Wextra

	@./app.exe
