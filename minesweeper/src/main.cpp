#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <iostream>
#include "map.hpp"
#include "bitmaps.hpp"
#include "core.hpp"

int main(){
    Core core;
    auto& config = Core::instance();
    if(!config.load_settings()){
        config.set_int("grid_width", 10);
        config.set_int("grid_height", 10);
        config.set_int("num_mines", 15);
        config.set_bool("fullscreen", false);
        config.save_settings();
    }

    Bitmaps tiles;
    if(!tiles.check_bitmaps()) {
        std::cerr << "Error loading bitmaps." << std::endl;
        return -1;
    }
    int grid_height=config.get_int("grid_height");
    int grid_width=config.get_int("grid_width");
    int num_mines=config.get_int("num_mines");
    Map mapaJuego(grid_width, grid_height);
    mapaJuego.generateMap(num_mines);
    mapaJuego.viewMap();

    ALLEGRO_DISPLAY* display = al_create_display(4 * tiles.get_bitmap_size(), 3 * tiles.get_bitmap_size());
    al_set_target_backbuffer(display);
    al_clear_to_color(al_map_rgb(0,0,0));
    tiles.draw_tile_map();
    al_flip_display();

    al_rest(5.0);
    tiles.destroy_bitmaps();

    return 0;
}