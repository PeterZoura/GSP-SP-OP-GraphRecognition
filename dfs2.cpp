#include <vector>
#include <memory>
#include <utility>
#include <iostream>
#include <fstream>
#include <stack>
#include <chrono>
#include <iomanip>

using std::pair;
using std::vector;
using std::unique_ptr;

namespace s=std;


void create_edges(int const & num_v, int const & num_e, vector<pair<int, int>> & edges, s::ifstream & istream){
    for (int i =0; i<num_e ; i++){
        int x, y;
        istream >> x >> y;
        edges.push_back(pair<int,int>(x, y));
    }
}


void create_adjacency_list(int const & n, int const & m, auto const & edges, auto & adj){
    for(int i = 0; i<n; i++){
        adj.emplace_back();
    }
    
    for(pair<int, int> e : edges){
        int x = e.first;
        int y = e.second;
        adj[x].emplace_back(y);
        adj[y].emplace_back(x);
    }
}

void dfs(int const & starting_vertex, vector<int> & dfsRank, auto & adj){
    s::stack<int> the_stack;
    the_stack.push(starting_vertex);
    dfsRank[starting_vertex]=1;
    int rank = 2;
    while(!the_stack.empty()){
        int descend = 0;
        int topOfStack = the_stack.top();
        for(int w : adj[topOfStack]){
            if(dfsRank[w]==-1){
                dfsRank[w]=rank++;
                the_stack.push(w);
                descend = 1;
                break;
            }
        }
        if(descend){
            continue;
        }else{
            the_stack.pop();
        }
    }
}

vector<int> initialize_dfs(int const & n, auto & adj){
    vector<int> dfsRank(n, -1);
    dfs(0, dfsRank, adj);
    return dfsRank;
}

int main(int argc, char* argv[]){
    std::ios::sync_with_stdio(false);

    s::ifstream is(argv[1]);
    int n, m;
    is >> n >> m;
    vector<pair<int, int>> edges;
    edges.reserve(m);
    vector<vector<int>> adj;
    adj.reserve(n);

    create_edges(n, m, edges, is);    
    create_adjacency_list(n, m, edges, adj);

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
    auto dfsRank = initialize_dfs(n, adj);
    auto time_end = s::chrono::steady_clock::now();
    
    s::chrono::duration<double, s::nano> time_elapsed = s::chrono::duration_cast<s::chrono::nanoseconds>(time_end - time_start);
    s::cout << "time elapsed is: " << s::setprecision(4) << time_elapsed.count()/1000 << "μs\n";
    
    //print DFS ranks for testing
    /*
    for(int i = 0; i<dfsRank.size(); i++){
        s::cout << "Vertex " << i << " | " << dfsRank[i] << "\n";
    }
    */
}
