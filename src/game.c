/**
 * Dosya: game.c
 * Amaç: Oyunun ana mantığını yönetir. Olay döngüsü, oyun durumları (Menü, Oyun, Bitiş)
 * ve fare tıklamalarıyla etkileşimler burada kodlanmıştır.
 */

#include "game.h"
#include <stdio.h>
#include "player.h"
#include "stats.h"
#include "render.h"
#include <stdlib.h> // rand() fonksiyonu için gerekli

// Oyunun o anki durumunu belirten Enum yapısı
typedef enum { 
    STATE_START,    // Giriş Menüsü
    STATE_PLAY,     // Oyun Oynanıyor
    STATE_GAMEOVER  // Oyun Bitti Ekranı
} GameState;

// Başlangıç durumu STATE_START olarak ayarlandı
static GameState state = STATE_START;

// Oyun içi nesneler
static Player player; // Kedi karakteri
static Stats stats;   // İstatistikler (Açlık, Mutluluk vb.)
static SDL_Texture* sushiSheet = NULL; // Kedinin resim dosyası (Sprite Sheet)

// --- PARTİKÜL SİSTEMİ (KALPLER) ---
// Oyuncuya geri bildirim vermek için ekranda uçuşan kalpler
#define MAX_PARTICLES 20 // Ekranda aynı anda en fazla 20 kalp olabilir

typedef struct {
    float x, y;   // Konum
    float vy;     // Dikey hız (Yukarı çıkma hızı)
    float life;   // Ömür (1.0'dan başlar, 0.0 olunca silinir)
    int active;   // Bu partikül şu an ekranda mı? (1: Evet, 0: Hayır)
} Particle;

static Particle particles[MAX_PARTICLES]; // Partikül havuzu

// Yeni bir kalp oluşturma fonksiyonu
void spawn_heart(float x, float y) {
    // Havuzdaki boş (active olmayan) ilk partikülü bul ve kullan
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (!particles[i].active) {
            particles[i].active = 1;
            particles[i].x = x;
            particles[i].y = y;
            // -50 ile -100 arasında rastgele bir yukarı hız ver
            particles[i].vy = -50.0f - (rand() % 50); 
            particles[i].life = 1.0f; // Tam ömürle başla
            break; // Bir tane oluşturunca döngüden çık
        }
    }
}

// Tüm aktif partikülleri güncelleme (Hareket ettirme ve ömrünü azaltma)
void update_particles(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].active) {
            particles[i].y += particles[i].vy * dt; // Yukarı hareket
            particles[i].life -= dt * 1.5f;         // Ömürden düş (Silinme hızı)
            
            // Ömrü bittiyse pasif hale getir
            if (particles[i].life <= 0) particles[i].active = 0;
        }
    }
}
// ----------------------------------

// BUTON TANIMLAMALARI (Koordinat ve Boyutlar: x, y, genişlik, yükseklik)
static SDL_Rect btnStart   = { 380, 280, 200, 50 }; // Başla butonu
static SDL_Rect btnRestart = { 380, 320, 200, 50 }; // Tekrar butonu

// Oyun içi eylem butonları
static SDL_Rect btnFeed    = { 140, 470, 160, 50 }; // Mama
static SDL_Rect btnPlay    = { 310, 470, 160, 50 }; // Oyna
static SDL_Rect btnRest    = { 480, 470, 160, 50 }; // Uyu
static SDL_Rect btnClean   = { 650, 470, 160, 50 }; // Temizle

// Yardımcı Fonksiyon: Bir noktanın (x,y) bir kutunun (rect) içinde olup olmadığını kontrol eder.
// Fare tıklamalarını algılamak için kullanılır.
static int point_in_rect(int x, int y, SDL_Rect r) {
    return (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h);
}

// Oyunu Başlatma (Init)
int game_init(Game* g, const char* title, int w, int h) {
    // SDL kütüphanesini başlat (Video ve Zamanlayıcı modülleri)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return 0;
    
    // Yazı tipi (Font) kütüphanesini başlat
    if (TTF_Init() == -1) return 0;
    
    // Resim yükleme kütüphanesini (PNG desteği ile) başlat
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return 0;

    // Pencereyi oluştur
    g->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    
    // Çiziciyi (Renderer) oluştur (Donanım hızlandırma aktif)
    g->renderer = SDL_CreateRenderer(g->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    // Kedinin görselini yükle
    sushiSheet = IMG_LoadTexture(g->renderer, "assets/sushi_sheet.png");
    
    // Alt sistemleri hazırla
    player_init(&player, sushiSheet, 32, 32); // Oyuncu ayarları
    stats_init(&stats);                       // İstatistikler sıfırla
    
    // Partikül havuzunu temizle (başlangıçta hiç kalp yok)
    for(int i=0; i<MAX_PARTICLES; i++) particles[i].active = 0;

    state = STATE_START; // Başlangıç ekranı
    g->running = 1;      // Oyun çalışıyor
    g->lastTick = SDL_GetTicks(); // Zaman sayacını başlat
    return 1;
}

// Olayları (Event) İşleme: Klavye ve Fare
void game_handle_events(Game* g) {
    SDL_Event e;
    // Kuyruktaki tüm olayları teker teker işle
    while (SDL_PollEvent(&e)) {
        // Çarpı işaretine basıldıysa oyunu kapat
        if (e.type == SDL_QUIT) g->running = 0;
        
        // Fare Tıklaması Kontrolü
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mx = e.button.x; // Tıklanan X koordinatı
            int my = e.button.y; // Tıklanan Y koordinatı
            
            // Eğer Başlangıç Ekranındaysak
            if (state == STATE_START) {
                if (point_in_rect(mx, my, btnStart)) state = STATE_PLAY;
            } 
            // Eğer Oyun Oynanıyorsa
            else if (state == STATE_PLAY) {
                // "Mama Ver" Butonu
                if (point_in_rect(mx, my, btnFeed)) {
                    stats_feed(&stats);
                    spawn_heart(player.x + 60, player.y); // Kedinin yanından kalp çıkar
                }
                // "Oyna" Butonu
                else if (point_in_rect(mx, my, btnPlay)) {
                    stats_play(&stats);
                    // İki kalp birden çıkar (daha mutlu)
                    spawn_heart(player.x + 60, player.y);
                    spawn_heart(player.x + 20, player.y - 20);
                }
                // "Uyu" Butonu
                else if (point_in_rect(mx, my, btnRest)) stats_rest(&stats);
                
                // "Temizle" Butonu
                else if (point_in_rect(mx, my, btnClean)) stats_clean(&stats);
            } 
            // Eğer Oyun Bittiyse
            else if (state == STATE_GAMEOVER) {
                // "Tekrar" butonuna basılırsa oyunu sıfırla
                if (point_in_rect(mx, my, btnRestart)) {
                    stats_init(&stats);
                    state = STATE_PLAY;
                }
            }
        }
    }
    
    // Klavye Kontrolleri (Kısayol Tuşları)
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    if (keys[SDL_SCANCODE_ESCAPE]) g->running = 0; // ESC ile çıkış
    
    if (state == STATE_PLAY) {
        // Oyuncuyu hareket ettir
        player_handle_input(&player, keys, g->dt);
        
        // Klavye kısayolları (F: Feed, P: Play, R: Rest)
        if (keys[SDL_SCANCODE_F]) { stats_feed(&stats); spawn_heart(player.x+60, player.y); }
        if (keys[SDL_SCANCODE_P]) { stats_play(&stats); spawn_heart(player.x+60, player.y); }
        if (keys[SDL_SCANCODE_R]) stats_rest(&stats);
    }
}

// Oyun Mantığını Güncelleme (Update)
void game_update(Game* g) {
    // Delta Time hesapla (Geçen süre)
    Uint32 now = SDL_GetTicks();
    g->dt = (now - g->lastTick) / 1000.0f; // Milisaniyeyi saniyeye çevir
    g->lastTick = now;

    if (state == STATE_PLAY) {
        stats_update_over_time(&stats, g->dt); // Açlık, enerji vb. zamanla değişimi
        player_update(&player, g->dt);         // Karakter hareketi güncellemesi
        update_particles(g->dt);               // Kalp efektlerini güncelle

        // Oyun Bitiş Kontrolü: Hepsi 0 ise oyun biter
        if (stats.hunger == 0 && stats.happiness == 0 && stats.energy == 0) state = STATE_GAMEOVER;
    }
}

// Çizim İşlemleri (Render)
void game_render(Game* g) {
    // Renk tanımları
    SDL_Color darkText = { 40, 40, 60, 255 };
    SDL_Color whiteText = { 255, 255, 255, 255 };
    SDL_Color titleColor = { 139, 0, 0, 255 };

    // --- BAŞLANGIÇ EKRANI ---
    if (state == STATE_START) {
        SDL_SetRenderDrawColor(g->renderer, 255, 228, 225, 255); // Açık pembe arka plan
        SDL_RenderClear(g->renderer);

        render_text_centered(g->renderer, "SUSHI SIMULATOR", 120, 64, titleColor);
        render_text_centered(g->renderer, "- Sanal Kedi -", 190, 20, darkText);

        render_button(g->renderer, btnStart);
        render_text(g->renderer, "BASLA", btnStart.x + 70, btnStart.y + 15, whiteText);
    }
    // --- OYUN EKRANI ---
    else if (state == STATE_PLAY) {
        SDL_SetRenderDrawColor(g->renderer, 250, 240, 230, 255); // Krem rengi arka plan
        SDL_RenderClear(g->renderer);

        // 1. Önce yerdeki kakaları çiz (Kedinin arkasında kalsın diye önce çiziyoruz)
        render_poops(g->renderer, stats.poopCount);

        // 2. Kediyi çiz
        player_render(&player, g->renderer);
        
        // 3. Partikülleri (Kalpler) en öne çiz
        for(int i=0; i<MAX_PARTICLES; i++) {
            if(particles[i].active) {
                render_heart(g->renderer, (int)particles[i].x, (int)particles[i].y, (int)(particles[i].life * 255));
            }
        }

        // 4. Gösterge panelini (HUD) çiz
        render_hud(g->renderer, &stats);

        // 5. Butonları çiz
        render_button(g->renderer, btnFeed);
        render_text(g->renderer, "MAMA",  btnFeed.x + 50, btnFeed.y + 15, whiteText);
        
        render_button(g->renderer, btnPlay);
        render_text(g->renderer, "OYNA",   btnPlay.x + 55, btnPlay.y + 15, whiteText);
        
        render_button(g->renderer, btnRest);
        render_text(g->renderer, "UYU", btnRest.x + 60, btnRest.y + 15, whiteText);

        render_button(g->renderer, btnClean);
        render_text(g->renderer, "TEMIZLE", btnClean.x + 40, btnClean.y + 15, whiteText);
    }
    // --- OYUN BİTTİ EKRANI ---
    else if (state == STATE_GAMEOVER) {
        SDL_SetRenderDrawColor(g->renderer, 60, 0, 0, 255); // Koyu kırmızı arka plan
        SDL_RenderClear(g->renderer);
        
        SDL_Color overColor = { 255, 100, 100, 255 };
        render_text_centered(g->renderer, "SUSHI'YE IYI BAKAMADIN :(", 200, 64, overColor);
        
        render_button(g->renderer, btnRestart);
        render_text(g->renderer, "TEKRAR", btnRestart.x + 65, btnRestart.y + 15, whiteText);
    }

    // Çizilen her şeyi ekrana yansıt (Double Buffering)
    render_present(g->renderer);
}

// Temizlik İşlemleri
void game_cleanup(Game* g) {
    if (sushiSheet) SDL_DestroyTexture(sushiSheet);
    if (g->renderer) SDL_DestroyRenderer(g->renderer);
    if (g->window) SDL_DestroyWindow(g->window);
    IMG_Quit(); SDL_Quit(); TTF_Quit();
}