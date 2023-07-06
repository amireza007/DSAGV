
#ifndef _TREEUP_H
#define _TREEUP_H


#include "mcfdefs.h"


void MCF_update_tree(
     long cycle_ori,
     long new_orientation,
     MCF_flow_t delta,
     MCF_flow_t new_flow,
     MCF_node_p iplus,
     MCF_node_p jplus,
     MCF_node_p iminus,
     MCF_node_p jminus,
     MCF_node_p w,
     MCF_arc_p bea,
     MCF_cost_t sigma
);


#endif
