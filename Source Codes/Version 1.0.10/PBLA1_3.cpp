
#define  MCF_LOWER_BOUNDS

#include "PBLA.h"



MCF_node_p MCF_primal_iminus(MCF_flow_p delta,
                             long       *xchange,
                             MCF_node_p iplus,
                             MCF_node_p jplus,
                             MCF_node_p *w     )
{
    MCF_node_p iminus = NULL;
    MCF_flow_t temp;


    /* There are two paths from iplus and jplus to the root.
       The first common node of these two paths and an arc on the
       cycle [iplus,root]+[jplus,root]-([iplus,root]*[jplus,root]
       will be determined, "*" means the average of the two
       paths.
        */
    while( iplus != jplus )
    {
        if( iplus->subtreesize < jplus->subtreesize )
        {  /* Proceed in path from iplus to root. */
            if( iplus->orientation )
            {
#ifdef MCF_LOWER_BOUNDS
                temp = iplus->basic_arc->lower;
                if( temp > -MCF_UNBOUNDED )
                {
                    temp = iplus->flow - temp;
                    if( *delta > temp )
                    {
                      iminus = iplus;
                      *delta = temp;
                      *xchange = 0 ;
                    }
                }
#else
                temp = iplus->flow;
                if( *delta > temp )
                {
                      iminus = iplus;
                      *delta = temp;
                      *xchange = 0 ;
                }
#endif
            }
            else
            {
                temp = iplus->basic_arc->upper;
                if( temp < MCF_UNBOUNDED )
                {
                    temp -= iplus->flow;
                    if( *delta > temp )
                    {
                      iminus = iplus;
                      *delta = temp;
                      *xchange = 0 ;
                    }
                }
            }
            iplus = iplus->pred;
        }
        else
        {  /* Proceed in path from jplus to root. */
            if( jplus->orientation )
            {
                temp = jplus->basic_arc->upper;
                if( temp < MCF_UNBOUNDED )
                {
                    temp -= jplus->flow;
                    if( *delta >= temp )
                    {
                      iminus = jplus;
                      *delta = temp;
                      *xchange = 1 ;
                   }
                }
            }
            else
            {
#ifdef MCF_LOWER_BOUNDS
                temp = jplus->basic_arc->lower;
                if( temp > -MCF_UNBOUNDED )
                {
                    temp = jplus->flow - temp;
                    if( *delta >= temp )
                    {
                      iminus = jplus;
                      *delta = temp;
                      *xchange = 1 ;
                   }
                }
#else
                temp = jplus->flow;
                if( *delta >= temp )
                {
                      iminus = jplus;
                      *delta = temp;
                      *xchange = 1 ;
                }
#endif
            }
            jplus = jplus->pred;
        }
    }

    *w = iplus;

    return iminus;
}
