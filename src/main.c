/**
 * Dosya: main.c
 * Amaç: Oyunun giriş noktasıdır (Entry Point).
 * * İşleyiş:
 * 1. Oyun yapısını (Game struct) oluşturur.
 * 2. `game_init` ile pencereyi ve gerekli kütüphaneleri başlatır.
 * 3. Oyun çalıştığı sürece (g.running) sürekli bir döngüde (Loop) olayları, güncellemeleri ve çizimleri yönetir.
 * 4. Oyun kapanınca `game_cleanup` ile hafızayı temizler.
 */

#include "game.h"

int main(int argc, char** argv) {
    Game g; // Oyunun ana veri yapısını oluştur

    // Oyunu başlat: Pencere ismi "SushiSim", Genişlik 960, Yükseklik 540
    // Eğer başlatma başarısız olursa (0 dönerse) programı hata koduyla (1) bitir.
    if (!game_init(&g, "SushiSim", 960, 540)) return 1;

    // --- OYUN DÖNGÜSÜ (GAME LOOP) ---
    // g.running 1 olduğu sürece bu döngü saniyede defalarca döner.
    while (g.running) {
        game_handle_events(&g); // Klavye/Fare hareketlerini dinle
        game_update(&g);        // Oyun mantığını (hareket, fizik, puan) güncelle
        game_render(&g);        // Her şeyi ekrana çiz
    }

    // Döngü bittiğinde (oyun kapanınca) kullanılan kaynakları serbest bırak.
    game_cleanup(&g);
    
    return 0; // Program başarıyla sonlandı.
}