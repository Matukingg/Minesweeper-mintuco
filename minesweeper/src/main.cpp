#include "core.hpp"
#include "map.hpp"
#include "bitmaps.hpp"
#include "graphicMgr.hpp"
#include "input.hpp"

int main() {
    Core& config = Core::instance();
    if (!config.load_settings()) {
        config.set_int("grid_width",  10);
        config.set_int("grid_height", 10);
        config.set_int("num_mines",   15);
        config.set_bool("fullscreen", false);
        config.save_settings();
    }

    Bitmaps tiles;
    if (!tiles.check_bitmaps()) return -1;

    int grid_width  = config.get_int("grid_width");
    int grid_height = config.get_int("grid_height");
    int num_mines   = config.get_int("num_mines");
    int cell_size   = tiles.get_bitmap_size();

    Map map(grid_width, grid_height);

    Graphic_Manager gfx(grid_width, grid_height, cell_size);
    Input input(gfx.get_display());

    // 1-second timer for the elapsed display
    ALLEGRO_TIMER* timer = al_create_timer(1.0);
    al_register_event_source(input.get_queue(), al_get_timer_event_source(timer));

    bool running     = true;
    bool game_over   = false;
    bool won         = false;
    bool first_click = true;
    int  elapsed     = 0;

    auto do_render = [&]() {
        gfx.render(map, tiles, game_over, won,
                   num_mines - map.count_flags(), elapsed);
    };

    auto do_reset = [&]() {
        al_stop_timer(timer);
        map.reset();
        game_over   = false;
        won         = false;
        first_click = true;
        elapsed     = 0;
        do_render();
    };

    do_render();

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(input.get_queue(), &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
            continue;
        }

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (elapsed < 999) elapsed++;
            do_render();
            continue;
        }

        if (ev.type != ALLEGRO_EVENT_MOUSE_BUTTON_UP) continue;

        int mx = ev.mouse.x;
        int my = ev.mouse.y;

        // Reset button (always active)
        if (ev.mouse.button == 1 && gfx.is_reset_click(mx, my)) {
            do_reset();
            continue;
        }

        if (game_over || won) continue;

        int toolbar_h = gfx.get_toolbar_height();
        if (my < toolbar_h) continue;

        int cx = mx / cell_size;
        int cy = (my - toolbar_h) / cell_size;

        if (ev.mouse.button == 1) {
            // Generate map on first click, guaranteed 0 at clicked cell
            if (first_click) {
                map.generateMap(num_mines, cx, cy);
                first_click = false;
                elapsed = 0;
                al_start_timer(timer);
            }

            bool hit;
            if (map.is_revealed(cx, cy) && map.get_value(cx, cy) > 0)
                hit = map.chord(cx, cy);
            else
                hit = map.reveal(cx, cy);

            if (hit) {
                map.reveal_all_mines();
                game_over = true;
                al_stop_timer(timer);
            } else if (map.check_win()) {
                won = true;
                al_stop_timer(timer);
            }
        } else if (ev.mouse.button == 2) {
            map.toggle_flag(cx, cy);
        }

        do_render();
    }

    al_destroy_timer(timer);
    tiles.destroy_bitmaps();
    return 0;
}
