#include<bits/stdc++.h>
using namespace std;

class RobotCleaning{
    private:
        int n, m, startID, dockID;
        vector<vector<int>> grid;
        vector<vector<pair<int, int>>> edge;
        vector<int> heuristic, dirtyNode;
        
        // Định nghĩa các hằng số là static const
        static const int EMPTY = 0;
        static const int OBSTACLE = 1;
        static const int DIRTY = 2;
        static const int ROBOT = 8;
        static const int DOCK = 9;
        static const int PATH = 3;
        
    public:
        void initData() {
            ifstream file("input.txt");
            if (!file.is_open()) {
                cout << "Cannot open input.txt" << endl;
                return;
            }
        
            file >> n >> m;
            grid.resize(n, vector<int> (m));
            heuristic.resize(n * m);
            dirtyNode.clear();
            
            for (int i=0;i<n;i++) 
                for (int j=0;j<m;j++) {
                    file >> grid[i][j];
                    int id = coordToID(j, i, m);
                    if (grid[i][j] == ROBOT) startID = id;
                    if (grid[i][j] == DOCK) dockID = id;
                    if (grid[i][j] == DIRTY) dirtyNode.push_back(id);
                }

            file.close();
            cout << "Robot initialized with grid " << n << "x" << m << endl;     
        }
        
        void saveToFile() {
            ofstream file("input.txt");
            if (!file.is_open()) {
                cout << "Cannot save to input.txt" << endl;
                return;
            }
            
            file << n << " " << m << endl;
            for (int i=0;i<n;i++) {
                for (int j=0;j<m;j++) {
                    file << grid[i][j] << " ";
                }
                file << endl;
            }
            file.close();
            cout << "Grid saved to input.txt" << endl;
        }
        
        // Tính năng 1: Hiển thị grid với ký hiệu đẹp
        void displayGridWithSymbols() {
            cout << "\n=== CURRENT GRID ===" << endl;
            cout << "Legend: R=Robot, D=Dock, X=Obstacle, *=Dirty, .=Empty, o=Path" << endl << endl;
            
            // Hiển thị số cột
            cout << "   ";
            for (int j=0;j<m;j++) cout << setw(2) << j << " ";
            cout << endl;
            
            for (int i=0;i<n;i++) {
                cout << setw(2) << i << " "; // Số hàng
                for (int j=0;j<m;j++) {
                    char symbol;
                    int cellValue = grid[i][j];
                    
                    // Dùng if-else thay vì switch-case
                    if (cellValue == ROBOT) symbol = 'R';
                    else if (cellValue == DOCK) symbol = 'D';
                    else if (cellValue == OBSTACLE) symbol = 'X';
                    else if (cellValue == DIRTY) symbol = '*';
                    else if (cellValue == PATH) symbol = 'o';
                    else if (cellValue == EMPTY) symbol = '.';
                    else symbol = '?';
                    
                    cout << " " << symbol << " ";
                }
                cout << endl;
            }
            
            // Hiển thị thông tin
            auto robotCoord = idToCoord(startID, m);
            auto dockCoord = idToCoord(dockID, m);
            cout << "\nRobot position: " << startID << " (" << robotCoord.first << "," << robotCoord.second << ")";
            cout << "\nDock position: " << dockID << " (" << dockCoord.first << "," << dockCoord.second << ")";
            cout << "\nDirty nodes: " << dirtyNode.size() << endl;
        }
        
        // Tính năng 2: Thay đổi grid
        void modifyGrid() {
            int choice;
            do {
                cout << "\n=== MODIFY GRID ===" << endl;
                cout << "1. Change robot position" << endl;
                cout << "2. Change dock position" << endl;
                cout << "3. Add obstacle" << endl;
                cout << "4. Remove obstacle" << endl;
                cout << "5. Add dirty spot" << endl;
                cout << "6. Remove dirty spot" << endl;
                cout << "7. Save changes and exit" << endl;
                cout << "8. Exit without saving" << endl;
                cout << "Choose: ";
                cin >> choice;
                
                int x, y, id;
                switch(choice) {
                    case 1: // Thay đổi robot
                        cout << "Enter new robot coordinates (x y): ";
                        cin >> x >> y;
                        if (isValidCoord(x, y)) {
                            // KIỂM TRA: Không cho đặt robot lên vật cản
                            if (grid[y][x] == OBSTACLE) {
                                cout << "ERROR: Cannot place robot on obstacle!" << endl;
                                break;
                            }
                            
                            // Xóa vị trí robot cũ
                            auto oldCoord = idToCoord(startID, m);
                            grid[oldCoord.second][oldCoord.first] = EMPTY;
                            
                            // Đặt robot mới
                            startID = coordToID(x, y, m);
                            grid[y][x] = ROBOT;
                            cout << "Robot moved to (" << x << "," << y << ")" << endl;
                        } else {
                            cout << "Invalid coordinates!" << endl;
                        }
                        break;
                        
                    case 2: // Thay đổi dock
                        cout << "Enter new dock coordinates (x y): ";
                        cin >> x >> y;
                        if (isValidCoord(x, y)) {
                            // KIỂM TRA: Không cho đặt dock lên vật cản
                            if (grid[y][x] == OBSTACLE) {
                                cout << "ERROR: Cannot place dock on obstacle!" << endl;
                                break;
                            }
                            
                            // Xóa dock cũ
                            auto oldCoord = idToCoord(dockID, m);
                            grid[oldCoord.second][oldCoord.first] = EMPTY;
                            
                            // Đặt dock mới
                            dockID = coordToID(x, y, m);
                            grid[y][x] = DOCK;
                            cout << "Dock moved to (" << x << "," << y << ")" << endl;
                        } else {
                            cout << "Invalid coordinates!" << endl;
                        }
                        break;
                        
                    case 3: // Thêm vật cản
                        cout << "Enter obstacle coordinates (x y): ";
                        cin >> x >> y;
                        if (isValidCoord(x, y)) {
                            // KIỂM TRA: Không cho đặt vật cản lên robot, dock, hoặc vết bẩn
                            if (grid[y][x] == ROBOT) {
                                cout << "ERROR: Cannot place obstacle on robot position!" << endl;
                            } else if (grid[y][x] == DOCK) {
                                cout << "ERROR: Cannot place obstacle on dock position!" << endl;
                            } else if (grid[y][x] == DIRTY) {
                                cout << "ERROR: Cannot place obstacle on dirty spot!" << endl;
                            } else if (grid[y][x] == OBSTACLE) {
                                cout << "ERROR: There is already an obstacle here!" << endl;
                            } else {
                                grid[y][x] = OBSTACLE;
                                cout << "Obstacle added at (" << x << "," << y << ")" << endl;
                            }
                        } else {
                            cout << "Invalid coordinates!" << endl;
                        }
                        break;
                        
                    case 4: // Xóa vật cản
                        cout << "Enter obstacle coordinates to remove (x y): ";
                        cin >> x >> y;
                        if (isValidCoord(x, y)) {
                            if (grid[y][x] == OBSTACLE) {
                                grid[y][x] = EMPTY;
                                cout << "Obstacle removed from (" << x << "," << y << ")" << endl;
                            } else {
                                cout << "ERROR: No obstacle at this position!" << endl;
                            }
                        } else {
                            cout << "Invalid coordinates!" << endl;
                        }
                        break;
                        
                    case 5: // Thêm vết bẩn
                        cout << "Enter dirty spot coordinates (x y): ";
                        cin >> x >> y;
                        if (isValidCoord(x, y)) {
                            // KIỂM TRA: Không cho đặt vết bẩn lên vật cản, robot, hoặc dock
                            if (grid[y][x] == OBSTACLE) {
                                cout << "ERROR: Cannot place dirty spot on obstacle!" << endl;
                            } else if (grid[y][x] == ROBOT) {
                                cout << "ERROR: Cannot place dirty spot on robot position!" << endl;
                            } else if (grid[y][x] == DOCK) {
                                cout << "ERROR: Cannot place dirty spot on dock position!" << endl;
                            } else if (grid[y][x] == DIRTY) {
                                cout << "ERROR: There is already a dirty spot here!" << endl;
                            } else {
                                grid[y][x] = DIRTY;
                                dirtyNode.push_back(coordToID(x, y, m));
                                cout << "Dirty spot added at (" << x << "," << y << ")" << endl;
                            }
                        } else {
                            cout << "Invalid coordinates!" << endl;
                        }
                        break;
                        
                    case 6: // Xóa vết bẩn
                        cout << "Enter dirty spot coordinates to remove (x y): ";
                        cin >> x >> y;
                        if (isValidCoord(x, y)) {
                            if (grid[y][x] == DIRTY) {
                                grid[y][x] = EMPTY;
                                id = coordToID(x, y, m);
                                dirtyNode.erase(remove(dirtyNode.begin(), dirtyNode.end(), id), dirtyNode.end());
                                cout << "Dirty spot removed from (" << x << "," << y << ")" << endl;
                            } else {
                                cout << "ERROR: No dirty spot at this position!" << endl;
                            }
                        } else {
                            cout << "Invalid coordinates!" << endl;
                        }
                        break;
                        
                    case 7: // Lưu và thoát
                        saveToFile();
                        cout << "Changes saved!" << endl;
                        return;
                        
                    case 8: // Thoát không lưu
                        cout << "Changes discarded!" << endl;
                        return;
                        
                    default:
                        cout << "Invalid choice!" << endl;
                }
                
                // Hiển thị grid sau mỗi thay đổi
                displayGridWithSymbols();
                
            } while (choice != 7 && choice != 8);
        }
        
        // Tính năng 3: Dọn dẹp với hiển thị trực quan
        void cleanWithVisualization() {
            if (dirtyNode.empty()) {
                cout << "No dirty nodes to clean!" << endl;
                return;
            }
            
            cout << "\n=== CLEANING VISUALIZATION ===" << endl;
            
            // Tạo bản sao của grid để hiển thị
            vector<vector<int>> displayGrid = grid;
            int current = startID;
            vector<int> remainingDirt = dirtyNode;
            int totalCost = 0;
            int step = 1;
            
            // Hiển thị trạng thái ban đầu
            cout << "\nInitial state:" << endl;
            displayVisualGrid(displayGrid, current, remainingDirt);
            
            while (!remainingDirt.empty()) {
                cout << "\n--- Step " << step++ << " ---" << endl;
                
                // Tìm node gần nhất
                int minCost = INT_MAX;
                int nearestNode = -1;
                vector<int> bestPath;
                
                for (int dirtyId : remainingDirt) {
                    auto result = findPath(current, dirtyId);
                    if (result.first != INT_MAX && result.first < minCost) {
                        minCost = result.first;
                        nearestNode = dirtyId;
                        bestPath = result.second;
                    }
                }
                
                if (nearestNode == -1) {
                    cout << "Cannot reach any remaining dirty nodes!" << endl;
                    break;
                }
                
                // Hiển thị đường đi
                cout << "Moving to dirty node " << nearestNode << " (Cost: " << minCost << ")" << endl;
                cout << "Path: ";
                for (int i=0; i<bestPath.size(); i++) {
                    cout << bestPath[i];
                    if (i != bestPath.size()-1) cout << " -> ";
                }
                cout << endl;
                
                // Cập nhật display grid với đường đi (bỏ qua điểm đầu và cuối)
                for (int i=1; i<bestPath.size()-1; i++) {
                    auto coord = idToCoord(bestPath[i], m);
                    if (displayGrid[coord.second][coord.first] == EMPTY) {
                        displayGrid[coord.second][coord.first] = PATH;
                    }
                }
                
                // Di chuyển robot
                auto oldCoord = idToCoord(current, m);
                displayGrid[oldCoord.second][oldCoord.first] = EMPTY; // Xóa robot cũ
                
                current = nearestNode;
                auto newCoord = idToCoord(current, m);
                displayGrid[newCoord.second][newCoord.first] = ROBOT; // Đặt robot mới
                
                // Xóa vết bẩn
                displayGrid[newCoord.second][newCoord.first] = ROBOT; // Robot đứng trên vết bẩn đã dọn
                remainingDirt.erase(remove(remainingDirt.begin(), remainingDirt.end(), nearestNode), remainingDirt.end());
                
                totalCost += minCost;
                
                // Hiển thị grid sau bước di chuyển
                displayVisualGrid(displayGrid, current, remainingDirt);
                cout << "Total cost so far: " << totalCost << endl;
            }
            
            // Quay về dock
            cout << "\n--- Returning to dock ---" << endl;
            auto dockPath = findPath(current, dockID);
            if (dockPath.first != INT_MAX) {
                // Hiển thị đường về dock
                for (int i=1; i<dockPath.second.size()-1; i++) {
                    auto coord = idToCoord(dockPath.second[i], m);
                    if (displayGrid[coord.second][coord.first] == EMPTY) {
                        displayGrid[coord.second][coord.first] = PATH;
                    }
                }
                
                // Di chuyển robot về dock
                auto oldCoord = idToCoord(current, m);
                displayGrid[oldCoord.second][oldCoord.first] = EMPTY;
                displayGrid[idToCoord(dockID, m).second][idToCoord(dockID, m).first] = ROBOT;
                
                totalCost += dockPath.first;
                
                cout << "Final state (robot at dock):" << endl;
                displayVisualGrid(displayGrid, dockID, remainingDirt);
                cout << "Total cleaning cost including return: " << totalCost << endl;
            }
            
            cout << "\n=== CLEANING COMPLETED ===" << endl;
        }
        
    private:
        bool isValidCoord(int x, int y) {
            return x >= 0 && x < m && y >= 0 && y < n;
        }
        
        void displayVisualGrid(const vector<vector<int>>& visualGrid, int currentPos, const vector<int>& remainingDirt) {
            cout << "   ";
            for (int j=0;j<m;j++) cout << setw(2) << j << " ";
            cout << endl;
            
            for (int i=0;i<n;i++) {
                cout << setw(2) << i << " ";
                for (int j=0;j<m;j++) {
                    char symbol;
                    int cellValue = visualGrid[i][j];
                    
                    // Dùng if-else thay vì switch-case
                    if (cellValue == ROBOT) symbol = 'R';
                    else if (cellValue == DOCK) symbol = 'D';
                    else if (cellValue == OBSTACLE) symbol = 'X';
                    else if (cellValue == DIRTY) {
                        // Kiểm tra xem vết bẩn này còn không
                        int id = coordToID(j, i, m);
                        if (find(remainingDirt.begin(), remainingDirt.end(), id) != remainingDirt.end()) {
                            symbol = '*';
                        } else {
                            symbol = '.'; // Đã dọn
                        }
                    }
                    else if (cellValue == PATH) symbol = 'o';
                    else if (cellValue == EMPTY) symbol = '.';
                    else symbol = '?';
                    
                    cout << " " << symbol << " ";
                }
                cout << endl;
            }
        }
        
    public:
        // Các hàm cũ giữ nguyên
        int getStartID() { return startID; } 
        void setStartID(int _id) { startID = _id; }
        void setDockID(int _id) { dockID = _id; }
        vector<int> getDirtyNode() { return dirtyNode; }
        int getDockID() { return dockID; }
        
        RobotCleaning() {
            n = 0; m = 0; startID = -1; dockID = -1;
        }
        
        int coordToID(int x, int y, int cols) {
            return y*cols + x;
        }
        
        pair<int,int> idToCoord(int id, int cols) {
            int y = id / cols;
            int x = id % cols;
            return {x, y};
        }
        
        void displayGrid() {
            for (int i=0;i<n;i++) {
                for (int j=0;j<m;j++) {
                    cout << grid[i][j] << " ";
                }
                cout << "\n";
            }
        }
        
        pair<int , vector<int>> findPath(int startId, int goalId) {
            if (startId < 0 || startId >= n*m || goalId < 0 || goalId >= n*m) {
                return {INT_MAX, {}};
            }
            if (startId == goalId) {
                return {0, {startId}};
            }

            int totalNode = n*m;
            vector<int> G(totalNode, INT_MAX);
            vector<int> F(totalNode, INT_MAX);
            vector<int> pre(totalNode, -1);
            vector<bool> vis(totalNode, false);
            
            // Heuristic
            int goalX = goalId % m;
            int goalY = goalId / m;
            for (int i = 0; i < totalNode; i++) {
                int x = i % m;
                int y = i / m;
                heuristic[i] = abs(x - goalX) + abs(y - goalY);
            }

            G[startId] = 0;
            F[startId] = heuristic[startId];

            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
            pq.push({F[startId], startId});
            vector<pair<int, int>> directions = {
                {-1, 0}, {1, 0}, {0, -1}, {0, 1},
                {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
            };

            while(!pq.empty()) {
                int current = pq.top().second;
                pq.pop();

                if (vis[current]) continue;
                vis[current] = true;

                if (current == goalId) break;

                int currentX = current % m;
                int currentY = current / m;

                for (auto& dir : directions) {
                    int newX = currentX + dir.first;
                    int newY = currentY + dir.second;
                    if (newX >= 0 && newX < m && newY >= 0 && newY < n) {
                        int nextId = newY * m + newX;
                        
                        if (grid[newY][newX] != OBSTACLE && !vis[nextId]) {
                            int cost = 1;
                            int newG = G[current] + cost;
                            
                            if (newG < G[nextId]) {
                                G[nextId] = newG;
                                pre[nextId] = current;
                                int newF = newG + heuristic[nextId];
                                pq.push({newF, nextId});
                            }
                        }
                    }
                }
            }

            vector<int> path;
            int node = goalId;
            while (node != -1) {
                path.push_back(node);
                node = pre[node];
            }
            reverse(path.begin(), path.end());
            return {G[goalId], path};
        }
};

signed main() {
    RobotCleaning rb;
    rb.initData();
    
    int choice;
    do {
        cout << "\n=== ROBOT CLEANING SYSTEM ===" << endl;
        cout << "1. View current grid and robot position" << endl;
        cout << "2. Modify grid (robot, dock, obstacles, dirty spots)" << endl;
        cout << "3. Clean all dirty spots with visualization" << endl;
        cout << "4. Exit" << endl;
        cout << "Choose: ";
        cin >> choice;
        
        switch(choice) {
            case 1:
                rb.displayGridWithSymbols();
                break;
            case 2:
                rb.modifyGrid();
                break;
            case 3:
                rb.cleanWithVisualization();
                break;
            case 4:
                cout << "Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 4);
    
    return 0;
}