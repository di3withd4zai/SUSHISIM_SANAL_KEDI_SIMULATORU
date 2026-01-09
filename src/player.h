#pragma once
#include <SDL.h>

typedef enum {
    ANIM_IDLE,
    ANIM_WALK,
    ANIM_EAT,
    ANIM_SLEEP,
    ANIM_PLAY
} AnimationState;

typedef enum {
    ACTION_NONE,
    ACTION_FEED,
    ACTION_PLAY,
    ACTION_SLEEP
} ActionType;

typedef struct {
    float x, y;
    float targetX, targetY;
    int hasTarget;

    AnimationState animState;
    AnimationState actionState;
    ActionType pendingAction;
    float actionTimer;

    SDL_Texture* texIdle;
    SDL_Texture* texWalk;
    SDL_Texture* texEat;
    SDL_Texture* texSleep;
    SDL_Texture* texPlay;

    int frameIndex;
    float frameTimer;
    SDL_RendererFlip flip;

    int w, h;

    int isEating;
    float eatTimer;
    float playTimer;
} Player;

void player_init(Player* p, SDL_Texture* tIdle, SDL_Texture* tWalk, SDL_Texture* tEat, SDL_Texture* tSleep, SDL_Texture* tPlay);
void player_set_target(Player* p, float x, float y, AnimationState actionOnArrival);
void player_update(Player* p, float dt);
void player_render(Player* p, SDL_Renderer* r);
