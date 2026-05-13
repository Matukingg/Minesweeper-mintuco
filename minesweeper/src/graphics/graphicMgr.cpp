#include "graphicMgr.hpp"
#include <allegro5/allegro_primitives.h>

Graphic_Manager::Graphic_Manager(int gw, int gh, int cs)
    : cell_size(cs), grid_w(gw), grid_h(gh) {
    display = al_create_display(gw * cs, gh * cs + TOOLBAR_H);
    font    = al_create_builtin_font();
}

Graphic_Manager::~Graphic_Manager() {
    al_destroy_font(font);
    al_destroy_display(display);
}

bool Graphic_Manager::is_reset_click(int x, int y) const {
    int bx = grid_w * cell_size / 2 - 15;
    return x >= bx && x <= bx + 30 && y >= 5 && y <= 35;
}

// Draws a 3-digit number at 2x font scale. screen_x is the anchor pixel; align is ALLEGRO_ALIGN_*.
void Graphic_Manager::draw_lcd_number(int n, float screen_x, int align) {
    if (n < -99) n = -99;
    if (n > 999) n = 999;

    ALLEGRO_TRANSFORM saved, scaled;
    al_copy_transform(&saved, al_get_current_transform());
    al_identity_transform(&scaled);
    al_scale_transform(&scaled, 2.0f, 2.0f);
    al_use_transform(&scaled);

    // Vertically center 16px text in 40px toolbar: top = (40-16)/2 = 12 → 6 in scaled space
    // %03d handles negatives: -1 → "-01" (sign + 2 digits), positives zero-padded to 3
    const char* fmt = (n < 0) ? "-%02d" : "%03d";
    int val = (n < 0) ? -n : n;
    al_draw_textf(font, al_map_rgb(220, 50, 50), screen_x / 2.0f, 6.0f, align, fmt, val);

    al_use_transform(&saved);
}

void Graphic_Manager::render_toolbar(bool game_over, bool won, int mines_remaining, int elapsed_secs) {
    int sw = grid_w * cell_size;

    // Background + bottom border
    al_draw_filled_rectangle(0, 0, sw, TOOLBAR_H, al_map_rgb(192, 192, 192));
    al_draw_line(0, TOOLBAR_H - 1, sw, TOOLBAR_H - 1, al_map_rgb(128, 128, 128), 1);

    // Mine counter — left side
    draw_lcd_number(mines_remaining, 8.0f, ALLEGRO_ALIGN_LEFT);

    // Timer — right side
    draw_lcd_number(elapsed_secs, sw - 8.0f, ALLEGRO_ALIGN_RIGHT);

    // Reset button — center
    int bx = sw / 2 - 15;
    al_draw_filled_rectangle(bx, 5, bx + 30, 35, al_map_rgb(220, 220, 220));
    al_draw_rectangle(bx + 0.5f, 5.5f, bx + 29.5f, 34.5f, al_map_rgb(80, 80, 80), 1);
    const char* face = won ? ":D" : (game_over ? ":(" : ":)");
    al_draw_text(font, al_map_rgb(0, 0, 0), bx + 15, 17, ALLEGRO_ALIGN_CENTRE, face);
}

void Graphic_Manager::render(const Map& map, const Bitmaps& bitmaps,
                              bool game_over, bool won,
                              int mines_remaining, int elapsed_secs) {
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0, 0, 0));

    render_toolbar(game_over, won, mines_remaining, elapsed_secs);

    for (int y = 0; y < map.get_height(); y++) {
        for (int x = 0; x < map.get_width(); x++) {
            ALLEGRO_BITMAP* bmp;
            if (map.is_flagged(x, y)) {
                bmp = bitmaps.get_flag();
            } else if (!map.is_revealed(x, y)) {
                bmp = bitmaps.get_covered();
            } else {
                int val = map.get_value(x, y);
                if      (val == -1) bmp = bitmaps.get_mine();
                else if (val ==  0) bmp = bitmaps.get_uncovered();
                else                bmp = bitmaps.get_number(val - 1);
            }
            al_draw_bitmap(bmp, x * cell_size, y * cell_size + TOOLBAR_H, 0);
        }
    }

    if (game_over || won) {
        int sw = grid_w * cell_size;
        int sh = grid_h * cell_size;
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_filled_rectangle(0, TOOLBAR_H, sw, TOOLBAR_H + sh, al_map_rgba(0, 0, 0, 160));
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        ALLEGRO_COLOR color = won ? al_map_rgb(80, 255, 80) : al_map_rgb(255, 60, 60);
        const char*   msg   = won ? "YOU WIN!"              : "GAME OVER";
        al_draw_text(font, color, sw / 2, TOOLBAR_H + sh / 2 - 4, ALLEGRO_ALIGN_CENTRE, msg);
    }

    al_flip_display();
}
