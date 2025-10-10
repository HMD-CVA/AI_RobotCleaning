#include <iostream>      
#include <fstream>       
#include <vector>    
#include <queue>                 
#include <algorithm>            
#include <chrono>                
#include <thread>                
#include <cmath>          
#include <cstdlib>     
#include <iomanip>         
#include <utility>  
#include <climits>  
#include <cstdint>          
#include <limits>
#include <unistd.h> // for usleep (Linux/Mac)
// #include <windows.h> // for Sleep (Windows)
using namespace std;

class PerformanceTimer {
private:
    chrono::high_resolution_clock::time_point startTime;
    long long executionTime;
    bool isRunning;

public:
    PerformanceTimer() : executionTime(0), isRunning(false) {}
    
    void start() {
        startTime = chrono::high_resolution_clock::now();
        isRunning = true;
    }
    
    void stop() {
        if (isRunning) {
            auto endTime = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);
            executionTime = duration.count();
            isRunning = false;
        }
    }
    
    long long getTime() const {
        return executionTime;
    }
    
    void reset() {
        executionTime = 0;
        isRunning = false;
    }
};

class RobotCleaning{
private:
    int n, m, startID, dockID;
    vector<vector<int>> grid;
    vector<int> heuristic, dirtyNode;
    
    static const int EMPTY = 0;
    static const int OBSTACLE = 1;
    static const int DIRTY = 2;
    static const int ROBOT = 8;
    static const int DOCK = 9;
    static const int PATH = 4;
    
    PerformanceTimer timer;
    
public:
    void initData() {
        ifstream file("input.txt");
        if (!file.is_open()) {
            cout << "Cannot open input.txt" << endl;
            return;
        }
    
        file >> n >> m;
        grid.resize(n, vector<int>(m));
        heuristic.resize(n * m);
        dirtyNode.clear();
        
        for (int i=0; i<n; i++) 
            for (int j=0; j<m; j++) {
                file >> grid[i][j];
                int id = coordToID(i, j);
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
        for (int i=0; i<n; i++) {
            for (int j=0; j<m; j++) {
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
    
    // Hiển thị grid với animation
    void displayAnimatedGrid(const vector<vector<int>>& displayGrid, int currentPos, 
                           const vector<int>& remainingDirt, int step, int totalCost, 
                           const string& status = "") {
        clearScreen();
        cout << "=== ROBOT CLEANING ANIMATION ===" << endl;
        cout << "Step: " << step << " | Total Cost: " << totalCost << endl;
        if (!status.empty()) cout << "Status: " << status << endl;
        cout << "Legend: R=Robot, D=Dock, X=Obstacle, *=Dirty, .=Empty, -=Path" << endl << endl;
        
        // Hiển thị số cột (y)
        cout << "   ";
        for (int j=0; j<m; j++) cout << setw(2) << j << " ";
        cout << endl;
        
        for (int i=0; i<n; i++) {
            cout << setw(2) << i << " "; // Hiển thị số hàng (x)
            for (int j=0; j<m; j++) {
                char symbol;
                int cellValue = displayGrid[i][j];
                
                if (cellValue == ROBOT) symbol = 'R';
                else if (cellValue == DOCK) symbol = 'D';
                else if (cellValue == OBSTACLE) symbol = 'X';
                else if (cellValue == DIRTY) {
                    int id = coordToID(i, j);
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
        auto currentCoord = idToCoord(currentPos);
        cout << "\nRobot at: " << currentPos << " (" << currentCoord.first << "," << currentCoord.second << ")";
        cout << " | Remaining dirty: " << remainingDirt.size() << endl;
    }
    
    void displayGridWithSymbols() {
        displayAnimatedGrid(grid, startID, dirtyNode, 0, 0, "Current State");
        waitForEnter();
    }
    
    void modifyGrid() {
        // Lưu trạng thái gốc
        vector<vector<int>> originalGrid = grid;
        vector<int> originalDirtyNode = dirtyNode;
        int originalStartID = startID;
        int originalDockID = dockID;
        
        int choice;
        do {
            // LUÔN HIỂN THỊ GRID KHI VÀO MODIFY
            clearScreen();
            displayAnimatedGrid(grid, startID, dirtyNode, 0, 0, "Grid Modification Mode");
            
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
                        if (grid[x][y] == OBSTACLE) {
                            cout << "ERROR: Cannot place robot on obstacle!" << endl;
                        } else {
                            // Xóa robot cũ
                            auto oldCoord = idToCoord(startID);
                            grid[oldCoord.first][oldCoord.second] = EMPTY;
                            // Đặt robot mới
                            startID = coordToID(x, y);
                            grid[x][y] = ROBOT;
                            cout << "Robot moved to (" << x << "," << y << ")" << endl;
                        }
                    } else {
                        cout << "Invalid coordinates! Grid size: " << n << "x" << m << endl;
                    }
                    break;
                    
                case 2: // Thay đổi dock
                    cout << "Enter new dock coordinates (x y): ";
                    cin >> x >> y;
                    if (isValidCoord(x, y)) {
                        if (grid[x][y] == OBSTACLE) {
                            cout << "ERROR: Cannot place dock on obstacle!" << endl;
                        } else {
                            auto oldCoord = idToCoord(dockID);
                            grid[oldCoord.first][oldCoord.second] = EMPTY;
                            dockID = coordToID(x, y);
                            grid[x][y] = DOCK;
                            cout << "Dock moved to (" << x << "," << y << ")" << endl;
                        }
                    } else {
                        cout << "Invalid coordinates! Grid size: " << n << "x" << m << endl;
                    }
                    break;
                    
                case 3: // Thêm vật cản
                    cout << "Enter obstacle coordinates (x y): ";
                    cin >> x >> y;
                    if (isValidCoord(x, y)) {
                        if (grid[x][y] == ROBOT) {
                            cout << "ERROR: Cannot place obstacle on robot position!" << endl;
                        } else if (grid[x][y] == DOCK) {
                            cout << "ERROR: Cannot place obstacle on dock position!" << endl;
                        } else if (grid[x][y] == DIRTY) {
                            // Xóa khỏi dirtyNode nếu có
                            id = coordToID(x, y);
                            dirtyNode.erase(remove(dirtyNode.begin(), dirtyNode.end(), id), dirtyNode.end());
                            grid[x][y] = OBSTACLE;
                            cout << "Obstacle added at (" << x << "," << y << ")" << endl;
                        } else if (grid[x][y] == OBSTACLE) {
                            cout << "ERROR: There is already an obstacle here!" << endl;
                        } else {
                            grid[x][y] = OBSTACLE;
                            cout << "Obstacle added at (" << x << "," << y << ")" << endl;
                        }
                    } else {
                        cout << "Invalid coordinates! Grid size: " << n << "x" << m << endl;
                    }
                    break;
                    
                case 4: // Xóa vật cản
                    cout << "Enter obstacle coordinates to remove (x y): ";
                    cin >> x >> y;
                    if (isValidCoord(x, y)) {
                        if (grid[x][y] == OBSTACLE) {
                            grid[x][y] = EMPTY;
                            cout << "Obstacle removed from (" << x << "," << y << ")" << endl;
                        } else {
                            cout << "ERROR: No obstacle at this position!" << endl;
                        }
                    } else {
                        cout << "Invalid coordinates! Grid size: " << n << "x" << m << endl;
                    }
                    break;
                    
                case 5: // Thêm vết bẩn
                    cout << "Enter dirty spot coordinates (x y): ";
                    cin >> x >> y;
                    if (isValidCoord(x, y)) {
                        if (grid[x][y] == OBSTACLE) {
                            cout << "ERROR: Cannot place dirty spot on obstacle!" << endl;
                        } else if (grid[x][y] == ROBOT) {
                            cout << "ERROR: Cannot place dirty spot on robot position!" << endl;
                        } else if (grid[x][y] == DOCK) {
                            cout << "ERROR: Cannot place dirty spot on dock position!" << endl;
                        } else if (grid[x][y] == DIRTY) {
                            cout << "ERROR: There is already a dirty spot here!" << endl;
                        } else {
                            grid[x][y] = DIRTY;
                            dirtyNode.push_back(coordToID(x, y));
                            cout << "Dirty spot added at (" << x << "," << y << ")" << endl;
                        }
                    } else {
                        cout << "Invalid coordinates! Grid size: " << n << "x" << m << endl;
                    }
                    break;
                    
                case 6: // Xóa vết bẩn
                    cout << "Enter dirty spot coordinates to remove (x y): ";
                    cin >> x >> y;
                    if (isValidCoord(x, y)) {
                        if (grid[x][y] == DIRTY) {
                            grid[x][y] = EMPTY;
                            id = coordToID(x, y);
                            dirtyNode.erase(remove(dirtyNode.begin(), dirtyNode.end(), id), dirtyNode.end());
                            cout << "Dirty spot removed from (" << x << "," << y << ")" << endl;
                        } else {
                            cout << "ERROR: No dirty spot at this position!" << endl;
                        }
                    } else {
                        cout << "Invalid coordinates! Grid size: " << n << "x" << m << endl;
                    }
                    break;
                    
                case 7: // Lưu và thoát
                    saveToFile();
                    cout << "Changes saved to file!" << endl;
                    waitForEnter();
                    return;
                    
                case 8: // Thoát không lưu - KHÔI PHỤC LẠI TRẠNG THÁI GỐC
                    initData();
                    cout << "All changes discarded! Returning to original state." << endl;
                    waitForEnter();
                    return;
                    
                default:
                    cout << "Invalid choice!" << endl;
            }
            
            // HIỂN THỊ GRID SAU MỖI THAY ĐỔI
            cout << "\nPress Enter to continue..." << endl;
            clearInputBuffer();
            cin.get();
            
        } while (choice != 7 && choice != 8);
    }
    
    // Tính năng 3: Dọn dẹp với animation
    void cleanWithVisualization() {
        if (dirtyNode.empty()) {
            cout << "No dirty nodes to clean!" << endl;
            waitForEnter();
            return;
        }
        
        cout << "\n=== CLEANING VISUALIZATION ===" << endl;
        cout << "Press Enter to start animation..." << endl;
        clearInputBuffer();
        cin.get();
        
        vector<vector<int>> displayGrid = grid;
        int current = startID;
        vector<int> remainingDirt = dirtyNode;
        int totalCost = 0;
        int step = 1;
        
        displayAnimatedGrid(displayGrid, current, remainingDirt, 0, totalCost, "Ready to start");
        delay(500);
        
        // Đo thời gian thực thi thực sự (không tính animation)
        timer.start();
        
        // Tính toán đường đi thực sự
        vector<pair<vector<int>, int>> allPaths;
        vector<int> tempRemaining = remainingDirt;
        int tempCurrent = current;
        int realTotalCost = 0;
        
        while (!tempRemaining.empty()) {
            int minCost = INT_MAX;
            int nearestNode = -1;
            vector<int> bestPath;
            
            for (int dirtyId : tempRemaining) {
                auto result = findPath(tempCurrent, dirtyId);
                if (result.first != INT_MAX && result.first < minCost) {
                    minCost = result.first;
                    nearestNode = dirtyId;
                    bestPath = result.second;
                }
            }
            
            if (nearestNode == -1) break;
            
            allPaths.push_back({bestPath, minCost});
            tempCurrent = nearestNode;
            realTotalCost += minCost;
            tempRemaining.erase(remove(tempRemaining.begin(), tempRemaining.end(), nearestNode), tempRemaining.end());
        }
        
        timer.stop();
        long long executionTime = timer.getTime();
        
        // Thực hiện animation với kết quả đã tính toán
        for (const auto& pathInfo : allPaths) {
            const vector<int>& bestPath = pathInfo.first;
            int pathCost = pathInfo.second;
            
            // Animation di chuyển đến node bẩn
            for (int i=1; i<bestPath.size(); i++) {
                // Xóa robot ở vị trí cũ và đánh dấu đường đi
                auto oldCoord = idToCoord(current);
                if (displayGrid[oldCoord.first][oldCoord.second] == ROBOT) {
                    displayGrid[oldCoord.first][oldCoord.second] = PATH;
                }
                
                // Cập nhật vị trí hiện tại
                current = bestPath[i];
                auto newCoord = idToCoord(current);
                
                // Đánh dấu đường đi cho tất cả các bước đã đi qua
                for (int j=1; j<=i; j++) {
                    auto pathCoord = idToCoord(bestPath[j]);
                    if (displayGrid[pathCoord.first][pathCoord.second] == EMPTY || 
                        displayGrid[pathCoord.first][pathCoord.second] == PATH) {
                        displayGrid[pathCoord.first][pathCoord.second] = PATH;
                    }
                }
                
                // Đặt robot ở vị trí mới
                displayGrid[newCoord.first][newCoord.second] = ROBOT;
                
                displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost + i,
                                "Moving to dirty spot... (" + to_string(i) + "/" + to_string(bestPath.size()-1) + ")");
                delay(200);
            }
            
            // Xóa vết bẩn và cập nhật
            remainingDirt.erase(remove(remainingDirt.begin(), remainingDirt.end(), current), remainingDirt.end());
            totalCost += pathCost;
            
            displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, 
                            "✓ Cleaned! Remaining: " + to_string(remainingDirt.size()));
            delay(500);
            step++;
        }
        
        // QUAY VỀ DOCK SAU KHI DỌN DẸP
        if (!remainingDirt.empty()) {
            cout << "Some nodes could not be cleaned. Returning to dock..." << endl;
        } else {
            cout << "All nodes cleaned successfully! Returning to dock..." << endl;
        }
        delay(1000);
        
        // Tìm đường về dock
        auto dockPathResult = findPath(current, dockID);
        if (dockPathResult.first != INT_MAX) {
            const vector<int>& dockPath = dockPathResult.second;
            int dockPathCost = dockPathResult.first;
            
            // Animation di chuyển về dock
            for (int i=1; i<dockPath.size(); i++) {
                // Xóa robot ở vị trí cũ và đánh dấu đường đi
                auto oldCoord = idToCoord(current);
                if (displayGrid[oldCoord.first][oldCoord.second] == ROBOT) {
                    displayGrid[oldCoord.first][oldCoord.second] = PATH;
                }
                
                // Cập nhật vị trí hiện tại
                current = dockPath[i];
                auto newCoord = idToCoord(current);
                
                // Đánh dấu đường đi cho tất cả các bước đã đi qua
                for (int j=1; j<=i; j++) {
                    auto pathCoord = idToCoord(dockPath[j]);
                    if (displayGrid[pathCoord.first][pathCoord.second] == EMPTY || 
                        displayGrid[pathCoord.first][pathCoord.second] == PATH) {
                        displayGrid[pathCoord.first][pathCoord.second] = PATH;
                    }
                }
                
                // Đặt robot ở vị trí mới
                displayGrid[newCoord.first][newCoord.second] = ROBOT;
                
                displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost + i,
                                "Returning to dock... (" + to_string(i) + "/" + to_string(dockPath.size()-1) + ")");
                delay(200);
            }
            
            totalCost += dockPathCost;
        }
        
        // Hiển thị kết quả cuối cùng
        displayAnimatedGrid(displayGrid, current, remainingDirt, step, totalCost, "Mission completed!");
        delay(1000);
        
        // Hiển thị kết quả cuối
        cout << "\n=== CLEANING SUMMARY ===" << endl;
        cout << "Execution time: " << executionTime << " microseconds" << endl;
        cout << "Total steps: " << step << endl;
        cout << "Total cost: " << totalCost << endl;
        cout << "Nodes cleaned: " << (dirtyNode.size() - remainingDirt.size()) << "/" << dirtyNode.size() << endl;
        if (!remainingDirt.empty()) {
            cout << "Failed to clean: " << remainingDirt.size() << " nodes" << endl;
        }
        cout << "Robot returned to dock: " << (current == dockID ? "YES" : "NO") << endl;
        waitForEnter();
    }
    
    // Tìm đường từ A đến B
    void findPathBetweenNodes() {
        int startX, startY, goalX, goalY;
        
        cout << "=== FIND PATH BETWEEN TWO NODES ===" << endl;
        cout << "Enter START coordinates (x y): ";
        if (!(cin >> startX >> startY)) {
            cout << "ERROR: Invalid input!" << endl;
            cin.clear();
            waitForEnter();
            return;
        }
        
        if (!isValidCoord(startX, startY)) {
            cout << "ERROR: Invalid start coordinates! Grid size: " << n << "x" << m << endl;
            waitForEnter();
            return;
        }
        
        if (grid[startX][startY] == OBSTACLE) {
            cout << "ERROR: Start position is an obstacle!" << endl;
            waitForEnter();
            return;
        }
        
        cout << "Enter GOAL coordinates (x y): ";
        if (!(cin >> goalX >> goalY)) {
            cout << "ERROR: Invalid input!" << endl;
            cin.clear();
            waitForEnter();
            return;
        }
        
        if (!isValidCoord(goalX, goalY)) {
            cout << "ERROR: Invalid goal coordinates! Grid size: " << n << "x" << m << endl;
            waitForEnter();
            return;
        }
        
        if (grid[goalX][goalY] == OBSTACLE) {
            cout << "ERROR: Goal position is an obstacle!" << endl;
            waitForEnter();
            return;
        }
        
        int startId = coordToID(startX, startY);
        int goalId = coordToID(goalX, goalY);
        
        if (startId == goalId) {
            cout << "ERROR: Start and goal are the same point!" << endl;
            waitForEnter();
            return;
        }
        
        cout << "\nFinding path... Press Enter to start animation..." << endl;
        clearInputBuffer();
        cin.get();
        
        // Đo thời gian thực thi thực sự
        timer.start();
        auto result = findPath(startId, goalId);
        timer.stop();
        long long executionTime = timer.getTime();
        
        if (result.first == INT_MAX) {
            cout << "❌ NO PATH FOUND!" << endl;
            cout << "Execution time: " << executionTime << " microseconds" << endl;
        } else {
            // Animation đường đi
            vector<vector<int>> displayGrid = grid;
            for (int i=0; i<result.second.size(); i++) {
                vector<vector<int>> tempGrid = grid;
                
                for (int j=0; j<=i; j++) {
                    auto coord = idToCoord(result.second[j]);
                    if (j == 0) {
                        tempGrid[coord.first][coord.second] = ROBOT;
                    } else if (j == result.second.size() - 1) {
                        tempGrid[coord.first][coord.second] = DOCK;
                    } else {
                        tempGrid[coord.first][coord.second] = PATH;
                    }
                }
                
                auto currentCoord = idToCoord(result.second[i]);
                for (int x=0; x<n; x++) {
                    for (int y=0; y<m; y++) {
                        if (tempGrid[x][y] == ROBOT && (x != currentCoord.first || y != currentCoord.second)) {
                            tempGrid[x][y] = PATH;
                        }
                    }
                }
                tempGrid[currentCoord.first][currentCoord.second] = ROBOT;
                
                string status = "Moving... (" + to_string(i+1) + "/" + to_string(result.second.size()) + ")";
                if (i == result.second.size() - 1) status = "Destination reached!";
                
                displayAnimatedGrid(tempGrid, result.second[i], vector<int>(), i+1, i, status);
                delay(300);
            }
            
            // Hiển thị kết quả
            cout << "=== PATH FINDING RESULT ===" << endl;
            cout << "Execution time: " << executionTime << " microseconds" << endl;
            cout << "From: (" << startX << "," << startY << ") to (" << goalX << "," << goalY << ")" << endl;
            cout << "✅ PATH FOUND SUCCESSFULLY!" << endl;
            cout << "• Total cost: " << result.first << " steps" << endl;
            cout << "• Path length: " << result.second.size() << " nodes" << endl;
            cout << "• Robot's path:" << endl << "  ";
            
            for (int i=0; i<result.second.size(); i++) {
                auto coord = idToCoord(result.second[i]);
                cout << "(" << coord.first << "," << coord.second << ")";
                if (i < result.second.size() - 1) cout << " → ";
                if ((i+1) % 5 == 0 && i < result.second.size() - 1) cout << endl << "  ";
            }
            cout << endl;
        }
        
        waitForEnter();
    }
    
    // TÍNH NĂNG MỚI: So sánh heuristic
    void compareHeuristics() {
        int startX, startY, goalX, goalY;
        
        cout << "=== COMPARE HEURISTICS ===" << endl;
        cout << "Enter START coordinates (x y): ";
        if (!(cin >> startX >> startY)) {
            cout << "ERROR: Invalid input!" << endl;
            cin.clear();
            waitForEnter();
            return;
        }
        
        if (!isValidCoord(startX, startY)) {
            cout << "ERROR: Invalid start coordinates! Grid size: " << n << "x" << m << endl;
            waitForEnter();
            return;
        }
        
        if (grid[startX][startY] == OBSTACLE) {
            cout << "ERROR: Start position is an obstacle!" << endl;
            waitForEnter();
            return;
        }
        
        cout << "Enter GOAL coordinates (x y): ";
        if (!(cin >> goalX >> goalY)) {
            cout << "ERROR: Invalid input!" << endl;
            cin.clear();
            waitForEnter();
            return;
        }
        
        if (!isValidCoord(goalX, goalY)) {
            cout << "ERROR: Invalid goal coordinates! Grid size: " << n << "x" << m << endl;
            waitForEnter();
            return;
        }
        
        if (grid[goalX][goalY] == OBSTACLE) {
            cout << "ERROR: Goal position is an obstacle!" << endl;
            waitForEnter();
            return;
        }
        
        int startId = coordToID(startX, startY);
        int goalId = coordToID(goalX, goalY);
        
        if (startId == goalId) {
            cout << "ERROR: Start and goal are the same point!" << endl;
            waitForEnter();
            return;
        }
        
        cout << "\nComparing heuristics..." << endl;
        
        // KẾT QUẢ RIÊNG CHO TỪNG HEURISTIC
        pair<int, vector<int>> resultManhattan, resultEuclidean;
        long long timeManhattan, timeEuclidean;
        
        // TEST MANHATTAN
        timer.start();
        resultManhattan = findPath(startId, goalId, false);
        timer.stop();
        timeManhattan = timer.getTime();
        
        // TEST EUCLIDEAN
        timer.start();
        resultEuclidean = findPath(startId, goalId, true);
        timer.stop();
        timeEuclidean = timer.getTime();
        
        // Hiển thị kết quả
        clearScreen();
        cout << "=== HEURISTIC COMPARISON RESULTS ===" << endl;
        cout << "From: (" << startX << "," << startY << ") to (" << goalX << "," << goalY << ")" << endl;
        cout << "=====================================" << endl;
        
        // Manhattan results
        cout << "\n🔷 MANHATTAN HEURISTIC:" << endl;
        cout << "• Execution time: " << timeManhattan << " microseconds" << endl;
        if (resultManhattan.first == INT_MAX) {
            cout << "• Result: NO PATH FOUND" << endl;
        } else {
            cout << "• Path cost: " << resultManhattan.first << " steps" << endl;
            cout << "• Path length: " << resultManhattan.second.size() << " nodes" << endl;
            cout << "• Path: ";
            for (int i=0; i<resultManhattan.second.size(); i++) {
                auto coord = idToCoord(resultManhattan.second[i]);
                cout << "(" << coord.first << "," << coord.second << ")";
                if (i < resultManhattan.second.size() - 1) cout << " → ";
                if ((i+1) % 4 == 0 && i < resultManhattan.second.size() - 1) cout << endl << "     ";
            }
            cout << endl;
        }
        
        // Euclidean results
        cout << "\n🔷 EUCLIDEAN HEURISTIC:" << endl;
        cout << "• Execution time: " << timeEuclidean << " microseconds" << endl;
        if (resultEuclidean.first == INT_MAX) {
            cout << "• Result: NO PATH FOUND" << endl;
        } else {
            cout << "• Path cost: " << resultEuclidean.first << " steps" << endl;
            cout << "• Path length: " << resultEuclidean.second.size() << " nodes" << endl;
            cout << "• Path: ";
            for (int i=0; i<resultEuclidean.second.size(); i++) {
                auto coord = idToCoord(resultEuclidean.second[i]);
                cout << "(" << coord.first << "," << coord.second << ")";
                if (i < resultEuclidean.second.size() - 1) cout << " → ";
                if ((i+1) % 4 == 0 && i < resultEuclidean.second.size() - 1) cout << endl << "     ";
            }
            cout << endl;
        }
        
        // So sánh chi tiết
        cout << "\n📊 COMPARISON SUMMARY:" << endl;
        cout << "----------------------------------------" << endl;
        
        if (resultManhattan.first != INT_MAX && resultEuclidean.first != INT_MAX) {
            // Cả hai tìm thấy đường đi
            cout << "✅ Both heuristics found a path" << endl;
            cout << "• Manhattan execution time: " << timeManhattan << " μs" << endl;
            cout << "• Euclidean execution time: " << timeEuclidean << " μs" << endl;
            cout << "• Time difference: " << abs(timeManhattan - timeEuclidean) << " μs" << endl;
            cout << "• Manhattan path cost: " << resultManhattan.first << " steps" << endl;
            cout << "• Euclidean path cost: " << resultEuclidean.first << " steps" << endl;
            cout << "• Cost difference: " << abs(resultManhattan.first - resultEuclidean.first) << " steps" << endl;
            
            // So sánh hiệu suất
            cout << "\n🏆 PERFORMANCE ANALYSIS:" << endl;
            if (timeManhattan < timeEuclidean) {
                double speedup = (double)(timeEuclidean - timeManhattan) / timeEuclidean * 100;
                cout << "• Manhattan is " << fixed << setprecision(1) << speedup << "% faster" << endl;
            } else if (timeEuclidean < timeManhattan) {
                double speedup = (double)(timeManhattan - timeEuclidean) / timeManhattan * 100;
                cout << "• Euclidean is " << fixed << setprecision(1) << speedup << "% faster" << endl;
            } else {
                cout << "• Both have identical execution time" << endl;
            }
            
            if (resultManhattan.first < resultEuclidean.first) {
                cout << "• Manhattan found a shorter path by " << (resultEuclidean.first - resultManhattan.first) << " steps" << endl;
            } else if (resultEuclidean.first < resultManhattan.first) {
                cout << "• Euclidean found a shorter path by " << (resultManhattan.first - resultEuclidean.first) << " steps" << endl;
            } else {
                cout << "• Both found paths of identical length" << endl;
            }
            
            // Đánh giá tổng quan
            cout << "\n💡 RECOMMENDATION:" << endl;
            if (timeManhattan < timeEuclidean && resultManhattan.first <= resultEuclidean.first) {
                cout << "• Manhattan heuristic is BETTER for this case" << endl;
            } else if (timeEuclidean < timeManhattan && resultEuclidean.first <= resultManhattan.first) {
                cout << "• Euclidean heuristic is BETTER for this case" << endl;
            } else if (timeManhattan < timeEuclidean) {
                cout << "• Manhattan is FASTER but Euclidean might find SHORTER paths" << endl;
            } else if (timeEuclidean < timeManhattan) {
                cout << "• Euclidean is FASTER but Manhattan might find SHORTER paths" << endl;
            } else {
                cout << "• Both heuristics perform SIMILARLY for this case" << endl;
            }
            
        } else if (resultManhattan.first != INT_MAX) {
            cout << "❌ Only Manhattan found a path" << endl;
            cout << "• Euclidean failed to find a path" << endl;
            cout << "• Manhattan execution time: " << timeManhattan << " μs" << endl;
            cout << "• Manhattan path cost: " << resultManhattan.first << " steps" << endl;
            cout << "💡 Manhattan heuristic is SUPERIOR for this case" << endl;
            
        } else if (resultEuclidean.first != INT_MAX) {
            cout << "❌ Only Euclidean found a path" << endl;
            cout << "• Manhattan failed to find a path" << endl;
            cout << "• Euclidean execution time: " << timeEuclidean << " μs" << endl;
            cout << "• Euclidean path cost: " << resultEuclidean.first << " steps" << endl;
            cout << "💡 Euclidean heuristic is SUPERIOR for this case" << endl;
            
        } else {
            cout << "❌ Neither heuristic found a path" << endl;
            cout << "• Both algorithms failed to find a valid path" << endl;
            cout << "• The destination might be unreachable from the start position" << endl;
        }
        
        waitForEnter();
    }
    
    // Hàm hỗ trợ
    void waitForEnter() {
        cout << "\nPress Enter to continue...";
        clearInputBuffer();
        cin.get();
    }
    
    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    
    bool isValidCoord(int x, int y) {
        return x >= 0 && x < n && y >= 0 && y < m;
    }
    
    int coordToID(int x, int y) {
        return x * m + y;
    }
    
    pair<int, int> idToCoord(int id) {
        int x = id / m;
        int y = id % m;
        return {x, y};
    }
    
    pair<int, vector<int>> findPath(int startId, int goalId, bool useEuclidean = false) {
        if (startId < 0 || startId >= n*m || goalId < 0 || goalId >= n*m) {
            return {INT_MAX, {}};
        }
        if (startId == goalId) {
            return {0, {startId}};
        }

        int totalNode = n * m;
        vector<int> G(totalNode, INT_MAX);
        vector<int> F(totalNode, INT_MAX);
        vector<int> pre(totalNode, -1);
        vector<bool> vis(totalNode, false);
        
        auto goalCoord = idToCoord(goalId);
        int goalX = goalCoord.first;
        int goalY = goalCoord.second;

        for (int i=0; i<totalNode; i++) {
            auto coord = idToCoord(i);
            int x = coord.first;
            int y = coord.second;
            
            // Heuristic tính đúng, không scale
            if (useEuclidean) {
                int dx = x - goalX;
                int dy = y - goalY;
                heuristic[i] = static_cast<int>(sqrt(dx*dx + dy*dy));
            } else {
                heuristic[i] = abs(x - goalX) + abs(y - goalY);
            }
        }

        G[startId] = 0;
        F[startId] = heuristic[startId];

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
        pq.push({F[startId], startId});
        
        vector<pair<int, int>> directions = {
            {-1,0}, {1,0}, {0,-1}, {0,1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
        };

        while(!pq.empty()) {
            int current = pq.top().second;
            pq.pop();

            if (vis[current]) continue;
            vis[current] = true;

            if (current == goalId) break;

            auto currentCoord = idToCoord(current);
            int currentX = currentCoord.first;
            int currentY = currentCoord.second;

            for (auto& dir : directions) {
                int newX = currentX + dir.first;
                int newY = currentY + dir.second;
                if (newX >=0 && newX < n && newY >=0 && newY < m) {
                    int nextId = coordToID(newX, newY);
                    
                    if (grid[newX][newY] != OBSTACLE && !vis[nextId]) {
                        int newG = G[current] + 1;
                        if (newG < G[nextId]) {
                            G[nextId] = newG;
                            F[nextId] = G[nextId] + heuristic[nextId];
                            pre[nextId] = current;
                            pq.push({F[nextId], nextId});
                        }
                    }
                }
            }
        }

        if (G[goalId] == INT_MAX) {
            return {INT_MAX, {}};
        }

        vector<int> path;
        int current = goalId;
        while (current != -1) {
            path.push_back(current);
            current = pre[current];
        }
        reverse(path.begin(), path.end());

        return {G[goalId], path};
    }
    
    void run() {
        initData();
        int choice;
        do {
            clearScreen();
            cout << "=== ROBOT CLEANING SIMULATION ===" << endl;
            cout << "1. Display current grid" << endl;
            cout << "2. Modify grid" << endl;
            cout << "3. Clean with visualization" << endl;
            cout << "4. Find path between two nodes" << endl;
            cout << "5. Compare heuristics" << endl;
            cout << "6. Exit" << endl;
            cout << "Choose: ";
            
            if (!(cin >> choice)) {
                cout << "Invalid input! Please enter a number." << endl;
                cin.clear();
                waitForEnter();
                continue;
            }
            
            switch(choice) {
                case 1:
                    displayGridWithSymbols();
                    break;
                case 2:
                    modifyGrid();
                    break;
                case 3:
                    cleanWithVisualization();
                    break;
                case 4:
                    findPathBetweenNodes();
                    break;
                case 5:
                    compareHeuristics();
                    break;
                case 6:
                    cout << "Goodbye!" << endl;
                    break;
                default:
                    cout << "Invalid choice!" << endl;
                    waitForEnter();
            }
        } while (choice != 6);
    }
};

signed main() {
    RobotCleaning robot;
    robot.run();
    return 0;
}