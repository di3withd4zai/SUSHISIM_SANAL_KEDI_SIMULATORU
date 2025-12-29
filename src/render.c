#include "render.h"
#include <stdio.h> // sprintf için gerekli

static TTF_Font* fontSmall = NULL;
static TTF_Font* fontBig = NULL;

// Yardımcı: Font yükleme
static TTF_Font* get_font(int size) {
    if (size > 30) {
        if (!fontBig) fontBig = TTF_OpenFont("assets/font.ttf", 64); // Başlık boyutu
        return fontBig;
    } else {
        if (!fontSmall) fontSmall = TTF_OpenFont("assets/font.ttf", 20); // Normal yazı boyutu
        return fontSmall;
    }
}

// Belirli bir koordinata yazı yazma
void render_text(SDL_Renderer* r, const char* text, int x, int y, SDL_Color color) {
    TTF_Font* f = get_font(20);
    if (!f) return;

    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text, color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_RenderCopy(r, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// Başlık gibi ortalanmış ve büyük yazılar için
void render_text_centered(SDL_Renderer* r, const char* text, int y, int size, SDL_Color color) {
    TTF_Font* f = get_font(size);
    if (!f) return;

    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text, color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    
    // Ekran genişliği 960 kabul edilerek ortalama hesabı:
    int x = (960 - surf->w) / 2;
    
    SDL_Rect dst = { x, y, surf->w, surf->h };
    SDL_RenderCopy(r, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

static void draw_bar(SDL_Renderer* r, int x, int y, int w, int h, int value, Uint8 rr, Uint8 gg, Uint8 bb) {
    // Arka plan (Koyu gri)
    SDL_Rect bg = {x, y, w, h};
    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    SDL_RenderFillRect(r, &bg);

    // Dolu kısım
    int fillW = (w * value) / 100;
    SDL_Rect fg = {x, y, fillW, h};
    SDL_SetRenderDrawColor(r, rr, gg, bb, 255);
    SDL_RenderFillRect(r, &fg);

    // Çerçeve
    SDL_SetRenderDrawColor(r, 50, 50, 50, 255);
    SDL_RenderDrawRect(r, &bg);
}

void render_clear(SDL_Renderer* r) {
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);
}

void render_hud(SDL_Renderer* r, const Stats* s) {
    // Panel
    SDL_Rect panel = {20, 20, 350, 150};
    SDL_SetRenderDrawColor(r, 255, 255, 255, 200); // Yarı saydam beyaz
    SDL_RenderFillRect(r, &panel);
    SDL_SetRenderDrawColor(r, 100, 100, 100, 255);
    SDL_RenderDrawRect(r, &panel);

    SDL_Color black = {0, 0, 0, 255}; // Yazılar SİYAH olsun ki okunsun

    // Etiketler ve Barlar
    render_text(r, "ACLIK", 30, 40, black);
    draw_bar(r, 140, 42, 200, 18, s->hunger, 255, 140, 0); // Turuncu

    render_text(r, "MUTLULUK", 30, 75, black);
    draw_bar(r, 140, 77, 200, 18, s->happiness, 255, 105, 180); // Pembe

    render_text(r, "ENERJI", 30, 110, black);
    draw_bar(r, 140, 112, 200, 18, s->energy, 65, 105, 225); // Mavi

    // Skor
    char scoreText[32];
    sprintf(scoreText, "PUAN: %d", s->score);
    render_text(r, scoreText, 30, 145, black);
}

void render_present(SDL_Renderer* r) {
    SDL_RenderPresent(r);
}

void render_button(SDL_Renderer* r, SDL_Rect rect) {
    SDL_SetRenderDrawColor(r, 100, 149, 237, 255); // Cornflower Blue
    SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawColor(r, 25, 25, 112, 255); // Midnight Blue (Çerçeve)
    SDL_RenderDrawRect(r, &rect);
}