#include "player.h"

void player_init(Player* p, SDL_Texture* sheet, int frameW, int frameH) {
    p->x = 200; p->y = 280;
    p->speed = 350.0f;

    p->state = ANIM_IDLE;
    p->frameIndex = 0;
    p->frameTimer = 0;
    
    p->flip = SDL_FLIP_NONE; // Başlangıçta normal (sağa) baksın

    p->sheet = sheet;
    p->frameW = frameW;
    p->frameH = frameH;
}

void player_handle_input(Player* p, const Uint8* keys, float dt) {
    float vx = 0, vy = 0;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])  vx -= 1;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) vx += 1;
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])    vy -= 1;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])  vy += 1;

    int moving = (vx != 0 || vy != 0);
    p->state = moving ? ANIM_WALK : ANIM_IDLE;

    // YENİ: Yön Kontrolü
    // Sadece sağa veya sola basılıyorsa yönü değiştir.
    // (Yukarı/aşağı basarken en son baktığı yöne bakmaya devam eder)
    if (vx < 0) p->flip = SDL_FLIP_HORIZONTAL; // Sola dön (Aynala)
    if (vx > 0) p->flip = SDL_FLIP_NONE;       // Sağa dön (Normal)

    // Hareket normalizasyonu ve pozisyon güncelleme
    if (moving) {
        float len = (vx*vx + vy*vy);
        if (len > 1) { vx *= 0.7071f; vy *= 0.7071f; }
        p->x += vx * p->speed * dt;
        p->y += vy * p->speed * dt;
    }
}

void player_update(Player* p, float dt) {
    int frameCount = 4;

    if (p->state == ANIM_IDLE) {
        p->frameIndex = 0;
        p->frameTimer = 0;
    } 
    else {
        float frameDuration = 0.12f; // Animasyon hızı
        p->frameTimer += dt;
        if (p->frameTimer >= frameDuration) {
            p->frameTimer = 0;
            p->frameIndex = (p->frameIndex + 1) % frameCount;
        }
    }

    // Sınır Kontrolü (Clamping)
    int catSize = 128; // 32 * 4
    if (p->x < 0) p->x = 0;
    if (p->y < 0) p->y = 0;
    if (p->x > 960 - catSize) p->x = 960 - catSize;
    if (p->y > 450 - catSize) p->y = 450 - catSize; 
}

void player_render(Player* p, SDL_Renderer* r) {
    // Animasyon satırını seç
    int row = (p->state == ANIM_IDLE) ? 0 : 1;

    SDL_Rect src = { p->frameIndex * p->frameW, row * p->frameH, p->frameW, p->frameH };
    SDL_Rect dst = { (int)p->x, (int)p->y, p->frameW * 4, p->frameH * 4 }; // 4 kat büyük çizim

    // YENİ: RenderCopyEx ile çizim (Döndürme ve Çevirme destekli)
    // 0.0 -> Açı (dönme yok)
    // NULL -> Dönme merkezi (ortası)
    // p->flip -> Yatay çevirme durumu
    SDL_RenderCopyEx(r, p->sheet, &src, &dst, 0.0, NULL, p->flip);
}
