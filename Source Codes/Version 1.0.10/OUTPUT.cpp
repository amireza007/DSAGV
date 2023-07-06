
#define  MCF_LOWER_BOUNDS
//#define  MCF_FLOAT

#include "output.h"
#include "stdio.h"
#include "stdlib.h"

double MCF_primal_obj( MCF_network_p net );

#define RETURN_MSG( message ) \
    { \
        if( out ) fclose( out ); \
        fprintf( stderr, "write_solution: %s", message ); \
        return( 1 ); \
    }


long MCF_write_solution(
                   char *infile,
                   char *outfile,
                   MCF_network_p net,
                   time_t sec )
{
    FILE *out = NULL;
    MCF_arc_p arc;
    char tmpbuf[128];

    if( !strcmp( infile, outfile ) )
    {
        if(( out = fopen( outfile, "at" )) == NULL )
            RETURN_MSG( "can't open output file\n" );
    }
    else if(( out = fopen( outfile, "wt" )) == NULL )
        RETURN_MSG( "can't open output file\n" );

    _strtime( tmpbuf );
    fprintf( out, "c OS time:\t %s  \n", tmpbuf);
    fprintf( out, "c Output to Minimum Cost Flow (MCF) Problem.\n", infile );
    switch(net->algorithm_opt)
    {
        case 0: fprintf( out, "c The problem has been solved by Network Simplex Algorithm (NSA). \nc\n" );
                break;
        case 2:        
        case 1: fprintf( out, "c The problem has been solved by Network Simplex Algorithm (NSA+). \nc\n" );
                break;
    }
    if( net->primal_unbounded )
        fprintf( out, "c The Problem is Unbounded!\n" );
    else if( net->dual_unbounded )
        fprintf( out, "c The Dual problem is Unbounded!\n" );
    else if( !(net->feasible) )
        fprintf( out, "c It Couldn't find a Feasible Flow\n" );
    else
    {
        fprintf( out, "c It needed %ld Iteration(s) in ", net->iterations );
        fprintf( out, "%ld Second(s).\n", sec );

        fprintf( out, "s Optimal Objective Function is %0.0f\n", net->optcost );


        /* Write flow values to output file. */
        fprintf( out, "c Flow       From         To         Amount \n" );
        fprintf( out, "c------------------------------------------ \n" );
        for( arc = net->arcs; arc != net->stop_arcs; arc++ )
        {
            if(arc->ident == MCF_DELETED) continue; // DNSA
            if( MCF_ABS(arc->flow) > (MCF_flow_t)MCF_ZERO_EPS )
#ifdef MCF_FLOAT
                fprintf( out, "f   % 10.10ld    % 10.10ld   % 15.2f\n", arc->tail->number,
                        arc->head->number, arc->flow );
#else
                fprintf( out, "f   % 10.0ld    % 10.0ld   % 10.0ld\n", arc->tail->number,
                        arc->head->number, arc->flow );
#endif
        }
    }

    fprintf( out, "c\nc All other variables are zero\n" );

    fclose(out);

    return 0;
}

long MCF_write_intermediate(MCF_network_p net)
{
    FILE *temp_out;
    MCF_arc_p arc;

    MCF_node_p node, root;
    void *stop;


    if(( temp_out = fopen( "tmp.tmp", "a+t" )) == NULL )
        return -1;
    fprintf(temp_out,"\n============Iteration=%d============\n",net->iterations);

  /*  fprintf(temp_out,"n=%d , m=%d , primal_unbounded=%d, dual_unbounded=%d,feasible=%d,optcost=%d,nodes=%d,stop_nodes=%d,arcs=%d,dummy_arcs=%d,stop_dummy=%d,iterations=%d, find_iminus=%d, find_bea=%d \n",
            net->n, net->m,
            net->primal_unbounded ,net->dual_unbounded,
            net->feasible , net->optcost , net->nodes,
            net->stop_nodes , net->arcs , net->dummy_arcs, net->stop_dummy ,
            net->iterations , net->find_iminus, net->find_bea );
    */
        /* Write flow values to output file. */
 /*   for( arc = net->arcs; arc != net->stop_arcs; arc++ )
    {
          fprintf(temp_out,"arc=%2d,tail->number=%d, head->number=%d,tail=%d, head=%10d,nextout=%10d, nextin=%10d,cost=%d , upper=%d , lower=%d ,flow=%d , ident=%d \n",
                  arc->tail->number, arc->head->number ,
                  arc->tail , arc->head,
                  arc->nextout, arc->nextin,
                  arc->cost , arc->upper , arc->lower , arc->flow , arc->ident);
    }
   */                                                    //   12345678901 1234567890
    fprintf(temp_out,"(t,h) | Status |    Tail   |    Head   |  Cost     | Reduced  |\n");
    fprintf(temp_out,"      |        | Potential | Potential |           |   Cost   |\n");
    fprintf(temp_out,"------|--------|-----------|-----------|-----------|----------|\n");

    root = node = net->nodes;
    stop = net->stop_nodes;
    for( arc = net->dummy_arcs; node < (MCF_node_p)stop; arc++, node++ )
    {
        float red_cost = (double)(arc->cost) - (double)(arc->tail->potential)
                 + (double)(arc->head->potential);
        switch( arc->ident )
        {
            case MCF_BASIC:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, B=%1d, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );
                 break;
           case MCF_AT_ZERO:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, Z=%1d, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );
                 break;
           case MCF_AT_LOWER:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, L=%1d, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );

                 break;
           case MCF_AT_UPPER:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, U=%1d, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );
                 break;
        }
    /*   node->basic_arc = arc;
       node->pred = root;
       node->child = NULL;
       node->right_sibling = node + 1;
       node->left_sibling = node - 1;
       node->subtreesize = 1;
       node->flow = node->balance;
       node->orientation = MCF_UP;
       node->potential = MCF_ZERO;

       arc->cost = MCF_MAX_ART_COST;
       arc->lower = (MCF_flow_t)MCF_ZERO;
       arc->tail = node;
       arc->head = root;
       arc->ident = MCF_BASIC;
      */
    }

    for( arc = net->arcs; arc < net->stop_arcs; arc++ )
    {
        float red_cost = (double)(arc->cost) - (double)(arc->tail->potential)
                 + (double)(arc->head->potential);
        switch( arc->ident )
        {
            case MCF_BASIC:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, B=%1d, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );
                 break;
           case MCF_AT_ZERO:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, Z=%ld, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );
                 break;
           case MCF_AT_LOWER:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, L=%ld, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );

                 break;
           case MCF_AT_UPPER:
                 fprintf(temp_out,"t->n=%2d, h->n=%2d, U=%ld, l=%1d, u=%10d, f=%3d, t->P=%10d,  h->P=%10d, cost=%10d, red_cost=%+0.0f\n",
                               arc->tail->number, arc->head->number, arc->ident, arc->lower, arc->upper, arc->flow, arc->tail->potential, arc->head->potential, arc->cost, red_cost );
                 break;
        }
    }

    fclose(temp_out);

    return 0;
}

long MCF_write_intermediate2(MCF_network_p net)
{
    FILE *temp_out;
    MCF_arc_p arc;

    MCF_node_p node, root;

    double Cost;
    void  *stop;
    int   i;


    if(( temp_out = fopen( "tmp.tmp", "a+t" )) == NULL )
        return -1;
    Cost = MCF_primal_obj(net);
    fprintf(temp_out,"|--------------Iteration=%5d, Objective Function: %12.0f  ------------|\n",net->iterations,Cost);

    fprintf(temp_out,"Current Nodes Number = ", node->number);
    for( node = net->nodes, i = 0; i <= net->n; i++, node++ )
    {
      if(node->ident != MCF_DELETED )
         fprintf(temp_out," %d ,", node->number);
    }
    fprintf(temp_out,"\n");

                 //   1234567|123456|12345678901|12345678901|12345678901|12345678901|123456
    fprintf(temp_out," (t,h) |Status|    Tail   |    Head   |  Cost     | Reduced   | Arc  \n");
    fprintf(temp_out,"       |B = T | Potential | Potential |           |   Cost    | flow \n");
    fprintf(temp_out,"-------|------|-----------|-----------|-----------|-----------|------\n");

    root = node = net->nodes;
    stop = net->stop_nodes;
    int n = 0;
 //   for( arc = net->dummy_arcs; node < (MCF_node_p)stop; arc++, node++ )
    for( arc = net->dummy_arcs; n < net->n; arc++, n++)
    {
        if(arc->ident == MCF_DELETED ) continue; // DNSA
        float red_cost = (double)(arc->cost) - (double)(arc->tail->potential)
                 + (double)(arc->head->potential);
        switch( arc->ident )
        {
            case MCF_INSERTED:               // 123456
                 fprintf(temp_out,"(%2d,%2d)|  I   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost, arc->flow);
                 break;
            case MCF_DELETED:               // 123456
                 fprintf(temp_out,"(%2d,%2d)|  D   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost, arc->flow);
                 break;
            case MCF_BASIC:               // 123456
                 fprintf(temp_out,"(%2d,%2d)|  B   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost, arc->flow);
                 break;
           case MCF_AT_ZERO:
                 fprintf(temp_out,"(%2d,%2d)|  Z   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost, arc->flow );
                 break;
           case MCF_AT_LOWER:
                 fprintf(temp_out,"(%2d,%2d)|  L   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost, arc->flow );

                 break;
           case MCF_AT_UPPER:
                 fprintf(temp_out,"(%2d,%2d)|  U   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost, arc->flow );
                 break;
        }
    }

    for( arc = net->arcs; arc < net->stop_arcs; arc++ )
    {
        if(arc->ident == MCF_DELETED ) continue; // DNSA
        float red_cost = (double)(arc->cost) - (double)(arc->tail->potential)
                 + (double)(arc->head->potential);
        switch( arc->ident )
        {
            case MCF_INSERTED:               // 123456
                 fprintf(temp_out,"(%2d,%2d)|  I   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost,arc->flow  );
                 break;
            case MCF_DELETED:               // 123456
                 fprintf(temp_out,"(%2d,%2d)|  D   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost,arc->flow  );
                 break;
            case MCF_BASIC:               // 123456
                 fprintf(temp_out,"(%2d,%2d)|  B   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost,arc->flow  );
                 break;
           case MCF_AT_ZERO:
                 fprintf(temp_out,"(%2d,%2d)|  Z   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost,arc->flow  );
                 break;
           case MCF_AT_LOWER:
                 fprintf(temp_out,"(%2d,%2d)|  L   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost,arc->flow  );

                 break;
           case MCF_AT_UPPER:
                 fprintf(temp_out,"(%2d,%2d)|  U   |%11d|%11d|%11d|%+11.0f|%6d\n",
                               arc->tail->number, arc->head->number,arc->tail->potential, arc->head->potential, arc->cost, red_cost,arc->flow  );
                 break;
        }
    }

    fclose(temp_out);

    return 0;
}

long MCF_write_intermediate3(MCF_network_p net)
{
    FILE *temp_out;
    MCF_arc_p arc;

    MCF_node_p node, root;

    double Cost;
    void  *stop;
    int   i;


    if(( temp_out = fopen( "tmp.tmp", "a+t" )) == NULL )
        return -1;
    Cost = MCF_primal_obj(net);
    fprintf(temp_out,"|---------------Iteration=%5d, Objective Function: %12.0f  ----------------------|\n",net->iterations,Cost);

                 //   123456|123456|123456|12345|12345|1234567|1234567|1234567|12345|123456|12345678901|1234567|890|
    fprintf(temp_out," Node |  pre |      | Pred|Child|Right  |Left   |Subtree|Flow |Orien-| Potential | Basic |\n");
    fprintf(temp_out,"Number|Ident | Ident|     |     |Sibling|Sibling|Size   |     |tation|           |( h, t)|\n");
    fprintf(temp_out,"------|------|------|-----|-----|-------|-------|-------|-----|------|-----------|-------|\n");

    for( node = net->nodes, i = 0; i <= net->n; i++, node++ )
    {
         if(node->ident == MCF_DELETED ) continue; // DNSA

         fprintf(temp_out," %4d |", node->number);
         switch(node->pre_ident )
         {
              case MCF_FIXED:
                   fprintf(temp_out,"  FIX |");
                   break;
              case MCF_DELETED:
                //   fprintf(temp_out,"  DEL |");
                   fprintf(temp_out,"  --- |");
                   break;
              case MCF_INSERTED:
                   fprintf(temp_out,"  INS |");
                   break;
         }
         switch(node->ident )
         {
              case MCF_FIXED:    /*  123456 */
                   fprintf(temp_out,"  FIX |");
                   break;
              case MCF_DELETED:
                 //  fprintf(temp_out,"  DEL |");
                   fprintf(temp_out,"  --- |");
                   break;
              case MCF_INSERTED:
                   fprintf(temp_out,"  INS |");
                   break;
         }
         if(node->pred)
            fprintf(temp_out," %3d |",node->pred->number );
         else
            fprintf(temp_out,"-----|");

         if(node->child)
            fprintf(temp_out," %3d |",node->child->number );
         else
            fprintf(temp_out,"-----|");

         if(node->right_sibling)
            fprintf(temp_out," %5d |",node->right_sibling->number );
         else
            fprintf(temp_out,"-------|");

         if(node->left_sibling)
            fprintf(temp_out," %5d |",node->left_sibling->number );
         else
            fprintf(temp_out,"-------|");

         fprintf(temp_out," %5d |", node->subtreesize     );
         fprintf(temp_out," %3d |", node->flow    );
         switch(node->orientation )
         {
              case MCF_UP:         //123456
                   fprintf(temp_out," UP   |");
                   break;
              case MCF_DOWN:
                   fprintf(temp_out,"DOWN  |");
                   break;
         }

         fprintf(temp_out,"%11d|", node->potential);
         if(node->basic_arc)
           fprintf(temp_out,"(%2d,%2d)|\n", node->basic_arc->tail->number , node->basic_arc->head->number);
         else
           fprintf(temp_out,"(--,--)|\n");

    }
    fclose(temp_out);

    return 0;
}
