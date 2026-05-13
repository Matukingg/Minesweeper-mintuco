#pragma once
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <array>
class Bitmaps {
private:
    static constexpr int BMP_SIZE = 64;
    static ALLEGRO_BITMAP* covered;
    static ALLEGRO_BITMAP* uncovered;
    static ALLEGRO_BITMAP* mine;
    static ALLEGRO_BITMAP* flag;
    static std::array<ALLEGRO_BITMAP*, 8> bmpNumber;
public:
    Bitmaps();
    inline bool check_bitmaps() const {
        if (!covered || !uncovered || !mine || !flag) return false;
        for (auto* b : bmpNumber) {
            if (!b) return false;
        }
        return true;
    }
    inline int get_bitmap_size() const {return BMP_SIZE;};
    inline void destroy_bitmaps() const {
        al_destroy_bitmap(covered);
        al_destroy_bitmap(uncovered);
        al_destroy_bitmap(flag);
        al_destroy_bitmap(mine);
        for(auto* bmp : bmpNumber) 
            al_destroy_bitmap(bmp);

    }
    void draw_tile_map();
};