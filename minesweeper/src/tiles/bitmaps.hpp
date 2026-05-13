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
    void destroy_bitmaps();

    inline bool check_bitmaps() const {
        if (!covered || !uncovered || !mine || !flag) return false;
        for (auto* b : bmpNumber) if (!b) return false;
        return true;
    }
    inline int get_bitmap_size() const { return BMP_SIZE; }

    inline ALLEGRO_BITMAP* get_covered()    const { return covered; }
    inline ALLEGRO_BITMAP* get_uncovered()  const { return uncovered; }
    inline ALLEGRO_BITMAP* get_mine()       const { return mine; }
    inline ALLEGRO_BITMAP* get_flag()       const { return flag; }
    inline ALLEGRO_BITMAP* get_number(int i) const { return bmpNumber[i]; }
};
