#include "graphicMgr.hpp"
#include <allegro5/allegro_primitives.h>
#include <algorithm>

static const struct { const char* name; const char* sub; int gw, gh, mines; }
PRESETS[3] = {
    {"FACIL",    "9x9   - 10 minas",  9,  9, 10},
    {"MEDIO",    "16x16 - 40 minas", 16, 16, 40},
    {"DIFICIL",  "30x16 - 99 minas", 30, 16, 99},
};

static constexpr int BTN_X = 30, BTN_W = 240, BTN_H = 50;
static constexpr int BTN_Y0 = 65, BTN_GAP = 65;

// ── construction ─────────────────────────────────────────────────────────────

Graphic_Manager::Graphic_Manager(int bs) : bmp_size(bs) {
    display = al_create_display(MENU_W, MENU_H);
    font    = al_create_builtin_font();
}

Graphic_Manager::~Graphic_Manager() {
    al_destroy_font(font);
    al_destroy_display(display);
}

// ── display resize ────────────────────────────────────────────────────────────

void Graphic_Manager::start_game(int gw, int gh) {
    grid_w = gw;
    grid_h = gh;

    // Fit the grid to the monitor, leaving ~80px for OS chrome + toolbar
    ALLEGRO_MONITOR_INFO info;
    al_get_monitor_info(0, &info);
    int screen_w = info.x2 - info.x1;
    int screen_h = info.y2 - info.y1;

    int cs = std::min({bmp_size,
                       screen_w / gw,
                       (screen_h - 80 - TOOLBAR_H) / gh});
    cell_size = std::max(cs, 8);

    al_resize_display(display, gw * cell_size, gh * cell_size + TOOLBAR_H);
}

void Graphic_Manager::show_menu() {
    grid_w = 0; grid_h = 0; cell_size = 0;
    al_resize_display(display, MENU_W, MENU_H);
}

// ── hit testing ──────────────────────────────────────────────────────────────

bool Graphic_Manager::is_menu_click(int x, int y) const {
    return x >= 5 && x <= 35 && y >= 5 && y <= 35;
}

bool Graphic_Manager::is_reset_click(int x, int y) const {
    int sw = grid_w * cell_size;
    int bx = sw / 2 - 15;
    return x >= bx && x <= bx + 30 && y >= 5 && y <= 35;
}

int Graphic_Manager::menu_click(int x, int y) const {
    if (x < BTN_X || x > BTN_X + BTN_W) return -1;
    for (int i = 0; i < 3; i++) {
        int by = BTN_Y0 + i * BTN_GAP;
        if (y >= by && y <= by + BTN_H) return i;
    }
    return -1;
}

// ── drawing helpers ───────────────────────────────────────────────────────────

void Graphic_Manager::draw_scaled_text(const char* text, float sx, float sy,
                                        int align, ALLEGRO_COLOR color, float scale) {
    ALLEGRO_TRANSFORM saved, t;
    al_copy_transform(&saved, al_get_current_transform());
    al_identity_transform(&t);
    al_scale_transform(&t, scale, scale);
    al_use_transform(&t);
    al_draw_text(font, color, sx / scale, sy / scale, align, text);
    al_use_transform(&saved);
}

void Graphic_Manager::draw_lcd_timer(int ms, float screen_x, int align) {
    int secs   = ms / 1000;
    int millis = ms % 1000;
    if (secs > 999) { secs = 999; millis = 999; }

    ALLEGRO_TRANSFORM saved, scaled;
    al_copy_transform(&saved, al_get_current_transform());
    al_identity_transform(&scaled);
    al_scale_transform(&scaled, 2.0f, 2.0f);
    al_use_transform(&scaled);

    al_draw_textf(font, al_map_rgb(220, 50, 50),
                  screen_x / 2.0f, 6.0f, align, "%03d.%03d", secs, millis);

    al_use_transform(&saved);
}

void Graphic_Manager::draw_lcd_number(int n, float screen_x, int align) {
    if (n < -99) n = -99;
    if (n > 999) n = 999;

    ALLEGRO_TRANSFORM saved, scaled;
    al_copy_transform(&saved, al_get_current_transform());
    al_identity_transform(&scaled);
    al_scale_transform(&scaled, 2.0f, 2.0f);
    al_use_transform(&scaled);

    const char* fmt = (n < 0) ? "-%02d" : "%03d";
    int val = (n < 0) ? -n : n;
    al_draw_textf(font, al_map_rgb(220, 50, 50), screen_x / 2.0f, 6.0f, align, fmt, val);

    al_use_transform(&saved);
}

// ── menu ─────────────────────────────────────────────────────────────────────

void Graphic_Manager::render_menu() {
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(28, 28, 28));

    draw_scaled_text("BUSCAMINAS", MENU_W / 2.0f, 20.0f,
                     ALLEGRO_ALIGN_CENTRE, al_map_rgb(240, 240, 240), 2.0f);

    for (int i = 0; i < 3; i++) {
        int by = BTN_Y0 + i * BTN_GAP;

        al_draw_filled_rectangle(BTN_X, by, BTN_X + BTN_W, by + BTN_H,
                                 al_map_rgb(60, 60, 60));
        al_draw_rectangle(BTN_X + 0.5f, by + 0.5f,
                          BTN_X + BTN_W - 0.5f, by + BTN_H - 0.5f,
                          al_map_rgb(140, 140, 140), 1);

        float cx = BTN_X + BTN_W / 2.0f;
        draw_scaled_text(PRESETS[i].name, cx, by + 9.0f,
                         ALLEGRO_ALIGN_CENTRE, al_map_rgb(230, 230, 230), 2.0f);
        al_draw_text(font, al_map_rgb(160, 160, 160),
                     cx, by + 35.0f, ALLEGRO_ALIGN_CENTRE, PRESETS[i].sub);
    }

    al_flip_display();
}

// ── toolbar ───────────────────────────────────────────────────────────────────

void Graphic_Manager::render_toolbar(bool game_over, bool won,
                                      int mines_remaining, int elapsed_ms) {
    int sw = grid_w * cell_size;

    al_draw_filled_rectangle(0, 0, sw, TOOLBAR_H, al_map_rgb(192, 192, 192));
    al_draw_line(0, TOOLBAR_H - 1, sw, TOOLBAR_H - 1, al_map_rgb(128, 128, 128), 1);

    // M (menu) button — far left
    al_draw_filled_rectangle(5, 5, 35, 35, al_map_rgb(220, 220, 220));
    al_draw_rectangle(5.5f, 5.5f, 34.5f, 34.5f, al_map_rgb(80, 80, 80), 1);
    al_draw_text(font, al_map_rgb(0, 0, 0), 20, 17, ALLEGRO_ALIGN_CENTRE, "M");

    // Mine counter — left of center
    draw_lcd_number(mines_remaining, 45.0f, ALLEGRO_ALIGN_LEFT);

    // Timer — right side
    draw_lcd_timer(elapsed_ms, sw - 8.0f, ALLEGRO_ALIGN_RIGHT);

    // Reset (face) button — center
    int bx = sw / 2 - 15;
    al_draw_filled_rectangle(bx, 5, bx + 30, 35, al_map_rgb(220, 220, 220));
    al_draw_rectangle(bx + 0.5f, 5.5f, bx + 29.5f, 34.5f, al_map_rgb(80, 80, 80), 1);
    const char* face = won ? ":D" : (game_over ? ":(" : ":)");
    al_draw_text(font, al_map_rgb(0, 0, 0), bx + 15, 17, ALLEGRO_ALIGN_CENTRE, face);
}

// ── game render ───────────────────────────────────────────────────────────────

void Graphic_Manager::render(const Map& map, const Bitmaps& bitmaps,
                              bool game_over, bool won,
                              int mines_remaining, int elapsed_ms) {
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0, 0, 0));

    render_toolbar(game_over, won, mines_remaining, elapsed_ms);

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
            al_draw_scaled_bitmap(bmp,
                0, 0, bmp_size, bmp_size,
                x * cell_size, y * cell_size + TOOLBAR_H,
                cell_size, cell_size, 0);
        }
    }

    if (game_over || won) {
        int sw = grid_w * cell_size;
        int sh = grid_h * cell_size;
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
        al_draw_filled_rectangle(0, TOOLBAR_H, sw, TOOLBAR_H + sh,
                                 al_map_rgba(0, 0, 0, 160));
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        ALLEGRO_COLOR color = won ? al_map_rgb(80, 255, 80) : al_map_rgb(255, 60, 60);
        const char*   msg   = won ? "YOU WIN!"              : "GAME OVER";
        draw_scaled_text(msg, sw / 2.0f, TOOLBAR_H + sh / 2.0f - 8.0f,
                         ALLEGRO_ALIGN_CENTRE, color, 2.0f);
    }

    al_flip_display();
}
