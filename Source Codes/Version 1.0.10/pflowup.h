

#ifndef _PFLOWUP_H
#define _PFLOWUP_H


#include "mcfdefs.h"

void MCF_primal_update_flow(
                 MCF_node_p iplus,
                 MCF_node_p jplus,
                 MCF_node_p w,
                 MCF_flow_t delta
                 );

// MCF_primal_update_flow( MCF_node_p, MCF_node_p, MCF_node_p,MCF_flow_t );


#endif
