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

// Butonlar (X koordinatları artık önemsiz çünkü ortada görünsün istiyoruz ama tıklama için gerekli)
// Ekran genişliği 960. Buton genişliği 200. X = (960-200)/2 = 380
static SDL_Rect btnStart   = { 380, 280, 200, 50 };
static SDL_Rect btnRestart = { 380, 320, 200, 50 };

// Oyun içi butonlar
static SDL_Rect btnFeed    = { 160, 470, 200, 50 };
static SDL_Rect btnPlay    = { 380, 470, 200, 50 };
static SDL_Rect btnRest    = { 600, 470, 200, 50 };

static int point_in_rect(int x, int y, SDL_Rect r) {
    return (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h);
}

// ... game_init, game_handle_events, game_update AYNI KALSIN (Değişiklik yok) ...
// (Sadece game_render'ı değiştiriyoruz, diğer fonksiyonları aynen koru veya önceki koddan al)

int game_init(Game* g, const char* title, int w, int h) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return 0;
    if (TTF_Init() == -1) return 0;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 0;

    g->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    sushiSheet = IMG_LoadTexture(g->renderer, "assets/sushi_sheet.png");
    
    player_init(&player, sushiSheet, 32, 32);
    stats_init(&stats);
    state = STATE_START;
    g->running = 1;
    g->lastTick = SDL_GetTicks();
    return 1;
}

void game_handle_events(Game* g) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) g->running = 0;
        
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx = e.button.x; int my = e.button.y;
            if (state == STATE_START) {
                if (point_in_rect(mx, my, btnStart)) state = STATE_PLAY;
            } else if (state == STATE_PLAY) {
                if (point_in_rect(mx, my, btnFeed)) stats_feed(&stats);
                else if (point_in_rect(mx, my, btnPlay)) stats_play(&stats);
                else if (point_in_rect(mx, my, btnRest)) stats_rest(&stats);
            } else if (state == STATE_GAMEOVER) {
                if (point_in_rect(mx, my, btnRestart)) {
                    stats_init(&stats);
                    state = STATE_PLAY;
                }
            }
        }
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_ESCAPE]) g->running = 0;
    if (state == STATE_PLAY) {
        // ... (Karakter hareketi burada çağrılmalı, önceki cevaptaki gibi) ...
        player_handle_input(&player, keys, g->dt);
        
        if (keys[SDL_SCANCODE_F]) stats_feed(&stats);
        if (keys[SDL_SCANCODE_P]) stats_play(&stats);
        if (keys[SDL_SCANCODE_R]) stats_rest(&stats);
    }
}

void game_update(Game* g) {
    Uint32 now = SDL_GetTicks();
    g->dt = (now - g->lastTick) / 1000.0f;
    g->lastTick = now;

    if (state == STATE_PLAY) {
        stats_update_over_time(&stats, g->dt);
        player_update(&player, g->dt);
        if (stats.hunger == 0 && stats.happiness == 0 && stats.energy == 0) state = STATE_GAMEOVER;
    }
}

// *** BURASI DÜZENLENDİ ***
void game_render(Game* g) {
    // Sık kullanılan renkler
    SDL_Color darkText = { 40, 40, 60, 255 }; // Koyu Lacivert/Siyah
    SDL_Color whiteText = { 255, 255, 255, 255 };

    if (state == STATE_START) {
        // Arka plan: Tatlı Pembe
        SDL_SetRenderDrawColor(g->renderer, 255, 228, 225, 255); 
        SDL_RenderClear(g->renderer);

        // OYUN İSMİ - BÜYÜK VE ORTALI
        SDL_Color titleColor = { 139, 0, 0, 255 }; // Koyu Kırmızı
        render_text_centered(g->renderer, "SUSHI SIMULATOR", 120, 64, titleColor);
        
        // Alt başlık
        render_text_centered(g->renderer, "- Sanal Kedi -", 190, 20, darkText);

        // Start butonu
        render_button(g->renderer, btnStart);
        render_text(g->renderer, "BASLA", btnStart.x + 70, btnStart.y + 15, whiteText);
    }
    else if (state == STATE_PLAY) {
        // Oyun içi arka plan: Bej
        SDL_SetRenderDrawColor(g->renderer, 250, 240, 230, 255);
        SDL_RenderClear(g->renderer);

        player_render(&player, g->renderer);
        render_hud(g->renderer, &stats);

        // Buton yazıları (Beyaz)
        render_button(g->renderer, btnFeed);
        render_text(g->renderer, "MAMA",  btnFeed.x + 70, btnFeed.y + 15, whiteText);
        
        render_button(g->renderer, btnPlay);
        render_text(g->renderer, "OYNA",   btnPlay.x + 75, btnPlay.y + 15, whiteText);
        
        render_button(g->renderer, btnRest);
        render_text(g->renderer, "UYU", btnRest.x + 80, btnRest.y + 15, whiteText);
    }
    else if (state == STATE_GAMEOVER) {
        // Game over arka plan: Koyu kırmızı
        SDL_SetRenderDrawColor(g->renderer, 60, 0, 0, 255);
        SDL_RenderClear(g->renderer);

        SDL_Color overColor = { 255, 100, 100, 255 };
        render_text_centered(g->renderer, "SUSHI'YE IYI BAKAMADIN :(", 200, 64, overColor);

        // Restart
        render_button(g->renderer, btnRestart);
        render_text(g->renderer, "TEKRAR", btnRestart.x + 65, btnRestart.y + 15, whiteText);
    }

    render_present(g->renderer);
}

void game_cleanup(Game* g) {
    if (sushiSheet) SDL_DestroyTexture(sushiSheet);
    if (g->renderer) SDL_DestroyRenderer(g->renderer);
    if (g->window) SDL_DestroyWindow(g->window);
    IMG_Quit(); SDL_Quit(); TTF_Quit();
}