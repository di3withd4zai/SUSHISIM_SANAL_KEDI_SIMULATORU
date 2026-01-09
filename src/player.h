#pragma once
#include <SDL.h>

// Kedinin Yapay Zeka Durumları
typedef enum {
    STATE_IDLE,     // Boşta
    STATE_WALKING,  // Bir hedefe yürüyor
    STATE_EATING,   // Yemek yiyor
    STATE_SLEEPING, // Uyuyor
    STATE_PLAYING   // Oynuyor
} PlayerState;

typedef struct {
    float x, y;             // Mevcut konum
    float targetX, targetY; // Hedef konum
    
    PlayerState state;      // Şu an ne yapıyor?
    PlayerState nextState;  // Hedefe varınca ne yapacak?
    float actionTimer;      // Eylem süresi (Yemek ne kadar sürecek?)

    SDL_Texture* texIdle;   // Kedi resmi
    SDL_Texture* texBox;    // Kutu resmi (Box3.png)
    
    int frameIndex;         // Animasyon karesi
    float frameTimer;       // Animasyon hızı
    SDL_RendererFlip flip;  // Yönü (Sağa/Sola)
    
    int w, h;               // Boyutlar
} Player;

void player_init(Player* p, SDL_Texture* tIdle, SDL_Texture* tBox);
void player_set_target(Player* p, float x, float y, PlayerState actionOnArrival);
void player_update(Player* p, float dt);
void player_render(Player* p, SDL_Renderer* r);