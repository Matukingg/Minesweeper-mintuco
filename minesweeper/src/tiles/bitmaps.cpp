#include "bitmaps.hpp"
#include <string>
#include <array>
#include <iostream>
ALLEGRO_BITMAP* Bitmaps::covered   = nullptr;
ALLEGRO_BITMAP* Bitmaps::uncovered = nullptr;
ALLEGRO_BITMAP* Bitmaps::flag      = nullptr;
ALLEGRO_BITMAP* Bitmaps::mine      = nullptr;
std::array<ALLEGRO_BITMAP*, 8> Bitmaps::bmpNumber{};

static ALLEGRO_BITMAP* load_or_abort(const std::string& path) {
    ALLEGRO_BITMAP* bmp = al_load_bitmap(path.c_str());
    if (!bmp) {
        std::cerr << "Error: no se pudo cargar «" << path << "»\n";
        std::exit(EXIT_FAILURE);
    }
    return bmp;
}

Bitmaps::Bitmaps() {
    std::string PATH= "/home/matuco/Documents/minesweeper/data/";
    covered   = load_or_abort(PATH+"covered.png");
    uncovered = load_or_abort(PATH+"uncovered.png");
    flag      = load_or_abort(PATH+"flag.png");
    mine      = load_or_abort(PATH+"mine.png");
    for(int i = 0; i <= 7; ++i) {
        std::string path = PATH+ "numbers/" + std::to_string(i+1) + ".png";
        bmpNumber[i]     = load_or_abort(path.c_str());
    }
}

void Bitmaps::draw_tile_map() {
    std::array<ALLEGRO_BITMAP*, 12> bitmaps = {covered, uncovered, mine, flag,
                                               bmpNumber[0], bmpNumber[1], 
                                               bmpNumber[2], bmpNumber[3], 
                                               bmpNumber[4], bmpNumber[5], 
                                               bmpNumber[6], bmpNumber[7]};
    for(int i=0; i<3; i++){
        for(int j=0; j<4; j++){
            int x=j*BMP_SIZE;
            int y=i*BMP_SIZE;
            al_draw_bitmap(bitmaps[i*4+j], x, y, 0);
        }
    }
}