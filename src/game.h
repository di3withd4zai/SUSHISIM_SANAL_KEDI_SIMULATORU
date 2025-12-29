#pragma once
#include <SDL.h>
#include <SDL_image.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    int running;

    Uint32 lastTick;
    float dt; // delta time (saniye)

} Game;

int game_init(Game* g, const char* title, int w, int h);
void game_handle_events(Game* g);
void game_update(Game* g);
void game_render(Game* g);
void game_cleanup(Game* g);
