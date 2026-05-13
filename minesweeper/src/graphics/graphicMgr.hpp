#pragma once
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <string>
#include "map.hpp"
#include "bitmaps.hpp"
#include "leaderboard.hpp"

class Graphic_Manager {
    static constexpr int TOOLBAR_H = 40;
    static constexpr int MENU_W    = 300;
    static constexpr int MENU_H    = 275;
    static constexpr int LB_W      = 820;
    static constexpr int LB_H      = 520;
    static constexpr int LB_TAB_H  = 45;
    static constexpr int LB_LIST_W = 310;

    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT*    font;
    int bmp_size;
    int cell_size = 0;
    int grid_w    = 0;
    int grid_h    = 0;

    void render_toolbar(bool game_over, bool won, int mines_remaining, int elapsed_ms);
    void draw_lcd_number(int n, float screen_x, int align);
    void draw_lcd_timer(int ms, float screen_x, int align);
    void draw_scaled_text(const char* text, float sx, float sy,
                          int align, ALLEGRO_COLOR color, float scale);
    void draw_graph(const Leaderboard& lb, int difficulty,
                    float gx, float gy, float gw, float gh);
public:
    explicit Graphic_Manager(int bmp_size);
    ~Graphic_Manager();

    ALLEGRO_DISPLAY* get_display()        const { return display; }
    int              get_toolbar_height() const { return TOOLBAR_H; }
    int              get_cell_size()      const { return cell_size; }

    // Display resize helpers
    void start_game(int gw, int gh);
    void show_menu();
    void show_leaderboard();

    // Hit testing — game toolbar
    bool is_reset_click(int x, int y) const;
    bool is_menu_click(int x, int y)  const;

    // Hit testing — menu
    int  menu_click(int x, int y) const; // 0-2=difficulty, 3=leaderboard, -1=none

    // Hit testing — leaderboard
    int  lb_tab_click(int x, int y)  const; // 0-2=difficulty, -1=none
    bool lb_back_click(int x, int y) const;

    // Rendering
    void render_menu();
    void render(const Map& map, const Bitmaps& bitmaps,
                bool game_over, bool won,
                int mines_remaining, int elapsed_ms,
                bool flip = true);
    void render_name_entry(const Map& map, const Bitmaps& bitmaps,
                           int mines_remaining, int elapsed_ms,
                           const std::string& input);
    void render_leaderboard(const Leaderboard& lb, int difficulty);
};
