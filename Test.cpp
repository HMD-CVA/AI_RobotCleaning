#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
using namespace std;

struct Cell {
	int x, y, parX = -1, parY = -1;
	int g = INT_MAX, h = INT_MAX, f = INT_MAX;
    bool isObstacle = false;
    bool isRoad = false;
};

int n, m, startX, startY, endX, endY;
int r[4] = { 0, 1, 0, -1 }, c[4] = {1, 0, -1, 0};
vector<vector<Cell>> cells;
vector<vector<bool>> closedList;
bool ok = false;
vector<pair<int, int>> openList;

int heuristicManhattan(Cell cell) {
    return abs(endX - cell.x) + abs(endY - cell.y);
}

int heuristicEuclidean(Cell cell) {
    return sqrt((endX - cell.x) * (endX - cell.x) + (endY - cell.y) * (endY - cell.y));
}

void readFile(string fn) {
    ifstream file(fn);
    if (!file.is_open()) {
        cout << "Khong mo duoc file";
        return;
    }

    file >> n >> m;

    cells.assign(n, vector<Cell>(m));
    closedList.assign(n, vector<bool>(m, false));

    for(int i=0; i<n; i++)
        for (int j = 0; j < m; j++) {
            char x;
            file >> x;
            cells[i][j].x = i;
            cells[i][j].y = j;
            if (x == '1') cells[i][j].isObstacle = true;
            else if (x == 's') {
                startX = i;
                startY = j;
            }
            else if (x == 'e') {
                endX = i;
                endY = j;
            }
        }

    cells[startX][startY].g = 0;
    cells[startX][startY].h = heuristicManhattan(cells[startX][startY]);
    cells[startX][startY].f = cells[startX][startY].h;
    openList.push_back({ startX, startY });
}

Cell getMinOpenList() {
    Cell re = cells[openList[0].first][openList[0].second];
    int index = 0;
    int k = openList.size();
    for (int i = 1; i < k; i++) {
        Cell curCell = cells[openList[i].first][openList[i].second];
        if (curCell.f < re.f || 
            (curCell.f == re.f && curCell.g < re.g)) {
            re = curCell;
            index = i;
        }
    }
    openList.erase(openList.begin() + index);
    return re;
}

bool isInOpenList(int x, int y) {
    int n = openList.size();
    for (int i = 0; i < n; i++)
        if (openList[i].first == x && openList[i].second == y) return true;
    return false;
}

void aStar() {
    while (!openList.empty()) {
        Cell curCell = getMinOpenList();
        closedList[curCell.x][curCell.y] = true;

        if (curCell.x == endX && curCell.y == endY) {
            ok = true;
            return;
        }

        for (int k = 0; k < 4; k++) {
            int i = r[k] + curCell.x, j = c[k] + curCell.y;
            if (i < 0 || j < 0 || i >= n || j >= m ||
                cells[i][j].isObstacle == true || closedList[i][j] == true)
                continue;

            cells[i][j].h = heuristicManhattan(cells[i][j]);
            int newG = cells[curCell.x][curCell.y].g + 1;
            if (cells[i][j].g > newG) {
                cells[i][j].g = newG;
                cells[i][j].f = newG + cells[i][j].h;
                cells[i][j].parX = curCell.x;
                cells[i][j].parY = curCell.y;
                if(!isInOpenList(i,j))
                    openList.push_back({ i,j });
            }
        }
    }
}

int main() {
    string fn = "map.txt";
    readFile(fn);
    aStar();
    
    if (!ok) {
        cout << "khong tim duoc duong di";
        return 0;
    }

    int i = endX, j = endY;
    while (i != startX || j != startY) {
        cells[i][j].isRoad = true;
        i = cells[i][j].parX;
        j = cells[i][j].parY;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            if (i == startX && j == startX) cout << 'S' << ' ';
            else if (i == endX && j == endX) cout << 'E' << ' ';
            else if (cells[i][j].isRoad) cout << '*' << ' ';
            else if (cells[i][j].isObstacle) cout << '1' << ' ';
            else cout << '0' << ' ';
        cout << endl;
    }

    cout << "Chi phi: " << cells[endX][endY].g;

    return 0;
}