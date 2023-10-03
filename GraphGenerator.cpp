#include <stdio.h>
#include <stdlib.h>

long tolong(char*);

//params: nC lC nK lK three_edges seed output_file 
int main(int n_args, char** args)
{
   srand(tolong(args[6]));
   
   long nC = tolong(args[1]);
   long lC = tolong(args[2]);
   long nK = tolong(args[3]);
   long lK = tolong(args[4]);
   long three_edges = tolong(args[5]); 

   long n=nC*lC+nK*lK;
   long m=nC*lC+nK*(lK*(lK-1))/2+(2+three_edges)*(nC+nK-1);
  
   //shuffle nodes
   long* nodes = (long*)malloc(sizeof(long)*n);
   for(long i=0;i<n;i++){nodes[i]=i;}
   for(long i=0;i<n;i++)
   {
      long j=i+rand()%(n-i);
      long temp=nodes[i];
      nodes[i]=nodes[j];
      nodes[j]=temp;
   }   

   //shuffle types
   char* graph_type = (char*)malloc(sizeof(char)*(nC+nK));
   long indx=0;
   for(long i=0;i<nC;i++){graph_type[indx++]=0;}
   for(long i=0;i<nK;i++){graph_type[indx++]=1;}
   for(long i=0;i<nC+nK;i++)
   {
      long j=i+rand()%(nC+nK-i);
      char temp=graph_type[i];
      graph_type[i]=graph_type[j];
      graph_type[j]=temp;
   }

   //create subgraph edges
   long* edges = (long*)malloc(sizeof(long)*m*2);
   long edge_indx=0;
   long* startNode = (long*)malloc(sizeof(long)*(nC+nK));
   long currentNode=0;
   for(long i=0;i<nC+nK;i++)
   {
      startNode[i]=currentNode;
      if(graph_type[i]==0)
      {
         for(long j=0;j<lC;j++)
         {
            edges[edge_indx++]=nodes[currentNode+j];
            edges[edge_indx++]=nodes[currentNode+(j+1)%lC];
         }
         currentNode+=lC;  
      }
      else
      {
         for(long j=0;j<lK;j++)
         {
            for(long k=j+1;k<lK;k++)
            {
               edges[edge_indx++]=nodes[currentNode+j];
               edges[edge_indx++]=nodes[currentNode+k];
            }
         }
         currentNode+=lK; 
      }
   }

   //connect the subgraphs in a tree structure
   for(long i=1;i<nC+nK;i++)
   {
      long j=rand()%i;
      long mod1=lC, mod2=lC;
      if(graph_type[i]==1){mod1=lK;}
      if(graph_type[j]==1){mod2=lK;}
      if(!three_edges)
      {
         long x1,y1,x2,y2;
         x1=rand()%mod1;
         x2=(x1+(1+rand()%(mod1-2)))%mod1;
         y1=rand()%mod2;
         y2=(y1+(1+rand()%(mod2-2)))%mod2;
         edges[edge_indx++]=nodes[startNode[i]+x1]; edges[edge_indx++]=nodes[startNode[j]+y1]; 
         edges[edge_indx++]=nodes[startNode[i]+x2]; edges[edge_indx++]=nodes[startNode[j]+y2]; 
      }
      else
      {
         long x1,y1,x2,y2,x3,y3;
         if(mod1==3){x1=0;x2=1;x3=2;}
         else
         {
            x1=rand()%mod1;
            x2=(x1+(2+rand()%(mod1-3)))%mod1;
            x3=(x1+(1+rand()%((mod1+x2-x1-1)%mod1)))%mod1;
         }
         if(mod2==3){y1=0;y2=1;y3=2;}
         else
         {
            y1=rand()%mod2;
            y2=(y1+(2+rand()%(mod2-3)))%mod2;
            y3=(y1+(1+rand()%((mod2+y2-y1-1)%mod2)))%mod2;
         }
         edges[edge_indx++]=nodes[startNode[i]+x1]; edges[edge_indx++]=nodes[startNode[j]+y1]; 
         edges[edge_indx++]=nodes[startNode[i]+x2]; edges[edge_indx++]=nodes[startNode[j]+y2]; 
         edges[edge_indx++]=nodes[startNode[i]+x3]; edges[edge_indx++]=nodes[startNode[j]+y3]; 
      }
   }

   //shuffle edges
   for(long i=0;i<m;i++)
   {
      long j=i+rand()%(m-i);
      long tempx=edges[2*i]; long tempy=edges[2*i+1];
      edges[2*i]=edges[2*j]; edges[2*i+1]=edges[2*j+1];
      if(rand()%2==0)
      {
         edges[2*j]=tempx; edges[2*j+1]=tempy;   
      }
      else
      {
         edges[2*j]=tempy; edges[2*j+1]=tempx;   
      }
   }

   //print graph
   FILE* fp = fopen(args[7],"w");
   fprintf(fp,"%ld %ld\n",n,m);
   for(long i=0;i<m;i++)
   {
      fprintf(fp,"%ld %ld\n",edges[2*i]+1,edges[2*i+1]+1);
   }
   fclose(fp);

   return 0;
}

long tolong(char* x)
{
   long ret=0;
   long l=0;
   while(x[l]!=0){l++;}
   l--;
   long d=1;
   while(l!=-1){ret+=(x[l]-'0')*d; d*=10; l--;}
   return ret;
}
