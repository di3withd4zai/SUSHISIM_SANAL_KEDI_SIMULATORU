#pragma once

typedef struct {
    int hunger;     // 0-100
    int happiness;  // 0-100
    int energy;     // 0-100

    float tickTimer; // her 1 saniyede bir otomatik düşürmek için
} Stats;

void stats_init(Stats* s);
void stats_update_over_time(Stats* s, float dt);
void stats_feed(Stats* s);
void stats_play(Stats* s);
void stats_rest(Stats* s);
