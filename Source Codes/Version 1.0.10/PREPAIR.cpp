
#define  MCF_LOWER_BOUNDS

//#define  MCF_FLOAT


#include "PREPAIR.h"

void MCF_switch_node_fixed_to_deleted(MCF_network_p net, MCF_node_p deleted_node)
{
    MCF_node_p  node, root;

    MCF_arc_p   arc;
    void       *stop;
    MCF_flow_t  lower, upper;
    double      tmp;
    MCF_cost_t  potential_from_root;

    long       cycle_ori;
    long       new_orientation;
    MCF_flow_t delta;
    MCF_flow_t new_flow;
    MCF_node_p iplus;
    MCF_node_p jplus;
    MCF_node_p iminus;
    MCF_node_p jminus;
    MCF_node_p w;
    MCF_arc_p  bea;
    MCF_cost_t sigma;

    MCF_arc_p  basic_arc_temp;
    MCF_arc_p  new_basic_arc;
    MCF_node_p father,child;
    MCF_node_p temp;
    MCF_node_p new_pred;
    long       orientation_temp;
    long       subtreesize_temp;
    long       subtreesize_iminus;
    long       new_subtreesize;
    MCF_flow_t flow_temp;

    temp = deleted_node;
    father = temp->pred;

    if( temp->child == NULL )
    {   // node 4,5,6,8 in the typical tree might be deleted
        if( !temp->right_sibling && !temp->left_sibling)
            father->child = NULL;
        if( temp->right_sibling ) // node 4 in the typical tree might be deleted
        {
            temp->right_sibling->left_sibling = NULL;
            // father->child = NULL ; // It can be set to something in the next instructions
            if(father->child == deleted_node)
               father->child = temp->right_sibling ;
        }

        if( temp->left_sibling ) // node 5 in the typical tree might be deleted
        {
            temp->left_sibling->right_sibling = NULL;
            // father->child = NULL ; // It can be set to something in the next instructions
            if(father->child == deleted_node)
               father->child = temp->left_sibling ;
        }
        w = net->nodes;
        for( temp = deleted_node->pred; temp != w; temp = temp->pred )
             temp->subtreesize -= 1;

        w->subtreesize -= 1;
    }else
    {  // node 3, 2, 1, 7 might be deleted
        temp = deleted_node->child;
        root = net->nodes ;
        while(temp)
        {
           temp->pred = root;
           // temp->right_sibling = NULL;
           temp->left_sibling = NULL;
           temp->basic_arc = net->dummy_arcs + (deleted_node->number - 1);
           temp->basic_arc->ident = MCF_BASIC;
           switch(temp->balance)
           {
              case  1: temp->orientation = MCF_UP;
                       temp->flow =  1;
                       break;
              case -1: temp->orientation = MCF_DOWN;
                       temp->flow = -1;
                       break;
           }
           if(!temp->right_sibling)
              break;
           temp = temp->right_sibling ;
        }
        temp->right_sibling = root->child; // Set right sibling of node 5 to 1 */
        root->child->left_sibling = temp ; // Set left  sibling of node 1 to 5 */

        temp = deleted_node;
        father = deleted_node->pred;
        if( temp->right_sibling ) // Node 6: Deleted node 3
        {
            temp->right_sibling->left_sibling = deleted_node->left_sibling;// NULL // change left sibling of 6
            if(father->child == deleted_node)
               father->child = temp->right_sibling ;  // change child of node 2 to 6
        }

        if( temp->left_sibling )  // Node 2: Deleted node 7
        {
            temp->left_sibling->right_sibling = deleted_node->right_sibling;// Change right sibling of 2
            if(father->child == deleted_node)
               father->child = temp->left_sibling ;  // change child of node 1 to 2
        }
        w = net->nodes;
        for( temp = deleted_node->pred; temp != w; temp = temp->pred )
             temp->subtreesize -= 1;

        w->subtreesize -= 1;

        /* Update of the node potentials */
        father = net->nodes ;
        RECURSION:
        temp = father->child;
        if( temp )
        {
        ITERATION:
            temp->potential = father->potential - temp->basic_arc->cost  ;
            father = temp;
            goto RECURSION;
        }
        temp = father->left_sibling;
        if( temp )
            goto ITERATION;
        temp = father->right_sibling;
        if( temp )
            goto ITERATION;
    }
}


void MCF_switch_node_inserted_to_fixed(MCF_network_p net, MCF_node_p inserted_node)
{
     MCF_node_p  node, root;

     MCF_arc_p   arc;
     void       *stop;
     MCF_flow_t  lower, upper;
     double      tmp;
     MCF_cost_t  potential_from_root;

    /* Initialize artificial node (called root) */
     root = node = net->nodes;
     potential_from_root = root->potential;

     arc = net->dummy_arcs + (inserted_node - root);

     inserted_node->basic_arc     = arc ;
     inserted_node->pred          = root;
     inserted_node->child         = NULL;

     node = inserted_node; node++;
     while(node->ident != MCF_FIXED ) node++;
     inserted_node->right_sibling = node ;

     node = inserted_node; node--;
     while(node->ident != MCF_FIXED) node--;
     inserted_node->left_sibling  = node ;

     inserted_node->subtreesize   = 1;

     arc->cost = MCF_MAX_ART_COST;
#ifdef MCF_LOWER_BOUNDS
     arc->lower = (MCF_flow_t)MCF_ZERO;
#endif
     arc->upper = 2*MCF_UNBOUNDED + 1; /* >max delta for a variable */

     inserted_node->flow += inserted_node->balance;

     if( inserted_node->flow >= 0 )
     { /* Make the artificial arc (i,0) active. */
        inserted_node->orientation = MCF_UP;
        inserted_node->potential = MCF_ZERO;
        arc->tail = inserted_node;
        arc->head = root;
        arc->ident = MCF_BASIC;
     }
     else
     {  /* Make the artificial arc (0,i) active. */
        inserted_node->orientation = MCF_DOWN;
        inserted_node->flow = -inserted_node->flow;
        inserted_node->potential = potential_from_root;
        arc->tail = root;
        arc->head = inserted_node;
        arc->ident = MCF_BASIC;
     }
     inserted_node->ident = MCF_FIXED;
}



long MCF_primal_repair_span_tree( MCF_network_p net, int Number_Of_AGVs )
{
    MCF_node_p  node, root;
    MCF_arc_p   arc;
    void       *stop;
    MCF_flow_t  lower, upper;
    double      tmp;
    MCF_cost_t  potential_from_root;

    node = net->nodes + Number_Of_AGVs +  1;
    for(stop = net->stop_nodes; node != stop; node++ )
    {
        switch(node->ident)
        {
           case MCF_FIXED:  // No Change
                break;
           case MCF_DELETED:
                if(node->pre_ident == MCF_DELETED)
                   break;
                else
                   MCF_switch_node_fixed_to_deleted(net, node);
                break;
           case MCF_INSERTED:
                MCF_switch_node_inserted_to_fixed(net, node);
                break;
        }
    }
    return 0;
}
