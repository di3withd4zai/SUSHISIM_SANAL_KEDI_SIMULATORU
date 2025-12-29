#pragma once
    typedef struct {
    int hunger;
    int happiness;
    int energy;
    
    int score; // YENİ: Puan değişkeni
    
    float tickTimer;
} Stats;

void stats_init(Stats* s);
void stats_update_over_time(Stats* s, float dt);
void stats_feed(Stats* s);
void stats_play(Stats* s);
void stats_rest(Stats* s);
