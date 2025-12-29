#include "game.h"

int main(int argc, char** argv) {
    Game g;
    if (!game_init(&g, "SushiSim", 960, 540)) return 1;

    while (g.running) {
        game_handle_events(&g);
        game_update(&g);
        game_render(&g);
    }

    game_cleanup(&g);
    return 0;
}
