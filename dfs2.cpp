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
        edges.push_back(s::pair<int, int>(x, y));
    }
}


vector<vector<int>> create_adjacency_list(int n, int m){
    vector<vector<int>> adj;
    for(int i=0 ; i<m ; i++){

    }
}

int main(int argc, char* argv[]){
    s::ifstream is(argv[1]);
    int n, m;
    is >> n >> m;
    vector<pair<int, int>> edges;
    create_edges(n, m, edges, is);
    for (pair<int, int> e : edges){
        s::cout << e.first << ' ' << e.second << '\n';
    }
}
