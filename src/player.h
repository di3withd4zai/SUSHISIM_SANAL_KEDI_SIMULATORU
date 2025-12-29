#pragma once
#include <SDL.h>

typedef enum { ANIM_IDLE = 0, ANIM_WALK = 1 } AnimState;

typedef struct {
    float x, y;
    float speed;

    AnimState state;
    int frameIndex;
    float frameTimer;
    
    // YENİ: Yüzün baktığı yönü tutmak için değişken
    SDL_RendererFlip flip; 

    SDL_Texture* sheet;
    int frameW, frameH; 
} Player;

void player_init(Player* p, SDL_Texture* sheet, int frameW, int frameH);
void player_handle_input(Player* p, const Uint8* keys, float dt);
void player_update(Player* p, float dt);
void player_render(Player* p, SDL_Renderer* r);