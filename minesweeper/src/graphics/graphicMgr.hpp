#pragma once
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "map.hpp"
#include "bitmaps.hpp"

class Graphic_Manager {
    static constexpr int TOOLBAR_H = 40;

    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT*    font;
    int cell_size;
    int grid_w, grid_h;

    void render_toolbar(bool game_over, bool won, int mines_remaining, int elapsed_secs);
    void draw_lcd_number(int n, float screen_x, int align);
public:
    Graphic_Manager(int grid_width, int grid_height, int cell_size);
    ~Graphic_Manager();

    ALLEGRO_DISPLAY* get_display()        const { return display; }
    int              get_toolbar_height() const { return TOOLBAR_H; }
    bool             is_reset_click(int x, int y) const;
    void             render(const Map& map, const Bitmaps& bitmaps,
                            bool game_over, bool won,
                            int mines_remaining, int elapsed_secs);
};
