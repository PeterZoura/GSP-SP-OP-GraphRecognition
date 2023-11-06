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

struct graphDataHolder{
    int n; //equal to |V| size of graph
    int e; //equal to |E| number of edges in the graph
    vector<int> parent; //vector containing data so that parent[3] gives the number of the vertex that is parent of 3 in the dfs tree
    vector<int> nDescendants; //vector containing number of descendants of a vertex including itself as a descendant.
    vector<int> dfsRank; //holds the number that represents in the order that the dfs accessed each vertex in the graph in.
    vector<vector<Edge>> adjList; //adjList[0] contains the vector of Edges that are connected to vertex 0. Edge contains target vertex and tree edge boolean.
};

namespace s=std;


void create_edges(int const & num_v, int const & num_e, vector<pair<int, int>>& edges, s::ifstream& istream) {
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

void dfs(int starting_vertex, graphDataHolder & gD) {
    s::stack<int> the_stack;
    the_stack.push(starting_vertex);
    gD.dfsRank[starting_vertex] = 1;
    int rank = 2;

    while (!the_stack.empty()) {
        int topOfStack = the_stack.top();
        bool descend = false;

        for (Edge w : gD.adjList[topOfStack]) {
            if (gD.dfsRank[w.target] == -1) {
                gD.dfsRank[w.target] = rank++;
                gD.parent[w.target] = topOfStack;

                getEdge(topOfStack, w.target, gD.adjList)->tree = true;
                getEdge(w.target, topOfStack, gD.adjList)->tree = true;
                
                the_stack.push(w.target);
                descend = true;
                break;
            }
        }

        if (descend) {
            continue;
        } else {
            if(topOfStack != starting_vertex){
                gD.nDescendants[gD.parent[topOfStack]] += gD.nDescendants[topOfStack];
            }
            the_stack.pop();
        }
    }
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

    graphDataHolder gD; //gD stands for Graph Data
    
    s::ifstream is(argv[1]);
    is >> gD.n >> gD.e;

    gD.parent = s::vector<int>(gD.n, -1);
    gD.dfsRank = s::vector<int>(gD.n, -1);
    gD.nDescendants = s::vector<int>(gD.n, 1);
    gD.adjList.resize(gD.n);
    
    vector<pair<int, int>> edges;
    create_edges(gD.n, gD.e, edges, is);
    create_adjacency_list(gD.n, edges, gD.adjList);

    auto time_start = s::chrono::steady_clock::now();
    dfs(0, gD);
    auto time_end = s::chrono::steady_clock::now();
    
    //Print adjacency list for debugging
    // for(int i =0; i<gD.adjList.size();i++){
    //     s::cout << "List " << i << " is: ";
    //     for(Edge j : gD.adjList[i]){
    //         char c = (j.tree?'T':'B');
    //         s::cout << c << j.target << " ";
    //     }
    //     s::cout << '\n';
    // }
    // s::cout << '\n';


    s::chrono::duration<double, s::nano> time_elapsed = s::chrono::duration_cast<s::chrono::nanoseconds>(time_end - time_start);
    s::cout << "time elapsed is: " << s::setprecision(4) << time_elapsed.count()/1000 << "μs\n";

    //Print DFS ranks for testing
    //print tree list
    s::cout << '\n';
    for (int i = 0; i < gD.dfsRank.size(); ++i) {
        s::cout << "Vertex " << i << " | DFS " << gD.dfsRank[i] << " | Parent " << gD.parent[i] << " | nDescendants " << gD.nDescendants[i] << "\n";
    }
    
}