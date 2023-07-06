

#ifndef _MCFUTIL_H
#define _MCFUTIL_H


#include "mcfdefs.h"


double MCF_primal_obj   ( MCF_network_p net );
double MCF_dual_obj     ( MCF_network_p net );
long MCF_primal_feasible( MCF_network_p net );
long MCF_dual_feasible  ( MCF_network_p net );
long MCF_is_basis       ( MCF_network_p net );
long MCF_is_balanced    ( MCF_network_p net );
long MCF_free           ( MCF_network_p net );

double MCF_get_cpu_time   ( void );
double MCF_get_system_time( void );
double MCF_get_wall_time  ( void );


#endif
