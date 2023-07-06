

#ifndef _PBEADEF_H
#define _PBEADEF_H


#define MCF_PRIMAL_BEA_PARAMETERS \
    MCF_arc_p , \
    int  , \
    long , \
    MCF_arc_p, \
    MCF_arc_p, \
    MCF_cost_p

#define MCF_DECLARE_PRIMAL_BEA_PARAMETERS \
    MCF_arc_p max_cost_arc  , \
    int  algorithm_opt, \
    long m, \
    MCF_arc_p arcs, \
    MCF_arc_p stop_arcs, \
    MCF_cost_p red_cost_of_bea



#define MCF_COMPUTE_RED_COST( y ) \
      y = arc->cost - arc->tail->potential + arc->head->potential



#define MCF_DUAL_INFEAS( c ) \
    ( c < -MCF_ZERO_EPS && arc->ident == MCF_AT_LOWER ) \
 || ( c > MCF_ZERO_EPS && arc->ident == MCF_AT_UPPER )



#define MCF_CMP_ASSIGN_BEST( c, best, abs_best, arc_index ) \
{ \
    if( c + abs_best < -MCF_ZERO_EPS && \
            (arc->ident == MCF_AT_LOWER || arc->ident == MCF_AT_ZERO) ) \
    { \
        best = c; \
        abs_best = MCF_ABS(c); \
        arc_index = arc; \
    } \
    else if( c - abs_best > MCF_ZERO_EPS && \
            (arc->ident == MCF_AT_UPPER || arc->ident == MCF_AT_ZERO) ) \
    { \
        best = c; \
        abs_best = MCF_ABS(c); \
        arc_index = arc; \
    } \
}



#define MCF_IMPROVE( vertex, first_list, next_list ) \
{ \
      for( arc = j->vertex->first_list; arc; arc = arc->next_list ) \
      if( arc->ident > MCF_BASIC ) \
      { \
           MCF_COMPUTE_RED_COST( red_cost ); \
           MCF_CMP_ASSIGN_BEST( red_cost, most_neg, abs_most_neg, bea ); \
      } \
}

#include "mcfdefs.h"

#endif /* _PBEADEF_H */
