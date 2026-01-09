#include "stats.h"

// Değeri 0 ile 100 arasında tutan yardımcı fonksiyon (Clamp)
static int clamp(int v) {
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

void stats_init(Stats* s) {
    s->hunger = 60;
    s->energy = 60;
    s->happiness = 60;
    s->score = 0;
    s->timer = 0;
}

void stats_update_over_time(Stats* s, float dt) {
    s->timer += dt;
    // Her 3 saniyede bir ihtiyaçlar artar (değerler düşer)
    if (s->timer > 3.0f) {
        s->hunger = clamp(s->hunger - 1);
        s->energy = clamp(s->energy - 1);
        s->happiness = clamp(s->happiness - 1);
        s->timer = 0;
    }
}

void stats_feed(Stats* s) { s->hunger = clamp(s->hunger + 20); s->score += 10; }
void stats_rest(Stats* s) { s->energy = clamp(s->energy + 30); s->hunger -= 5; } // Uyumak acıktırır
void stats_play(Stats* s) { s->happiness = clamp(s->happiness + 20); s->energy -= 10; } // Oynamak yorar