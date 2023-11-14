//usage: ./dfs2 <input_graph> <# of runs>
//It runs many times and gives average runtime

#include <vector>
#include <iostream>
#include <fstream>
#include <stack>
#include <chrono>
#include <iomanip>

using std::pair;
using std::vector;


struct graphDataHolder{
    int n; //equal to |V| size of graph
    int e; //equal to |E| number of edges in the graph
    vector<int> parent; //vector containing data so that parent[3] gives the number of the vertex that is parent of 3 in the dfs tree
    vector<int> nDescendants; //vector containing number of descendants of a vertex including itself as a descendant.
    vector<int> dfsRank; //holds the number that represents in the order that the dfs accessed each vertex in the graph in.
    vector<vector<int>> adjList; //adjList[0] contains the vector of Edges that are connected to vertex 0. Edge contains target vertex and tree edge boolean.
    vector<pair<int, int>> ear; //holds ear decomposition data. pair.first must be source, pair.second must be sink
};

namespace s=std;

void create_adjacency_list(int const &, vector<pair<int, int>> const &, vector<vector<int>> &);
void create_edges(int const &, int const &, vector<pair<int, int>>&, s::ifstream&);
bool isAncestor(int  const &, int  const &, graphDataHolder  const &);
bool lexiCompare(int const &, int const &, int const &, int const &, graphDataHolder  const &);
bool isTree(int const &, int const &, graphDataHolder const &);
void genCS(int const &, graphDataHolder &);
double main2(char *);
int main(int argc, char* argv[]) {
    s::ios::sync_with_stdio(false); //I heard this helps optimize
    int times = atoi(argv[2]);
    double ave = 0.0;
    for(int i = 0; i<times; i++){
        ave += main2(argv[1]);
    }
    s::cout << "average of " << times << " runs: " << ave/(0.0 + times) << "μs\n";
    return 0;
}

double main2(char * inputFileName){
    graphDataHolder gD; //gD stands for Graph Data
    
    s::ifstream is(inputFileName);
    is >> gD.n >> gD.e;
    
    gD.dfsRank = s::vector<int>(gD.n, -1);
    gD.parent = s::vector<int>(gD.n, -1);
    gD.nDescendants = s::vector<int>(gD.n, 1);
    gD.ear = s::vector<pair<int, int>>(gD.n, s::pair<int, int>(-1, -1));
    gD.adjList.resize(gD.n);
    
    vector<pair<int, int>> edges;
    create_edges(gD.n, gD.e, edges, is);
    create_adjacency_list(gD.n, edges, gD.adjList);

    auto time_start = s::chrono::steady_clock::now();
    genCS(0, gD);
    auto time_end = s::chrono::steady_clock::now();
    s::chrono::duration<double, s::nano> time_elapsed = s::chrono::duration_cast<s::chrono::nanoseconds>(time_end - time_start);
    //s::cout << "time elapsed is: " << s::setprecision(4) << time_elapsed.count()/1000 << "μs\n";
    

    //Print adjacency list for debugging
    // for(int i = 0; i<gD.adjList.size();i++){
    //     s::cout << "List " << i << " is: ";
    //     for(int j : gD.adjList[i]){
    //         char c = (isTree(j, i, gD)?'T':'B');
    //         s::cout << c << j << " ";
    //     }
    //     s::cout << '\n';
    // }
    // s::cout << '\n';

    // Print DFS ranks for testing
    // s::cout << '\n';
    // for (int i = 0; i < gD.dfsRank.size(); ++i) {
    //     s::cout << "Vertex " << i << " | DFS " << gD.dfsRank[i] << " | Parent " << gD.parent[i] << " | nDescendants " << gD.nDescendants[i] << " | ear " << gD.ear[i].first << ',' << gD.ear[i].second << "\n";
    // }

    return time_elapsed.count()/1000;
}

void genCS(int const & starting_vertex, graphDataHolder & gD) {
    s::stack<int> the_stack;
    the_stack.push(starting_vertex);
    int dfsNumber = 1;
    gD.dfsRank[starting_vertex] = dfsNumber++;

    while (!the_stack.empty()) {
        int topOfStack = the_stack.top();
        bool descend = false;

        for (int w : gD.adjList[topOfStack]) {
            if (gD.dfsRank[w] == -1) {
                gD.parent[w] = topOfStack;
                gD.dfsRank[w] = dfsNumber++;
                the_stack.push(w);
                descend = true;
                break;
            }else if(gD.dfsRank[w] < gD.dfsRank[topOfStack] && w != gD.parent[topOfStack]){
                //back edge detected
                gD.ear[topOfStack].first = topOfStack;
                gD.ear[topOfStack].second = w;
            }
        }
        if (descend) {
            continue;
        }
        
        if(topOfStack != starting_vertex){
            gD.nDescendants[gD.parent[topOfStack]] += gD.nDescendants[topOfStack];
        }
        /*
        dfsRank[topOfStack] > 2; this is an easier way of writing
        topOfStack != starting_vertex && parent[topOfStack] != starting_vertex
        
        both of those are necessary because:
        1. if I only check topOfStack != starting_vertex, then when the vertex with the root r as parent is backed into as the search
            is reversing up the tree, the if condition will succeed and gD.ear[gD.parent[topOfStack]] will run which produces an error
            because the result is -1 and the function will use that parameter to find gD.dfsRank[-1] which doesn't exist.
        2. if I only check parent[topOfStack] != starting_vertex, then when the root r is backed into as the search reverses up the tree
            the if condition will succeed and gD.ear[gD.parent[topOfStack]] will run which produces an error because gD.ear[-1] doesn't
            exist.
        */
        if(gD.dfsRank[topOfStack] > 2 && lexiCompare(gD.ear[topOfStack].first, gD.ear[topOfStack].second, gD.ear[gD.parent[topOfStack]].first, gD.ear[gD.parent[topOfStack]].second, gD)){
                gD.ear[gD.parent[topOfStack]].first = gD.ear[topOfStack].first; 
                gD.ear[gD.parent[topOfStack]].second = gD.ear[topOfStack].second;
        }
        the_stack.pop();
    }
}

void create_adjacency_list(int const & n, vector<pair<int, int>> const & edges, vector<vector<int>>& adj) {
    adj.resize(n);
    for (const auto& e : edges) {
        int x = e.first;
        int y = e.second;
        adj[x].emplace_back(y);
        adj[y].emplace_back(x);
    }
}

void create_edges(int const & num_v, int const & num_e, vector<pair<int, int>>& edges, s::ifstream& istream) {
    edges.reserve(num_e);
    for (int i = 0; i < num_e; ++i) {
        int x, y;
        istream >> x >> y;
        edges.emplace_back( (x>y?y:x), (x>y?x:y));
    }
}

bool isAncestor(int const & a, int const & b, graphDataHolder const & gD){
   return (gD.dfsRank[a] <= gD.dfsRank[b] && gD.dfsRank[b] < ( gD.dfsRank[a] + gD.nDescendants[a] ) );
}

//return true if back-edge (q <-- p) is smaller than (y <-- x)
//return false if (y <-- x) is smaller than (q <-- p)
bool lexiCompare(int const & p, int const & q, int const & x, int const & y, graphDataHolder const & gD){
    return x == -1 || (gD.dfsRank[q] < gD.dfsRank[y]) 
            || ( (gD.dfsRank[q] == gD.dfsRank[y]) && (gD.dfsRank[p] < gD.dfsRank[x]) && !isAncestor(p, x, gD) ) 
            || ( (gD.dfsRank[q] == gD.dfsRank[y]) && isAncestor(x, p, gD) );
   }

bool isTree(int const & a, int  const & b, graphDataHolder const & gD){
    return (gD.parent[a] == b || gD.parent[b] == a);
}