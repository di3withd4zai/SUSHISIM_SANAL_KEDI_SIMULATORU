#pragma once

// Oyun İstatistikleri Veri Yapısı
typedef struct {
    int hunger;     // Açlık (0-100)
    int happiness;  // Mutluluk (0-100)
    int energy;     // Enerji (0-100)

    int score;      // Toplanan Puan
    int poopCount;  // Yerdeki kaka sayısı

    float tickTimer; // Zamanlayıcı
} Stats;

// Fonksiyon tanımları...
void stats_init(Stats* s);
void stats_update_over_time(Stats* s, float dt);
void stats_feed(Stats* s);
void stats_play(Stats* s);
void stats_rest(Stats* s);
void stats_clean(Stats* s);