#pragma once
#include <SDL.h>

// Animasyon durumları (Duran veya Yürüyen)
typedef enum { ANIM_IDLE = 0, ANIM_WALK = 1 } AnimState;

// Oyuncu (Kedi) Veri Yapısı
typedef struct {
    float x, y;     // Konum
    float speed;    // Hız

    AnimState state; // Şu anki animasyon durumu
    int frameIndex;  // Hangi karenin çizileceği
    float frameTimer;
    
    // YENİ: Yüzün baktığı yönü tutmak için değişken (Aynalama)
    SDL_RendererFlip flip; 

    SDL_Texture* sheet; // Görsel kaynak
    int frameW, frameH; // Kare boyutları
} Player;

// Fonksiyon tanımları...
void player_init(Player* p, SDL_Texture* sheet, int frameW, int frameH);
void player_handle_input(Player* p, const Uint8* keys, float dt);
void player_update(Player* p, float dt);
void player_render(Player* p, SDL_Renderer* r);