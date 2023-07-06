#include  <stdio.h>
#include  <math.h>
#include  <iomanip.h>
#include  <stdlib.h>
#include  <search.h>
#include  <time.h>
#include  <algorithm>
#include  <functional>
#include  <string.h>
#include  <conio.h>
#include  "pbeadef.h"

#define K 200
#define B 20



static void sort_basket( long min, long max );


static long nr_group;
static long group_pos;
static long group_off;
static long first_group;


typedef struct basket
{
    MCF_arc_p a;
    MCF_cost_t cost;
    MCF_cost_t abs_cost;
} BASKET;


static long basket_size;
static BASKET basket[B+K+1];
static BASKET *perm[B+K+1];

static long initialize = 1;
static MCF_arc_p arc_base = (MCF_arc_p)NULL;
static MCF_arc_p arc_end  = (MCF_arc_p)NULL;

typedef int (*Comp_Function)(const void*, const void*);
int Compare_perm(struct basket *elem1, struct basket *elem2)
{
    if ( elem1->abs_cost > elem2->abs_cost)
         return -1;
    else if (elem1->abs_cost < elem2->abs_cost)
             return 1;
         else
             return 0;
}



#define CATT(k,b) MCF_reset_mpp_module_ ## k ## _ ## b
#define RESET(x,y) CATT(x,y)

void RESET(K,B)( )
{
    initialize = 1;
}

#define CAT(k,b) MCF_primal_bea_mpp_ ## k ## _ ## b
#define BEAMPP(x,y) CAT(x,y)

MCF_arc_p BEAMPP(K,B)( MCF_DECLARE_PRIMAL_BEA_PARAMETERS )
{
    long i, next, old_group_pos;
    MCF_arc_p arc;
    MCF_cost_t red_cost;

    if( initialize )
    {
        for( i=1; i < K+B+1; i++ )
            perm[i] = &(basket[i]);
        nr_group = ( (m-1) / K ) + 1;

        switch(algorithm_opt )
        {
            case 1: randomize();
                    group_pos = random(nr_group);
                    break;
            case 2: group_pos = (long ) (max_cost_arc - arcs ) / K ;
                    break;
            case 0: group_pos = 0;
                    group_off   = 0;
        }

        basket_size = 0;
        arc_base = arcs;
        arc_end  = stop_arcs;
        initialize = 0;
    }
    else if( arc_base != arcs || arc_end > stop_arcs )
    {
        putch(7); // This block can be deleted beacuse it will not have any entry.
        for( i=1; i < K+B+1; i++ )
            perm[i]->a = (MCF_arc_p)NULL;
        nr_group = ( (m-1) / K ) + 1;
        group_pos = 0;
        basket_size = 0;
    }
    else
    {
        for( i = 2, next = 0; i <= B && i <= basket_size; i++ )
        {
            arc = perm[i]->a;
            MCF_COMPUTE_RED_COST( red_cost );
            if( MCF_DUAL_INFEAS( red_cost ) )
            {
                next++;
                perm[next]->a = arc;
                perm[next]->cost = red_cost;
                perm[next]->abs_cost = MCF_ABS(red_cost);
            }
        }
        basket_size = next;
    }

    old_group_pos = group_pos;

 //   if( algorithm_opt )
 //      group_pos = random(nr_group); //  9 May 2004

NEXT:
    /* price next group */
   /*
    switch(algorithm_opt )
    {
        case 1: arc = arcs + group_pos + K / (iteration + 1);
                break;
        case 0: arc = arcs + group_pos;
    }
   */
    arc = arcs + group_pos;
    for( ; arc < stop_arcs ; arc += nr_group )
    {
        if( arc->ident > MCF_BASIC )
        {
            MCF_COMPUTE_RED_COST( red_cost );
            if( MCF_DUAL_INFEAS( red_cost) )
            {
                basket_size++;
                perm[basket_size]->a = arc;
                perm[basket_size]->cost = red_cost;
                perm[basket_size]->abs_cost = MCF_ABS(red_cost);
            }
        }
    }

    if( ++group_pos == nr_group )
        group_pos = 0;

    if( basket_size < B  && group_pos != old_group_pos )
        goto NEXT;

    if( basket_size == 0 )
    {
        initialize = 1;
        *red_cost_of_bea = 0;
        return NULL;
    }

     sort_basket( 1, basket_size );

   //  qsort ((void *) perm[1], basket_size, sizeof(struct basket),(Comp_Function ) Compare_perm);

    *red_cost_of_bea = perm[1]->cost;
    return( perm[1]->a );
}


static void sort_basket( long min, long max )
{
    register long l, r;

    register MCF_cost_t cut;
    register BASKET *xchange;

    l = min; r = max;

    cut = perm[ (long)( (l+r) / 2 ) ]->abs_cost;

    do
    {
        while( perm[l]->abs_cost > cut )
            l++;
        while( cut > perm[r]->abs_cost )
            r--;

        if( l < r )
        {
            xchange = perm[l];
            perm[l] = perm[r];
            perm[r] = xchange;
        }
        if( l <= r )
        {
            l++; r--;
        }

    }
    while( l <= r );

    if( min < r )
        sort_basket( min, r );
    if( l < max && l <= B )
        sort_basket( l, max );
}
