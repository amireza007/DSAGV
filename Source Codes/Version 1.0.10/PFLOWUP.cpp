
#include "pflowup.h"


void MCF_primal_update_flow(
                 MCF_node_p iplus,
                 MCF_node_p jplus,
                 MCF_node_p w,
                 MCF_flow_t delta
                 )
{
    /* Update arc flows from iplus to w along the basis 
       path [iplus, root].
    */
    for( ; iplus != w; iplus = iplus->pred )
    {
        if( iplus->orientation )
            iplus->flow -= delta;
        else
            iplus->flow += delta;
    }

    /* Update arc flows from jplus to w along the basis 
       path [iplus, root]. 
    */
    for( ; jplus != w; jplus = jplus->pred )
    {
        if( jplus->orientation )
            jplus->flow += delta;
        else
            jplus->flow -= delta;
    }
}
