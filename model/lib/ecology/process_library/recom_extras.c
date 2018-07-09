/*
 *
 *  ENVIRONMENTAL MODELLING SUITE (EMS)
 *  
 *  File: model/lib/ecology/process_library/recom_extras.c
 *  
 *  Description:
 *  Performs misc. tasks for RECOM to handle the fact that we don't
 *  have a fully qualified prm file. These ar:
 *  o) Forces a find_index on COD
 *  o) Does the chlorophyll sum
 *  
 *  Copyright:
 *  Copyright (c) 2018. Commonwealth Scientific and Industrial
 *  Research Organisation (CSIRO). ABN 41 687 119 230. All rights
 *  reserved. See the license file for disclaimer and full
 *  use/redistribution conditions.
 *  
 *  $Id: recom_extras.c 5846 2018-06-29 04:14:26Z riz008 $
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "ecology_internal.h"
#include "stringtable.h"
#include "utils.h"
#include "ecofunct.h"
#include "constants.h"
#include "eprocess.h"
#include "cell.h"
#include "column.h"
#include "einterface.h"
#include "recom_extras.h"

typedef struct {

  /* tracers */
  int PhyL_Chl_i;
  int PhyS_Chl_i;
  int MPB_Chl_i;
  int Tricho_Chl_i;
  int COD_i;

  /* Output */
  int Chl_a_sum_i;
  int at_440_i;
  int ap_670_i;
  int bt_550_i;
  int Kd_490_i;

  int PAR_i;
  int PAR_z_i;
  int K_heat_i;

  int EpiPAR_i;
  int EpiPAR_sg_i;

  int SG_shear_mort_i;
  int SGH_shear_mort_i;  
  int SGP_shear_mort_i;  
  int SGD_shear_mort_i;

  int IN_up_i;
  int ON_up_i; 
  int mucus_i;
  int CS_bleach_i;
  int photochemicalquench_i;
  int nonphotochemicalquench_i;

  int CO32_i;
  int HCO3_i;
  int pco2surf_i;
  int dpCO2_i;
  int omega_ar_i;
  int PH_i;

  int oxy_sat_i;
  int O2_flux_i;

  // MODIS - [412 443 488 531 547 667 678 748 470 555 645]

  int OC3M_i;
  int TSSM_i;
  int KD490M_i;
  int R_412_i;
  int R_443_i;
  int R_488_i;
  int R_531_i;
  int R_547_i;
  int R_667_i;
  int R_678_i;
  int R_748_i;
  int R_470_i;
  int R_555_i;
  int R_645_i;
  int R_590_i;

  int OC3V_i;
  int R_410_i;
  int R_486_i;
  int R_551_i;
  int R_671_i;
  int R_745_i;

  int R_510_i;
  int R_640_i;

  int R_482_i;
  int R_561_i;
  int R_655_i;

  int NTU_i;
  
} workspace;

void recom_extras_init(eprocess* p)
{
  ecology* e = p->ecology;
  stringtable* tracers = e->tracers;
  stringtable* epis = e->epis;
  workspace* ws = malloc(sizeof(workspace));
  
  int OFFSET_EPI = tracers->n * 2;

  /* Initialise all to non-existent */
  ws->PhyL_Chl_i = -1;
  ws->PhyS_Chl_i = -1;
  ws->MPB_Chl_i = -1;
  ws->Tricho_Chl_i = -1;
  ws->COD_i = -1;
  
  /* Output */
  ws->Chl_a_sum_i = -1;
  ws->oxy_sat_i = -1;
  ws->at_440_i = -1;
  ws->ap_670_i = -1;
  ws->bt_550_i = -1;
  ws->Kd_490_i = -1;
  ws->PAR_i = -1;
  ws->K_heat_i = -1;
  ws->PAR_z_i = -1;
  
  ws->EpiPAR_i = -1;
  ws->EpiPAR_sg_i = -1;
  
  ws->IN_up_i = -1;
  ws->ON_up_i = -1; 
  ws->mucus_i = -1;
  ws->CS_bleach_i = -1;

  ws->CO32_i = -1;
  ws->HCO3_i = -1;
  ws->pco2surf_i = -1;
  ws->dpCO2_i = -1;
  ws->omega_ar_i = -1;
  ws->PH_i = -1;

  ws->oxy_sat_i = -1;
  ws->O2_flux_i = -1;
  
  /* cache the workspace */
  p->workspace = ws;

  /* unused */
  ws->COD_i = e->find_index(tracers, "COD", e);
}

void recom_extras_postinit(eprocess* p)
{
  ecology* e = p->ecology;
  stringtable* tracers = e->tracers;
  stringtable* epis = e->epis;
  workspace* ws = p->workspace;
  
  int OFFSET_EPI = tracers->n * 2;

  int dummy_i;

  if (process_present(e,PT_WC,"phytoplankton_spectral_grow_wc(small)")){
    ws->PhyL_Chl_i   = e->find_index(tracers, "PhyL_Chl", e);
    ws->PhyS_Chl_i   = e->find_index(tracers, "PhyS_Chl", e);
    ws->MPB_Chl_i    = e->find_index(tracers, "MPB_Chl", e);
    ws->Tricho_Chl_i = e->find_index(tracers, "Tricho_Chl", e);
    ws->Chl_a_sum_i  = e->find_index(tracers, "Chl_a_sum", e);
  }

  if (process_present(e,PT_WC,"light_spectral_wc")){
    ws->PAR_i = e->find_index(tracers, "PAR", e);
    ws->PAR_z_i = e->find_index(tracers, "PAR_z", e);
    ws->at_440_i   = e->find_index(tracers, "at_440", e);
    ws->ap_670_i   = e->find_index(tracers, "ap_670", e);
    ws->bt_550_i = e->find_index(tracers, "bt_550", e);
    ws->Kd_490_i = e->find_index(tracers, "Kd_490", e);
    ws->K_heat_i = e->find_index(tracers, "K_heat", e);
  }

  if (process_present(e,PT_WC,"gas_exchange_wc(carbon,oxygen)")){
    ws->oxy_sat_i = e->find_index(tracers, "Oxy_sat", e);
    ws->O2_flux_i = e->find_index(tracers, "O2_flux", e);
  }

  if (process_present(e,PT_WC,"gas_exchange_wc(oxygen,carbon)")){
    ws->oxy_sat_i = e->find_index(tracers, "Oxy_sat", e);
    ws->O2_flux_i = e->find_index(tracers, "O2_flux", e);
  }

  if (process_present(e,PT_WC,"gas_exchange_wc(oxygen)")){
    ws->oxy_sat_i = e->find_index(tracers, "Oxy_sat", e);
    ws->O2_flux_i = e->find_index(tracers, "O2_flux", e);
  }

  if (process_present(e,PT_EPI,"light_spectral_uq_epi")){
    ws->EpiPAR_i = e->find_index(epis, "EpiPAR", e) + OFFSET_EPI;
    ws->EpiPAR_sg_i = e->find_index(epis, "EpiPAR_sg", e) + OFFSET_EPI;
  }

  if (process_present(e,PT_WC,"light_spectral_wc") && process_present(e,PT_EPI,"light_spectral_uq_epi")){
    
    /* Avoid duplication if within 1.5 nm of another satellite. */

    /* MODIS */
    
    ws->OC3M_i = e->find_index(epis, "OC3M", e) + OFFSET_EPI;
    ws->TSSM_i = e->find_index(epis, "TSSM", e) + OFFSET_EPI;
    ws->KD490M_i = e->find_index(epis, "KD490M", e) + OFFSET_EPI;
    ws->R_412_i = e->find_index(epis, "R_412", e) + OFFSET_EPI; 
    ws->R_443_i = e->find_index(epis, "R_443", e) + OFFSET_EPI;
    ws->R_488_i = e->find_index(epis, "R_488", e) + OFFSET_EPI;
    ws->R_531_i = e->find_index(epis, "R_531", e) + OFFSET_EPI;
    ws->R_547_i = e->find_index(epis, "R_547", e) + OFFSET_EPI;
    ws->R_667_i = e->find_index(epis, "R_667", e) + OFFSET_EPI;
    ws->R_678_i = e->find_index(epis, "R_678", e) + OFFSET_EPI;
    ws->R_748_i = e->find_index(epis, "R_748", e) + OFFSET_EPI;
    ws->R_470_i = e->find_index(epis, "R_470", e) + OFFSET_EPI;
    ws->R_555_i = e->find_index(epis, "R_555", e) + OFFSET_EPI;
    ws->R_645_i = e->find_index(epis, "R_645", e) + OFFSET_EPI;

    /* VIIRS - assume MODIS 443 nm */

    ws->R_410_i = e->find_index(epis, "R_410", e) + OFFSET_EPI;
    ws->R_486_i = e->find_index(epis, "R_486", e) + OFFSET_EPI;
    ws->R_551_i = e->find_index(epis, "R_551", e) + OFFSET_EPI;
    ws->R_671_i = e->find_index(epis, "R_671", e) + OFFSET_EPI;
    ws->R_745_i = e->find_index(epis, "R_745", e) + OFFSET_EPI;

    ws->OC3V_i = e->find_index(epis, "OC3V", e) + OFFSET_EPI;

    /* Himawari-8 - assume MODIS 470 nm */

    ws->R_510_i = e->find_index(epis, "R_510", e) + OFFSET_EPI;
    ws->R_640_i = e->find_index(epis, "R_640", e) + OFFSET_EPI;

    /* Landsat8 */

    ws->R_482_i = e->find_index(epis, "R_482", e) + OFFSET_EPI;
    // ws->R_561_i = e->find_index(epis, "R_561", e) + OFFSET_EPI;
    ws->R_655_i = e->find_index(epis, "R_655", e) + OFFSET_EPI;

    /* Sentinal-3 - 412, 443, 490 in MODIS, 510 in Himawari8, */

    dummy_i = e->find_index(epis, "R_400", e) + OFFSET_EPI;
    dummy_i = e->find_index(epis, "R_560", e) + OFFSET_EPI;
    dummy_i = e->find_index(epis, "R_620", e) + OFFSET_EPI;
    dummy_i = e->find_index(epis, "R_665", e) + OFFSET_EPI;
    dummy_i = e->find_index(epis, "R_681", e) + OFFSET_EPI;
    dummy_i = e->find_index(epis, "R_710", e) + OFFSET_EPI;
    dummy_i = e->find_index(epis, "R_753", e) + OFFSET_EPI;

    dummy_i = e->find_index(epis, "OC4Me", e) + OFFSET_EPI;

    /* NTU comparison */

    ws->R_590_i = e->find_index(epis, "R_590", e) + OFFSET_EPI;
    ws->NTU_i = e->find_index(tracers, "Turbidity", e);

    dummy_i = e->find_index(tracers, "Fluorescence", e);
    dummy_i = e->find_index(epis, "nFLH", e) + OFFSET_EPI;

    dummy_i = e->find_index(epis, "Secchi", e) + OFFSET_EPI;
    dummy_i = e->find_index(epis, "SWR_bot_abs", e) + OFFSET_EPI;

  }

  /* add diagnostics if particular processes specificed */ 
     
  if (process_present(e,PT_EPI,"coral_spectral_grow_epi")){
    ws->IN_up_i = e->find_index(epis, "Coral_IN_up", e) + OFFSET_EPI ;
    ws->ON_up_i = e->find_index(epis, "Coral_ON_up", e) + OFFSET_EPI; 
    ws->mucus_i = e->find_index(epis, "mucus", e) + OFFSET_EPI;
  }
  if (process_present(e,PT_EPI,"coral_spectral_grow_bleach_epi")){
    ws->IN_up_i = e->find_index(epis, "Coral_IN_up", e) + OFFSET_EPI ;
    ws->ON_up_i = e->find_index(epis, "Coral_ON_up", e) + OFFSET_EPI; 
    ws->mucus_i = e->find_index(epis, "mucus", e) + OFFSET_EPI;
    ws->CS_bleach_i = e->find_index(epis, "CS_bleach", e) + OFFSET_EPI;

    // ws->photochemicalquench_i = e->find_index(epis, "photochemicalquench", e) + OFFSET_EPI;
    // ws->nonphotochemicalquench_i = e->find_index(epis, "nonphotochemicalquench", e) + OFFSET_EPI;
  }
  if (process_present(e,PT_EPI,"seagrass_spectral_mortality_proto_epi(Zostera)")){
    ws->SG_shear_mort_i = e->find_index(epis, "SG_shear_mort", e) + OFFSET_EPI ;
  }
  if (process_present(e,PT_EPI,"seagrass_spectral_mortality_proto_epi(Halophila)")){
    ws->SGH_shear_mort_i = e->find_index(epis, "SGH_shear_mort", e) + OFFSET_EPI ;
  }
  if (process_present(e,PT_EPI,"seagrass_spectral_mortality_proto_epi(Posidonia)")){
    ws->SGP_shear_mort_i = e->find_index(epis, "SGP_shear_mort", e) + OFFSET_EPI ;
  }    
  if (process_present(e,PT_EPI,"seagrass_spectral_mortality_proto_epi(Deep)")){
    ws->SGD_shear_mort_i = e->find_index(epis, "SGD_shear_mort", e) + OFFSET_EPI ;
  }
  if (process_present(e,PT_GEN,"carbon_chemistry_wc")){
    ws->CO32_i = e->find_index(tracers,"CO32", e);
    ws->HCO3_i = e->find_index(tracers,"HCO3", e);
    ws->pco2surf_i = e->find_index(tracers,"pco2surf", e);
    ws->dpCO2_i = e->find_index(tracers,"dpCO2", e);
    ws->omega_ar_i = e->find_index(tracers,"omega_ar", e);
    ws->PH_i = e->find_index(tracers, "PH", e);
  }
}

void recom_extras_destroy(eprocess* p)
{
    free(p->workspace);
}

void recom_extras_postcalc(eprocess* p, void* pp)
{
    workspace* ws = p->workspace;
    double* y = ((cell*) pp)->y;

    /* Sum up total chloropyhll */

    if (ws->Chl_a_sum_i > -1){
      y[ws->Chl_a_sum_i] = y[ws->PhyL_Chl_i] + y[ws->PhyS_Chl_i] +
	y[ws->MPB_Chl_i] + y[ws->Tricho_Chl_i];
    }
}
