/* Dijkstra's single-source shortest path algorithm implemented 
   with a binary heap.  H.R. Andersen (DTU) 26/10-95 + 07/10-97 */

#include <stdio.h>
#include <stdlib.h>        /* exit()                         */
#include <string.h>        /* strcmp                         */
#include <time.h>          /* clock, clock_t, CLOCKS_PER_SEC */

#include <sys/time.h>
#include <sys/resource.h>

struct Entry {
  int node;
  float weight;
  struct Entry *next;
};

typedef struct Entry* EntryPtr;

EntryPtr *adj;
float    *d;
int      *h;    /* heap: h[i]=v, i: index in heap, v: node */
int      *hinv; /* the inverse: h[i]=v <=> hinv[v]=i */
int      last;  /* last elm. in heap */
int      *pi;
int      n, m;  /* # of vertices, edges */

#define infinity       1.8e308
#define MALLOC(t,n)    (t *) malloc( n*sizeof(t) ) 
#define CHECKPTR(p)    if (!p) Error("Out of memory")

void Error(char *text)
{ printf("\nERROR: %s\n", text);
  exit(1);
}

void Initialize()
{ int i;
  adj  = MALLOC(EntryPtr, n); CHECKPTR(adj );
  d    = MALLOC(float   , n); CHECKPTR(d   );
  h    = MALLOC(int     , n); CHECKPTR(h   );
  hinv = MALLOC(int     , n); CHECKPTR(hinv);
  pi   = MALLOC(int     , n); CHECKPTR(pi  );
  for (i=0; i<n; i++) adj[i] = NULL;
}

void InsertEdge(int u, int v, float w)
{ EntryPtr ptr;
  if (u<0 || u>=n || v<0 || v>=n) Error("Wrong vertex numbers");
  ptr = adj[u];
  adj[u] = (struct Entry*) malloc(sizeof(struct Entry));
  adj[u]->next = ptr;
  adj[u]->node = v;
  adj[u]->weight = w;
}

void LoadGraph(char* filename)
{ int u, v;
  float w;
  FILE *infile = fopen(filename,"r");
  if (!infile) Error("Could not open file");
  fscanf(infile,"%d %d", &n, &m);
  printf("Reading %s with %d vertices and %d edges \n", filename, n, m);
  Initialize(); 
  while (fscanf(infile,"%d %d %f",&u,&v,&w) != EOF) 
    InsertEdge(u, v, w);
}      

void InitializeSingleSource()
{ int i;
  for (i=0; i<n; i++) { d[i] = infinity; pi[i] = -1; }
  d[0] = 0.0; 
}

void InitializeQ() /* vertex i at location i */
{ int i;
  last = n;
  for (i=0; i<n; i++) { h[i] = i; hinv[i] = i; }
}

void swap(int v, int u)
{ int tmp = h[v];
  h[v] = h[u];
  hinv[h[v]] = v;
  h[u] = tmp;
  hinv[tmp] = u; 
}

void down(int v)
{ int l = 2*v+1;
  int r = 2*v+2;
  int vnew;
  if (l<last)
    { if (r<last)
	{ if (d[h[l]]<d[h[r]])  vnew=l;
	  else  vnew=r;
	}
      else vnew=l;
      if (d[h[vnew]]<d[h[v]])
	{ swap(v,vnew);
	  down(vnew);
	}
    }
}

void up(int v)
{ if (v>0)
    { int p = (v-1)/2; 
      if (d[h[p]]>d[h[v]]) { swap(p,v); up(p); }   
    }
}

int DeleteMinQ()
{ int u = h[0];
  last = last-1;
  swap(0,last);
  hinv[u] = -1;
  down(0);
  return u;
}

void Print_pi(int j)
{ if (j>0) 
    { Print_pi(pi[j]); 
      printf("%d ", j);
    }
}

void Relax(int u, int v, float w)
{ if (d[v] > d[u]+w) 
    { d[v] = d[u]+w;
      pi[v] = u;
      up(hinv[v]);
    }
}

/* Returns amount of CPU time used by the process in milliseconds */
long get_cpu_time()
{
  struct rusage rusage;
 
  getrusage ( RUSAGE_SELF, &rusage);
  return (rusage.ru_utime.tv_sec * 1000 + (rusage.ru_utime.tv_usec/1000)
          + rusage.ru_stime.tv_sec * 1000 + (rusage.ru_stime.tv_usec/1000) );
}

void main(int argc, char **argv)
{ int u;
  clock_t cl1, cl2;
  EntryPtr vptr;


  LoadGraph(argv[1]);

  cl1 = get_cpu_time();

  InitializeSingleSource();
  InitializeQ();
  while (last>0)
    { u = DeleteMinQ();

      vptr = adj[u];
      while (vptr)
	{ Relax(u,vptr->node,vptr->weight);
	  vptr=vptr->next;
	}
    }
  cl2 = get_cpu_time ();
  printf("Weight of shortest path from 0 to %d: %.1f\n", n-1, d[n-1]);
  printf("Path: %d ", 0); Print_pi(n-1);
  printf("\nTime: %d ms\n", (cl2-cl1));
}


 