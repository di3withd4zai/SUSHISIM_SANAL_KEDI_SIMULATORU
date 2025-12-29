/**
 * Dosya: game.h
 * Amaç: Oyunun ana veri yapısını (struct Game) ve temel fonksiyon prototiplerini içerir.
 * Diğer dosyalar bu başlık dosyasını kullanarak oyun motoruna erişir.
 */

#pragma once
#include <SDL.h>
#include <SDL_image.h>

// Oyunun genel durumunu ve pencere bilgilerini tutan yapı
typedef struct {
    SDL_Window* window;     // İşletim sistemi penceresi
    SDL_Renderer* renderer; // Ekrana çizim yapan araç (fırça gibi düşünülebilir)
    int running;            // Oyunun devam edip etmediğini kontrol eden bayrak (1: Devam, 0: Bitir)

    Uint32 lastTick;        // Son karenin işlendiği zaman (milisaniye cinsinden)
    float dt;               // Delta Time: İki kare arasında geçen süre (saniye). 
                            // Oyunun hızının bilgisayar hızından bağımsız olmasını sağlar.
} Game;

// Fonksiyon Prototipleri (Diğer dosyalardan çağrılacak fonksiyonların imzaları)
int game_init(Game* g, const char* title, int w, int h); // Oyunu başlatır
void game_handle_events(Game* g); // Girdi (Input) işlemlerini yönetir
void game_update(Game* g);        // Matematiksel güncellemeleri yapar
void game_render(Game* g);        // Ekrana çizim yapar
void game_cleanup(Game* g);       // Kapatırken temizlik yapar