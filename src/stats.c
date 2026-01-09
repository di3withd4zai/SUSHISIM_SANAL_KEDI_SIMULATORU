/**
 * Dosya: stats.c
 * Amaç: Oyun istatistiklerini (açlık, mutluluk, enerji, puan) yönetir.
 * Zamanla değerlerin düşmesi ve etkileşimler burada hesaplanır.
 */

#include "stats.h"
#include <stdlib.h> // rand() için

// Yardımcı Fonksiyon: Değeri 0 ile 100 arasında tutar (Clamp)
static int clamp100(int v){
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

// İstatistikleri başlat
void stats_init(Stats* s) {
    s->hunger = 70;    // %70 tokluk ile başla
    s->happiness = 60; // %60 mutluluk
    s->energy = 65;    // %65 enerji
    s->score = 0;      // Puan 0
    
    s->poopCount = 0; // Başlangıçta kaka yok
    s->tickTimer = 0.0f;
}

// Zamanla değişen istatistikleri güncelle
void stats_update_over_time(Stats* s, float dt) {
    s->tickTimer += dt;
    
    // Her 1 saniyede bir (Timer 1.0'ı geçince) işlem yap
    while (s->tickTimer >= 1.0f) { 
        s->tickTimer -= 1.0f; // Sayacı bir saniye düşür

        // Zamanla açlık ve enerji düşer
        s->hunger = clamp100(s->hunger - 2);
        s->energy = clamp100(s->energy - 1);
        
        // --- KAKA MEKANİĞİ ---
        // Yerdeki kaka sayısı 5'ten azsa ve %5 şans tutarsa kaka yap
        if (s->poopCount < 5 && (rand() % 100) < 5) { 
            s->poopCount++;
            // Kaka yapınca midesi boşalır, açlığı artar (değer düşer)
            s->hunger = clamp100(s->hunger - 5);
        }

        // --- MUTLULUK HESABI ---
        // Yerde kaka varsa mutluluk daha hızlı düşer (Ceza puanı)
        int penalty = s->poopCount * 2; 
        
        // Açlık çok düşükse (<25) mutluluk hızla azalır
        if (s->hunger < 25) s->happiness = clamp100(s->happiness - 2 - penalty);
        else s->happiness = clamp100(s->happiness - 1 - penalty);

        // --- PUAN SİSTEMİ ---
        // Kedi iyi durumdaysa ve ortam temizse çok puan ver
        if (s->happiness > 50 && s->hunger > 50 && s->poopCount == 0) {
            s->score += 10;
        } else {
            s->score += 1; // Sadece hayatta kaldığı için az puan
        }
    }
}

// Mama Verilince
void stats_feed(Stats* s) {
    s->hunger = clamp100(s->hunger + 12);     // Karnı doyar
    s->happiness = clamp100(s->happiness + 2); // Biraz mutlu olur
}

// Oyun Oynaninca
void stats_play(Stats* s) {
    s->happiness = clamp100(s->happiness + 10); // Çok mutlu olur
    s->energy = clamp100(s->energy - 6);        // Ama yorulur
    s->hunger = clamp100(s->hunger - 3);        // Ve acıkır
}

// Uyuyunca
void stats_rest(Stats* s) {
    s->energy = clamp100(s->energy + 12); // Enerji dolar
    s->hunger = clamp100(s->hunger - 2);  // Uyurken de acıkır
}

// Temizlik Yapılınca
void stats_clean(Stats* s) {
    if (s->poopCount > 0) {
        s->poopCount = 0; // Tüm kakaları temizle
        s->happiness = clamp100(s->happiness + 5); // Kedi temiz ortamda mutlu olur
    }
}
