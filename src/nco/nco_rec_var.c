/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rec_var.c,v 1.5 2004-01-01 20:41:43 zender Exp $ */

/* Purpose: Record variable utilities */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL)
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_rec_var.h" /* Record variable utilities */

void
rec_var_dbg /* [fnc] Aid in debugging problems with record dimension */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const dbg_sng) /* I [sng] Debugging message to print */
{
  /* Purpose: Aid in debugging problems with record dimension */
  /* Usage: if(dbg_lvl == 73) rec_var_dbg(out_id,"After ncvarput()"); */
  int nbr_dmn_fl;
  int nbr_var_fl;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  long dmn_sz;

  (void)fprintf(stderr,"%s: DBG %s\n",prg_nm_get(),dbg_sng);
  (void)nco_inq(nc_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  if(rec_dmn_id == NCO_REC_DMN_UNDEFINED){
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, no record dimension\n",prg_nm_get(),nbr_dmn_fl,nbr_var_fl);
  }else{
    (void)nco_inq_dimlen(nc_id,rec_dmn_id,&dmn_sz);
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, record dimension size is %li\n",prg_nm_get(),nbr_dmn_fl,nbr_var_fl,dmn_sz);
  } /* end else */
  (void)fflush(stderr);
} /* end rec_var_dbg() */

void
rec_crd_chk /* Check for monotonicity of coordinate values */
(const var_sct * const var, /* I [sct] Coordinate to check for monotonicity */
 const char * const fl_in, /* I [sng] Input filename */
 const char * const fl_out, /* I [sng] Output filename */
 const long idx_rec, /* I [idx] Index of record coordinate in input file */
 const long idx_rec_out) /* I [idx] Index of record coordinate in output file */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Check for monotonicity of coordinate values */

  enum monotonic_direction{
    decreasing, /* 0 */
    increasing}; /* 1 */

  static double rec_crd_val_lst;
  static double rec_crd_val_crr;

  static int monotonic_direction;

  /* Use implicit type conversion */
  switch(var->type){
  case NC_FLOAT: rec_crd_val_crr=var->val.fp[0]; break; 
  case NC_DOUBLE: rec_crd_val_crr=var->val.dp[0]; break; 
  case NC_INT: rec_crd_val_crr=var->val.lp[0]; break;
  case NC_SHORT: rec_crd_val_crr=var->val.sp[0]; break;
  case NC_CHAR: rec_crd_val_crr=var->val.cp[0]; break;
  case NC_BYTE: rec_crd_val_crr=var->val.bp[0]; break;
    default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  if(idx_rec_out > 1){
    if(((rec_crd_val_crr > rec_crd_val_lst) && monotonic_direction == decreasing) ||
       ((rec_crd_val_crr < rec_crd_val_lst) && monotonic_direction == increasing))
      (void)fprintf(stderr,"%s: WARNING Record coordinate \"%s\" does not monotonically %s between (input file %s record indices: %ld, %ld) (output file %s record indices %ld, %ld) record coordinate values %f, %f\n",prg_nm_get(),var->nm,(monotonic_direction == decreasing ? "decrease" : "increase"),fl_in,idx_rec-1,idx_rec,fl_out,idx_rec_out-1,idx_rec_out,rec_crd_val_lst,rec_crd_val_crr);
  }else if(idx_rec_out == 1){
    if(rec_crd_val_crr > rec_crd_val_lst) monotonic_direction=increasing; else monotonic_direction=decreasing;
  } /* end if */
    
  rec_crd_val_lst=rec_crd_val_crr;

} /* end rec_crd_chk() */

