/**
 * Dosya: src/player.c
 * Aciklama: Kedinin hareketi ve animasyonlari.
 */

#include "player.h"
#include <math.h>

#define SCALE 4
#define FRAME_W 32
#define FRAME_H 32
#define SPEED 220.0f
#define ARRIVE_RADIUS 16.0f

#define IDLE_FRAMES 6
#define WALK_FRAMES 6
#define EAT_FRAMES 4
#define SLEEP_FRAMES 2
#define PLAY_FRAMES 4

static int get_texture_frames(SDL_Texture* tex) {
    int texW = 0;
    int texH = 0;
    if (!tex) return 1;
    if (SDL_QueryTexture(tex, NULL, NULL, &texW, &texH) != 0) return 1;
    if (texH > 0 && (texW % texH) == 0) return texW / texH;
    if (texW >= FRAME_W && (texW % FRAME_W) == 0) return texW / FRAME_W;
    return 1;
}

static void get_anim_config(AnimationState state, int* frames, float* frameDelay) {
    *frames = IDLE_FRAMES;
    *frameDelay = 0.18f;

    switch (state) {
        case ANIM_WALK:
            *frames = WALK_FRAMES;
            *frameDelay = 0.12f;
            break;
        case ANIM_EAT:
            *frames = EAT_FRAMES;
            *frameDelay = 0.12f;
            break;
        case ANIM_SLEEP:
            *frames = SLEEP_FRAMES;
            *frameDelay = 0.12f;
            break;
        case ANIM_PLAY:
            *frames = PLAY_FRAMES;
            *frameDelay = 0.12f;
            break;
        default:
            break;
    }
}

static float get_action_duration(AnimationState state) {
    switch (state) {
        case ANIM_EAT:
            return 0.6f;
        case ANIM_SLEEP:
            return 4.2f;
        case ANIM_PLAY:
            return 1.5f;
        default:
            return 0.0f;
    }
}

static SDL_Texture* get_anim_texture(Player* p, AnimationState state, int* framesOut, float* frameDelayOut) {
    SDL_Texture* tex = NULL;
    int frames = 1;
    float frameDelay = 0.2f;

    get_anim_config(state, &frames, &frameDelay);

    switch (state) {
        case ANIM_WALK:
            tex = p->texWalk;
            break;
        case ANIM_EAT:
            tex = p->texEat;
            break;
        case ANIM_SLEEP:
            tex = p->texSleep;
            break;
        case ANIM_PLAY:
            tex = p->texPlay;
            break;
        default:
            tex = p->texIdle;
            break;
    }

    if (!tex) {
        tex = p->texIdle;
        get_anim_config(ANIM_IDLE, &frames, &frameDelay);
    }

    if (tex) {
        int texFrames = get_texture_frames(tex);
        if (texFrames > 0) frames = texFrames;
    }

    if (framesOut) *framesOut = frames;
    if (frameDelayOut) *frameDelayOut = frameDelay;

    return tex;
}

void player_init(Player* p, SDL_Texture* tIdle, SDL_Texture* tWalk, SDL_Texture* tEat, SDL_Texture* tSleep, SDL_Texture* tPlay) {
    p->x = 600;
    p->y = 350;
    p->targetX = p->x;
    p->targetY = p->y;
    p->hasTarget = 0;
    p->animState = ANIM_IDLE;
    p->actionState = ANIM_IDLE;
    p->pendingAction = ACTION_NONE;
    p->actionTimer = 0.0f;

    p->texIdle = tIdle;
    p->texWalk = tWalk;
    p->texEat = tEat;
    p->texSleep = tSleep;
    p->texPlay = tPlay;

    p->w = FRAME_W;
    p->h = FRAME_H;
    p->frameIndex = 0;
    p->frameTimer = 0.0f;
    p->flip = SDL_FLIP_NONE;
    p->isEating = 0;
    p->eatTimer = 0.0f;
    p->playTimer = 0.0f;
}

void player_set_target(Player* p, float x, float y, AnimationState actionOnArrival) {
    float dx = x - p->x;
    float dy = y - p->y;
    float dist = sqrtf(dx * dx + dy * dy);

    p->targetX = x;
    p->targetY = y;
    p->actionState = actionOnArrival;
    p->pendingAction = ACTION_NONE;
    if (actionOnArrival == ANIM_EAT) p->pendingAction = ACTION_FEED;
    else if (actionOnArrival == ANIM_PLAY) p->pendingAction = ACTION_PLAY;
    else if (actionOnArrival == ANIM_SLEEP) p->pendingAction = ACTION_SLEEP;
    p->hasTarget = 1;

    if (dist <= ARRIVE_RADIUS) {
        p->x = x;
        p->y = y;
        p->hasTarget = 0;
        if (p->pendingAction == ACTION_PLAY) {
            p->animState = ANIM_PLAY;
            p->playTimer = get_action_duration(ANIM_PLAY);
        } else if (p->pendingAction == ACTION_FEED) {
            p->animState = ANIM_EAT;
            p->actionTimer = get_action_duration(ANIM_EAT);
            p->isEating = 1;
            p->eatTimer = 0.6f;
        } else if (p->pendingAction == ACTION_SLEEP) {
            p->animState = ANIM_SLEEP;
            p->actionTimer = get_action_duration(ANIM_SLEEP);
        } else {
            p->animState = ANIM_IDLE;
        }
        p->pendingAction = ACTION_NONE;
    } else {
        p->animState = ANIM_WALK;
    }

    p->frameIndex = 0;
    p->frameTimer = 0.0f;
}

void player_update(Player* p, float dt) {
    int frames = 1;
    float frameDelay = 0.2f;

    get_anim_texture(p, p->animState, &frames, &frameDelay);

    p->frameTimer += dt;
    if (p->frameTimer >= frameDelay) {
        p->frameIndex = (p->frameIndex + 1) % frames;
        p->frameTimer = 0.0f;
    }

    if (p->animState == ANIM_WALK && p->hasTarget) {
        float dx = p->targetX - p->x;
        float dy = p->targetY - p->y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist <= ARRIVE_RADIUS) {
            p->x = p->targetX;
            p->y = p->targetY;
            p->hasTarget = 0;
            if (p->pendingAction == ACTION_PLAY) {
                p->animState = ANIM_PLAY;
                p->playTimer = get_action_duration(ANIM_PLAY);
            } else if (p->pendingAction == ACTION_FEED) {
                p->animState = ANIM_EAT;
                p->actionTimer = get_action_duration(ANIM_EAT);
                p->isEating = 1;
                p->eatTimer = 0.6f;
            } else if (p->pendingAction == ACTION_SLEEP) {
                p->animState = ANIM_SLEEP;
                p->actionTimer = get_action_duration(ANIM_SLEEP);
            } else {
                p->animState = ANIM_IDLE;
            }
            p->pendingAction = ACTION_NONE;
            p->frameIndex = 0;
            p->frameTimer = 0.0f;
        } else if (dist > 0.001f) {
            p->x += (dx / dist) * SPEED * dt;
            p->y += (dy / dist) * SPEED * dt;
            p->flip = (dx < 0.0f) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        }
    } else if (p->animState == ANIM_EAT || p->animState == ANIM_SLEEP) {
        p->actionTimer -= dt;
        if (p->actionTimer <= 0.0f) {
            p->animState = ANIM_IDLE;
            p->frameIndex = 0;
            p->frameTimer = 0.0f;
        }
    }

    if (p->animState == ANIM_PLAY) {
        p->playTimer -= dt;
        if (p->playTimer <= 0.0f) {
            p->playTimer = 0.0f;
            p->animState = ANIM_IDLE;
            p->frameIndex = 0;
            p->frameTimer = 0.0f;
        }
    }

    if (p->isEating) {
        p->eatTimer -= dt;
        if (p->eatTimer <= 0.0f) {
            p->isEating = 0;
            p->eatTimer = 0.0f;
        }
    }
}

void player_render(Player* p, SDL_Renderer* r) {
    int frames = 1;
    float frameDelay = 0.2f;
    SDL_Texture* tex = get_anim_texture(p, p->animState, &frames, &frameDelay);

    SDL_Rect src = { p->frameIndex * p->w, 0, p->w, p->h };
    SDL_Rect dst = { (int)p->x, (int)p->y, p->w * SCALE, p->h * SCALE };

    if (p->isEating && p->animState == ANIM_EAT) {
        float t = (float)SDL_GetTicks();
        dst.x += (int)(sinf(t * 0.08f) * 2.5f);
        dst.y += (int)(-fabsf(sinf(t * 0.10f)) * 2.0f);
    }

    SDL_RenderCopyEx(r, tex, &src, &dst, 0.0, NULL, p->flip);
}
