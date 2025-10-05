#include<bits/stdc++.h>
using namespace std;

int n, H[1000], G[10001], F[10001], pre[10001], vis[10001], s, e;
vector<vector<pair<int,int>>> edge; 

signed main()
{
    freopen("input.txt", "r", stdin);
    cin >> n;

    s = 0; e = 7;

    edge.resize(n);
    for (int i=0;i<n;i++) {
        for (int j=0;j<n;j++) {
            int w; cin >> w; 
            if (w>0) edge[i].push_back({j, w});
        }
    }
    for (int i=0;i<n;i++) {
        vis[i] = -1;
        G[i] = F[i] = INT_MAX;
        pre[i] = -1;
        cin >> H[i];
    }

    G[s] = 0;
    F[s] = H[s];
    vis[s] = 1; // O = 1, C = 0

    while(true) {
        int N = -1, res = INT_MAX;
        for (int i=0;i<n;i++) { // Chon N thuoc O co F[N] min
            if (vis[i] == 1 && res > F[i]) {
                res = F[i];
                N = i;
            }
        }

        if (N == -1 || N == e) break;

        vis[N] = 0;
        for (pair<int, int> i : edge[N]) {
            int Q = i.first;
            int W = i.second;

            if (vis[Q]==1) {
                if (G[Q] > G[N] + W) {
                    G[Q] = G[N] + W;
                    F[Q] = G[Q] + H[Q];
                    pre[Q] = N;
                }
            }
            else if (vis[Q]==-1) {
                G[Q] = G[N] + W;
                F[Q] = G[Q] + H[Q];
                pre[Q] = N;
                vis[Q] = 1;
            }
        }
    }
    cout << G[e] << "\n";

    int tmp = e;
    do {
        cout << tmp << " ";
        tmp = pre[tmp];
    } while(pre[tmp]!=-1);
    cout << s;
}




// // Định nghĩa 1 ô trong ma trận đường đi
// class Cells{
//     private:
//         int x, y, cost;         // Toạ độ
//         bool isObstactle; // Chướng ngại vật
//         bool hasDirt;     // Chỗ cần lau
//     public:
//         int getOx() {
//             return x;
//         }
//         int getOy() {
//             return y;
//         }
//         int getCost() {
//             return cost;
//         }
//         bool getObstactle() {
//             return isObstactle;
//         }
//         bool getDirty() {
//             return hasDirt;
//         }
//         void setToaDoCost(int _x, int _y, int _cost) {
//             x = _x;
//             y = _y;
//             cost = _cost;
//             return;
//         }
//         void setObstactle(bool _obsactle) {
//             isObstactle = _obsactle;
//             return;
//         }
//         void setDirty(bool _dirt) {
//             hasDirt = _dirt;
//             return;
//         }
//         Cells(int _x = 0, int _y = 0, int _cost = 0, bool _obstactle = false, bool _dirt = false) {
//             x = _x;
//             y = _y;
//             cost = _cost;
//             isObstactle = _obstactle;
//             hasDirt = _dirt;
//         } 
//         // bool Cells:: operator == (Cells &other) {
//         //     return this->x == other.x && this->y == other.y;
//         // } 
// };

// class CleaningRobot{
//     private:
//         vector<vector<Cells>> grid; // Lưới toạ độ
//         int width, height;
//         Cells startCell;
//         vector<Cells> dirtyCells;
//     public:
//         void setWH (int _we, int _he) {
//             width = _we;
//             height = _he;
//             return;
//         }
//         bool isValid(int x, int y) {
//             return x >= 0 && x < width && y >= 0 && y < height;
//         }
//         CleaningRobot(int _width, int _height) { // Khởi tạo ma trận
//             width = _width;
//             height = _height;
//             grid.resize(height, vector<Cells>(width));
//             for (int i=0;i<width;i++)
//                 for (int j=0;j<height;j++)
//                     grid[i][j] = Cells(i, j, 0, false, false);
//         }
//         void setStart(int x, int y) {
//             if (isValid(x, y)) startCell = grid[x][y];
//         }
//         void setCost(int x, int y, int cost) {
//             if (!isValid(x, y)) return;
//             grid[x][y].setToaDoCost(x, y, cost);
//         }
//         void setObstactle(int x, int y) {
//             if (isValid(x, y)) grid[x][y].setObstactle(true);
//         }
//         void setDirt(int x, int y) {
//             if (isValid(x, y)) {
//                 grid[y][x].setDirty(true);
//                 dirtyCells.push_back(grid[y][x]);
//             }
//         }
//         double heuristic(Cells a, Cells b) { // h(n): Ước lượng khoảng cách từ n đến điểm đích
//             return abs(a.getOx() - b.getOx()) + abs(a.getOy() - b.getOy());
//         }
//         vector<Cells> findPath(Cells start, Cells goal) {
//             if (!isValid(start.getOx(), start.getOy()) || !isValid(goal.getOx(), goal.getOy())) return {};
//             vector<int> Close, Open, gS(width, INT_MAX), hS, fS(width, INT_MAX);
//             int startE = start.getOx();
//             int endE = goal.getOx();
//             Open.push_back(startE);
//             gS[startE] = 0;
//             fS[startE] = hS[startE];
//             while (!Open.empty()) {
//                 int fN = INT_MAX, N = -1;
//                 for (int i:Open) { // Chọn N thuộc O có f(N) nhỏ nhất
//                     if (fN > fS[i]) {
//                         fN = fS[i];
//                         N = i;
//                     }
//                 }
//                 if (N==endE) {
//                     break;
//                 }
//                 Open.erase(remove(Open.begin(), Open.end(), N), Open.end());
//                 Close.push_back(N);
//             }
//         }
//         void displayGrid() {
//             cout << "\nBản đồ:" << endl;
//             for (int y = 0; y < height; y++) {
//                 for (int x = 0; x < width; x++) {
//                     // if (grid[y][x].getOx() == startCell.getOx() && grid[y][x].getOy() == startCell.getOy()) {
//                     //     cout << "S "; // Start
//                     // } else if (grid[y][x].getObstactle()) {
//                     //     cout << "X "; // Vật cản
//                     // } else if (grid[y][x].getDirty()) {
//                     //     cout << "D "; // Vết bẩn
//                     // } else {
//                     //    cout << ". "; // Ô trống
//                     //}
//                     cout << grid[x][y].getCost() << " " ;
//                 }
//                 cout << endl;
//             }
//         }
// };

// signed main()
// {
//     freopen("input.txt", "r", stdin);
//     int n; cin >> n;
//     int h[n];
//     CleaningRobot rb(n, n);
//     for (int i=0;i<n;i++) {
//         for (int j=0;j<n;j++) {
//             int _cost;
//             cin >> _cost;
//             rb.setCost(i, j, _cost);
//         }
//     }
//     rb.displayGrid();
// }
