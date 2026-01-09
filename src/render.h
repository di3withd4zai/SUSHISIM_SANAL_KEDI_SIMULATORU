#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "stats.h"

// Çizim fonksiyonları tanımları...
void render_text(SDL_Renderer* r, const char* text, int x, int y, SDL_Color color);
void render_text_centered(SDL_Renderer* r, const char* text, int y, int size, SDL_Color color);
void render_text_centered_in_rect(SDL_Renderer* r, const char* text, SDL_Rect rect, SDL_Color color);

void render_clear(SDL_Renderer* r);
void render_hud(SDL_Renderer* r, const Stats* s);
void render_present(SDL_Renderer* r);

typedef struct { SDL_Rect rect; } Button;
void render_button(SDL_Renderer* r, SDL_Rect rect);

// Efekt çizimleri
void render_poops(SDL_Renderer* r, int count);
void render_heart(SDL_Renderer* r, int x, int y, int alpha);
