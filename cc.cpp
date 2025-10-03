#include<bits/stdc++.h>
using namespace std;

// Định nghĩa 1 ô trong ma trận đường đi
class Cells{
    private:
        int x, y;         // Toạ độ
        bool isObstactle; // Chướng ngại vật
        bool hasDirt;     // Chỗ cần lau
    public:
        int getOx(){
            return x;
        }
        int getOy(){
            return y;
        }
        void setToaDo(int _x, int _y) {
            x = _x;
            y = _y;
            return;
        }
        void setObstactle(bool _obsactle) {
            isObstactle = _obsactle;
            return;
        }
        void setDirty(bool _dirt) {
            hasDirt = _dirt;
            return;
        }
        Cells(int _x = 0, int _y = 0, bool _obstactle = false, bool _dirt = false) {
            x = _x;
            y = _y;
            isObstactle = _obstactle;
            hasDirt = _dirt;
        } 
        bool Cells:: operator == (Cells &other) {
            return this->x == other.x && this->y == other.y;
        } 
};

class CleaningRobot{
    private:
        vector<vector<Cells>> grid; // Lưới toạ độ
        int width, height;
        Cells startCell;
        vector<Cells> dirtyCells;
    public:
        bool isValid(int x, int y) {
            return x >= 0 && x < width && y >= 0 && y < height;
        }
        CleaningRobot(int _width, int _height) { // Khởi tạo ma trận
            width = _width;
            height = _height;
            grid.resize(height, vector<Cells>(width));
            for (int i=0;i<width;i++)
                for (int j=0;j<height;j++)
                    grid[i][j] = Cells(i, j, false, false);
        }
        void setStart(int x, int y) {
            if (isValid(x, y)) startCell = grid[x][y];
        }
        void setObstactle(int x, int y) {
            if (isValid(x, y)) grid[x][y].setObstactle(true);
        }
        void setDirt(int x, int y) {
            if (isValid(x, y)) {
                grid[y][x].setDirty(true);
                dirtyCells.push_back(grid[y][x]);
            }
        }
        double heuristic(Cells a, Cells b) { // h(n): Ước lượng khoảng cách từ n đến điểm đích
            return abs(a.getOx() - b.getOx()) + abs(a.getOy() - b.getOy());
        }
        vector<Cells> findPath(Cells start, Cells goal) {
            if (!isValid(start.getOx(), start.getOy()) || !isValid(goal.getOx(), goal.getOy())) return {};
            
        }
};
