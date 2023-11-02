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

struct Edge{
    int target;
    bool tree;
};
Edge * getEdge(int, int,vector<vector<Edge>> &);

namespace s=std;


void create_edges(int num_v, int num_e, vector<pair<int, int>>& edges, s::ifstream& istream) {
    edges.reserve(num_e);
    for (int i = 0; i < num_e; ++i) {
        int x, y;
        istream >> x >> y;
        edges.emplace_back( (x>y?y:x), (x>y?x:y));
    }
}

void create_adjacency_list(int const n, vector<pair<int, int>> const & edges, vector<vector<Edge>>& adj) {
    adj.resize(n);
    for (const auto& e : edges) {
        int x = e.first;
        int y = e.second;
        adj[x].emplace_back(y, false);
        adj[y].emplace_back(x, false);
    }
}

void dfs(int starting_vertex, vector<int>& dfsRank, vector<vector<Edge>>& adj) {
    s::stack<int> the_stack;
    the_stack.push(starting_vertex);
    dfsRank[starting_vertex] = 1;
    int rank = 2;

    while (!the_stack.empty()) {
        int topOfStack = the_stack.top();
        bool descend = false;

        for (Edge w : adj[topOfStack]) {
            if (dfsRank[w.target] == -1) {
                dfsRank[w.target] = rank++;
                getEdge(topOfStack, w.target, adj)->tree = true;
                getEdge(w.target, topOfStack, adj)->tree = true;
                the_stack.push(w.target);
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

vector<int> initialize_dfs(int n, vector<vector<Edge>> & adj) {
    vector<int> dfsRank(n, -1);
    dfs(0, dfsRank, adj);
    return dfsRank;
}

Edge * getEdge(int a, int b, vector<vector<Edge>> & adj){
    int m = 0;
    for(auto i : adj[a]){
        if(i.target==b){
            return &adj[a][m];
        }
        m++;
    }
    return nullptr;
}

int main(int argc, char* argv[]) {
    s::ios::sync_with_stdio(false);

    s::ifstream is(argv[1]);
    int n, m;
    is >> n >> m;

    vector<pair<int, int>> edges;
    create_edges(n, m, edges, is);

    vector<vector<Edge>> adj;
    create_adjacency_list(n, edges, adj);

    auto time_start = s::chrono::steady_clock::now();
    auto dfsRank = initialize_dfs(n, adj);
    auto time_end = s::chrono::steady_clock::now();
    
    //Print adjacency list for debugging
    for(int i =0; i<adj.size();i++){
        s::cout << "List " << i << " is: ";
        for(Edge j : adj[i]){
            char c = (j.tree?'T':'B');
            s::cout << c << j.target << " ";
        }
        s::cout << '\n';
    }
    s::cout << '\n';


    s::chrono::duration<double, s::nano> time_elapsed = s::chrono::duration_cast<s::chrono::nanoseconds>(time_end - time_start);
    s::cout << "time elapsed is: " << s::setprecision(4) << time_elapsed.count()/1000 << "μs\n";

    //Print DFS ranks for testing
    //print tree list
    s::cout << '\n';
    for (int i = 0; i < dfsRank.size(); ++i) {
        s::cout << "Vertex " << i << " | " << dfsRank[i] << "\n";
    }
    
}