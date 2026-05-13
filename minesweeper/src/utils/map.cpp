#include "map.hpp"
#include <random>
void Map::generateMap(const int mines){
    int rows = mapData.size();
    int cols = mapData[0].size();
    if (mines > rows * cols) {
        std::cerr << "Error: Number of mines exceeds available tiles." << std::endl;
        return;
    }
    std::vector<std::vector<bool>> hasMine(rows, std::vector<bool>(cols, false));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rowDist(0, rows - 1);
    std::uniform_int_distribution<> colDist(0, cols - 1);

    int placed = 0;
    while (placed < mines) {
        int r = rowDist(gen);
        int c = colDist(gen);
        if (!hasMine[r][c]) {
            hasMine[r][c] = true;
            ++placed;
        }
    }

    switch (0){
    case 0:
        for(int y=0; y<rows; y++){
            for(int x=0; x<cols; x++){
                if(hasMine[y][x]){
                    setTile(x,y,-1);
                }
            }
        }
    case 1:
        for(int y=0; y<rows; y++){
            for(int x=0; x<cols; x++){
                if(hasMine[y][x]) continue;
                int mineCount=0;
                for(int dy=-1; dy<=1; dy++){
                    for(int dx=-1; dx<=1; dx++){
                        if(dy == 0 && dx == 0) continue;
                        int nx = x + dx;
                        int ny = y + dy;
                        if(nx >= 0 && nx < cols && ny >= 0 && ny < rows && hasMine[ny][nx]){
                            mineCount++;
                        }
                    }
                }
                setTile(x, y, mineCount);
            }
            
        }
    }
}