/**
 * Dosya: src/game.h
 * Açıklama: Oyunun ana yapısını ve durumlarını (Menü, Oyun, Pause) tanımlar.
 */

#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

// OYUN DURUMLARI (STATE MACHINE)
// Oyunun o an hangi ekranda olduğunu kontrol ederiz.
typedef enum {
    STATE_START,     // Giriş Ekranı (Başla butonu burada)
    STATE_PLAY,     // Oyunun oynandığı asıl ekran
    STATE_PAUSE,     // Oyun duraklatıldı ekranı
    STATE_GAME_OVER // Oyun bitti ekrani
} GameState;

// OYUN NESNESİ (STRUCT)
// Pencere, çizici ve oyunun genel verilerini tek bir pakette tutar.
typedef struct {
    SDL_Window* window;     // İşletim sistemi penceresi
    SDL_Renderer* renderer; // Ekrana çizim yapan araç (Fırçamız)
    int running;            // Oyun döngüsü devam etsin mi? (1: Evet, 0: Hayır)
    
    GameState state;        // Şu anki ekran durumu
    
    int width;              // Pencere Genişliği
    int height;             // Pencere Yüksekliği

    Uint32 lastTick;        // Son karenin işlendiği zaman (FPS hesabı için)
    float dt;               // Delta Time: İki kare arasında geçen süre (Saniye cinsinden)
} Game;

// Fonksiyon Prototipleri (Diğer dosyalardan erişilecek fonksiyonlar)
int game_init(Game* g, const char* title, int w, int h); // Başlatma
void game_handle_events(Game* g); // Klavye/Fare dinleme
void game_update(Game* g);        // Mantıksal hesaplamalar
void game_render(Game* g);        // Ekrana çizim yapma
void game_cleanup(Game* g);       // Temizlik ve kapatma

