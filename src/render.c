#include "render.h"

static void draw_bar(SDL_Renderer* r, int x, int y, int w, int h, int value, Uint8 rr, Uint8 gg, Uint8 bb) {
    // arka
    SDL_Rect bg = {x, y, w, h};
    SDL_SetRenderDrawColor(r, 40, 40, 55, 255);
    SDL_RenderFillRect(r, &bg);

    // dolu kısım
    int fillW = (w * value) / 100;
    SDL_Rect fg = {x, y, fillW, h};
    SDL_SetRenderDrawColor(r, rr, gg, bb, 255);
    SDL_RenderFillRect(r, &fg);

    // çerçeve
    SDL_SetRenderDrawColor(r, 180, 180, 200, 255);
    SDL_RenderDrawRect(r, &bg);
}

void render_clear(SDL_Renderer* r) {
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);
}

void render_hud(SDL_Renderer* r, const Stats* s) {
    // HUD alanı
    SDL_Rect panel = {20, 20, 320, 110};
    SDL_SetRenderDrawColor(r, 15, 15, 25, 220);
    SDL_RenderFillRect(r, &panel);
    SDL_SetRenderDrawColor(r, 180, 180, 200, 255);
    SDL_RenderDrawRect(r, &panel);

    draw_bar(r, 40, 40, 280, 18, s->hunger,    220, 140, 80);
    draw_bar(r, 40, 70, 280, 18, s->happiness, 140, 220, 160);
    draw_bar(r, 40,100, 280, 18, s->energy,    120, 160, 240);
}

void render_present(SDL_Renderer* r) {
    SDL_RenderPresent(r);
}
void render_button(SDL_Renderer* r, SDL_Rect rect) {
    SDL_SetRenderDrawColor(r, 20, 20, 30, 255);
    SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawColor(r, 200, 200, 220, 255);
    SDL_RenderDrawRect(r, &rect);
}
static TTF_Font* font = NULL;

void render_text(SDL_Renderer* r, const char* text, int x, int y) {
    if (!font) {
        font = TTF_OpenFont("assets/font.ttf", 20);
        if (!font) return;
    }

    SDL_Color color = { 230, 230, 240, 255 };
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text, color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);

    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_RenderCopy(r, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}
