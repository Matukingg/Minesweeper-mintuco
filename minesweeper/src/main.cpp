#include <algorithm>
#include <string>
#include "core.hpp"
#include "map.hpp"
#include "bitmaps.hpp"
#include "graphicMgr.hpp"
#include "input.hpp"
#include "leaderboard.hpp"
#include "profile.hpp"

static const struct { int gw, gh, mines; } PRESETS[3] = {
    { 9,  9, 10},
    {16, 16, 40},
    {30, 16, 99},
};

int main() {
    Core& config = Core::instance();

    Bitmaps tiles;
    if (!tiles.check_bitmaps()) return -1;

    Leaderboard leaderboard;
    leaderboard.load();

    ProfileManager pm;
    pm.load();
    // Restore last-used profile
    std::string saved_prof = config.get_string("active_profile", "");
    if (!saved_prof.empty() && pm.exists(saved_prof))
        pm.set_active(saved_prof);

    Graphic_Manager gfx(tiles.get_bitmap_size());
    Input input(gfx.get_display());

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 20.0);
    al_register_event_source(input.get_queue(), al_get_timer_event_source(timer));

    enum Phase { MENU, PLAYING, NAME_ENTRY, LEADERBOARD, PROFILES } phase = MENU;

    int    grid_width  = 0, grid_height = 0, num_mines = 0;
    int    cell_size   = 0, game_diff   = 0, lb_diff   = 0;
    Map    map(1, 1);
    bool   running     = true, game_over = false, won = false, first_click = true;
    int    elapsed_ms  = 0;
    double start_time  = 0.0;
    std::string name_input, prof_input;

    // ── helpers ───────────────────────────────────────────────────────────────

    auto do_render = [&]() {
        gfx.render(map, tiles, game_over, won,
                   num_mines - map.count_flags(), elapsed_ms);
    };

    auto save_score_and_show_lb = [&](const std::string& name) {
        leaderboard.add({name, game_diff, elapsed_ms});
        leaderboard.save();
        lb_diff = game_diff;
        gfx.show_leaderboard();
        phase = LEADERBOARD;
        gfx.render_leaderboard(leaderboard, lb_diff);
    };

    auto do_reset = [&]() {
        al_stop_timer(timer);
        map.reset();
        game_over = won = false;
        first_click = true;
        elapsed_ms = 0; start_time = 0.0;
        do_render();
    };

    auto go_to_menu = [&]() {
        al_stop_timer(timer);
        elapsed_ms = 0; start_time = 0.0;
        game_over = won = false;
        first_click = true;
        phase = MENU;
        gfx.show_menu();
        gfx.render_menu(pm);
    };

    auto go_to_leaderboard = [&](int diff) {
        lb_diff = diff;
        gfx.show_leaderboard();
        phase = LEADERBOARD;
        gfx.render_leaderboard(leaderboard, lb_diff);
    };

    auto start_game = [&](int preset) {
        game_diff   = preset;
        grid_width  = PRESETS[preset].gw;
        grid_height = PRESETS[preset].gh;
        num_mines   = PRESETS[preset].mines;
        gfx.start_game(grid_width, grid_height);
        cell_size   = gfx.get_cell_size();
        map         = Map(grid_width, grid_height);
        game_over = won = false;
        first_click = true;
        elapsed_ms = 0; start_time = 0.0;
        al_stop_timer(timer);
        phase = PLAYING;
        do_render();
    };

    auto set_active_profile = [&](const std::string& name) {
        pm.set_active(name);
        config.set_string("active_profile", name);
        config.save_settings();
    };

    gfx.render_menu(pm);

    // ── event loop ────────────────────────────────────────────────────────────

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(input.get_queue(), &ev);

        switch (ev.type) {

        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            running = false;
            break;

        case ALLEGRO_EVENT_TIMER:
            if (phase == PLAYING && !game_over && !won && !first_click) {
                elapsed_ms = std::min((int)((al_get_time()-start_time)*1000), 999999);
                do_render();
            }
            break;

        case ALLEGRO_EVENT_KEY_CHAR:
            if (phase == NAME_ENTRY) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                    if (!name_input.empty()) name_input.pop_back();
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER ||
                           ev.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER) {
                    if (!name_input.empty()) { save_score_and_show_lb(name_input); break; }
                } else if (ev.keyboard.unichar >= 32 && ev.keyboard.unichar < 127
                           && (int)name_input.size() < 20) {
                    name_input += (char)ev.keyboard.unichar;
                }
                gfx.render_name_entry(map, tiles, num_mines-map.count_flags(),
                                      elapsed_ms, name_input);
            } else if (phase == PROFILES) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
                    if (!prof_input.empty()) prof_input.pop_back();
                } else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER ||
                           ev.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER) {
                    if (!prof_input.empty() && !pm.exists(prof_input)) {
                        pm.add(prof_input);
                        pm.save();
                        set_active_profile(prof_input);
                        prof_input = "";
                    }
                } else if (ev.keyboard.unichar >= 32 && ev.keyboard.unichar < 127
                           && (int)prof_input.size() < 20) {
                    prof_input += (char)ev.keyboard.unichar;
                }
                gfx.render_profiles(pm, prof_input);
            }
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
            int mx = ev.mouse.x, my = ev.mouse.y;

            // ── MENU ──────────────────────────────────────────────────────────
            if (phase == MENU) {
                if (ev.mouse.button == 1) {
                    int d = gfx.menu_click(mx, my);
                    if      (d >= 0 && d <= 2) start_game(d);
                    else if (d == 3)           go_to_leaderboard(0);
                    else if (d == 4) {
                        prof_input = "";
                        gfx.show_profiles();
                        phase = PROFILES;
                        gfx.render_profiles(pm, prof_input);
                    }
                }
                break;
            }

            // ── PROFILES ──────────────────────────────────────────────────────
            if (phase == PROFILES) {
                if (ev.mouse.button == 1) {
                    int count = (int)pm.get_all().size();
                    if (gfx.prof_back_click(mx, my)) {
                        go_to_menu();
                    } else if (gfx.prof_add_click(mx, my)) {
                        if (!prof_input.empty() && !pm.exists(prof_input)) {
                            pm.add(prof_input);
                            pm.save();
                            set_active_profile(prof_input);
                            prof_input = "";
                        }
                        gfx.render_profiles(pm, prof_input);
                    } else {
                        int del = gfx.prof_delete_click(mx, my, count);
                        if (del >= 0) {
                            pm.remove(del);
                            pm.save();
                            config.set_string("active_profile", pm.get_active());
                            config.save_settings();
                            gfx.render_profiles(pm, prof_input);
                        } else {
                            int sel = gfx.prof_select_click(mx, my, count);
                            if (sel >= 0) {
                                set_active_profile(pm.get_all()[sel]);
                                go_to_menu(); // immediately return with new profile active
                            }
                        }
                    }
                }
                break;
            }

            // ── LEADERBOARD ───────────────────────────────────────────────────
            if (phase == LEADERBOARD) {
                if (ev.mouse.button == 1) {
                    int tab = gfx.lb_tab_click(mx, my);
                    if      (tab >= 0)               { lb_diff=tab; gfx.render_leaderboard(leaderboard,lb_diff); }
                    else if (gfx.lb_back_click(mx,my)) go_to_menu();
                }
                break;
            }

            // ── NAME_ENTRY (mouse ignored) ─────────────────────────────────────
            if (phase == NAME_ENTRY) break;

            // ── PLAYING ───────────────────────────────────────────────────────
            if (ev.mouse.button==1 && gfx.is_menu_click(mx,my))  { go_to_menu(); break; }
            if (ev.mouse.button==1 && gfx.is_reset_click(mx,my)) { do_reset();   break; }
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
                if (map.is_revealed(cx,cy) && map.get_value(cx,cy) > 0)
                    hit = map.chord(cx, cy);
                else
                    hit = map.reveal(cx, cy);

                if (hit) {
                    elapsed_ms = std::min((int)((al_get_time()-start_time)*1000), 999999);
                    map.reveal_all_mines();
                    game_over = true;
                    al_stop_timer(timer);
                    do_render();
                } else if (map.check_win()) {
                    elapsed_ms = std::min((int)((al_get_time()-start_time)*1000), 999999);
                    won = true;
                    al_stop_timer(timer);
                    if (pm.has_active()) {
                        // Auto-save under active profile, skip name entry
                        save_score_and_show_lb(pm.get_active());
                    } else {
                        name_input = "";
                        phase = NAME_ENTRY;
                        gfx.render_name_entry(map, tiles,
                                              num_mines-map.count_flags(),
                                              elapsed_ms, name_input);
                    }
                } else {
                    do_render();
                }
            } else if (ev.mouse.button == 2) {
                map.toggle_flag(cx, cy);
                // Check win after flagging (all-mines-flagged condition)
                if (!first_click && map.check_win()) {
                    elapsed_ms = std::min((int)((al_get_time()-start_time)*1000), 999999);
                    won = true;
                    al_stop_timer(timer);
                    if (pm.has_active()) {
                        save_score_and_show_lb(pm.get_active());
                    } else {
                        name_input = "";
                        phase = NAME_ENTRY;
                        gfx.render_name_entry(map, tiles,
                                              num_mines-map.count_flags(),
                                              elapsed_ms, name_input);
                    }
                } else {
                    do_render();
                }
            }
            break;
        }

        default: break;
        }
    }

    al_destroy_timer(timer);
    tiles.destroy_bitmaps();
    return 0;
}
