/**
 * Dosya: src/game.c
 * Açıklama: Oyunun ana döngüsü, olayların işlenmesi ve EKRANA ÇİZİM.
 * DUZENLEME: 1920x1080 Çözünürlük, Yeni Sprite Koordinatları ve Hizalamalar.
 */

#include "game.h"
#include "player.h"
#include "stats.h"
#include "render.h"
#include <stdio.h>

// --- EKRAN AYARLARI (Full HD) ---
#define W_WIDTH  1920
#define W_HEIGHT 1080

// =============================================================
// >>> GÖRSEL AYARLAR (SPRITE SLICING) <<<
// =============================================================

// 1. YATAK (Furnitures.png dosyasından kesilecek alan)
#define BED_SRC_X 180
#define BED_SRC_Y 220
#define BED_SRC_W 135
#define BED_SRC_H 90

// 2. MAMA KABI (Furnitures.png)
#define BOWL_SRC_X 250
#define BOWL_SRC_Y 324
#define BOWL_SRC_W 60
#define BOWL_SRC_H 50

// 3. OYUNCAK (Furnitures.png) - YENİ EKLENDİ
#define TOY_SRC_X 370
#define TOY_SRC_Y 10
#define TOY_SRC_W 80
#define TOY_SRC_H 120

// 4. UI BUTONLARI (free.png)
#define BTN_SRC_X 192
#define BTN_SRC_Y 60
#define BTN_SRC_W 16
#define BTN_SRC_H 16

// =============================================================
// >>> YENİ KONUMLAR VE BOYUTLAR <<<
// =============================================================

// Mobilya Ölçeği (2x)
#define FURNITURE_SCALE 2
// UI Ölçeği (3x)
#define UI_SCALE 3

// YATAĞIN YENİ KONUMU
#define POS_BED_X    1500
#define POS_BED_Y    700

// MAMA KABININ YENİ KONUMU
#define POS_BOWL_X   300
#define POS_BOWL_Y   800

// OYUNCAĞIN YENİ KONUMU
#define POS_TOY_X    800
#define POS_TOY_Y    750

// Oyun Değişkenleri
static Player player;
static Stats stats;

// Resim Dosyaları (Texture)
static SDL_Texture* tFurn = NULL; // Mobilyalar (Yatak, Mama Kabı, Oyuncak burada)
static SDL_Texture* tIdle = NULL; // Kedi
static SDL_Texture* tUI   = NULL; // Butonlar

// --- BUTON AYARLARI ---
// Genişlik: 16 * 3 = 48px (Eskiden çok genişti, şimdi ikon gibi kare olacak)
// Ancak yazının sığması için kodda genişliği biraz artırabiliriz veya sadece ikonu çizeriz.
// Senin verdiğin kodda buton genişliği 144px (16*9) olarak ayarlanmış, bu mantıklı.

// [ MAMA ] [ OYNA ] [ UYU ]
SDL_Rect btnFeed  = { 500, 900, 48, 48 }; // Y eksenini 1080p'ye göre aşağı çektim (800)
SDL_Rect btnPlay  = { 1000, 900, 48, 48 }; 
SDL_Rect btnSleep = { 1500, 900, 48, 48 }; 

// Menü Butonları (Ortalanmış)
// 1920 / 2 = 960 (Merkez) -> 960 - (144/2) = 888
SDL_Rect btnStart  = { 888, 500, 48, 48 };
SDL_Rect btnResume = { 888, 450, 48, 48 };
SDL_Rect btnExit   = { 888, 550, 48, 48 };

int game_init(Game* g, const char* title, int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    
    if (!render_init_fonts()) {
        printf("HATA: 'assets/font.ttf' dosyasi bulunamadi!\n");
        return 0;
    }

    g->width = W_WIDTH;
    g->height = W_HEIGHT;
    g->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, 0);
    g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // --- RESİMLERİ YÜKLEME ---
    // NOT: Oyuncak da artık 'Furnitures.png' içinde olduğu için tToy diye ayrı yüklemeye gerek yok.
    tFurn = IMG_LoadTexture(g->renderer, "assets/Furnitures.png");
    tIdle = IMG_LoadTexture(g->renderer, "assets/Idle.png");
    tUI   = IMG_LoadTexture(g->renderer, "assets/free.png"); 

    if(!tFurn || !tIdle || !tUI) {
        printf("UYARI: Resimler yuklenemedi!\n");
    }

    // Player'a oyuncak dokusu olarak tFurn (Mobilyalar) gönderiyoruz
    player_init(&player, tIdle, tFurn);
    stats_init(&stats);
    
    g->running = 1;
    g->state = STATE_MENU; 
    g->lastTick = SDL_GetTicks();
    return 1;
}

int is_clicked(int mx, int my, SDL_Rect btn) {
    return (mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h);
}

void game_handle_events(Game* g) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) g->running = 0;
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            if (g->state == STATE_PLAY) g->state = STATE_PAUSE;
            else if (g->state == STATE_PAUSE) g->state = STATE_PLAY;
        }

        if(e.type == SDL_MOUSEBUTTONDOWN) {
            int mx = e.button.x, my = e.button.y;

            if (g->state == STATE_MENU) {
                if (is_clicked(mx, my, btnStart)) g->state = STATE_PLAY;
            }
            else if (g->state == STATE_PAUSE) {
                if (is_clicked(mx, my, btnResume)) g->state = STATE_PLAY;
                if (is_clicked(mx, my, btnExit)) g->running = 0;
            }
            else if (g->state == STATE_PLAY) {
                if (is_clicked(mx, my, btnFeed))  
                    player_set_target(&player, POS_BOWL_X + 20, POS_BOWL_Y - 30, STATE_EATING);
                
                if (is_clicked(mx, my, btnPlay))  
                    player_set_target(&player, POS_TOY_X, POS_TOY_Y, STATE_PLAYING);
                
                if (is_clicked(mx, my, btnSleep)) 
                    player_set_target(&player, POS_BED_X + 50, POS_BED_Y + 40, STATE_SLEEPING);
            }
        }
    }
}

void game_update(Game* g) {
    Uint32 now = SDL_GetTicks();
    g->dt = (now - g->lastTick) / 1000.0f;
    g->lastTick = now;

    if (g->state == STATE_PLAY) {
        PlayerState oldS = player.state;
        
        player_update(&player, g->dt);
        stats_update_over_time(&stats, g->dt);
        
        if(oldS == STATE_EATING && player.state == STATE_IDLE) stats_feed(&stats);
        if(oldS == STATE_SLEEPING && player.state == STATE_IDLE) stats_rest(&stats);
        if(oldS == STATE_PLAYING && player.state == STATE_IDLE) stats_play(&stats);
    }
}

void game_render(Game* g) {
    SDL_SetRenderDrawColor(g->renderer, 240, 230, 210, 255);
    SDL_RenderClear(g->renderer);

    SDL_Color cBlack = {0,0,0,255};
    SDL_Color cWhite = {255,255,255,255};
    SDL_Color cDebugRed = {255,100,100,255};
    SDL_Color cDebugGreen = {100,255,100,255};
    SDL_Color cDebugBlue = {100,100,255,255};

    SDL_Rect srcUI = { BTN_SRC_X, BTN_SRC_Y, BTN_SRC_W, BTN_SRC_H };

    if (g->state == STATE_MENU) {
        render_text_centered_x(g->renderer, "SUSHI SIMULATOR", W_WIDTH, 300, cBlack, 64);
        render_text_centered_x(g->renderer, "- Sanal Kedi -", W_WIDTH, 380, cBlack, 24);
        render_sprite(g->renderer, tUI, srcUI, btnStart, cDebugBlue);
        render_text_in_box(g->renderer, "BASLA", btnStart, cWhite);
    }
    else if (g->state == STATE_PLAY || g->state == STATE_PAUSE) {
        // 1. Yatak
        SDL_Rect srcBed = { BED_SRC_X, BED_SRC_Y, BED_SRC_W, BED_SRC_H };
        SDL_Rect dstBed = { POS_BED_X, POS_BED_Y, BED_SRC_W * FURNITURE_SCALE, BED_SRC_H * FURNITURE_SCALE };
        render_sprite(g->renderer, tFurn, srcBed, dstBed, cDebugBlue);

        // 2. Mama Kabı
        SDL_Rect srcBowl = { BOWL_SRC_X, BOWL_SRC_Y, BOWL_SRC_W, BOWL_SRC_H };
        SDL_Rect dstBowl = { POS_BOWL_X, POS_BOWL_Y, BOWL_SRC_W * FURNITURE_SCALE, BOWL_SRC_H * FURNITURE_SCALE };
        render_sprite(g->renderer, tFurn, srcBowl, dstBowl, cDebugGreen);

        // 3. Oyuncak (Artık tFurn'dan çekiliyor)
        SDL_Rect srcToy = { TOY_SRC_X, TOY_SRC_Y, TOY_SRC_W, TOY_SRC_H };
        SDL_Rect dstToy = { POS_TOY_X, POS_TOY_Y, TOY_SRC_W * FURNITURE_SCALE, TOY_SRC_H * FURNITURE_SCALE };
        render_sprite(g->renderer, tFurn, srcToy, dstToy, cDebugRed);

        // 4. Kedi
        player_render(&player, g->renderer);

        // 5. Butonlar
        render_sprite(g->renderer, tUI, srcUI, btnFeed, cDebugRed);
        render_text_in_box(g->renderer, "MAMA", btnFeed, cWhite);

        render_sprite(g->renderer, tUI, srcUI, btnPlay, cDebugRed);
        render_text_in_box(g->renderer, "OYNA", btnPlay, cWhite);

        render_sprite(g->renderer, tUI, srcUI, btnSleep, cDebugRed);
        render_text_in_box(g->renderer, "UYU", btnSleep, cWhite);

        // 6. İstatistikler
        char info[128];
        sprintf(info, "Aclik: %d  Enerji: %d  Mutluluk: %d", stats.hunger, stats.energy, stats.happiness);
        render_text(g->renderer, info, 20, 20, cBlack, 24);

        if (g->state == STATE_PAUSE) {
            SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 150);
            SDL_Rect scr = {0,0,W_WIDTH, W_HEIGHT};
            SDL_RenderFillRect(g->renderer, &scr);
            SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_NONE);

            render_text_centered_x(g->renderer, "DURAKLATILDI", W_WIDTH, 400, cWhite, 64);
            render_sprite(g->renderer, tUI, srcUI, btnResume, cDebugBlue);
            render_text_in_box(g->renderer, "DEVAM", btnResume, cWhite);
            render_sprite(g->renderer, tUI, srcUI, btnExit, cDebugBlue);
            render_text_in_box(g->renderer, "CIKIS", btnExit, cWhite);
        }
    }
    SDL_RenderPresent(g->renderer);
}

void game_cleanup(Game* g) {
    SDL_DestroyTexture(tFurn); SDL_DestroyTexture(tIdle);
    SDL_DestroyTexture(tUI);
    SDL_DestroyRenderer(g->renderer); SDL_DestroyWindow(g->window);
    IMG_Quit(); TTF_Quit(); SDL_Quit();
}