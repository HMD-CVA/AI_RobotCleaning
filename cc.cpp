#include<bits/stdc++.h>
#include <unistd.h> // for usleep (Linux/Mac)
// #include <windows.h> // for Sleep (Windows)
using namespace std;

class RobotCleaning{
    private:
        int n, m, startID, dockID;
        vector<vector<int>> grid;
        vector<vector<pair<int, int>>> edge;
        vector<int> heuristic, dirtyNode;
        
        static const int EMPTY = 0;
        static const int OBSTACLE = 1;
        static const int DIRTY = 2;
        static const int ROBOT = 8;
        static const int DOCK = 9;
        static const int PATH = 4;
        
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
        
        void clearScreen() {
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
        }
        
        void delay(int milliseconds) {
            #ifdef _WIN32
                Sleep(milliseconds);
            #else
                usleep(milliseconds * 1000);
            #endif
        }
        
        // Hiển thị grid với animation đẹp
        void displayAnimatedGrid(const vector<vector<int>>& displayGrid, int currentPos, 
                               const vector<int>& remainingDirt, int step, int totalCost, 
                               const string& status = "") {
            clearScreen();
            cout << "=== ROBOT CLEANING ANIMATION ===" << endl;
            cout << "Step: " << step << " | Total Cost: " << totalCost << endl;
            if (!status.empty()) cout << "Status: " << status << endl;
            cout << "Legend: R=Robot, D=Dock, X=Obstacle, *=Dirty, .=Empty, -=Path" << endl << endl;
            
            // Hiển thị số cột
            cout << "   ";
            for (int j=0;j<m;j++) cout << setw(2) << j << " ";
            cout << endl;
            
            for (int i=0;i<n;i++) {
                cout << setw(2) << i << " ";
                for (int j=0;j<m;j++) {
                    char symbol;
                    int cellValue = displayGrid[i][j];
                    
                    if (cellValue == ROBOT) symbol = 'R';
                    else if (cellValue == DOCK) symbol = 'D';
                    else if (cellValue == OBSTACLE) symbol = 'X';
                    else if (cellValue == DIRTY) {
                        int id = coordToID(j, i, m);
                        if (find(remainingDirt.begin(), remainingDirt.end(), id) != remainingDirt.end()) {
                            symbol = '*';
                        } else {
                            symbol = '.';
                        }
                    }
                    else if (cellValue == PATH) symbol = '-';
                    else if (cellValue == EMPTY) symbol = '.';
                    else symbol = '?';
                    
                    cout << " " << symbol << " ";
                }
                cout << endl;
            }
            
            // Hiển thị thông tin
            auto currentCoord = idToCoord(currentPos, m);
            cout << "\nRobot at: " << currentPos << " (" << currentCoord.first << "," << currentCoord.second << ")";
            cout << " | Remaining dirty: " << remainingDirt.size() << endl;
        }
        
        void displayGridWithSymbols() {
            displayAnimatedGrid(grid, startID, dirtyNode, 0, 0, "Current State");
            cout << "\nPress Enter to continue..." << endl;
            cin.ignore();
            cin.get();
        }
        
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
                            if (grid[y][x] == OBSTACLE) {
                                cout << "ERROR: Cannot place robot on obstacle!" << endl;
                                break;
                            }
                            auto oldCoord = idToCoord(startID, m);
                            grid[oldCoord.second][oldCoord.first] = EMPTY;
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
                            if (grid[y][x] == OBSTACLE) {
                                cout << "ERROR: Cannot place dock on obstacle!" << endl;
                                break;
                            }
                            auto oldCoord = idToCoord(dockID, m);
                            grid[oldCoord.second][oldCoord.first] = EMPTY;
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
                displayAnimatedGrid(grid, startID, dirtyNode, 0, 0, "Grid Modified");
                cout << "\nPress Enter to continue..." << endl;
                cin.ignore();
                cin.get();
                
            } while (choice != 7 && choice != 8);
        }
        
        // Tính năng 3: Dọn dẹp với animation mượt mà
        void cleanWithVisualization() {
            if (dirtyNode.empty()) {
                cout << "No dirty nodes to clean!" << endl;
                return;
            }
            
            cout << "\n=== CLEANING VISUALIZATION ===" << endl;
            cout << "Press Enter to start animation..." << endl;
            cin.ignore();
            cin.get();
            
            vector<vector<int>> displayGrid = grid;
            int current = startID;
            vector<int> remainingDirt = dirtyNode;
            int totalCost = 0;
            int step = 1;
            
            // Hiển thị trạng thái ban đầu
            displayAnimatedGrid(displayGrid, current, remainingDirt, 0, totalCost, "Ready to start");
            delay(2000);
            
            while (!remainingDirt.empty()) {
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
                    displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, "Cannot reach remaining nodes!");
                    delay(3000);
                    break;
                }
                
                // ANIMATION: Di chuyển đến node bẩn
                displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, 
                                  "Moving to dirty node " + to_string(nearestNode));
                delay(1000);
                
                // Di chuyển từng bước với animation - SỬA LỖI 2 CHỮ R
                for (int i=1; i<bestPath.size(); i++) {
                    // Tạo grid tạm thời cho animation - LUÔN bắt đầu từ displayGrid gốc
                    vector<vector<int>> tempGrid = displayGrid;
                    
                    // Đánh dấu đường đi đã qua (không bao gồm vị trí hiện tại)
                    for (int j=1; j<i; j++) {
                        auto pathCoord = idToCoord(bestPath[j], m);
                        if (tempGrid[pathCoord.second][pathCoord.first] == EMPTY) {
                            tempGrid[pathCoord.second][pathCoord.first] = PATH;
                        }
                    }
                    
                    // QUAN TRỌNG: Đảm bảo chỉ có 1 chữ R duy nhất
                    // Xóa robot khỏi tất cả các vị trí trước đó
                    for (int y=0; y<n; y++) {
                        for (int x=0; x<m; x++) {
                            if (tempGrid[y][x] == ROBOT) {
                                tempGrid[y][x] = EMPTY;
                            }
                        }
                    }
                    
                    // Đặt robot ở vị trí mới
                    auto newCoord = idToCoord(bestPath[i], m);
                    tempGrid[newCoord.second][newCoord.first] = ROBOT;
                    
                    // Hiển thị animation
                    displayAnimatedGrid(tempGrid, bestPath[i], remainingDirt, step, totalCost + i,
                                      "Moving... (" + to_string(i) + "/" + to_string(bestPath.size()-1) + ")");
                    delay(500);
                }
                
                // Cập nhật grid chính thức sau khi hoàn thành di chuyển
                for (int i=1; i<bestPath.size()-1; i++) {
                    auto coord = idToCoord(bestPath[i], m);
                    if (displayGrid[coord.second][coord.first] == EMPTY) {
                        displayGrid[coord.second][coord.first] = PATH;
                    }
                }
                
                // Cập nhật vị trí robot và xóa vết bẩn
                auto oldCoord = idToCoord(current, m);
                displayGrid[oldCoord.second][oldCoord.first] = PATH;
                
                current = nearestNode;
                auto newCoord = idToCoord(current, m);
                displayGrid[newCoord.second][newCoord.first] = ROBOT;
                
                remainingDirt.erase(remove(remainingDirt.begin(), remainingDirt.end(), nearestNode), remainingDirt.end());
                totalCost += minCost;
                
                // Hiển thị kết quả sau khi dọn
                displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, "Cleaned node " + to_string(nearestNode) + "! Remaining: " + to_string(remainingDirt.size()));
                delay(1500);
                
                step++;
                
                if (!remainingDirt.empty()) {
                    displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, "Planning next move...");
                    delay(1000);
                }
            }
            
            // Quay về dock với animation - SỬA LỖI 2 CHỮ R
            if (remainingDirt.empty()) {
                displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, "All clean! Returning to dock...");
                delay(2000);
                
                auto dockResult = findPath(current, dockID);
                if (dockResult.first != INT_MAX) {
                    vector<int> dockPath = dockResult.second;
                    
                    // Animation di chuyển về dock
                    for (int i=1; i<dockPath.size(); i++) {
                        vector<vector<int>> tempGrid = displayGrid;
                        
                        // Đánh dấu đường đi
                        for (int j=1; j<i; j++) {
                            auto pathCoord = idToCoord(dockPath[j], m);
                            if (tempGrid[pathCoord.second][pathCoord.first] == EMPTY) {
                                tempGrid[pathCoord.second][pathCoord.first] = PATH;
                            }
                        }
                        
                        // Xóa robot khỏi tất cả các vị trí
                        for (int y=0; y<n; y++) {
                            for (int x=0; x<m; x++) {
                                if (tempGrid[y][x] == ROBOT) {
                                    tempGrid[y][x] = EMPTY;
                                }
                            }
                        }
                        
                        // Đặt robot ở vị trí mới
                        auto newCoord = idToCoord(dockPath[i], m);
                        tempGrid[newCoord.second][newCoord.first] = ROBOT;
                        
                        displayAnimatedGrid(tempGrid, dockPath[i], remainingDirt, step, totalCost + i,
                                          "Returning to dock... (" + to_string(i) + "/" + to_string(dockPath.size()-1) + ")");
                        delay(500);
                    }
                    
                    totalCost += dockResult.first;
                    
                    // Cập nhật grid cuối cùng - Đảm bảo chỉ có 1 chữ R
                    for (int y=0; y<n; y++) {
                        for (int x=0; x<m; x++) {
                            if (displayGrid[y][x] == ROBOT) {
                                displayGrid[y][x] = PATH;
                            }
                        }
                    }
                    
                    // Đặt robot ở dock
                    displayGrid[idToCoord(dockID, m).second][idToCoord(dockID, m).first] = ROBOT;
                    
                    displayAnimatedGrid(displayGrid, dockID, remainingDirt, step, totalCost, 
                                      "Mission Complete! Robot at dock.");
                } else {
                    displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, 
                                      "Cannot return to dock!");
                }
            }
            
            cout << "\nPress Enter to return to main menu..." << endl;
            cin.ignore();
            cin.get();
        }
        
    // private:
        bool isValidCoord(int x, int y) {
            return x >= 0 && x < m && y >= 0 && y < n;
        }
        
    // public:
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
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        
        cout << "\n=== ROBOT CLEANING SYSTEM ===" << endl;
        cout << "1. View current grid and robot position" << endl;
        cout << "2. Modify grid (robot, dock, obstacles, dirty spots)" << endl;
        cout << "3. Clean all dirty spots with ANIMATION" << endl;
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
                cout << "Press Enter to continue..." << endl;
                cin.ignore();
                cin.get();
        }
    } while (choice != 4);
    
    return 0;
}