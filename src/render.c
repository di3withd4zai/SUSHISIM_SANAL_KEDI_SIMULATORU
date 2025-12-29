/**
 * Dosya: render.c
 * Amaç: Ekrana grafiksel öğelerin çizilmesinden sorumludur.
 * Yazı yazma, durum çubukları (bar), butonlar ve efekt çizimleri burada yapılır.
 */

#include "render.h"
#include <stdio.h> 

// Fontlar için statik değişkenler
static TTF_Font* fontSmall = NULL;
static TTF_Font* fontBig = NULL;

// Font yükleme yardımcısı (Performans için yüklü fontu tekrar kullanır)
static TTF_Font* get_font(int size) {
    if (size > 30) {
        if (!fontBig) fontBig = TTF_OpenFont("assets/font.ttf", 64);
        return fontBig;
    } else {
        if (!fontSmall) fontSmall = TTF_OpenFont("assets/font.ttf", 20);
        return fontSmall;
    }
}

// Belirli bir koordinata yazı yazma fonksiyonu
void render_text(SDL_Renderer* r, const char* text, int x, int y, SDL_Color color) {
    TTF_Font* f = get_font(20); // Küçük fontu al
    if (!f) return;

    // Yazıyı yüzeye (surface) çiz
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text, color);
    if (!surf) return;

    // Yüzeyi dokuya (texture) çevir (GPU için)
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_Rect dst = { x, y, surf->w, surf->h };
    
    // Ekrana kopyala
    SDL_RenderCopy(r, tex, NULL, &dst);

    // Geçici hafızayı temizle
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// Ekrana ortalanmış yazı yazma (Başlıklar için)
void render_text_centered(SDL_Renderer* r, const char* text, int y, int size, SDL_Color color) {
    TTF_Font* f = get_font(size);
    if (!f) return;

    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, text, color);
    if (!surf) return;

    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    
    // X koordinatını ekran genişliğine göre ortala: (EkranGenişliği - YazıGenişliği) / 2
    int x = (960 - surf->w) / 2;
    SDL_Rect dst = { x, y, surf->w, surf->h };
    
    SDL_RenderCopy(r, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// Durum Çubuğu (Bar) Çizimi (Örn: Can, Açlık barı)
static void draw_bar(SDL_Renderer* r, int x, int y, int w, int h, int value, Uint8 rr, Uint8 gg, Uint8 bb) {
    // 1. Arka planı çiz (Gri)
    SDL_Rect bg = {x, y, w, h};
    SDL_SetRenderDrawColor(r, 200, 200, 200, 255);
    SDL_RenderFillRect(r, &bg);

    // 2. Doluluk oranını hesapla ve çiz (Renkli)
    int fillW = (w * value) / 100; // Genişlik * Yüzde
    SDL_Rect fg = {x, y, fillW, h};
    SDL_SetRenderDrawColor(r, rr, gg, bb, 255); // İstenen renk
    SDL_RenderFillRect(r, &fg);

    // 3. Çerçeve çiz (Koyu Gri)
    SDL_SetRenderDrawColor(r, 50, 50, 50, 255);
    SDL_RenderDrawRect(r, &bg);
}

// Kaka Çizimi (Kahverengi kareler)
void render_poops(SDL_Renderer* r, int count) {
    SDL_SetRenderDrawColor(r, 101, 67, 33, 255); // Koyu Kahve
    for (int i = 0; i < count; i++) {
        // Her kakanın yeri biraz farklı olsun diye ofset ekliyoruz
        SDL_Rect p = { 500 + (i * 60), 400 + (i % 2) * 20, 24, 20 };
        SDL_RenderFillRect(r, &p);
        
        // Detay çizgisi
        SDL_SetRenderDrawColor(r, 80, 50, 20, 255);
        SDL_RenderDrawRect(r, &p);
        SDL_SetRenderDrawColor(r, 101, 67, 33, 255); 
    }
}

// Kalp Efekti Çizimi (Piksel art şeklinde kalp)
void render_heart(SDL_Renderer* r, int x, int y, int alpha) {
    if (alpha < 0) alpha = 0;
    // BlendMode, şeffaflık (alpha) kullanabilmek için gereklidir
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(r, 255, 105, 180, alpha); // Hot Pink
    
    int s = 4; // Kalbin piksel boyutu
    // Kalbi oluşturan küçük karelerin koordinatları
    SDL_Rect rects[] = {
        {x-s, y-s, s, s}, {x+s, y-s, s, s},
        {x-2*s, y, s, s}, {x-s, y, s, s}, {x, y, s, s}, {x+s, y, s, s}, {x+2*s, y, s, s},
        {x-s, y+s, s, s}, {x, y+s, s, s}, {x+s, y+s, s, s},
        {x, y+2*s, s, s}
    };
    // Hepsini tek seferde çiz
    SDL_RenderFillRects(r, rects, 11);
    
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
}

// Arayüz (HUD) Çizimi
void render_hud(SDL_Renderer* r, const Stats* s) {
    // Sol üstteki panel kutusu
    SDL_Rect panel = {20, 20, 350, 150};
    SDL_SetRenderDrawColor(r, 255, 255, 255, 200); // Yarı saydam beyaz
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(r, &panel);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    
    SDL_SetRenderDrawColor(r, 100, 100, 100, 255); // Çerçeve
    SDL_RenderDrawRect(r, &panel);

    SDL_Color black = {0, 0, 0, 255}; 

    // Barları ve yazıları çiz
    render_text(r, "ACLIK", 30, 40, black);
    draw_bar(r, 140, 42, 200, 18, s->hunger, 255, 140, 0); // Turuncu

    render_text(r, "MUTLULUK", 30, 75, black);
    draw_bar(r, 140, 77, 200, 18, s->happiness, 255, 105, 180); // Pembe

    render_text(r, "ENERJI", 30, 110, black);
    draw_bar(r, 140, 112, 200, 18, s->energy, 65, 105, 225); // Mavi

    // Puanı metne çevirip yaz
    char scoreText[32];
    sprintf(scoreText, "PUAN: %d", s->score);
    render_text(r, scoreText, 30, 145, black);
}

// Çizimi ekrana yansıt (Buffer swap)
void render_present(SDL_Renderer* r) {
    SDL_RenderPresent(r);
}

// Buton Çizimi
void render_button(SDL_Renderer* r, SDL_Rect rect) {
    SDL_SetRenderDrawColor(r, 100, 149, 237, 255); // Mavi tonu dolgu
    SDL_RenderFillRect(r, &rect);
    SDL_SetRenderDrawColor(r, 25, 25, 112, 255);   // Koyu lacivert çerçeve
    SDL_RenderDrawRect(r, &rect);
}