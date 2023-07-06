

#ifndef _PBEA_H
#define _PBEA_H


#include "mcfdefs.h"
#include "pbeadef.h"


MCF_arc_p MCF_primal_bea_all  ( MCF_DECLARE_PRIMAL_BEA_PARAMETERS );

MCF_arc_p MCF_primal_bea_cycle( MCF_DECLARE_PRIMAL_BEA_PARAMETERS );

void MCF_reset_mpp_module_30_5( void );
     MCF_arc_p MCF_primal_bea_mpp_30_5( MCF_DECLARE_PRIMAL_BEA_PARAMETERS );

void MCF_reset_mpp_module_50_10( void );
     MCF_arc_p MCF_primal_bea_mpp_50_10( MCF_DECLARE_PRIMAL_BEA_PARAMETERS );

void MCF_reset_mpp_module_200_20( void );
     MCF_arc_p MCF_primal_bea_mpp_200_20( MCF_DECLARE_PRIMAL_BEA_PARAMETERS );

void MCF_reset_mpp_module_200_25( void );
    MCF_arc_p MCF_primal_bea_mpp_200_25( MCF_DECLARE_PRIMAL_BEA_PARAMETERS );


#endif
