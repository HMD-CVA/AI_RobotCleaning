#include<bits/stdc++.h>
using namespace std;
class RobotCleaning{
    private:
        int n;
        vector<vector<pair<int, int>>> edge;
        vector<int> H;
    public:
        RobotCleaning(int _n, vector<vector<pair<int, int>>> _edge, vector<int> _H){
            n = _n;
            edge = _edge;
            H = _H;
            cout << "Robot initialized with " << n << " nodes" << endl;
        }
        void displayEdgeInfo() {
            cout << "\nEdge Information:" << endl;
            for (int i = 0; i < n; i++) {
                if (!edge[i].empty()) {
                    cout << "Node " << i << " -> ";
                    for (const auto& neighbor : edge[i]) {
                        cout << neighbor.first << "(cost:" << neighbor.second << ") ";
                    }
                    cout << endl;
                }
            }
        }
        pair<int , vector<int>> findPath(int startId, int goalId) {
            // cout << "Finding path: " << startId << " -> " << goalId << endl;
            if (startId < 0 || startId >= n || goalId < 0 || goalId >= n) {
                cout << "Invalid start or goal ID!" << endl;
                return {};
            }
            vector<int> G(n, INT_MAX);
            vector<int> F(n, INT_MAX);
            vector<int> pre(n, -1);
            vector<int> vis(n, -1);
            // vector<int> H(n, 0);
            // int goalX = goalId % (int)sqrt(n);
            // int goalY = goalId / (int)sqrt(n);
            // for (int i = 0; i < n; i++) {
            //     int x = i % (int)sqrt(n);
            //     int y = i / (int)sqrt(n);
            //     H[i] = abs(x - goalX) + abs(y - goalY);
            // }
            G[startId] = 0;
            F[startId] = H[startId];
            vis[startId] = 1;
            while (true) {
                int current = -1;
                int minF = INT_MAX;
                
                // Tìm node có F nhỏ nhất
                for (int i = 0; i < n; i++) {
                    if (vis[i] == 1 && F[i] < minF) {
                        minF = F[i];
                        current = i;
                    }
                }
                
                if (current == -1) {
                    cout << "No path found!" << endl;
                    return {};
                }
                
                if (current == goalId) {
                    // cout << "Path found! Cost: " << G[goalId] << endl;
                    break;
                }
                
                vis[current] = 0;
                
                // Duyệt neighbors
                for (pair<int, int> neighbor : edge[current]) {
                    int next = neighbor.first;
                    int cost = neighbor.second;
                    
                    if (vis[next] == 1) {
                        if (G[next] > G[current] + cost) {
                            G[next] = G[current] + cost;
                            F[next] = G[next] + H[next];
                            pre[next] = current;
                        }
                    } else if (vis[next] == -1) {
                        G[next] = G[current] + cost;
                        F[next] = G[next] + H[next];
                        pre[next] = current;
                        vis[next] = 1;
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
            
            // cout << "Path: ";
            // for (int i = 0; i < path.size(); i++) {
            //     cout << path[i];
            //     if (i != path.size() - 1) cout << " -> ";
            // }
            // cout << endl;
            
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
                if (minNode == -1) return ;

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
        }
};

signed main() 
{
    freopen("input.txt", "r", stdin);
    int n; cin >> n;
    vector<vector<pair<int, int>>> edge;
    vector<int> H;
    edge.resize(n);
    H.resize(n);
    for (int i=0;i<n;i++) {
        for (int j=0;j<n;j++) {
            int w;
            cin >> w;
            if (w > 0) edge[i].push_back({j, w});
        }
    }
    for (int i=0;i<n;i++) cin >> H[i];
    RobotCleaning rb(n, edge, H);
    // rb.findPath(0, 7);

    // vector<int> sP = rb.findPath(0, 7).second;
    // for (int v:sP) cout << v << " "; 
    // cout << endl ;
    // cout << rb.findPath(0, 7).first; 

    vector<int> dirtyNodes = {2, 5, 7};
    rb.cleanAllDirty(dirtyNodes, 0);
}   