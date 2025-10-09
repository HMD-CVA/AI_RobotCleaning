#include<bits/stdc++.h>
using namespace std;
class RobotCleaning{
    private:
        int n, m;
        vector<vector<int>> grid;
        vector<vector<pair<int, int>>> edge;
        vector<int> heuristic;
    public:
        void initData() {
            ifstream file("input.txt");
            if (!file.is_open()) {
                cout << "Cannot open input.txt" << endl;
                return;
            }
        
            file >> n >> m;
            grid.resize(n, vector<int> (m));
            for (int i=0;i<n;i++) 
                for (int j=0;j<m;j++) 
                    file >> grid[i][j];

            updateEdge();
            file.close();
            cout << "Robot initialized with " << n << " nodes" << endl;     
        }   
        RobotCleaning() {
            n = 0;
            m = 0;
        }
        RobotCleaning(int _n, vector<vector<pair<int, int>>> _edge, vector<int> _H){
            n = _n;
            edge = _edge;
            heuristic = _H;
            cout << "Robot initialized with " << n << " nodes" << endl;
        }
        void updateEdge() {
            edge.resize(n);
            heuristic.resize(n * m);
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
                    if (grid[i][j] == INT_MAX) cout << "C"; 
                    else if (grid[i][j] == -1) cout << "D";
                    else cout << grid[i][j];
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
                        
                        if (grid[newY][newX] != INT_MAX && !vis[nextId]) {
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
        void cleanAllDirty(vector<int> dirtyNodes, int startId) {
            if (dirtyNodes.empty()) {
                cout << "No dirty nodes!" << endl;
                return;
            }
            int current = startId;
            vector<int> remainingDirt = dirtyNodes;
            int totalCost = 0;
            
            while (!remainingDirt.empty()) {
                cout << current << " ";
                int minCost = INT_MAX, minNode = -1;
                vector<int> minPath;
                for (int i=0;i<remainingDirt.size();i++) {
                    pair<int, vector<int>> res = findPath(current, remainingDirt[i]);
                    if (minCost > res.first) {
                        minCost = res.first;
                        minNode = remainingDirt[i];
                        minPath = res.second;
                    }
                }
                if (minNode == -1) break;

                cout << "Move to node " << minNode << " (Cost: " << minCost << ")" << endl;
                cout << "Path: ";
                for (int node : minPath) cout << node << " ";
                    cout << endl;
                
                totalCost += minCost;
                current = minNode;
                remainingDirt.erase(
                    remove(remainingDirt.begin(), remainingDirt.end(), minNode),
                    remainingDirt.end()
                );
                cout << "Cleaned node " << minNode << ". Remaining: " << remainingDirt.size() << " nodes" << endl;
            }
            cout << "Total: " << totalCost ;
        }
};

signed main() 
{ 
    RobotCleaning rb;
    
    rb.initData();
    rb.displayGrid();
    cout << "\n";
    
    pair<int, vector<int>> test = rb.findPath(0, 7);
    for (int v:test.second) cout << v << " "; 
    cout << "\n" << test.first << "\n\n"; 

    vector<int> dirtyNodes = {2, 5, 7};
    //rb.cleanAllDirty(dirtyNodes, 0);
}   