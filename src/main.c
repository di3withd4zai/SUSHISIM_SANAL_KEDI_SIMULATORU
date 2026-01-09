/**
 * Dosya: src/main.c
 * Açıklama: Programın giriş noktası.
 */

#include "game.h"

int main(int argc, char** argv) {
    Game g;
    
    // Oyunu 1920x1080 HD çözünürlükte başlat
    if (!game_init(&g, "SushiSim: Sanal Kedi Simulatoru", 1920, 1080)) {
        return 1;
    }

    // Oyun Döngüsü (Game Loop)
    while (g.running) {
        game_handle_events(&g); // Girdileri işle
        game_update(&g);        // Oyun dünyasını güncelle
        game_render(&g);        // Ekrana çiz
    }

    // Çıkışta belleği temizle
    game_cleanup(&g);
    return 0;
}

