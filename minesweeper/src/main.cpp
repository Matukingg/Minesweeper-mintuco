#include <algorithm>
#include "core.hpp"
#include "map.hpp"
#include "bitmaps.hpp"
#include "graphicMgr.hpp"
#include "input.hpp"

static const struct { int gw, gh, mines; } PRESETS[3] = {
    { 9,  9, 10},
    {16, 16, 40},
    {30, 16, 99},
};

int main() {
    Core::instance(); // initialize Allegro

    Bitmaps tiles;
    if (!tiles.check_bitmaps()) return -1;

    int cell_size = 0; // set by start_game() based on screen resolution

    Graphic_Manager gfx(tiles.get_bitmap_size());
    Input input(gfx.get_display());

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 20.0); // 20 Hz for smooth ms display
    al_register_event_source(input.get_queue(), al_get_timer_event_source(timer));

    enum Phase { MENU, PLAYING } phase = MENU;

    int grid_width = 0, grid_height = 0, num_mines = 0;
    Map map(1, 1); // placeholder until difficulty is chosen

    bool running     = true;
    bool game_over   = false;
    bool won         = false;
    bool   first_click = true;
    int    elapsed_ms  = 0;
    double start_time  = 0.0;

    auto do_render = [&]() {
        gfx.render(map, tiles, game_over, won,
                   num_mines - map.count_flags(), elapsed_ms);
    };

    auto do_reset = [&]() {
        al_stop_timer(timer);
        map.reset();
        game_over   = false;
        won         = false;
        first_click = true;
        elapsed_ms  = 0;
        start_time  = 0.0;
        do_render();
    };

    auto go_to_menu = [&]() {
        al_stop_timer(timer);
        elapsed_ms  = 0;
        start_time  = 0.0;
        game_over   = false;
        won         = false;
        first_click = true;
        phase       = MENU;
        gfx.show_menu();
        gfx.render_menu();
    };

    auto start_game = [&](int preset) {
        grid_width  = PRESETS[preset].gw;
        grid_height = PRESETS[preset].gh;
        num_mines   = PRESETS[preset].mines;
        gfx.start_game(grid_width, grid_height);
        cell_size   = gfx.get_cell_size();
        map         = Map(grid_width, grid_height);
        game_over   = false;
        won         = false;
        first_click = true;
        elapsed_ms  = 0;
        start_time  = 0.0;
        al_stop_timer(timer);
        phase = PLAYING;
        do_render();
    };

    gfx.render_menu();

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(input.get_queue(), &ev);

        switch (ev.type) {

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;

        case ALLEGRO_EVENT_TIMER:
            if (phase == PLAYING && !game_over && !won && !first_click) {
                elapsed_ms = std::min((int)((al_get_time() - start_time) * 1000), 999999);
                do_render();
            }
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
            int mx = ev.mouse.x, my = ev.mouse.y;

            if (phase == MENU) {
                if (ev.mouse.button == 1) {
                    int d = gfx.menu_click(mx, my);
                    if (d >= 0) start_game(d);
                }
                break;
            }

            // PLAYING phase
            if (ev.mouse.button == 1 && gfx.is_menu_click(mx, my)) {
                go_to_menu();
                break;
            }
            if (ev.mouse.button == 1 && gfx.is_reset_click(mx, my)) {
                do_reset();
                break;
            }

            if (game_over || won) break;

            int toolbar_h = gfx.get_toolbar_height();
            if (my < toolbar_h) break;

            int cx = mx / cell_size;
            int cy = (my - toolbar_h) / cell_size;

            if (ev.mouse.button == 1) {
                if (first_click) {
                    map.generateMap(num_mines, cx, cy);
                    first_click = false;
                    start_time  = al_get_time();
                    elapsed_ms  = 0;
                    al_start_timer(timer);
                }

                bool hit;
                if (map.is_revealed(cx, cy) && map.get_value(cx, cy) > 0)
                    hit = map.chord(cx, cy);
                else
                    hit = map.reveal(cx, cy);

                if (hit) {
                    elapsed_ms = std::min((int)((al_get_time() - start_time) * 1000), 999999);
                    map.reveal_all_mines();
                    game_over = true;
                    al_stop_timer(timer);
                } else if (map.check_win()) {
                    elapsed_ms = std::min((int)((al_get_time() - start_time) * 1000), 999999);
                    won = true;
                    al_stop_timer(timer);
                }
            } else if (ev.mouse.button == 2) {
                map.toggle_flag(cx, cy);
            }

            do_render();
            break;
        }

        default: break;
        }
    }

    al_destroy_timer(timer);
    tiles.destroy_bitmaps();
    return 0;
}
