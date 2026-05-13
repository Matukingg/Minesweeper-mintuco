#include "map.hpp"
#include <random>
#include <algorithm>

Map::Map(int w, int h) : width(w), height(h) {
    mapData.assign(h, std::vector<int>(w, 0));
    revealed.assign(h, std::vector<bool>(w, false));
    flagged.assign(h, std::vector<bool>(w, false));
}

void Map::generateMap(int mines, int safe_x, int safe_y) {
    // Build candidate list, excluding the 3x3 safe zone
    std::vector<std::pair<int,int>> candidates;
    candidates.reserve(width * height);
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (x < safe_x - 1 || x > safe_x + 1 || y < safe_y - 1 || y > safe_y + 1)
                candidates.push_back({x, y});

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(candidates.begin(), candidates.end(), gen);

    int to_place = std::min(mines, (int)candidates.size());
    for (int i = 0; i < to_place; i++)
        mapData[candidates[i].second][candidates[i].first] = -1;

    // Neighbor counts
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (mapData[y][x] == -1) continue;
            int count = 0;
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue;
                    int nx = x+dx, ny = y+dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height && mapData[ny][nx] == -1)
                        count++;
                }
            mapData[y][x] = count;
        }
    }
}

void Map::reset() {
    mapData.assign(height, std::vector<int>(width, 0));
    revealed.assign(height, std::vector<bool>(width, false));
    flagged.assign(height, std::vector<bool>(width, false));
}

int Map::count_flags() const {
    int count = 0;
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (flagged[y][x]) count++;
    return count;
}

bool Map::in_bounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

void Map::flood_fill(int x, int y) {
    if (!in_bounds(x, y) || revealed[y][x] || flagged[y][x]) return;
    revealed[y][x] = true;
    if (mapData[y][x] == 0) {
        for (int dy = -1; dy <= 1; dy++)
            for (int dx = -1; dx <= 1; dx++)
                if (dy != 0 || dx != 0)
                    flood_fill(x + dx, y + dy);
    }
}

bool Map::reveal(int x, int y) {
    if (!in_bounds(x, y) || revealed[y][x] || flagged[y][x]) return false;
    if (mapData[y][x] == -1) { revealed[y][x] = true; return true; }
    flood_fill(x, y);
    return false;
}

bool Map::chord(int x, int y) {
    if (!in_bounds(x, y) || !revealed[y][x] || mapData[y][x] <= 0) return false;

    int flag_count = 0;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            int nx = x+dx, ny = y+dy;
            if (in_bounds(nx, ny) && flagged[ny][nx]) flag_count++;
        }

    if (flag_count != mapData[y][x]) return false;

    bool hit_mine = false;
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            int nx = x+dx, ny = y+dy;
            if (in_bounds(nx, ny) && !revealed[ny][nx] && !flagged[ny][nx])
                if (reveal(nx, ny)) hit_mine = true;
        }
    return hit_mine;
}

void Map::toggle_flag(int x, int y) {
    if (!in_bounds(x, y) || revealed[y][x]) return;
    flagged[y][x] = !flagged[y][x];
}

void Map::reveal_all_mines() {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (mapData[y][x] == -1) revealed[y][x] = true;
}

bool Map::check_win() const {
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            if (mapData[y][x] != -1 && !revealed[y][x]) return false;
    return true;
}
