#pragma once
#include <iostream>
#include <vector>

class Map {
private:
    int width, height;
    std::vector<std::vector<int>>  mapData;
    std::vector<std::vector<bool>> revealed;
    std::vector<std::vector<bool>> flagged;

    bool in_bounds(int x, int y) const;
    void flood_fill(int x, int y);
public:
    Map(int width, int height);

    // safe_x/safe_y: the 3x3 zone around this cell is guaranteed mine-free (-> forces a 0)
    void generateMap(int mines, int safe_x, int safe_y);
    void reset();         // clears to blank state; call generateMap separately on first click
    int  count_flags() const;

    bool reveal(int x, int y);   // returns true if a mine was hit
    bool chord(int x, int y);    // returns true if a mine was hit
    void toggle_flag(int x, int y);
    void reveal_all_mines();
    bool check_win() const;

    inline int  get_width()  const { return width; }
    inline int  get_height() const { return height; }
    inline int  get_value(int x, int y)   const { return mapData[y][x]; }
    inline bool is_revealed(int x, int y) const { return revealed[y][x]; }
    inline bool is_flagged(int x, int y)  const { return flagged[y][x]; }
};
