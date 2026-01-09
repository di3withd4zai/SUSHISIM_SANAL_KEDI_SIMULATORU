/**
 * Dosya: src/render.c
 * Açıklama: Çizim fonksiyonlarının kodları.
 */

#include "render.h"

// Fontları hafızada tutmak için static değişkenler
static TTF_Font* fontSmall = NULL; // Küçük yazılar (Butonlar vb.)
static TTF_Font* fontBig = NULL;   // Büyük yazılar (Başlıklar)

int render_init_fonts() {
    if (TTF_Init() == -1) return 0;
    // 'assets' klasöründeki font dosyasını yükle
    fontSmall = TTF_OpenFont("assets/font.ttf", 24);
    fontBig   = TTF_OpenFont("assets/font.ttf", 64);
    return (fontSmall && fontBig); // İkisi de yüklendiyse 1 döner
}

// İstenen boyuta göre doğru fontu seçer
static TTF_Font* get_font(int size) {
    if (size > 30) return fontBig;
    return fontSmall;
}

// Yazıyı ortalayarak yazma (Matematiksel hesaplama içerir)
void render_text_centered_x(SDL_Renderer* r, const char* text, int screenW, int y, SDL_Color color, int size) {
    TTF_Font* f = get_font(size);
    if (!f) return;
    
    // Yazıyı yüzey (surface) olarak oluştur
    SDL_Surface* surf = TTF_RenderText_Blended(f, text, color);
    if (!surf) return;
    
    // Yüzeyi dokuya (texture) çevir (GPU için)
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    
    // ORTALAMA FORMÜLÜ: (Ekran Genişliği - Yazı Genişliği) / 2
    int x = (screenW - surf->w) / 2;
    
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(r, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// Buton içine yazı ortalama
void render_text_in_box(SDL_Renderer* r, const char* text, SDL_Rect box, SDL_Color color) {
    TTF_Font* f = fontSmall;
    if (!f) return;
    
    SDL_Surface* surf = TTF_RenderText_Blended(f, text, color);
    if (!surf) return;
    
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    
    // Kutunun merkezini bulma formülü
    int x = box.x + (box.w - surf->w) / 2;
    int y = box.y + (box.h - surf->h) / 2;
    
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(r, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// Normal yazı yazma
void render_text(SDL_Renderer* r, const char* text, int x, int y, SDL_Color color, int size) {
    TTF_Font* f = get_font(size);
    if (!f) return;
    SDL_Surface* surf = TTF_RenderText_Blended(f, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(r, tex, NULL, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// Güvenli resim çizme fonksiyonu
void render_sprite(SDL_Renderer* r, SDL_Texture* tex, SDL_Rect src, SDL_Rect dst, SDL_Color debugColor) {
    if (tex) {
        // Resim varsa, resimden 'src' parçasını alıp ekrana 'dst' konumuna çiz
        SDL_RenderCopy(r, tex, &src, &dst);
    } else {
        // Resim YÜKLENEMEDİYSE, yerine renkli bir kutu çiz.
        // Bu sayede oyun çökmez ve hatayı görsel olarak anlarız.
        SDL_SetRenderDrawColor(r, debugColor.r, debugColor.g, debugColor.b, 255);
        SDL_RenderFillRect(r, &dst);
    }
}