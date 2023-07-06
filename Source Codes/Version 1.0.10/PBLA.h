

#ifndef _PBLA_H
#define _PBLA_H


#include "mcfdefs.h"


MCF_node_p MCF_primal_iminus( MCF_flow_p delta, long *xchange,
                                     MCF_node_p iplus, MCF_node_p jplus,
                                     MCF_node_p *w
                                     );


#endif
