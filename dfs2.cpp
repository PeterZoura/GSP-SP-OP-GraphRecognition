#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>
#include <stack>
#include <chrono>
#include <iomanip>

using std::pair;
using std::vector;
using std::tuple;

namespace s=std;


void create_edges(int num_v, int num_e, vector<tuple<int, int, bool>>& edges, s::ifstream& istream) {
    edges.reserve(num_e);
    for (int i = 0; i < num_e; ++i) {
        int x, y;
        istream >> x >> y;
        edges.emplace_back( (x>y?y:x), (x>y?x:y), false);
    }
}

void create_adjacency_list(int const n, vector<tuple<int, int, bool>> const & edges, vector<vector<int>>& adj) {
    adj.resize(n);
    for (const auto& e : edges) {
        int x = s::get<0>(e);
        int y = s::get<1>(e);
        adj[x].emplace_back(y);
        adj[y].emplace_back(x);
    }
}

void dfs(int starting_vertex, vector<int>& dfsRank, const vector<vector<int>>& adj, vector<tuple<int, int, bool>> & edges) {
    s::stack<int> the_stack;
    the_stack.push(starting_vertex);
    dfsRank[starting_vertex] = 1;
    int rank = 2;

    while (!the_stack.empty()) {
        int topOfStack = the_stack.top();
        bool descend = false;

        for (int w : adj[topOfStack]) {
            if (dfsRank[w] == -1) {
                s::erase(edges, s::tuple{(w>topOfStack?topOfStack:w), (w>topOfStack?w:topOfStack), false});
                edges.emplace_back((w>topOfStack?topOfStack:w), (w>topOfStack?w:topOfStack), true);
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

vector<int> initialize_dfs(int n, vector<vector<int>> const & adj, vector<tuple<int, int, bool>> & edges) {
    vector<int> dfsRank(n, -1);
    dfs(0, dfsRank, adj, edges);
    return dfsRank;
}

int main(int argc, char* argv[]) {
    s::ios::sync_with_stdio(false);

    s::ifstream is(argv[1]);
    int n, m;
    is >> n >> m;

    vector<tuple<int, int, bool>> edges;
    create_edges(n, m, edges, is);

    vector<vector<int>> adj;
    create_adjacency_list(n, edges, adj);

    /*
    //Print adjacency list for debugging
    for(int i =0; i<adj.size();i++){
        s::cout << "List " << i << " is: ";
        for(int j : adj[i]){
            s::cout << j << " ";
        }
        s::cout << '\n';
    }
    s::cout << '\n';
    */
   
    auto time_start = s::chrono::steady_clock::now();
    auto dfsRank = initialize_dfs(n, adj, edges);
    auto time_end = s::chrono::steady_clock::now();


    s::chrono::duration<double, s::nano> time_elapsed = s::chrono::duration_cast<s::chrono::nanoseconds>(time_end - time_start);
    s::cout << "time elapsed is: " << s::setprecision(4) << time_elapsed.count()/1000 << "μs\n";

    // Print DFS ranks for testing
    //print tree list
    // for(tuple<int, int, bool> const & e : edges){
    //     if(s::get<2>(e) == true){
    //         s::cout << "Tree edge: (" << s::get<0>(e) << ',' << s::get<1>(e) << ")\n";
    //     }
    // }    
    // s::cout << '\n';
    // for (int i = 0; i < dfsRank.size(); ++i) {
    //     s::cout << "Vertex " << i << " | " << dfsRank[i] << "\n";
    // }
    
}
