#include "player.h"

void player_init(Player* p, SDL_Texture* sheet, int frameW, int frameH) {
    p->x = 200; p->y = 280;
    p->speed = 220.0f;

    p->state = ANIM_IDLE;
    p->frameIndex = 0;
    p->frameTimer = 0;

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

    // normalize (çok kasmadan)
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
        return;
    }

    float frameDuration = 0.12f;
    p->frameTimer += dt;
    if (p->frameTimer >= frameDuration) {
        p->frameTimer = 0;
        p->frameIndex = (p->frameIndex + 1) % frameCount;
    }
}


void player_render(Player* p, SDL_Renderer* r) {
    // Sheet düzeni:
    // satır 0: idle  (4 frame)
    // satır 1: walk  (4 frame)
    int row = (p->state == ANIM_IDLE) ? 0 : 1;

    SDL_Rect src = { p->frameIndex * p->frameW, row * p->frameH, p->frameW, p->frameH };
    SDL_Rect dst = { (int)p->x, (int)p->y, p->frameW * 2, p->frameH * 2 }; // 2x büyüt (pixel vibe)

    SDL_RenderCopy(r, p->sheet, &src, &dst);
}
