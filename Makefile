build:
	clang -std=c17 ./src/*.c -I/opt/homebrew/opt/sdl2/include -L/opt/homebrew/opt/sdl2/lib -lSDL2 -o raycast;
run:
	./raycast;

clean:
	rm raycast;
