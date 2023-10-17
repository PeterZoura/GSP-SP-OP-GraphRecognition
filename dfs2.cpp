#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <fstream>

using std::pair;
using std::vector;
using std::unique_ptr;

namespace s=std;


void create_edges(int num_v, int num_e, vector<pair<int, int>> & edges, s::ifstream & istream){
    for (int i =0; i<num_e ; i++){
        int x, y;
        istream >> x >> y;
        edges.push_back(s::make_pair(x, y));
    }
}


void create_adjacency_list(int const n, int const m, auto & edges, auto & adj){
    for(int i = 0; i<n; i++){
        adj.push_back(vector<int>{});
    }
    
    for(pair<int, int> e : edges){
        int x = e.first;
        int y = e.second;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }
}

vector<int> dfs(int const n, auto & adjacency_list){
    vector<int> dfs{n, -1};
    int rank = 0;
    return dfs;
}

int main(int argc, char* argv[]){
    s::ifstream is(argv[1]);
    int n, m;
    is >> n >> m;
    vector<pair<int, int>> edges;
    vector<vector<int>> adj;

    create_edges(n, m, edges, is);    
    create_adjacency_list(n, m, edges, adj);
    for(vector<int> e : adj){
        for( int i : e){
            s::cout << i << ", ";
        }
        s::cout << '\n';
    }
}
