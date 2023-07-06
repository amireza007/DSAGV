
#define  MCF_LOWER_BOUNDS
#undef   MCF_DEBUG_NO
//#define  MCF_DEBUG_2
//#define  MCF_DEBUG_3

#include "mcf.h"
#include "pflowup.h"
#include "PSIMPLEX.h"


long MCF_primal_net_simplex( MCF_network_p net )
{
    MCF_flow_t    delta;
    MCF_flow_t    new_flow;
    long          opt = 0;
    long          xchange;
    long          new_orientation;
    MCF_node_p    iplus;
    MCF_node_p    jplus;
    MCF_node_p    iminus;
    MCF_node_p    jminus;
    MCF_node_p    w;
    MCF_node_p    temp;
    MCF_arc_p     bea;
    MCF_arc_p     bla;
    MCF_arc_p     arcs          = net->arcs;
    MCF_arc_p     stop_arcs     = net->stop_arcs;
    MCF_arc_p     max_cost_arc  = net->max_cost_arc ;
    long          m = net->m;
    long          new_set;
    MCF_cost_t    red_cost_of_bea;
    long          *iterations = &(net->iterations);
    MCF_arc_p     (*findbea)( MCF_DECLARE_PRIMAL_BEA_PARAMETERS )
                  = (MCF_arc_p(*)( MCF_PRIMAL_BEA_PARAMETERS ))(net->find_bea);


#if defined MCF_DEBUG
    printf( "%8ld: %10.6f\n", *iterations, MCF_dual_obj(net) );
#endif

#if defined MCF_DEBUG_2
    FILE *temp_out;

    temp_out = fopen( "tmp.tmp", "w" );
    fprintf(temp_out,"\n");
    fclose(temp_out);
#endif


    while( !opt )
    {
#if defined MCF_DEBUG_2
   //       MCF_write_intermediate2(net);
#endif

#if defined MCF_DEBUG_3
          MCF_write_intermediate2(net);
          MCF_write_intermediate3(net);
#endif

        /* Find basis entering arc */
        if( (bea = findbea( max_cost_arc, net->algorithm_opt , m, arcs, stop_arcs, &red_cost_of_bea )) != NULL )
        {
            (*iterations)++;
#if defined MCF_DEBUG_2
                temp_out = fopen( "tmp.tmp", "a+t" );
                fprintf(temp_out,"Entering arc: %d---->%d\n",bea->tail->number ,bea->head->number  );
                fclose(temp_out);
#endif
            if( red_cost_of_bea > MCF_ZERO )
            { /* use backward arc. */
                iplus = bea->head;
                jplus = bea->tail;
            }
            else
            {  /* use forward arc. */
                iplus = bea->tail;
                jplus = bea->head;
            }

            /* Find iminus of basis leaving arc,
               bea = [i-,pred(i-)].
            */
#ifdef MCF_LOWER_BOUNDS
            delta = bea->upper - bea->lower;
#else
            delta = bea->upper;
#endif
            /* let's hope that (upper-lower) is a MCF_flow_t */
            iminus = MCF_primal_iminus( &delta, &xchange, iplus, jplus, &w );

            /* If delta is unbounded, the problem is
               unbounded because there is a negative
               uncapazitated cycle
            */
            if( delta > MCF_UNBOUNDED-1 )
            {
                net->primal_unbounded = 1;

                MCF_reset_mpp_module_30_5();
                MCF_reset_mpp_module_50_10();
                MCF_reset_mpp_module_200_20();
                MCF_reset_mpp_module_200_25();

                return( (long)bea );
            }


            /* If bea = bla change bounds */
            if( !iminus )
            {
                if( bea->ident == MCF_AT_UPPER)
                    bea->ident = MCF_AT_LOWER;
                else
                    bea->ident = MCF_AT_UPPER;

                if( delta > MCF_ZERO_EPS )
                    MCF_primal_update_flow( iplus, jplus, w, delta );
            }
            else /* bea <> bla */
            {   /* If necessary, exchange iplus and jplus */
                if( xchange )
                {
                    temp = jplus;
                    jplus = iplus;
                    iplus = temp;
                }

                jminus = iminus->pred;
                bla = iminus->basic_arc;

#if defined MCF_DEBUG_2
                temp_out = fopen( "tmp.tmp", "a+t" );
                fprintf(temp_out,"Entering Arc: %d---->%d, Leaving Arc: %d--->%d\n",jplus->number,iplus->number,jminus->number,iminus->number );
                fclose(temp_out);
#endif
                if( xchange != iminus->orientation )
                    new_set = MCF_AT_LOWER;
                else
                    new_set = MCF_AT_UPPER;

                if( red_cost_of_bea > 0 )
                {
                    if( bea->ident == MCF_AT_UPPER )
                        new_flow = bea->upper - delta;
                    else
                        new_flow = -delta;
                }
                else
                {
                    if( bea->ident == MCF_AT_LOWER )
                    {
#ifdef MCF_LOWER_BOUNDS
                        new_flow = bea->lower + delta;
#else
                        new_flow = delta;
#endif
                    }
                    else
                        new_flow = delta;
                }

                if( bea->tail == iplus )
                    new_orientation = MCF_UP;
                else
                    new_orientation = MCF_DOWN;

                MCF_update_tree( !xchange, new_orientation,
                                 delta   , new_flow, iplus, jplus, iminus,
                                 jminus  , w , bea, red_cost_of_bea );

                bea->ident = MCF_BASIC;   /* bea becomes a basic arc */
                bla->ident = new_set; /* bla becomes a nonbasic arc */
            }

#if defined MCF_DEBUG
            if( !(*iterations % 50000) )
                printf( "%8ld: %10.6f\n", *iterations, MCF_dual_obj(net) );
#endif
        }
        else
            opt = 1;
    }

    net->iterations = *iterations;

    net->feasible = 1;
    MCF_primal_feasible( net );
#ifdef MCF_DEBUG
    MCF_dual_feasible(net);
#endif

    return 0;
}
