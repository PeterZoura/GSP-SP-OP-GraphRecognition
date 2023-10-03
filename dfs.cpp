//usage: ./Tsin <input_graph> <output_file>

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int n, m;
int* edges;
int* G; int* adjListAddress;

void create_adj();
void find_cut_edges();

int* dfs;
int Nr=1;

void Find_cut_pairs(int);


int main(int n_args, char** args)
{
   FILE* fp = fopen(args[1],"r");
   fscanf(fp,"%d %d",&n,&m);
   edges = (int*)malloc(sizeof(int)*2*m);
   for(int i=0;i<m;i++)
   {
      int x,y;
      fscanf(fp,"%d %d",&x,&y);
      edges[2*i]=x; edges[2*i+1]=y;
   }
   fclose(fp);

   create_adj();

   struct timeval begin, end;
   gettimeofday(&begin, 0);  

   find_cut_edges();


   gettimeofday(&end, 0);
	long seconds = end.tv_sec - begin.tv_sec;
	long microseconds = end.tv_usec - begin.tv_usec;
	double elapsed = seconds + microseconds*1e-6;
	printf("Total time= %f\n", elapsed);

   fp = fopen(args[2],"w");

   for(int i=0;i<n;i++)
   {
         fprintf(fp,"node: %d , dfs: %d\n",i ,dfs[i]);
   }
   fprintf(fp, "%lf\n", elapsed);
   fclose(fp);
   
   return 0;
}


void find_cut_edges()
{
 
   dfs = (int*)malloc(sizeof(int)*n); 
   for(int i=0;i<n;i++){dfs[i]=-1;}
   Find_cut_pairs(0);  
}

void Find_cut_pairs(int v)
{
   dfs[v]=Nr++;

   int* temp_vertex = (int*)malloc(sizeof(int)*n);
   int* temp_out = (int*)malloc(sizeof(int)*n);
   int stack_pointer=0;
   //temp_vertex stores vertices
   temp_vertex[0]=v; temp_out[0]=adjListAddress[v];   //temp_out stores adj_list addresses of vertices (address of vertex v for example)
   //            ^ add first vertex onto stack
   //             that is adding the root node into the stack

while(stack_pointer!=-1)//this is the DFS stack pointer, stack contains nodes
//
{  //set v to top vertex on stack
   v=temp_vertex[stack_pointer]; //DFS stack = temp_vertex
   char descend=0;   //boolean determines if algorithm should descend down the tree

   //1 iterate through adjacent vertices to vertex on top of stack
   for(int i=temp_out[stack_pointer];i<adjListAddress[v+1];i++)
   {
      int w=G[i]; //w is a vertex adjacent to v
      //1.1
      if(dfs[w]==-1)    //if it's not yet visited, assign dfs number
      {
         dfs[w]=Nr++;//add w to the top of the stack if it hasn't been assigned a DFS number yet
         temp_vertex[stack_pointer+1]=w; temp_out[stack_pointer+1]=adjListAddress[w];
		 
         temp_out[stack_pointer]=i;      //why is temp_out[stack_pointer] assigned with i when i already equals temp_out[stack_pointer]?
         descend=1; break;  //this line breaks out of the loop so that adjacent nodes to v stop being searched and instead
      }
   }

   if(descend){stack_pointer++; continue;}   //if the node has children to explore then descend to explore them
                                             //descending means to increase the stack pointer to traverse up the stack towards the top
                                             //since children get added on top of the stack, this is equivalent to descending down one branch

   stack_pointer--;  //when the vertex on top of the stack has been fully explored, decrement the stack pointer to backtrack during DFS 
}//end while

}//end find_cut_pairs



void create_adj()
{
   G = (int*)malloc(sizeof(int)*4*m);				//array of 4 ints per edges	each block represents 1 edge	0000'0000'0000'0000'0000 for m blocks
   adjListAddress = (int*)malloc(sizeof(int)*(n+1));		//array of ints. size = |V| + 1		0'0'0'0'0'0'0'0'0'0'0'0'0'0		why is there 1 extra int? and what does it represent?
   for(int i=0;i<=n;i++){adjListAddress[i]=0;}			//initialize adjListAddress to zeroes
   for(int i=0;i<m;i++){						//for every edge
												//note that the first element is skipped in the assignment for loop, so the first element is always 0
		adjListAddress[edges[2*i]+1]++;				//references each source vertex 's' in edge list and increments adjListAddress[s] by 1
		adjListAddress[edges[2*i+1]+1]++;				//references each target vertex 't' in edge list and increments adjListAddress[t] by 1
	}//this for loop records the degree of each vertex by reading how many times it appears as a target and as a source of an edge in the edge list.
	
   int* nextOut = (int*)malloc(sizeof(int)*(n+1));	//array of ints, 1 per vertex, but there is one more int here than there are vertices
   nextOut[0]=0;	//initialize only the first element to 0 (maybe every other element then represents every vertex and 0 is the extra 1 int)
   
   for(int i=1;i<=n;i++){ //for each vertex
		adjListAddress[i]+=adjListAddress[i-1]; //increase each vertex's degree by x where x is the previous vertex's degree, but the first one is increased by 0
		nextOut[i]=adjListAddress[i];		//copy adjListAddress into nextOut, this is because nextOut will be changed later, but adjListAddress will preserve the adj list offsets
   } //this is to set the offset in G for each vertex's adjacency list.
   
   
   /*
   each vertex v has degree i, this represents "how many vertices v is adjacent to"
   so the degree also represents how many spaces v's adjacency list needs.
   adjListAddress is first initalized to the degree of each vertex,
   then it is reassigned so that each vertex has the degree of the previous vertices added to it
   this is because adjListAddress[v] represents the offset of v's adjacency list in the greater array G which stores the adjacency lists
   so adjListAddress[v] represents the number of vertices v is adjacent to plus the number of vertices all previous vertices are adjacent to.
   G[adjListAddress[v]] gives us the index in G to access the beginning of the adjacency list of v.
   */
   
   for(int i=0;i<m;i++)
   {		//edges array has 2 ints per edge, each block represents 1 edge		00'00'00'00'00'00... for m blocks
			//clearly these 2 ints represent the number of the source vertex and the number of the target vertex of each edge
      int x=edges[2*i]; 	//read an edge into the variables x and y
	  int y=edges[2*i+1];	//(x, y) is the edge we are referencing
      G[nextOut[x]++]=y;	//access the adjacency list offset for vertex x and set the first adjacent vertex to y
							//then increment the adjacency list offset so that the next vertex that's added in the list
							//gets added at the end of the list, rather than at the beginning.
      G[nextOut[y]++]=x;	//repeat for the adjacency list of vertex y
							//this incrementing of the offsets in nextOut is the reason that we copied the list adjListAddress rather than modifying adjListAddress
							//because adjListAddress is needed to be used to access the beginnings of the adjacency lists of each vertex later in the program.
   }
}










