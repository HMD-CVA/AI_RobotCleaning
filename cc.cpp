#include<bits/stdc++.h>
using namespace std;
class RobotCleaning{
    private:
        int n, m, startID, dockID;
        vector<vector<int>> grid;
        vector<vector<pair<int, int>>> edge;
        vector<int> heuristic, dirtyNode;
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
            for (int i=0;i<n;i++) 
                for (int j=0;j<m;j++) {
                    file >> grid[i][j];
                    int id = coordToID(j, i, m);
                    if (grid[i][j] == 8) startID = id;
                    if (grid[i][j] == 9) dockID = id;
                    if (grid[i][j] == 2) dirtyNode.push_back(id);
                }

            // updateEdge();
            file.close();
            cout << "Robot initialized with " << n << " nodes" << endl;     
        }  
        int getStartID() {
            return startID;
        } 
        vector<int> getDirtyNode() {
            return dirtyNode;
        }
        int getDockID() {
            return dockID;
        }
        RobotCleaning() {
            n = 0;
            m = 0;
            startID = -1;
            dockID = -1;
        }
        RobotCleaning(int _n, vector<vector<pair<int, int>>> _edge, vector<int> _H){
            n = _n;
            edge = _edge;
            heuristic = _H;
            cout << "Robot initialized with " << n << " nodes" << endl;
        }
        void updateEdge() {
            edge.resize(n);
           
            for (int i=0;i<n;i++) {
                for (int j=0;j<m;j++) {
                    if (grid[i][j] > 0 && grid[i][j]!=INT_MAX) edge[i].push_back({j, grid[i][j]});
                }
            }
        }
        void setObstactle(int x, int y) {
            grid[x][y] = INT_MAX;
            updateEdge();
        }
        void setDirty(int x, int y) {
            grid[x][y] = -1;
            return;
        }
        int coordToID(int x, int y, int cols) {
            return y*cols + x;
        }
        pair<int,int> idToCoord(int id, int cols) {
            int y = id / cols;
            int x = id % cols;
            return {x, y};
        }
        void displayEdgeInfo() {
            cout << "\nEdge Information:" << endl;
            for (int i = 0; i < n; i++) {
                if (!edge[i].empty()) {
                    cout << "Node " << i << " -> ";
                    for (pair<int, int> neighbor : edge[i]) {
                        cout << neighbor.first << "(cost:" << neighbor.second << ") ";
                    }
                    cout << endl;
                }
            }
        }
        void displayGrid() {
            for (int i=0;i<n;i++) {
                for (int j=0;j<m;j++) {
                    // if (grid[i][j] == 1) cout << "C"; 
                    // else if (grid[i][j] == 2) cout << "D";
                    // else 
                    cout << grid[i][j];
                    cout << " ";
                }
                cout << "\n";
            }
        }
        pair<int , vector<int>> findPath(int startId, int goalId) {
            // cout << "Finding path: " << startId << " -> " << goalId << endl;
            if (startId < 0 || startId >= n*m || goalId < 0 || goalId >= n*m) {
                cout << "Invalid start or goal ID!" << endl;
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
                {-1, 0},   // trái
                {1, 0},    // phải  
                {0, -1},   // lên
                {0, 1},    // xuống
                {-1, -1},  // trái-lên
                {-1, 1},   // trái-xuống
                {1, -1},   // phải-lên
                {1, 1}     // phải-xuống
            };

            while(!pq.empty()) {
                int current = pq.top().second;
                int currentF = pq.top().first;
                pq.pop();

                if (vis[current]) continue;
                vis[current] = true;

                if (current == goalId) break;

                int currentX = current % m;
                int currentY = current / m;

                for (pair<int, int> it:directions) {
                    int newX = currentX + it.first;
                    int newY = currentY + it.second;
                    if (newX >= 0 && newX < m && newY >= 0 && newY < n) {
                        int nextId = newY * m + newX;
                        
                        if (grid[newY][newX] != 1 && !vis[nextId]) { // (grid[newY][newX] == 0 || grid[newY][newX] == 2)
                            int cost = 1; // Cost mặc định
                            int newG = G[current] + cost;
                            
                            if (newG < G[nextId]) {
                                G[nextId] = newG;
                                pre[nextId] = current;
                                int newF = newG + heuristic[nextId];
                                F[nextId] = newF;
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
        // void cleanAllDirty(vector<int> dirtyNodes, int startId) {
        //     if (dirtyNodes.empty()) {
        //         cout << "No dirty nodes!" << endl;
        //         return;
        //     }
        //     int current = startId;
        //     vector<int> remainingDirt = dirtyNodes;
        //     int totalCost = 0;
            
        //     while (!remainingDirt.empty()) {
        //         cout << current << " ";
        //         int minCost = INT_MAX, minNode = -1;
        //         vector<int> minPath;
        //         for (int i=0;i<remainingDirt.size();i++) {
        //             pair<int, vector<int>> res = findPath(current, remainingDirt[i]);
        //             if (minCost > res.first) {
        //                 minCost = res.first;
        //                 minNode = remainingDirt[i];
        //                 minPath = res.second;
        //             }
        //         }
        //         if (minNode == -1) break;

        //         cout << "Move to node " << minNode << " (Cost: " << minCost << ")" << endl;
        //         cout << "Path: ";
        //         for (int node : minPath) cout << node << " ";
        //             cout << endl;
                
        //         totalCost += minCost;
        //         current = minNode;
        //         remainingDirt.erase(
        //             remove(remainingDirt.begin(), remainingDirt.end(), minNode),
        //             remainingDirt.end()
        //         );
        //         cout << "Cleaned node " << minNode << ". Remaining: " << remainingDirt.size() << " nodes" << endl;
        //     }
        //     cout << "Total: " << totalCost ;
        // }

        void cleanAllDirty(vector<int> dirtyNodes, int startId) {
            if (dirtyNodes.empty()) {
                cout << "No dirty nodes to clean!" << endl;
                return;
            }
            
            cout << "\n=== START CLEANING ===" << endl;
            cout << "Total dirty nodes: " << dirtyNodes.size() << endl;
            
            int current = startId;
            vector<int> remainingDirt = dirtyNodes;
            int totalCost = 0;
            int step = 1;
            
            while (!remainingDirt.empty()) {
                cout << "\n--- Step " << step++ << " ---" << endl;
                cout << "Current position: " << current;
                auto coord = idToCoord(current, m);
                cout << " (" << coord.first << "," << coord.second << ")" << endl;
                
                int minCost = INT_MAX, minNode = -1;
                vector<int> minPath;
                
                // Find nearest dirty node
                for (int i = 0; i < remainingDirt.size(); i++) {
                    int dirtyId = remainingDirt[i];
                    auto result = findPath(current, dirtyId);
                    
                    cout << "  Check dirty node " << dirtyId;
                    auto dirtyCoord = idToCoord(dirtyId, m);
                    cout << " (" << dirtyCoord.first << "," << dirtyCoord.second << ") - ";
                    
                    if (result.first == INT_MAX) {
                        cout << "NO PATH" << endl;
                    } else {
                        cout << "Cost: " << result.first << endl;
                        if (result.first < minCost) {
                            minCost = result.first;
                            minNode = dirtyId;
                            minPath = result.second;
                        }
                    }
                }
                
                if (minNode == -1) {
                    cout << "ERROR: Cannot reach any remaining dirty nodes!" << endl;
                    cout << "Remaining nodes: ";
                    for (int node : remainingDirt) {
                        auto coord = idToCoord(node, m);
                        cout << node << "(" << coord.first << "," << coord.second << ") ";
                    }
                    cout << endl;
                    break;
                }
                
                auto dirtyCoord = idToCoord(minNode, m);
                cout << "Moving to dirty node " << minNode << " (" << dirtyCoord.first << "," << dirtyCoord.second << ")" << endl;
                cout << "Cost: " << minCost << endl;
                cout << "Path: ";
                for (int node : minPath) cout << node << " ";
                cout << endl;
                
                totalCost += minCost;
                current = minNode;
                
                // Remove cleaned node
                remainingDirt.erase(
                    remove(remainingDirt.begin(), remainingDirt.end(), minNode),
                    remainingDirt.end()
                );
                
                cout << "Cleaned! Remaining: " << remainingDirt.size() << " nodes" << endl;
            }
            
            cout << "\n=== CLEANING COMPLETED ===" << endl;
            cout << "Total cost: " << totalCost << endl;
            cout << "Final position: " << current << endl;
            
            // Go back to dock
            auto dockPath = findPath(current, dockID);
            if (dockPath.first != INT_MAX) {
                cout << "Returning to dock, cost: " << dockPath.first << endl;
                totalCost += dockPath.first;
                cout << "Total cost including return: " << totalCost << endl;
            } else {
                cout << "Cannot return to dock!" << endl;
            }
        }
};

signed main() 
{ 
    RobotCleaning rb;
    
    rb.initData();
    rb.displayGrid();
    cout << "\n";// << rb.getDockID() << "\n";
    
    pair<int, vector<int>> test = rb.findPath(rb.getStartID(), rb.getDockID());
    for (int v:test.second) cout << v << " "; 
    cout << "\n" << test.first << "\n\n"; 

    rb.cleanAllDirty(rb.getDirtyNode(), rb.getStartID());
}   