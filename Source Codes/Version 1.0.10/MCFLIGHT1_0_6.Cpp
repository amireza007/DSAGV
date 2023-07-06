
//#define  MCF_FLOAT

#define  PRIMAL
#define  MCF_LOWER_BOUNDS

// #define TSP-PORT
#define  MCF_DEBUG_4

#include "conio.h"
#include "mcf.h"
#include "mcfutil.h"

int MCF_NSA_Solve( int argc, char *InFile, char *OutFile,MCF_network_p net,int SF,int Number_Of_AGVs,int balance)
{

    double start_cpu_time, simplex_cpu_time;

    char infile[81];
    char outfile[81];

    long stat = 0, Max_Cost;


    strcpy( infile, InFile);
    strcpy( outfile, OutFile);

    #if defined TSP-PORT
      memset( (void *)(&net), 0, (size_t)(sizeof(MCF_network_t)) );
      stat = MCF_read_dimacs_min( infile, &net );
      if( stat )
        return -1;
    #endif


    switch(net->algorithm_opt)
    {
       //case 1 :
       //case 2 :
       case 0 :  if( net->m < 10000 )
                    net->find_bea = MCF_primal_bea_mpp_30_5;
                 else if( net->m > 100000 )
                         net->find_bea = MCF_primal_bea_mpp_200_20;
                      else
                         net->find_bea = MCF_primal_bea_mpp_50_10;
                 break;

        case 1 :
        case 2 : net->find_bea = MCF_primal_bea_mpp_200_25;
                 net->block_size = 200;

   }

    simplex_cpu_time = MCF_get_cpu_time();

    if(SF)  // Static Flag
      MCF_primal_start_artificial( net );
    else
    {
      if(!balance)
         MCF_primal_repair_span_tree( net , Number_Of_AGVs);
#if defined MCF_DEBUG_4
    FILE *temp_out;
    temp_out = fopen( "tmp.tmp", "a" );
    fprintf(temp_out,"After Constructing the Tree\n");
    fclose(temp_out);
    MCF_write_intermediate2(net);
    MCF_write_intermediate3(net);
#endif
    }
    if(MCF_is_balanced(net ) == 1)
       putch(7);

    MCF_primal_net_simplex( net );

    simplex_cpu_time = MCF_get_cpu_time() - simplex_cpu_time;
    net->cpu_time = simplex_cpu_time;

   // printf( "\n%s: %ld nodes / %ld arcs\n", infile, (net.n), (net.m) );
    if( net->primal_unbounded )
    {
        printf( "\n   >>> problem primal unbounded <<<\n" );
        return -1;
    }

    if( net->dual_unbounded )
    {
        printf( "\n   >>> problem dual unbounded <<<\n" );
        return -1;
    }

    if( net->feasible == 0 )
    {
        printf( "\n   >>> problem infeasible or unbounded <<<\n" );
        return -1;
    }


    net->optcost = MCF_primal_obj(net);
    if( MCF_ABS(MCF_dual_obj(net)-net->optcost)>(double)MCF_ZERO_EPS )
        printf( "NETWORK SIMPLEX: primal-dual objective mismatch!?\n" );
#ifdef MCF_FLOAT
    printf( "Primal optimal objective          : %10.6f\n", MCF_primal_obj(&net) );
    printf( "Dual optimal objective            : %10.6f\n", MCF_dual_obj(&net) );
#else
    // printf( "Primal optimal objective          : %10.0f\n",  MCF_primal_obj(&net) );
    // printf( "Dual optimal objective            : %10.0f\n",  MCF_dual_obj(&net) );
#endif

 //   printf( "Write solution to file %s\n", outfile );
    MCF_write_solution( infile, outfile, net, simplex_cpu_time );

 //   MCF_free( &net );

  //  if(!SF && !balance) // just for check the spanning tree
  //     exit(0);

    return 0;
}

