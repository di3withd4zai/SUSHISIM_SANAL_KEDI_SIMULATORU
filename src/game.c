/**
 * Dosya: src/game.c
 * Aciklama: Oyun dongusu, durumlar, olaylar ve cizim.
 */

#include "game.h"
#include "player.h"
#include "stats.h"
#include "render.h"
#include <stdio.h>
#include <math.h>


// --- EKRAN AYARLARI ---
#define SCREEN_W 1920
#define SCREEN_H 1080
#define UI_H 200
#define WORLD_H (SCREEN_H - UI_H)

// --- HUD ---
#define HUD_X 40
#define HUD_Y 20
#define HUD_LABEL_W 140
#define HUD_BAR_W 320
#define HUD_BAR_H 18
#define HUD_GAP 14

// --- UI PANELI ---
#define UI_PANEL_Y WORLD_H
#define UI_MARGIN 40
#define BTN_W 96
#define BTN_H 96
#define BTN_GAP 40
#define ACTION_BTN_Y (UI_PANEL_Y + 90)
#define STAT_Y (UI_PANEL_Y + 20)
#define LABEL_H 22
#define LABEL_GAP 12
#define UI_SCALE 4.0f
#define CAT_SCALE 4.0f

#define BOX3_FRAME_W 32
#define BOX3_FRAME_H 32
#define BOX3_FPS 10.0f
#define BOX3_SCALE 2.0f
#define BOX3_FORCE_FULL 0
#define DEBUG 0

#define SLEEP_FRAME_W 64
#define SLEEP_FRAME_H 64
#define SLEEP_SCALE 2.0f

// --- DUNYA KONUMLARI ---
#define BED_X 1360
#define BED_Y 380

#define BOWL_X 260
#define BOWL_Y 610

#define TOY_X 900
#define TOY_Y 520

#define OBJECT_SCALE 2

#define CENTER_BTN_X ((SCREEN_W - BTN_W) / 2)
#define START_BTN_Y 520
#define RESUME_BTN_Y 440
#define RESTART_BTN_Y 540

typedef struct {
    SDL_Rect rect;
    SDL_Texture* texture;
    const char* label;
} Button;

static void reset_game_state();
static int is_clicked(int mx, int my, SDL_Rect btn);
static void clamp_rect(SDL_Rect* rect);
static void clamp_rect_world(SDL_Rect* rect);
static void apply_texture_size(SDL_Texture* tex, int* w, int* h, int scale);
static void apply_button_texture(Button* b, SDL_Texture* tex);
static void render_button(SDL_Renderer* r, const Button* b, SDL_Texture* fallbackTex, SDL_Rect srcFallback, SDL_Color fallbackColor);
static void render_button_label(SDL_Renderer* r, const Button* b, SDL_Color color);
static void render_stat_bar(SDL_Renderer* r, const char* label, int value, int x, int y, SDL_Color fillColor, SDL_Color textColor);
static void render_texture_or_rect(SDL_Renderer* r, SDL_Texture* tex, SDL_Rect dst, SDL_Color fallbackColor);
static void layout_buttons();

// Oyun Degiskenleri
static Player player;
static Stats stats;

// Texture'lar
static SDL_Texture* tBed = NULL;
static SDL_Texture* tBowl = NULL;
static SDL_Texture* tToy = NULL;

static SDL_Texture* tCatIdle = NULL;
static SDL_Texture* tCatSleep = NULL;
static SDL_Texture* tBoxEffect = NULL;

static SDL_Texture* tBtnStart = NULL;
static SDL_Texture* tBtnPause = NULL;
static SDL_Texture* tBtnContinue = NULL;
static SDL_Texture* tBtnRestart = NULL;
static SDL_Texture* tBtnFeed = NULL;
static SDL_Texture* tBtnSleep = NULL;
static SDL_Texture* tBtnPlay = NULL;

static int bedDstW = 260;
static int bedDstH = 180;
static int bowlDstW = 120;
static int bowlDstH = 90;
static int toyDstW = 160;
static int toyDstH = 160;

static int boxFrameIndex = 0;
static float boxFrameTimer = 0.0f;
static int box3LoggedNull = 0;
static int boxFrameCount = 1;
static int boxFrameW = BOX3_FRAME_W;
static int boxFrameH = BOX3_FRAME_H;

static const char* cat_state_name(AnimationState state) {
    switch (state) {
        case ANIM_IDLE: return "IDLE";
        case ANIM_WALK: return "WALK";
        case ANIM_EAT: return "EAT";
        case ANIM_SLEEP: return "SLEEP";
        case ANIM_PLAY: return "PLAY";
        default: return "UNK";
    }
}

static const char* render_cat(SDL_Renderer* r, SDL_Rect bedRect) {
    if (player.animState == ANIM_SLEEP && tCatSleep) {
        SDL_Rect src = { 0, 0, SLEEP_FRAME_W, SLEEP_FRAME_H };
        SDL_Rect dst = { 0, 0, (int)(SLEEP_FRAME_W * SLEEP_SCALE), (int)(SLEEP_FRAME_H * SLEEP_SCALE) };
        dst.x = bedRect.x + (int)((bedRect.w - dst.w) * 0.5f);
        dst.y = bedRect.y + (int)((bedRect.h - dst.h) * 0.5f) - 10;
        clamp_rect_world(&dst);
        SDL_RenderCopy(r, tCatSleep, &src, &dst);
        return "SLEEP";
    }

    if (player.animState == ANIM_PLAY) {
        if (!tBoxEffect) {
            if (!box3LoggedNull) {
                printf("BOX3_TEXTURE_NULL: %s\n", IMG_GetError());
                box3LoggedNull = 1;
            }
            SDL_Rect idleDst = {
                (int)player.x,
                (int)player.y,
                player.w * CAT_SCALE,
                player.h * CAT_SCALE
            };
            clamp_rect_world(&idleDst);
            SDL_SetRenderDrawColor(r, 180, 60, 60, 255);
            SDL_RenderFillRect(r, &idleDst);
            return "IDLE";
        }

        SDL_Rect idleDst = {
            (int)player.x,
            (int)player.y,
            player.w * CAT_SCALE,
            player.h * CAT_SCALE
        };
        float boxScale = (float)idleDst.h / (float)boxFrameH;
        SDL_Rect dst = {
            (int)(player.x + (idleDst.w * 0.5f) - (boxFrameW * boxScale * 0.5f)),
            (int)(player.y + idleDst.h - (boxFrameH * boxScale)),
            (int)(boxFrameW * boxScale),
            (int)(boxFrameH * boxScale)
        };
        clamp_rect_world(&dst);
#if BOX3_FORCE_FULL
        SDL_RenderCopy(r, tBoxEffect, NULL, &dst);
#else
        if (boxFrameCount > 1) {
            SDL_Rect src = { boxFrameIndex * boxFrameW, 0, boxFrameW, boxFrameH };
            SDL_RenderCopy(r, tBoxEffect, &src, &dst);
        } else {
            SDL_RenderCopy(r, tBoxEffect, NULL, &dst);
        }
#endif
#if DEBUG
        SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
        SDL_RenderDrawRect(r, &dst);
#endif
        return "BOX3";
    }

    if (player.animState == ANIM_EAT) {
        player_render(&player, r);
        return "EAT";
    }

    player_render(&player, r);
    return "IDLE";
}

// Butonlar
static Button btnFeed  = { { UI_MARGIN, ACTION_BTN_Y, BTN_W, BTN_H }, NULL, "MAMA" };
static Button btnPlay  = { { UI_MARGIN + (BTN_W + BTN_GAP), ACTION_BTN_Y, BTN_W, BTN_H }, NULL, "OYNA" };
static Button btnSleep = { { UI_MARGIN + 2 * (BTN_W + BTN_GAP), ACTION_BTN_Y, BTN_W, BTN_H }, NULL, "UYU" };
static Button btnPause = { { SCREEN_W - UI_MARGIN - BTN_W, ACTION_BTN_Y, BTN_W, BTN_H }, NULL, NULL };

static Button btnStart   = { { CENTER_BTN_X, START_BTN_Y, BTN_W, BTN_H }, NULL, NULL };
static Button btnResume  = { { CENTER_BTN_X, RESUME_BTN_Y, BTN_W, BTN_H }, NULL, NULL };
static Button btnRestart = { { CENTER_BTN_X, RESTART_BTN_Y, BTN_W, BTN_H }, NULL, NULL };

static void reset_game_state() {
    stats_init(&stats);
    player_init(&player, tCatIdle, NULL, NULL, tCatSleep, NULL);
    player.x = (SCREEN_W / 2) - (player.w * 2);
    player.y = (WORLD_H / 2) - (player.h * 2);
    boxFrameIndex = 0;
    boxFrameTimer = 0.0f;
}

static int is_clicked(int mx, int my, SDL_Rect btn) {
    return (mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h);
}

static void clamp_rect(SDL_Rect* rect) {
    if (rect->w > SCREEN_W) rect->w = SCREEN_W;
    if (rect->h > SCREEN_H) rect->h = SCREEN_H;
    if (rect->x < 0) rect->x = 0;
    if (rect->y < 0) rect->y = 0;
    if (rect->x + rect->w > SCREEN_W) rect->x = SCREEN_W - rect->w;
    if (rect->y + rect->h > SCREEN_H) rect->y = SCREEN_H - rect->h;
}

static void clamp_rect_world(SDL_Rect* rect) {
    if (rect->w > SCREEN_W) rect->w = SCREEN_W;
    if (rect->h > WORLD_H) rect->h = WORLD_H;
    if (rect->x < 0) rect->x = 0;
    if (rect->y < 0) rect->y = 0;
    if (rect->x + rect->w > SCREEN_W) rect->x = SCREEN_W - rect->w;
    if (rect->y + rect->h > WORLD_H) rect->y = WORLD_H - rect->h;
}

static void apply_texture_size(SDL_Texture* tex, int* w, int* h, int scale) {
    int texW = 0;
    int texH = 0;
    if (!tex) return;
    if (SDL_QueryTexture(tex, NULL, NULL, &texW, &texH) == 0) {
        if (w) *w = texW * scale;
        if (h) *h = texH * scale;
    }
}

static void apply_button_texture(Button* b, SDL_Texture* tex) {
    int texW = 0;
    int texH = 0;
    b->texture = tex;
    if (!tex) return;
    if (SDL_QueryTexture(tex, NULL, NULL, &texW, &texH) == 0) {
        b->rect.w = (int)(texW * UI_SCALE);
        b->rect.h = (int)(texH * UI_SCALE);
    }
}

static void render_button(SDL_Renderer* r, const Button* b, SDL_Texture* fallbackTex, SDL_Rect srcFallback, SDL_Color fallbackColor) {
    SDL_Rect rect = b->rect;
    clamp_rect(&rect);

    if (b->texture) {
        SDL_RenderCopy(r, b->texture, NULL, &rect);
    } else if (fallbackTex) {
        render_sprite(r, fallbackTex, srcFallback, rect, fallbackColor);
    } else {
        SDL_SetRenderDrawColor(r, fallbackColor.r, fallbackColor.g, fallbackColor.b, 255);
        SDL_RenderFillRect(r, &rect);
    }
}

static void render_button_label(SDL_Renderer* r, const Button* b, SDL_Color color) {
    if (!b->label || b->label[0] == '\0') return;
    SDL_Rect labelBox = { b->rect.x, b->rect.y - (LABEL_H + LABEL_GAP), b->rect.w, LABEL_H };
    clamp_rect(&labelBox);
    render_text_in_box(r, b->label, labelBox, color);
}

static void render_stat_bar(SDL_Renderer* r, const char* label, int value, int x, int y, SDL_Color fillColor, SDL_Color textColor) {
    int clamped = value;
    if (clamped < 0) clamped = 0;
    if (clamped > 100) clamped = 100;

    SDL_Rect bg = { x + HUD_LABEL_W, y, HUD_BAR_W, HUD_BAR_H };
    SDL_Rect fill = { x + HUD_LABEL_W, y, (HUD_BAR_W * clamped) / 100, HUD_BAR_H };
    clamp_rect(&bg);
    clamp_rect(&fill);

    render_text(r, label, x, y - 2, textColor, 24);

    SDL_SetRenderDrawColor(r, 60, 60, 60, 255);
    SDL_RenderFillRect(r, &bg);

    SDL_SetRenderDrawColor(r, fillColor.r, fillColor.g, fillColor.b, 255);
    SDL_RenderFillRect(r, &fill);

    SDL_SetRenderDrawColor(r, 20, 20, 20, 255);
    SDL_RenderDrawRect(r, &bg);
}

static void render_texture_or_rect(SDL_Renderer* r, SDL_Texture* tex, SDL_Rect dst, SDL_Color fallbackColor) {
    clamp_rect(&dst);
    if (tex) {
        SDL_RenderCopy(r, tex, NULL, &dst);
        return;
    }
    SDL_SetRenderDrawColor(r, fallbackColor.r, fallbackColor.g, fallbackColor.b, 255);
    SDL_RenderFillRect(r, &dst);
}

static void layout_buttons() {
    int totalW = btnFeed.rect.w + btnPlay.rect.w + btnSleep.rect.w + (2 * BTN_GAP);
    int startX = (SCREEN_W - totalW) / 2;
    btnFeed.rect.x = startX;
    btnPlay.rect.x = startX + btnFeed.rect.w + BTN_GAP;
    btnSleep.rect.x = btnPlay.rect.x + btnPlay.rect.w + BTN_GAP;

    btnFeed.rect.y = ACTION_BTN_Y;
    btnPlay.rect.y = ACTION_BTN_Y;
    btnSleep.rect.y = ACTION_BTN_Y;

    btnPause.rect.x = SCREEN_W - UI_MARGIN - btnPause.rect.w;
    btnPause.rect.y = ACTION_BTN_Y;

    btnStart.rect.x = (SCREEN_W - btnStart.rect.w) / 2;
    btnStart.rect.y = START_BTN_Y;

    btnResume.rect.x = (SCREEN_W - btnResume.rect.w) / 2;
    btnResume.rect.y = RESUME_BTN_Y;

    btnRestart.rect.x = (SCREEN_W - btnRestart.rect.w) / 2;
    btnRestart.rect.y = RESTART_BTN_Y;
}

int game_init(Game* g, const char* title, int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    if (!render_init_fonts()) {
        printf("HATA: 'assets/font.ttf' dosyasi bulunamadi!\n");
        return 0;
    }

    g->width = SCREEN_W;
    g->height = SCREEN_H;
    g->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
    g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    tBed  = IMG_LoadTexture(g->renderer, "assets/bed.png");
    tBowl = IMG_LoadTexture(g->renderer, "assets/bowl.png");
    tToy  = IMG_LoadTexture(g->renderer, "assets/toy.png");

    tCatIdle  = IMG_LoadTexture(g->renderer, "assets/Idle.png");
    tCatSleep = IMG_LoadTexture(g->renderer, "assets/sleep.png");
    tBoxEffect = IMG_LoadTexture(g->renderer, "assets/Box3.png");
    printf("BOX3_PATH=assets/Box3.png\n");

    tBtnStart    = IMG_LoadTexture(g->renderer, "assets/start.png");
    tBtnPause    = IMG_LoadTexture(g->renderer, "assets/pausebtn.png");
    tBtnContinue = IMG_LoadTexture(g->renderer, "assets/continuebtn.png");
    tBtnRestart  = IMG_LoadTexture(g->renderer, "assets/restartbtn.png");
    tBtnFeed     = IMG_LoadTexture(g->renderer, "assets/feedbtn.png");
    tBtnSleep    = IMG_LoadTexture(g->renderer, "assets/sleepbtn.png");
    tBtnPlay     = IMG_LoadTexture(g->renderer, "assets/gamebtn.png");

    if (!tBed) printf("UYARI: assets/bed.png yuklenemedi!\n");
    if (!tBowl) printf("UYARI: assets/bowl.png yuklenemedi!\n");
    if (!tToy) printf("UYARI: assets/toy.png yuklenemedi!\n");
    if (!tCatIdle) printf("UYARI: assets/Idle.png yuklenemedi!\n");
    if (!tCatSleep) printf("UYARI: assets/sleep.png yuklenemedi!\n");
    if (!tBoxEffect) printf("UYARI: assets/Box3.png yuklenemedi! %s\n", IMG_GetError());
    if (!tBtnStart) printf("UYARI: assets/start.png yuklenemedi!\n");
    if (!tBtnPause) printf("UYARI: assets/pausebtn.png yuklenemedi!\n");
    if (!tBtnContinue) printf("UYARI: assets/continuebtn.png yuklenemedi!\n");
    if (!tBtnRestart) printf("UYARI: assets/restartbtn.png yuklenemedi!\n");
    if (!tBtnFeed) printf("UYARI: assets/feedbtn.png yuklenemedi!\n");
    if (!tBtnSleep) printf("UYARI: assets/sleepbtn.png yuklenemedi!\n");
    if (!tBtnPlay) printf("UYARI: assets/gamebtn.png yuklenemedi!\n");

    apply_texture_size(tBed, &bedDstW, &bedDstH, OBJECT_SCALE);
    apply_texture_size(tBowl, &bowlDstW, &bowlDstH, OBJECT_SCALE);
    apply_texture_size(tToy, &toyDstW, &toyDstH, OBJECT_SCALE);

    if (tBoxEffect) {
        int texW = 0;
        int texH = 0;
        if (SDL_QueryTexture(tBoxEffect, NULL, NULL, &texW, &texH) == 0) {
            printf("BOX3_TEX_W=%d BOX3_TEX_H=%d\n", texW, texH);
            boxFrameH = texH;
            if (BOX3_FRAME_W > 0 && (texW % BOX3_FRAME_W) == 0) {
                boxFrameW = BOX3_FRAME_W;
                boxFrameCount = texW / BOX3_FRAME_W;
            } else {
                printf("BOX3_FRAME_MISMATCH\n");
                boxFrameCount = 4;
                boxFrameW = (boxFrameCount > 0) ? (texW / boxFrameCount) : texW;
            }
        }
    }

    apply_button_texture(&btnStart, tBtnStart);
    apply_button_texture(&btnPause, tBtnPause);
    apply_button_texture(&btnResume, tBtnContinue);
    apply_button_texture(&btnRestart, tBtnRestart);
    apply_button_texture(&btnFeed, tBtnFeed);
    apply_button_texture(&btnSleep, tBtnSleep);
    apply_button_texture(&btnPlay, tBtnPlay);

    layout_buttons();

    reset_game_state();

    g->running = 1;
    g->state = STATE_START;
    g->lastTick = SDL_GetTicks();
    return 1;
}

void game_handle_events(Game* g) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) g->running = 0;
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            if (g->state == STATE_PLAY) g->state = STATE_PAUSE;
            else if (g->state == STATE_PAUSE) g->state = STATE_PLAY;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mx = e.button.x;
            int my = e.button.y;

            if (g->state == STATE_START) {
                if (is_clicked(mx, my, btnStart.rect)) {
                    reset_game_state();
                    g->state = STATE_PLAY;
                }
            } else if (g->state == STATE_PAUSE) {
                if (is_clicked(mx, my, btnResume.rect)) g->state = STATE_PLAY;
                if (is_clicked(mx, my, btnRestart.rect)) {
                    reset_game_state();
                    g->state = STATE_START;
                }
            } else if (g->state == STATE_GAME_OVER) {
                if (is_clicked(mx, my, btnRestart.rect)) {
                    reset_game_state();
                    g->state = STATE_START;
                }
            } else if (g->state == STATE_PLAY) {
                if (is_clicked(mx, my, btnPause.rect)) g->state = STATE_PAUSE;

                if (is_clicked(mx, my, btnFeed.rect)) {
                    int eatX = BOWL_X + (bowlDstW / 2);
                    int eatY = BOWL_Y + (bowlDstH / 2);
                    player_set_target(&player, eatX, eatY, ANIM_EAT);
                }

                if (is_clicked(mx, my, btnPlay.rect)) {
                    int playX = TOY_X + (toyDstW / 2);
                    int playY = TOY_Y + (toyDstH / 2);
                    player_set_target(&player, playX, playY, ANIM_PLAY);
                    boxFrameIndex = 0;
                    boxFrameTimer = 0.0f;
                }

                if (is_clicked(mx, my, btnSleep.rect)) {
                    int sleepX = BED_X + (bedDstW / 3);
                    int sleepY = BED_Y + (bedDstH / 3);
                    player_set_target(&player, sleepX, sleepY, ANIM_SLEEP);
                }
            }
        }
    }
}

void game_update(Game* g) {
    Uint32 now = SDL_GetTicks();
    g->dt = (now - g->lastTick) / 1000.0f;
    g->lastTick = now;

    if (g->state == STATE_PLAY) {
        AnimationState oldState = player.animState;

        player_update(&player, g->dt);
        stats_update_over_time(&stats, g->dt);
        if (player.animState == ANIM_PLAY) {
            float frameStep = 1.0f / BOX3_FPS;
            boxFrameTimer += g->dt;
            while (boxFrameTimer >= frameStep) {
                int frames = (boxFrameCount > 0) ? boxFrameCount : 1;
                boxFrameIndex = (boxFrameIndex + 1) % frames;
                boxFrameTimer -= frameStep;
            }
        } else {
            boxFrameIndex = 0;
            boxFrameTimer = 0.0f;
        }

        if (oldState == ANIM_EAT && player.animState == ANIM_IDLE) stats_feed(&stats);
        if (oldState == ANIM_SLEEP && player.animState == ANIM_IDLE) stats_rest(&stats);
        if (oldState == ANIM_PLAY && player.animState == ANIM_IDLE) stats_play(&stats);

        if (stats.hunger <= 0 && stats.energy <= 0 && stats.happiness <= 0) {
            g->state = STATE_GAME_OVER;
            player.animState = ANIM_IDLE;
            player.frameIndex = 0;
            player.frameTimer = 0.0f;
        }
    }
}

void game_render(Game* g) {
    SDL_SetRenderDrawColor(g->renderer, 238, 230, 210, 255);
    SDL_RenderClear(g->renderer);

    SDL_Rect worldRect = { 0, 0, SCREEN_W, WORLD_H };
    clamp_rect(&worldRect);
    SDL_SetRenderDrawColor(g->renderer, 232, 224, 206, 255);
    SDL_RenderFillRect(g->renderer, &worldRect);

    SDL_Color cBlack = {0, 0, 0, 255};
    SDL_Color cWhite = {255, 255, 255, 255};
    SDL_Color cAccent = {110, 160, 200, 255};

    if (g->state == STATE_START) {
        render_text_centered_x(g->renderer, "SUSHI SIMULATOR", SCREEN_W, 260, cBlack, 64);
        render_text_centered_x(g->renderer, "Sanal Kedi", SCREEN_W, 340, cBlack, 24);
        render_button(g->renderer, &btnStart, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);
    } else if (g->state == STATE_PLAY || g->state == STATE_PAUSE) {
        SDL_Rect dstBed = { BED_X, BED_Y, bedDstW, bedDstH };
        render_texture_or_rect(g->renderer, tBed, dstBed, cAccent);

        SDL_Rect dstBowl = { BOWL_X, BOWL_Y, bowlDstW, bowlDstH };
        if (player.animState == ANIM_EAT && player.isEating) {
            float t = (float)SDL_GetTicks();
            float scale = 1.0f + (0.06f * sinf(t * 0.05f));
            dstBowl.w = (int)(bowlDstW * scale);
            dstBowl.h = (int)(bowlDstH * scale);
            dstBowl.x = BOWL_X + (bowlDstW - dstBowl.w) / 2;
            dstBowl.y = BOWL_Y + (bowlDstH - dstBowl.h) / 2;
        }
        render_texture_or_rect(g->renderer, tBowl, dstBowl, cAccent);

        SDL_Rect dstToy = { TOY_X, TOY_Y, toyDstW, toyDstH };
        render_texture_or_rect(g->renderer, tToy, dstToy, cAccent);

        {
            SDL_Rect bedRect = { BED_X, BED_Y, bedDstW, bedDstH };
            render_cat(g->renderer, bedRect);
        }

        SDL_Rect uiPanel = { 0, UI_PANEL_Y, SCREEN_W, UI_H };
        clamp_rect(&uiPanel);
        SDL_SetRenderDrawColor(g->renderer, 210, 200, 180, 255);
        SDL_RenderFillRect(g->renderer, &uiPanel);

        SDL_Rect divider = { 0, UI_PANEL_Y - 2, SCREEN_W, 2 };
        clamp_rect(&divider);
        SDL_SetRenderDrawColor(g->renderer, 190, 180, 165, 255);
        SDL_RenderFillRect(g->renderer, &divider);

        render_stat_bar(g->renderer, "Aclik", stats.hunger, HUD_X, HUD_Y, (SDL_Color){200, 80, 80, 255}, cBlack);
        render_stat_bar(g->renderer, "Enerji", stats.energy, HUD_X, HUD_Y + HUD_BAR_H + HUD_GAP, (SDL_Color){80, 180, 120, 255}, cBlack);
        render_stat_bar(g->renderer, "Mutluluk", stats.happiness, HUD_X, HUD_Y + 2 * (HUD_BAR_H + HUD_GAP), (SDL_Color){80, 120, 200, 255}, cBlack);

        render_button(g->renderer, &btnFeed, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);
        render_button_label(g->renderer, &btnFeed, cBlack);

        render_button(g->renderer, &btnPlay, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);
        render_button_label(g->renderer, &btnPlay, cBlack);

        render_button(g->renderer, &btnSleep, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);
        render_button_label(g->renderer, &btnSleep, cBlack);

        render_button(g->renderer, &btnPause, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);

        char info[128];
        sprintf(info, "Aclik: %d  Enerji: %d  Mutluluk: %d", stats.hunger, stats.energy, stats.happiness);
        render_text(g->renderer, info, UI_MARGIN, STAT_Y, cBlack, 24);

        if (g->state == STATE_PAUSE) {
            SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 140);
            SDL_Rect scr = { 0, 0, SCREEN_W, SCREEN_H };
            clamp_rect(&scr);
            SDL_RenderFillRect(g->renderer, &scr);
            SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_NONE);

            render_text_centered_x(g->renderer, "DURAKLATILDI", SCREEN_W, 320, cWhite, 64);
            render_button(g->renderer, &btnResume, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);
            render_button(g->renderer, &btnRestart, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);
            {
                SDL_Rect labelResume = { btnResume.rect.x, btnResume.rect.y - (LABEL_H + LABEL_GAP), btnResume.rect.w, LABEL_H };
                SDL_Rect labelRestart = { btnRestart.rect.x, btnRestart.rect.y - (LABEL_H + LABEL_GAP), btnRestart.rect.w, LABEL_H };
                clamp_rect(&labelResume);
                clamp_rect(&labelRestart);
                render_text_in_box(g->renderer, "DEVAM ET", labelResume, cWhite);
                render_text_in_box(g->renderer, "YENIDEN BASLA", labelRestart, cWhite);
            }
        }
    } else if (g->state == STATE_GAME_OVER) {
        SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 180);
        SDL_Rect scr = { 0, 0, SCREEN_W, SCREEN_H };
        clamp_rect(&scr);
        SDL_RenderFillRect(g->renderer, &scr);
        SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_NONE);

        render_text_centered_x(g->renderer, "SUSHI'YE IYI BAKAMADIN :(", SCREEN_W, 320, cWhite, 64);
        render_button(g->renderer, &btnRestart, NULL, (SDL_Rect){0, 0, 0, 0}, cAccent);
    }

    SDL_RenderPresent(g->renderer);
}

void game_cleanup(Game* g) {
    SDL_DestroyTexture(tBed);
    SDL_DestroyTexture(tBowl);
    SDL_DestroyTexture(tToy);

    SDL_DestroyTexture(tCatIdle);
    SDL_DestroyTexture(tCatSleep);
    SDL_DestroyTexture(tBoxEffect);

    SDL_DestroyTexture(tBtnStart);
    SDL_DestroyTexture(tBtnPause);
    SDL_DestroyTexture(tBtnContinue);
    SDL_DestroyTexture(tBtnRestart);
    SDL_DestroyTexture(tBtnFeed);
    SDL_DestroyTexture(tBtnSleep);
    SDL_DestroyTexture(tBtnPlay);

    SDL_DestroyRenderer(g->renderer);
    SDL_DestroyWindow(g->window);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
