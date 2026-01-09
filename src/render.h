/**
 * Dosya: render.h
 * Amaç: Ekrana yazı yazma, resim çizme ve özel efekt fonksiyonlarını tanımlar.
 */

#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

// Yazı tiplerini (font) yükler. Başarılıysa 1 döner.
int render_init_fonts();

// Ekrana veya butona yazı yazar (Sol üst köşeye hizalı)
void render_text(SDL_Renderer* r, const char* text, int x, int y, SDL_Color color, int size);

// Yazıyı belirtilen genişliğe göre ortalayarak yazar (Başlıklar için)
void render_text_centered_x(SDL_Renderer* r, const char* text, int screenW, int y, SDL_Color color, int size);

// Yazıyı bir kutunun (butonun) tam ortasına yazar
void render_text_in_box(SDL_Renderer* r, const char* text, SDL_Rect box, SDL_Color color);

// Sprite Sheet'ten (büyük resimden) parça kesip çizer.
// Eğer resim yüklenemediyse, yerine renkli bir kutu çizer (Hata ayıklama için).
void render_sprite(SDL_Renderer* r, SDL_Texture* tex, SDL_Rect src, SDL_Rect dst, SDL_Color debugColor);