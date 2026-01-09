/**
 * Dosya: src/game.c
 * Açıklama: Oyunun ana döngüsü, olayların işlenmesi ve EKRANA ÇİZİM.
 * NOT: Resimlerin neresinin kesileceği (Sprite Slicing) buradaki 'DEFINE' ayarlarındadır.
 */

#include "game.h"
#include "player.h"
#include "stats.h"
#include "render.h"
#include <stdio.h>

// --- EKRAN AYARLARI ---
#define W_WIDTH  1280
#define W_HEIGHT 720

// =============================================================
// >>> GÖRSEL AYARLAR (SPRITE SLICING) <<<
// =============================================================
// Buradaki değerleri değiştirerek resimlerin doğru yerini seçebilirsin.

// 1. YATAK (Furnitures.png dosyasından kesilecek alan)
// Sol üst köşeden başlar. x:0, y:16, genişlik:48, yükseklik:32
#define BED_SRC_X 190
#define BED_SRC_Y 230
#define BED_SRC_W 132
#define BED_SRC_H 90

// 2. MAMA KABI (Furnitures.png dosyasından)
#define BOWL_SRC_X 250
#define BOWL_SRC_Y 390
#define BOWL_SRC_W 60
#define BOWL_SRC_H 45

// 3. UI BUTONLARI (free.png dosyasından)
#define BTN_SRC_X 0
#define BTN_SRC_Y 0
#define BTN_SRC_W 48
#define BTN_SRC_H 16

// 4. EKRANDAKİ KONUMLAR (Eşyalar nerede dursun?)
#define POS_BED_X    900
#define POS_BED_Y    300
#define POS_BOWL_X   150
#define POS_BOWL_Y   450
#define POS_TOY_X    550
#define POS_TOY_Y    500
// =============================================================

// Oyun Değişkenleri
static Player player;
static Stats stats;
// Resim Dosyaları (Texture)
static SDL_Texture* tFurn = NULL; // Mobilyalar
static SDL_Texture* tIdle = NULL; // Kedi
static SDL_Texture* tBox  = NULL; // Oyuncak Kutu
static SDL_Texture* tUI   = NULL; // Butonlar

// Butonların Ekrandaki Alanları {x, y, w, h}
SDL_Rect btnFeed  = { 150, 620, 192, 64 };
SDL_Rect btnPlay  = { 540, 620, 192, 64 };
SDL_Rect btnSleep = { 930, 620, 192, 64 };

// Menü Butonları
SDL_Rect btnStart  = { 540, 350, 192, 64 };
SDL_Rect btnResume = { 540, 300, 192, 64 };
SDL_Rect btnExit   = { 540, 400, 192, 64 };

int game_init(Game* g, const char* title, int w, int h) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    
    // Yazı tiplerini yükle
    if (!render_init_fonts()) {
        printf("HATA: 'assets/font.ttf' dosyasi bulunamadi!\n");
        return 0;
    }

    // Pencereyi oluştur
    g->width = W_WIDTH;
    g->height = W_HEIGHT;
    g->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, 0);
    g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // --- RESİMLERİ YÜKLEME KISMI ---
    tFurn = IMG_LoadTexture(g->renderer, "assets/Furnitures.png");
    tIdle = IMG_LoadTexture(g->renderer, "assets/Idle.png");
    tBox  = IMG_LoadTexture(g->renderer, "assets/Box3.png"); // Kutu resmi
    tUI   = IMG_LoadTexture(g->renderer, "assets/free.png"); // UI resmi

    if(!tFurn || !tIdle || !tBox || !tUI) {
        printf("UYARI: Bazi resimler yuklenemedi. Renkli kutular kullanilacak.\n");
    }

    player_init(&player, tIdle, tBox);
    stats_init(&stats);
    
    g->running = 1;
    g->state = STATE_MENU; // Oyun MENÜ ile başlar
    g->lastTick = SDL_GetTicks();
    return 1;
}

// Tıklama kontrolü
int is_clicked(int mx, int my, SDL_Rect btn) {
    return (mx >= btn.x && mx <= btn.x + btn.w && my >= btn.y && my <= btn.y + btn.h);
}

void game_handle_events(Game* g) {
    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) g->running = 0;

        // Klavye Kontrolü (ESC)
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            if (g->state == STATE_PLAY) g->state = STATE_PAUSE;
            else if (g->state == STATE_PAUSE) g->state = STATE_PLAY;
        }

        // Fare Kontrolü
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
                // Kediye Emir Ver
                if (is_clicked(mx, my, btnFeed))  
                    player_set_target(&player, POS_BOWL_X, POS_BOWL_Y - 40, STATE_EATING);
                
                if (is_clicked(mx, my, btnPlay))  
                    player_set_target(&player, POS_TOY_X, POS_TOY_Y, STATE_PLAYING);
                
                if (is_clicked(mx, my, btnSleep)) 
                    player_set_target(&player, POS_BED_X + 20, POS_BED_Y + 20, STATE_SLEEPING);
            }
        }
    }
}

void game_update(Game* g) {
    // Delta Time (Geçen Süre) Hesabı
    Uint32 now = SDL_GetTicks();
    g->dt = (now - g->lastTick) / 1000.0f;
    g->lastTick = now;

    if (g->state == STATE_PLAY) {
        PlayerState oldS = player.state;
        
        player_update(&player, g->dt);
        stats_update_over_time(&stats, g->dt);
        
        // Eylem bitince puan ver
        if(oldS == STATE_EATING && player.state == STATE_IDLE) stats_feed(&stats);
        if(oldS == STATE_SLEEPING && player.state == STATE_IDLE) stats_rest(&stats);
        if(oldS == STATE_PLAYING && player.state == STATE_IDLE) stats_play(&stats);
    }
}

void game_render(Game* g) {
    // 1. Arka Plan (Sıcak Bej Rengi)
    SDL_SetRenderDrawColor(g->renderer, 240, 230, 210, 255);
    SDL_RenderClear(g->renderer);

    SDL_Color cBlack = {0,0,0,255};
    SDL_Color cWhite = {255,255,255,255};
    
    // Hata ayıklama renkleri (Resim yoksa bu renkler görünür)
    SDL_Color cDebugRed = {255,100,100,255};
    SDL_Color cDebugGreen = {100,255,100,255};
    SDL_Color cDebugBlue = {100,100,255,255};

    // UI Kesim Alanı (Buton Kalıbı)
    SDL_Rect srcUI = { BTN_SRC_X, BTN_SRC_Y, BTN_SRC_W, BTN_SRC_H };

    // --- MENÜ EKRANI ---
    if (g->state == STATE_MENU) {
        render_text_centered_x(g->renderer, "SUSHI SIMULATOR", W_WIDTH, 150, cBlack, 64);
        render_text_centered_x(g->renderer, "- Sanal Kedi -", W_WIDTH, 230, cBlack, 24);
        
        render_sprite(g->renderer, tUI, srcUI, btnStart, cDebugBlue);
        render_text_in_box(g->renderer, "BASLA", btnStart, cWhite);
    }
    
    // --- OYUN EKRANI ---
    else if (g->state == STATE_PLAY || g->state == STATE_PAUSE) {
        
        // 1. Yatak Çizimi (Furnitures.png içinden keserek)
        SDL_Rect srcBed = { BED_SRC_X, BED_SRC_Y, BED_SRC_W, BED_SRC_H };
        SDL_Rect dstBed = { POS_BED_X, POS_BED_Y, BED_SRC_W * 4, BED_SRC_H * 4 };
        render_sprite(g->renderer, tFurn, srcBed, dstBed, cDebugBlue);

        // 2. Mama Kabı Çizimi
        SDL_Rect srcBowl = { BOWL_SRC_X, BOWL_SRC_Y, BOWL_SRC_W, BOWL_SRC_H };
        SDL_Rect dstBowl = { POS_BOWL_X, POS_BOWL_Y, BOWL_SRC_W * 4, BOWL_SRC_H * 4 };
        render_sprite(g->renderer, tFurn, srcBowl, dstBowl, cDebugGreen);

        // 3. Oyuncak Kutu (Yerde duruyor)
        SDL_Rect srcToy = { 0, 0, 32, 32 };
        SDL_Rect dstToy = { POS_TOY_X, POS_TOY_Y, 64, 64 };
        render_sprite(g->renderer, tBox, srcToy, dstToy, cDebugRed);

        // 4. Kedi Çizimi
        player_render(&player, g->renderer);

        // 5. Alt Butonlar
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

        // --- PAUSE EKRANI (Yarı Saydam) ---
        if (g->state == STATE_PAUSE) {
            SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(g->renderer, 0, 0, 0, 150);
            SDL_Rect scr = {0,0,W_WIDTH, W_HEIGHT};
            SDL_RenderFillRect(g->renderer, &scr);
            SDL_SetRenderDrawBlendMode(g->renderer, SDL_BLENDMODE_NONE);

            render_text_centered_x(g->renderer, "DURAKLATILDI", W_WIDTH, 200, cWhite, 64);
            
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
    SDL_DestroyTexture(tBox); SDL_DestroyTexture(tUI);
    SDL_DestroyRenderer(g->renderer); SDL_DestroyWindow(g->window);
    IMG_Quit(); TTF_Quit(); SDL_Quit();
}