/* Dijkstra's single-source shortest path algorithm implemented
   with a binary heap.   */

#include <stdio.h>
#include <stdlib.h>        /* exit()                         */
#include <string.h>        /* strcmp                         */
#include <alloc.h>

#define DIJKSTRA_DEBUG

struct   GEntry {
  int    node;
  int    weight;
  struct GEntry *next;
};

typedef struct GEntry* GEntryPtr;

GEntryPtr *DIJ_Adj;
int       *DIJ_Distance;
int       *DIJ_Heap;    /* heap: h[i]=v, i: index in heap, v: node */
int       *DIJ_HeapInv; /* the inverse: h[i]=v <=> hinv[v]=i */
int        DIJ_Last;    /* last elm. in heap */
int       *DIJ_Gpi;
int        DIJ_NumberOfNodes, DIJ_NumberOfEdges;  /* # of vertices, edges */

#define MaxInt         (int)(2147483647)
#define infinity       MaxInt
#define MALLOC(t,n)    (t *) malloc( n*sizeof(t) )
#define CHECKPTR(p)    if (!p) Error("Out of memory")

void Error(char *text)
{
  printf("\nERROR: %s\n", text);
 // ShowMessage("Not Enough Memory !! ", "Error", MB_OK);
  // exit(1);
}

void DIJ_Initialize()
{
  int  i;
  DIJ_Adj         = MALLOC(GEntryPtr, DIJ_NumberOfNodes ); CHECKPTR(DIJ_Adj );
  DIJ_Distance    = MALLOC(int      , DIJ_NumberOfNodes ); CHECKPTR(DIJ_Distance   );
  DIJ_Heap        = MALLOC(int      , DIJ_NumberOfNodes ); CHECKPTR(DIJ_Heap   );
  DIJ_HeapInv     = MALLOC(int      , DIJ_NumberOfNodes ); CHECKPTR(DIJ_HeapInv);
  DIJ_Gpi         = MALLOC(int      , DIJ_NumberOfNodes ); CHECKPTR(DIJ_Gpi );

  for (i=0; i<DIJ_NumberOfNodes ; i++)
      DIJ_Adj[i] = NULL;
}

void DIJ_InsertEdge(int u, int v, int w)
{
  GEntryPtr ptr;

//  if (u<0 || u>=DIJ_NumberOfNodes  || v<0 || v>=DIJ_NumberOfNodes )
 //     Error("Wrong vertex numbers");

  ptr = DIJ_Adj[u];
  DIJ_Adj[u] = (struct GEntry*) malloc(sizeof(struct GEntry));
  DIJ_Adj[u]->next = ptr;
  DIJ_Adj[u]->node = v;
  DIJ_Adj[u]->weight = w;
}

void DIJ_LoadGraph()
{
  int u, v;
  int w;
  FILE *infile = fopen("graph3.txt","r");
  if (!infile) Error("Could not open file");
  fscanf(infile,"%d %d", &DIJ_NumberOfNodes , &DIJ_NumberOfEdges);
  printf("Reading %s with %d vertices and %d edges \n", "Graph3.txt", DIJ_NumberOfNodes , DIJ_NumberOfEdges);
  DIJ_Initialize();
  while (fscanf(infile,"%d %d %d",&u,&v,&w) != EOF)
    DIJ_InsertEdge(u, v, w);
}

void DIJ_InitializeSingleSource()
{
  int i;
  for (i=0; i<DIJ_NumberOfNodes ; i++)
  {
     DIJ_Distance[i] = infinity;
     DIJ_Gpi[i] = -1;
  }
  DIJ_Distance[0] = 0.0;
}

void DIJ_InitializeQ() /* vertex i at location i */
{
  int i;
  DIJ_Last = DIJ_NumberOfNodes ;
  for (i=0; i<DIJ_NumberOfNodes ; i++)
  {
      DIJ_Heap[i] = i;
      DIJ_HeapInv[i] = i;
  }
}

void DIJ_Swap(int v, int u)
{
  int tmp = DIJ_Heap[v];
  DIJ_Heap[v] = DIJ_Heap[u];
  DIJ_HeapInv[DIJ_Heap[v]] = v;
  DIJ_Heap[u] = tmp;
  DIJ_HeapInv[tmp] = u;
}

void DIJ_Down(int v)
{
  int l = 2*v+1;
  int r = 2*v+2;
  int vnew;
  if (l < DIJ_Last)
  {
    if (r < DIJ_Last)
    {
      if (DIJ_Distance[DIJ_Heap[l]]<DIJ_Distance[DIJ_Heap[r]])
         vnew=l;
      else
         vnew=r;
    }else
      vnew=l;

    if (DIJ_Distance[DIJ_Heap[vnew]]<DIJ_Distance[DIJ_Heap[v]])
    {
         DIJ_Swap(v,vnew);
         DIJ_Down(vnew);
    }
  }
}

void DIJ_Up(int v)
{
  if (v>0)
  {
    int p = (v-1)/2;
    if (DIJ_Distance[DIJ_Heap[p]] > DIJ_Distance[DIJ_Heap[v]] )
    {
       DIJ_Swap(p,v);
       DIJ_Up(p);
    }
  }
}

int DIJ_DeleteMinQ()
{
  int u = DIJ_Heap[0];
  DIJ_Last = DIJ_Last - 1;
  DIJ_Swap(0,DIJ_Last);
  DIJ_HeapInv[u] = -1;
  DIJ_Down(0);
  return u;
}

void DIJ_Print_pi(int j,FILE *fp)
{
  #if defined DIJKSTRA_DEBUG
  if (j>0)
  {
        DIJ_Print_pi(DIJ_Gpi[j],fp);
        fprintf(fp," ---> %d ", j);
  }
  #endif
}

void DIJ_Relax(int u, int v, int w)
{
  if (DIJ_Distance[v] > DIJ_Distance[u]+w)
  {
    DIJ_Distance[v] = DIJ_Distance[u]+w;
    DIJ_Gpi[v] = u;
    DIJ_Up(DIJ_HeapInv[v]);
  }
}

void  DIJ_Free()
{
  free(DIJ_Adj );
  free(DIJ_Distance   );
  free(DIJ_Heap   );
  free(DIJ_HeapInv);
  free(DIJ_Gpi );
}
/*
void Main_Dijkstra()
{
  int u;
  GEntryPtr vptr;

#if defined DIJKSTRA_DEBUG
    FILE *temp_out;

    temp_out = fopen( "dij.tmp", "w" );
    fprintf(temp_out,"\n");
    fclose(temp_out);
#endif

  DIJ_LoadGraph();

  DIJ_InitializeSingleSource();
  DIJ_InitializeQ();

  while (DIJ_Last > 0)
  {
     u = DIJ_DeleteMinQ();

     vptr = DIJ_Adj[u];
     while (vptr)
     {
       DIJ_Relax(u,vptr->node,vptr->weight);
       vptr=vptr->next;
     }
  }
  #if defined DIJKSTRA_DEBUG
      int Dest = 8; // DIJ_NumberOfNodes-1;
      temp_out = fopen( "dij.tmp", "a+t" );
      fprintf(temp_out,"Shortest Path (0--->%d) = %d\n", Dest , DIJ_Distance [Dest] );
      fprintf(temp_out,"Path: %d ", 0);
      DIJ_Print_pi(Dest,temp_out);
      fclose(temp_out);
  #endif
  DIJ_Free();
}
*/
