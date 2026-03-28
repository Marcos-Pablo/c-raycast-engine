#include "base_defs.h"
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

bool initialize_window();
void destroy_window();
void setup();
void render();
void processInput();
void update();

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool isGameRunning = false;
int ticksLastFrame = 0;

int playerx;
int playery;

int main() {
    isGameRunning = initialize_window();

    setup();
    while (isGameRunning) {
        processInput();
        update();
        render();
    }

    destroy_window();
    return 0;
}

bool initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    return true;
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void setup() {
    playerx = 0;
    playery = 0;
}

void processInput() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        isGameRunning = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            isGameRunning = false;
        }
        break;
    }
}

void update() {
    // waste some time until we reach the target frame time length
    int timeToWait = FRAME_TIME_LENGHT - (SDL_GetTicks() - ticksLastFrame);

    if (timeToWait > 0 && timeToWait <= FRAME_TIME_LENGHT) {
        SDL_Delay(timeToWait);
    }

    float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;

    ticksLastFrame = SDL_GetTicks();

    playerx += 50 * deltaTime;
    playery += 50 * deltaTime;
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect rect = {playerx, playery, 20, 20};
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}
