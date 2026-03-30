build:
	clang -std=c17 ./src/*.c -I/opt/homebrew/opt/sdl2/include -L/opt/homebrew/opt/sdl2/lib -lSDL2 -o raycast;

build-debug:
	clang -g -std=c17 ./src/*.c -I/opt/homebrew/opt/sdl2/include -L/opt/homebrew/opt/sdl2/lib -lSDL2 -o raycast;
run:
	./raycast;

run-debug:
	lldb ./raycast;

clean:
	rm raycast;
