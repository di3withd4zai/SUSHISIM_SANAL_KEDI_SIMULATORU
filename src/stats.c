#include "stats.h"

static int clamp100(int v){
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

void stats_init(Stats* s) {
    s->hunger = 70;
    s->happiness = 60;
    s->energy = 65;
    s->tickTimer = 0.0f;
}

void stats_update_over_time(Stats* s, float dt) {
    s->tickTimer += dt;
    if (s->tickTimer >= 1.0f) { // her 1 saniye
        s->tickTimer = 0.0f;

        s->hunger = clamp100(s->hunger - 2);
        s->energy = clamp100(s->energy - 1);

        // Açlık çok düşerse mutluluk da düşsün
        if (s->hunger < 25) s->happiness = clamp100(s->happiness - 2);
        else s->happiness = clamp100(s->happiness - 1);
    }
}

void stats_feed(Stats* s) {
    s->hunger = clamp100(s->hunger + 12);
    s->happiness = clamp100(s->happiness + 2);
}

void stats_play(Stats* s) {
    s->happiness = clamp100(s->happiness + 10);
    s->energy = clamp100(s->energy - 6);
    s->hunger = clamp100(s->hunger - 3);
}

void stats_rest(Stats* s) {
    s->energy = clamp100(s->energy + 12);
    s->hunger = clamp100(s->hunger - 2);
}
