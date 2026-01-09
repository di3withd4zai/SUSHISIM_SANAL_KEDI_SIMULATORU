#pragma once

typedef struct {
    int hunger;     // Açlık (0-100)
    int energy;     // Enerji (0-100)
    int happiness;  // Mutluluk (0-100)
    int score;      // Puan
    float timer;    // Zamanlayıcı (Süre tutmak için)
} Stats;

void stats_init(Stats* s);
void stats_update_over_time(Stats* s, float dt); // Zamanla azalma
void stats_feed(Stats* s); // Yemek yiyince
void stats_rest(Stats* s); // Uyuyunca
void stats_play(Stats* s); // Oynayınca