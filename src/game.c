#include "game.h"
#include <stdio.h>
#include "player.h"
#include "stats.h"
#include "render.h"

typedef enum { STATE_START, STATE_PLAY, STATE_GAMEOVER } GameState;

static GameState state = STATE_START;

static Player player;
static Stats stats;
static SDL_Texture* sushiSheet = NULL;

// Butonlar (GLOBAL)
static SDL_Rect btnStart   = { 380, 260, 200, 60 };
static SDL_Rect btnFeed    = { 20,  460, 180, 50 };
static SDL_Rect btnPlay    = { 210, 460, 180, 50 };
static SDL_Rect btnRest    = { 400, 460, 180, 50 };
static SDL_Rect btnRestart = { 380, 320, 200, 60 };

static int point_in_rect(int x, int y, SDL_Rect r) {
    return (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h);
}

int game_init(Game* g, const char* title, int w, int h) {

    if (TTF_Init() == -1) {
    printf("TTF_Init error: %s\n", TTF_GetError());
    return 0;
}

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 0;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("IMG_Init error: %s\n", IMG_GetError());
        return 0;
    }

    g->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    if (!g->window) {
        printf("Window error: %s\n", SDL_GetError());
        return 0;
    }

    g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g->renderer) {
        printf("Renderer error: %s\n", SDL_GetError());
        return 0;
    }

    sushiSheet = IMG_LoadTexture(g->renderer, "assets/sushi_sheet.png");
    if (!sushiSheet) {
        printf("Texture load error: %s\n", IMG_GetError());
        return 0;
    }

    player_init(&player, sushiSheet, 32, 32); // sprite frame = 32x32
    stats_init(&stats);

    state = STATE_START;

    g->running = 1;
    g->lastTick = SDL_GetTicks();
    g->dt = 0;

    return 1;
}

void game_handle_events(Game* g) {
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            g->running = 0;
        }

        // Mouse click
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx = e.button.x;
            int my = e.button.y;

            if (state == STATE_START) {
                if (point_in_rect(mx, my, btnStart)) {
                    stats_init(&stats);
                    player.x = 200;
                    player.y = 280;
                    player.frameIndex = 0;
                    player.frameTimer = 0;
                    state = STATE_PLAY;
                }
            }
            else if (state == STATE_PLAY) {
                if (point_in_rect(mx, my, btnFeed)) stats_feed(&stats);
                else if (point_in_rect(mx, my, btnPlay)) stats_play(&stats);
                else if (point_in_rect(mx, my, btnRest)) stats_rest(&stats);
            }
            else if (state == STATE_GAMEOVER) {
                if (point_in_rect(mx, my, btnRestart)) {
                    stats_init(&stats);
                    player.x = 200;
                    player.y = 280;
                    player.frameIndex = 0;
                    player.frameTimer = 0;
                    state = STATE_PLAY;
                }
            }
        }
    }

    // Klavye (event loop dışında)
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_ESCAPE]) g->running = 0;

    // Enter ile restart (GAMEOVER)
    if (state == STATE_GAMEOVER) {
        if (keys[SDL_SCANCODE_RETURN]) {
            stats_init(&stats);
            player.x = 200;
            player.y = 280;
            player.frameIndex = 0;
            player.frameTimer = 0;
            state = STATE_PLAY;
        }
    }

    // Klavye aksiyonları sadece PLAY ekranında
    if (state == STATE_PLAY) {
        if (keys[SDL_SCANCODE_F]) stats_feed(&stats);
        if (keys[SDL_SCANCODE_P]) stats_play(&stats);
        if (keys[SDL_SCANCODE_R]) stats_rest(&stats);
    }
}

void game_update(Game* g) {
    Uint32 now = SDL_GetTicks();
    Uint32 diff = now - g->lastTick;
    g->lastTick = now;
    g->dt = (float)diff / 1000.0f;

    if (state == STATE_PLAY) {
        // EKLENMESİ GEREKEN KISIM BURASI:
        // Klavyeyi oku ve player_handle_input'a gönder
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        player_handle_input(&player, keys, g->dt);
        // -----------------------------

        stats_update_over_time(&stats, g->dt);
        player_update(&player, g->dt);

        // Kaybetme koşulu
        if (stats.hunger == 0 && stats.happiness == 0 && stats.energy == 0) {
            state = STATE_GAMEOVER;
        }
    }
}

void game_render(Game* g) {
    render_clear(g->renderer);

    if (state == STATE_START) {
        SDL_SetRenderDrawColor(g->renderer, 20, 20, 30, 255);
        SDL_RenderClear(g->renderer);

        // Süs butonları
        render_button(g->renderer, btnFeed);
        render_text(g->renderer, "BESLE",  btnFeed.x + 55, btnFeed.y + 15);
        
        render_button(g->renderer, btnPlay);
        render_text(g->renderer, "OYNA",   btnPlay.x + 60, btnPlay.y + 15);
        
        render_button(g->renderer, btnRest);
        render_text(g->renderer, "DINLEN", btnRest.x + 45, btnRest.y + 15);

        // Start butonu ve yazısı
        render_button(g->renderer, btnStart);
        render_text(g->renderer, "START", btnStart.x + 60, btnStart.y + 18);
    }
    else if (state == STATE_PLAY) {
        SDL_Rect ground = { 0, 0, 960, 540 };
        SDL_SetRenderDrawColor(g->renderer, 25, 25, 35, 255);
        SDL_RenderFillRect(g->renderer, &ground);

        player_render(&player, g->renderer);
        render_hud(g->renderer, &stats);

        // Oyun butonları
        render_button(g->renderer, btnFeed);
        render_text(g->renderer, "BESLE",  btnFeed.x + 55, btnFeed.y + 15);
        
        render_button(g->renderer, btnPlay);
        render_text(g->renderer, "OYNA",   btnPlay.x + 60, btnPlay.y + 15);
        
        render_button(g->renderer, btnRest);
        render_text(g->renderer, "DINLEN", btnRest.x + 45, btnRest.y + 15);
    }
    else if (state == STATE_GAMEOVER) {
        SDL_SetRenderDrawColor(g->renderer, 10, 10, 15, 255);
        SDL_RenderClear(g->renderer);

        render_text(g->renderer, "GAME OVER", 380, 240);

        // Restart butonu ve yazısı
        render_button(g->renderer, btnRestart);
        render_text(g->renderer, "RESTART", btnRestart.x + 45, btnRestart.y + 18);
    }

    render_present(g->renderer); // BU EN SONDA OLMALI
}


void game_cleanup(Game* g) {
    if (sushiSheet) SDL_DestroyTexture(sushiSheet);
    if (g->renderer) SDL_DestroyRenderer(g->renderer);
    if (g->window) SDL_DestroyWindow(g->window);

    IMG_Quit();
    SDL_Quit();
    TTF_Quit();

}
