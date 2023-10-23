#include <vector>
#include <iostream>
#include <fstream>
#include <stack>
#include <chrono>
#include <iomanip>

using std::pair;
using std::vector;

namespace s=std;


void create_edges(int num_v, int num_e, vector<vector<int>> & edges, s::ifstream& istream) {
    for (int i = 0; i < num_e; ++i) {
        int x, y;
        istream >> x >> y;
        edges[x][y] = 1;
        edges[y][x] = 1;
    }
}

/*
void create_adjacency_list(int const n, vector<pair<int, int>> const & edges, vector<vector<int>>& adj) {
    adj.resize(n);
    for (const auto& e : edges) {
        int x = e.first;
        int y = e.second;
        adj[x].emplace_back(y);
        adj[y].emplace_back(x);
    }
}
*/

void dfs(int n, int starting_vertex, vector<int>& dfsRank, vector<vector<int>> & edges) {
    s::stack<int> the_stack;
    the_stack.push(starting_vertex);
    dfsRank[starting_vertex] = 1;
    int rank = 2;

    while (!the_stack.empty()) {
        int topOfStack = the_stack.top();
        bool descend = false;
        for (int w=0; w < n; w++) {
            if (dfsRank[w] == -1 && edges[topOfStack][w]==1) {
                edges[topOfStack][w] = 2; //this marks it as a tree edge
                edges[w][topOfStack] = 2; //it needs to be kept symmetric
                dfsRank[w] = rank++;
                the_stack.push(w);
                descend = true;
                break;
            }
        }

        if (descend) {
            continue;
        } else {
            the_stack.pop();
        }
    }
}

vector<int> initialize_dfs(int n, vector<vector<int>> & edges) {
    vector<int> dfsRank(n, -1);
    dfs(n, 0, dfsRank, edges);
    return dfsRank;
}

int main(int argc, char* argv[]) {
    s::ios::sync_with_stdio(false);

    s::ifstream is(argv[1]);
    int n, m;
    is >> n >> m;

    vector<vector<int>> edges(n, vector<int>(n, 0));
    create_edges(n, m, edges, is);

    //vector<vector<int>> adj;
    //create_adjacency_list(n, edges, adj);

    
    
    auto time_start = s::chrono::steady_clock::now();
    auto dfsRank = initialize_dfs(n, edges);
    auto time_end = s::chrono::steady_clock::now();

    /*
    //Print adjacency list for debugging
    for(int i = 0; i<n;i++){
        s::cout << "List " << i << " is: ";
        for(int j = 0; j < n; j++){
            if(edges[i][j] > 0){
                s::cout << j << " ";
            }
        }
        s::cout << '\n';
    }
    s::cout << '\n';
    
    //Tree edge list
    for(int i = 0; i < n; i++){
        s::cout << "List " << i << " is: ";
        for(int j = 0; j < n; j++){
            if(edges[i][j] > 1){
                s::cout << j << " ";
            }
        }
        s::cout << '\n';
    }
    s::cout << '\n';
    */

    s::chrono::duration<double, s::nano> time_elapsed = s::chrono::duration_cast<s::chrono::nanoseconds>(time_end - time_start);
    s::cout << "time elapsed is: " << s::setprecision(4) << time_elapsed.count()/1000 << "μs\n";

    // Print DFS ranks for testing
    
    /*
    for (int i = 0; i < dfsRank.size(); ++i) {
        s::cout << "Vertex " << i << " | " << dfsRank[i] << "\n";
    }
    */
}
