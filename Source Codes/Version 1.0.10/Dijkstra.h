#include "stdio.h"

struct   GEntry {
  int    node;
  int    weight;
  struct GEntry *next;
};

typedef struct GEntry* GEntryPtr;

extern GEntryPtr *DIJ_Adj;
extern int       *DIJ_Distance;
extern int       *DIJ_Heap;    /* heap: h[i]=v, i: index in heap, v: node */
extern int       *DIJ_HeapInv; /* the inverse: h[i]=v <=> hinv[v]=i */
extern int        DIJ_Last;    /* last elm. in heap */
extern int       *DIJ_Gpi;
extern int        DIJ_NumberOfNodes, DIJ_NumberOfEdges;  /* # of vertices, edges */

void DIJ_Initialize();
void DIJ_InsertEdge(int u, int v, int w);
void DIJ_LoadGraph();
void DIJ_InitializeSingleSource();
void DIJ_InitializeQ();  /* vertex i at location i */
void DIJ_Swap(int v, int u);
void DIJ_Down(int v);
void DIJ_Up(int v);

extern int DIJ_DeleteMinQ();

void DIJ_Print_pi(int j,FILE *fp);
void DIJ_Relax(int u, int v, int w);
void Main_Dijkstra();
void DIJ_Free();








 