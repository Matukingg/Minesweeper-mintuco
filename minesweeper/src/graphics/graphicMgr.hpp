#pragma once
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "map.hpp"
#include "bitmaps.hpp"

class Graphic_Manager {
    static constexpr int TOOLBAR_H = 40;
    static constexpr int MENU_W    = 300;
    static constexpr int MENU_H    = 260;

    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT*    font;
    int bmp_size;          // native bitmap pixel size (64)
    int cell_size = 0;     // actual rendered cell size (may shrink for large grids)
    int grid_w    = 0;
    int grid_h    = 0;

    void render_toolbar(bool game_over, bool won, int mines_remaining, int elapsed_ms);
    void draw_lcd_number(int n, float screen_x, int align);
    void draw_lcd_timer(int ms, float screen_x, int align);
    void draw_scaled_text(const char* text, float screen_x, float screen_y,
                          int align, ALLEGRO_COLOR color, float scale);
public:
    explicit Graphic_Manager(int bmp_size);
    ~Graphic_Manager();

    ALLEGRO_DISPLAY* get_display()        const { return display; }
    int              get_toolbar_height() const { return TOOLBAR_H; }
    int              get_cell_size()      const { return cell_size; }

    void start_game(int gw, int gh);
    void show_menu();

    bool is_reset_click(int x, int y) const;
    bool is_menu_click(int x, int y)  const;
    int  menu_click(int x, int y)     const; // 0=easy,1=medium,2=hard, -1=none

    void render_menu();
    void render(const Map& map, const Bitmaps& bitmaps,
                bool game_over, bool won,
                int mines_remaining, int elapsed_ms);
};
