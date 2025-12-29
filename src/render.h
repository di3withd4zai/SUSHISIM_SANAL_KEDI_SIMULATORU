#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "stats.h"

void render_text(SDL_Renderer* r, const char* text, int x, int y);
void render_clear(SDL_Renderer* r);
void render_hud(SDL_Renderer* r, const Stats* s);
void render_present(SDL_Renderer* r);
typedef struct {
    SDL_Rect rect;
} Button;
void render_button(SDL_Renderer* r, SDL_Rect rect);
