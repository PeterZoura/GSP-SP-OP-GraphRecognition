//usage: ./dfs <input_graph>

/*
terminology guide
input file is a list of pairs of integers
the first pair represents n and m, the number of vertices and the number of edges in the graph
each next pair represents an edge, labelled by two vertex numbers

- adj is an array containing the adjacency lists of each vertex, stored one after another in the array
- adjAddress is an array containing the index to go to in "adj" in order to get the start of the adjacency list for vertex number i
for example:
   adj[i] returns the first vertex in the adjacency list for vertex j
   adj[i+1] returns the next vertex in the adjacency list for vertex j
   adjAddress[j] returns the index i to use in adj[i] to get the adjacency list for vertex j

   so that adj[adjAddress[3]] gets me the first vertex in the adjacency list for vertex 3

   for (int x = adjAddress[i]; x < adjAddress[i+1]; x++){
      in each iteration of loop, x will be the index of each vertex in the adjacency list for vertex i
      so that each iteration
      adj[x]
      will be a different vertex in the adjacency list for vertex i
      until x reaches the adjacency list index for vertex i+1, then the loop stops.
   }

- stack is an array used as a stack
- stack_pointer is the index that represents the top of the stack
   if stack_pointer is set to 0 then that means there is one vertex in the stack
   stack[0] would return that vertex number. you can use that vertex number with adjAddress[]
   to get the adacency list index for the vertex, you could use that index with adj[] to get the adjacency list of the vertex
   for example
   there is one vertex in the stack at the beginning
   stack_pointer = 0
   v = stack[stack_pointer] //returns the vertex number for the vertex on top of the stack
   i = adjAddress[v] //returns the index for adj[] for that vertex on top of the stack
   v2 = adj[i] //returns the first vertex adjacent to the vertex we started with.

   or all in one line

   v2 = adj[adjAddress[stack[stack_pointer]]]

- stackAddress is an address array corresponding to the vertices in the stack
   since adjAddress[stack[stack_pointer]] is a pain to write and to look at, we simply maintain another stack to hold the
   adjacency list index numbers corresponding to the vertex numbers in stack[]
   
   so 
   v2 = adj[adjAddress[stack[stack_pointer]]]
   is the same as
   v2 = adj[stackAddress[stack_pointer]]

*/

#include <atomic>
#include <vector>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <chrono>


int n, m; //number of vertices and of edges
int* edges; //2*m size array to contain all the edges
int* parent; //parent[i] returns the parent of vertex i for purpose of calculating number of descendants
//bool* treeEdges; //boolean array corresponding to edges array which states which ones are a tree edge
int* nDescendants;
int* dfs;
int* adj; int* adjAddress;
int* ear;

void create_adjacency_list();
bool isAncestor(int, int);
bool lexiCompare(int p, int q, int x, int y); //true if back-edge p-q is smaller than x-y 
bool isTree(int, int);
void DFS(int);
double main2(char *);

int Nr=1;


int main(int n_args, char** args)
{
   FILE* fp = fopen(args[1],"r");
   if(fscanf(fp,"%d %d",&n,&m)){
      dfs = (int*)malloc(sizeof(int)*n); 
      for(int i=0;i<n;i++){ dfs[i]=-1; }
      parent = (int*)malloc(sizeof(int)*n);
      for(int i=0; i<n; i++){ parent[i]=-1; }
      nDescendants = (int*)malloc(sizeof(int)*n);
      for(int i = 0; i<n; i++){ nDescendants[i]=1; }
      ear = (int*)malloc(sizeof(int)*2*n);
      for(int i = 0; i<n;i++){
         ear[2*i] = -1;
         ear[2*i+1] = -1;}
      edges = (int*)malloc(sizeof(int)*2*m);
      for(int i=0;i<m;i++){
         int x,y;
         fscanf(fp,"%d %d",&x,&y);
         edges[2*i]=(x>y)?y:x;
         edges[2*i+1]=(x>y)?x:y;}
   }else{
      printf("Failure in reading input\n");
      return -1;
   }
   fclose(fp);

   create_adjacency_list();

   //measure the start time with a barrier that prevents the compiler from reordering
   std::atomic_thread_fence(std::memory_order_seq_cst);
   auto time_start = std::chrono::steady_clock::now();
   std::atomic_thread_fence(std::memory_order_seq_cst);
   
   DFS(0);
   
   //measure the end time with a barrier that prevents the compiler from reordering
   std::atomic_thread_fence(std::memory_order_seq_cst);
   auto time_end = std::chrono::steady_clock::now();
   std::atomic_thread_fence(std::memory_order_seq_cst);
   
   std::chrono::duration<double, std::nano> elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_start);
   std::cout << std::setprecision(4) << elapsed.count()/1000 << "μs is the average time\n";

   // for(int i = 0; i<n; i++){
   // printf("i: %d | dfs: %d | nd: %d | ear: %d,%d\n", i, dfs[i], nDescendants[i], ear[2*i], ear[2*i+1]);
   // }

   // printf("\nAdjacency List: \n");
   // for (int i=0;i<n;i++){
   //    printf("List %d: ", i);
   //    for(int j=adjAddress[i]; j<adjAddress[i+1]; j++){
   //       if(adj[j].tree){
   //          printf("T%d, ", adj[j].target);
   //       }else{
   //          printf("B%d, ", adj[j].target);
   //       }
   //    }
   //    printf("\n");
   // }

   // This code writes node data to an output file.
   // But since I implemented a loop that takes average
   // This code will overwrite the file with the contents 
   // Of the last run of the search, which should be identical
   // To the output of every run.
   // fp = fopen(args[2],"w");
   // for(int i=0;i<n;i++)
   // {
   //       fprintf(fp,"node: %d , dfs: %d\n",i ,dfs[i]);
   // }
   // //fprintf(fp, "%lf\n", elapsed);
   // fclose(fp);
   
   return elapsed.count()/1000;
}

void DFS(int r)
{
   int v = r;
   dfs[v]=Nr++;

   int* stack = (int*)malloc(sizeof(int)*n);
   int* stackAddress = (int*)malloc(sizeof(int)*n);
   int stack_pointer=0;
   //stack stores vertices
   stack[0]=v; stackAddress[0]=adjAddress[v];   //stackAddress stores adj_list addresses of vertices (address of vertex v for example)
   //            ^ add first vertex onto stack
   //             that is adding the root node into the stack

while(stack_pointer!=-1)//this is the DFS stack pointer, stack contains nodes
//
{  //set v to top vertex on stack
   v=stack[stack_pointer]; //stack holds the Depth First Search stack
   char descend=0;   //this boolean determines if algorithm should descend down the tree (if the current node has unexplored children, then it will descend)

   //1 iterate through adjacent vertices to vertex on top of stack
   for(int i=stackAddress[stack_pointer];i<adjAddress[v+1];i++)
   {
      int w=adj[i]; //w is a vertex adjacent to v
      //1.1
      if(dfs[w]==-1)    //if it's not yet visited, assign dfs number
      {
         parent[w]=v;
         dfs[w]=Nr++;//add w to the top of the stack if it hasn't been assigned a DFS number yet
         stack[stack_pointer+1]=w; stackAddress[stack_pointer+1]=adjAddress[w];         
		 
         //stackAddress[stack_pointer]=i;      //why is stackAddress[stack_pointer] assigned with i when i already equals stackAddress[stack_pointer]? comment this out for now
         descend=1; break;  //this line breaks out of the loop so that adjacent nodes to v stop being searched and instead we descend to the children to explore them
      }else if(dfs[w] < dfs[v] && w != parent[v]){
         ear[2*v] = v;
         ear[2*v+1] = w;
      }      
   }

   if(descend){stack_pointer++; continue;}   //if the node has children to explore then descend to explore them
                                             //descending means to increase the stack pointer to traverse up the stack towards the top
                                             //since children get added on top of the stack, this is equivalent to descending down one branch
   if(v != r){//r is root vertex, there is no such thing as parent[r], so we skip this line if v is the root r.
      nDescendants[parent[v]]+=nDescendants[v];
   }
   
   if(dfs[v] > 2 && lexiCompare(ear[2*v], ear[2*v+1], ear[2*parent[v]], ear[2*parent[v]+1])){
      ear[2*parent[v]] = ear[2*v];
      ear[2*parent[v]+1] = ear[2*v+1];
   }
   stack_pointer--;  //when the vertex on top of the stack has been fully explored, decrement the stack pointer to backtrack during DFS 
}//end while

}//end DFS

void create_adjacency_list()
{
   adj = (int*)malloc(sizeof(int)*4*m);				//array of 4 ints per edges	each block represents 1 edge	0000'0000'0000'0000'0000 for m blocks
   adjAddress = (int*)malloc(sizeof(int)*(n+1));		//array of ints. size = |V| + 1		0'0'0'0'0'0'0'0'0'0'0'0'0'0		why is there 1 extra int? and what does it represent?
   for(int i=0;i<=n;i++){adjAddress[i]=0;}			//initialize adjAddress to zeroes
   for(int i=0;i<m;i++){						//for every edge
												//note that the first element is skipped in the assignment for loop, so the first element is always 0
		adjAddress[edges[2*i]+1]++;				//references each source vertex 's' in edge list and increments adjAddress[s] by 1
		adjAddress[edges[2*i+1]+1]++;				//references each target vertex 't' in edge list and increments adjAddress[t] by 1
	}//this for loop records the degree of each vertex by reading how many times it appears as a target and as a source of an edge in the edge list.
	
   int* nextOut = (int*)malloc(sizeof(int)*(n+1));	//array of ints, 1 per vertex, but there is one more int here than there are vertices
   nextOut[0]=0;	//initialize only the first element to 0 (maybe every other element then represents every vertex and 0 is the extra 1 int)
   
   for(int i=1;i<=n;i++){ //for each vertex
		adjAddress[i]+=adjAddress[i-1]; //increase each vertex's degree by x where x is the previous vertex's degree, but the first one is increased by 0
		nextOut[i]=adjAddress[i];		//copy adjAddress into nextOut, this is because nextOut will be changed later, but adjAddress will preserve the adj list offsets
   } //this is to set the offset in adj for each vertex's adjacency list.
   
   
   /*
   each vertex v has degree i, this represents "how many vertices v is adjacent to"
   so the degree also represents how many spaces v's adjacency list needs.
   adjAddress is first initalized to the degree of each vertex,
   then it is reassigned so that each vertex has the degree of the previous vertices added to it
   this is because adjAddress[v] represents the offset of v's adjacency list in the greater array adj which stores the adjacency lists
   so adjAddress[v] represents the number of vertices v is adjacent to plus the number of vertices all previous vertices are adjacent to.
   adj[adjAddress[v]] gives us the index in adj to access the beginning of the adjacency list of v.
   */
   
   for(int i=0;i<m;i++)
   {		//edges array has 2 ints per edge, each block represents 1 edge		00'00'00'00'00'00... for m blocks
			//clearly these 2 ints represent the number of the source vertex and the number of the target vertex of each edge
      int x=edges[2*i]; 	//read an edge into the variables x and y
	   int y=edges[2*i+1];	//(x, y) is the edge we are referencing
      adj[nextOut[x]++]=y;	//access the adjacency list offset for vertex x and set the first adjacent vertex to y
		            	      //then increment the adjacency list offset so that the next vertex that's added in the list
						   	   //gets added at the end of the list, rather than at the beginning.
      adj[nextOut[y]++]=x;	//repeat for the adjacency list of vertex y
		                     //this incrementing of the offsets in nextOut is the reason that we copied the list adjAddress rather than modifying adjAddress
							      //because adjAddress is needed to be used to access the beginnings of the adjacency lists of each vertex later in the program.
   }
}

//is a the ancestor of b?
//return true if yes.
bool isAncestor(int a, int b){
   return (dfs[a] <= dfs[b] && dfs[b] < ( dfs[a] + nDescendants[a] ) );
}

//return true if back-edge (q <-- p) is smaller than (y <-- x)
//return false if (y <-- x) is smaller than (q <-- p)
bool lexiCompare(int p, int q, int x, int y){
   return x == -1 || (dfs[q] < dfs[y]) 
   || ( (dfs[q] == dfs[y]) && (dfs[p] < dfs[x]) && !isAncestor(p, x) ) 
   || ( (dfs[q] == dfs[y]) && isAncestor(x, p) );
}

bool isTree(int a, int b){
    return (parent[a] == b || parent[b] == a);
}




