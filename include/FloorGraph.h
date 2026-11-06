#pragma once
#include <vector>
#include <queue>
#include "Types.h"
#include "Stations.h"

// GRAPH: the cafe floor. Each PC is a vertex, and PCs that sit next to each
// other are connected. Layout for 10 PCs:
//     1 - 2 - 3 - 4 - 5
//     |   |   |   |   |
//     6 - 7 - 8 - 9 - 10
// The same edges are stored as an adjacency list and an adjacency matrix.
class FloorGraph {
    std::vector<int> adj[NUM_STATIONS + 1];                       // adjacency list
    bool matrix[NUM_STATIONS + 1][NUM_STATIONS + 1] = {};         // adjacency matrix

    void addEdge(int a, int b) {
        adj[a].push_back(b); adj[b].push_back(a);
        matrix[a][b] = matrix[b][a] = true;
    }

    void dfsCollect(int u, int need, const StationManager& sm,
                    bool seen[], std::vector<int>& out) const {
        seen[u] = true;
        out.push_back(u);
        for (int v = 1; v <= NUM_STATIONS && (int)out.size() < need; v++)
            if (matrix[u][v] && !seen[v] && sm.getStatus(v) == AVAILABLE)
                dfsCollect(v, need, sm, seen, out);
    }

public:
    FloorGraph() {
        int rows = NUM_STATIONS / GRID_COLS;
        for (int r = 0; r < rows; r++)
            for (int c = 0; c < GRID_COLS; c++) {
                int id = r * GRID_COLS + c + 1;
                if (c < GRID_COLS - 1) addEdge(id, id + 1);          // right neighbor
                if (r < rows - 1)      addEdge(id, id + GRID_COLS);  // neighbor below
            }
    }

    // BFS over the adjacency list: closest free PC to a given PC, O(V + E).
    // Used when a customer asks for a seat near a friend.
    int nearestFree(int from, const StationManager& sm, int& dist) const {
        bool seen[NUM_STATIONS + 1] = {false};
        int d[NUM_STATIONS + 1] = {0};
        std::queue<int> q;
        q.push(from);
        seen[from] = true;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            if (u != from && sm.getStatus(u) == AVAILABLE) { dist = d[u]; return u; }
            for (int v : adj[u])
                if (!seen[v]) { seen[v] = true; d[v] = d[u] + 1; q.push(v); }
        }
        return -1;
    }

    // DFS over the adjacency matrix: find a block of connected free PCs so a
    // group can sit together, O(V^2).
    bool findGroup(int size, const StationManager& sm, std::vector<int>& out) const {
        for (int start = 1; start <= NUM_STATIONS; start++) {
            if (sm.getStatus(start) != AVAILABLE) continue;
            bool seen[NUM_STATIONS + 1] = {false};
            out.clear();
            dfsCollect(start, size, sm, seen, out);
            if ((int)out.size() == size) return true;
        }
        out.clear();
        return false;
    }
};
