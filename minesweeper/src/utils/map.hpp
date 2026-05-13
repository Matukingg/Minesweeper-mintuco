#pragma once
#include <iostream>
#include <vector>
#include <string>

class Map {
private:
    std::vector<std::vector<int>> mapData;
public:
    Map(const int width, const int height){
        mapData.resize(height, std::vector<int>(width, 0));
    };
    void generateMap(const int mines);

    inline void setTile(const int x, const int y, const int value) {
        if (y >= 0 && y < mapData.size() && x >= 0 && x < mapData[0].size()) {mapData[y][x] = value;}};

    inline void viewMap() const {
        for (const auto& row : mapData) {
            for (const auto& tile : row) {
                if (tile==-1){
                    std::cout << "M ";
                } else if (tile == 0) {
                    std::cout << ". ";
                } else {
                    std::cout << tile << " ";
                }
            }
            std::cout << std::endl;
        }
    };
};