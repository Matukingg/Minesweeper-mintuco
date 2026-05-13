#include "bitmaps.hpp"
#include <string>
#include <iostream>

ALLEGRO_BITMAP* Bitmaps::covered   = nullptr;
ALLEGRO_BITMAP* Bitmaps::uncovered = nullptr;
ALLEGRO_BITMAP* Bitmaps::flag      = nullptr;
ALLEGRO_BITMAP* Bitmaps::mine      = nullptr;
std::array<ALLEGRO_BITMAP*, 8> Bitmaps::bmpNumber{};

static ALLEGRO_BITMAP* load_or_abort(const std::string& path) {
    ALLEGRO_BITMAP* bmp = al_load_bitmap(path.c_str());
    if (!bmp) {
        std::cerr << "Error: could not load «" << path << "»\n";
        std::exit(EXIT_FAILURE);
    }
    return bmp;
}

Bitmaps::Bitmaps() {
    const std::string PATH = "/home/matuco/Documents/minesweeper/data/";
    covered   = load_or_abort(PATH + "covered.png");
    uncovered = load_or_abort(PATH + "uncovered.png");
    flag      = load_or_abort(PATH + "flag.png");
    mine      = load_or_abort(PATH + "mine.png");
    for (int i = 0; i < 8; ++i)
        bmpNumber[i] = load_or_abort(PATH + "numbers/" + std::to_string(i + 1) + ".png");
}

void Bitmaps::destroy_bitmaps() {
    al_destroy_bitmap(covered);   covered   = nullptr;
    al_destroy_bitmap(uncovered); uncovered = nullptr;
    al_destroy_bitmap(flag);      flag      = nullptr;
    al_destroy_bitmap(mine);      mine      = nullptr;
    for (auto*& bmp : bmpNumber) { al_destroy_bitmap(bmp); bmp = nullptr; }
}
