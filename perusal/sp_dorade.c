/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

# define NEW_ALLOC_SCHEME

# include <ctype.h>
# include <string.h>
# include <sys/types.h>
# include <errno.h>
# include <dirent.h>
# include "soloii.h"
# include "sii_externals.h"
# include <solo_window_structs.h>
# include <solo_defs.h>
# include <dd_files.h>
# include <dd_defines.h>
# include <dorade_headers.h>
# include <dd_general_info.h>
# include <sed_shared_structs.h>

/*
 * This file contains routines
 * 
 * solo_cell_lut
 * solo_color_cells
 * 
 */
/* c------------------------------------------------------------------------ */

void solo_cell_lut(frme)
  int frme;
{
    /* this routine creates a lookup table of source data cell numbers
     * corresponding to a cell number based on uniform cell spacing
     * this is necessary because we are likely to have nonuniform cell spacing
     * and the rasterization algorithm needs uniform cell spacing
     */
    int ww, g, i, ii, j, k, n, nc, ng;
    WW_PTR wwptr, solo_return_wwptr();
    struct dd_general_info *dgi, *dd_window_dgi();
    struct cell_d *celv;
    float f, r, gs, half_gs, rmax;
    double d, fabs();
    short *lut;

    wwptr = solo_return_wwptr(frme);
    lut = wwptr->data_cell_lut;
    ww = wwptr->lead_sweep->window_num;
    dgi = dd_window_dgi(ww, "");

    celv = dgi->dds->celvc;	/* corrected cell vector */
    nc = celv->number_cells-1;
    rmax = celv->dist_cells[nc];
    f = rmax - celv->dist_cells[0];

    if(gs=wwptr->uniform_cell_spacing) {
    }
    else {
	/* find an early gate spacing > 0
	 */
	for(ii=0; ii < MAXCVGATES; ii++) {
	    gs = celv->dist_cells[ii+1] - celv->dist_cells[ii];
	    if(gs > 0)
		  break;
	}
	wwptr->uniform_cell_spacing = gs;
    }
    /* get out in front of the radar */
    for(g=0; g < nc && celv->dist_cells[g] < .5*gs; g++);
    r = celv->dist_cells[g];
    /*
     * now create the lookup table
     */
    for(ii=0;; r+=gs) {
	if(fabs((double)(r-celv->dist_cells[g])) >
	   .5*fabs((double)(celv->dist_cells[g+1] -celv->dist_cells[g]))) {
	    /*
	     * The distance between the target range and the current cell
	     * is greater that half the distance between the current cell
	     * and the next cell so start using the next cell
	     */
	    g++;
	    if(g+1 > nc)
		  break;
	}
	*(lut+ii++) = g;
    }
    for(; r < rmax+.5*gs; r+=gs) {
	  *(lut+ii++) = g;
    }
    wwptr->number_cells = ii;
    wwptr->uniform_cell_one = celv->dist_cells[*lut];
}
/* c------------------------------------------------------------------------ */

void solo_color_cells(frme)
  int frme;
{
    /* associate a color number with each data cell value for the
     * the particular parameter for this ray
     */
    int ww, jj, kk, ng;
    register int cn, ii;
    WW_PTR wwptr, solo_return_wwptr();
    struct dd_general_info *dgi, *dd_window_dgi();
    short *ss, *clut, *tt, sval;
    int scaled_thr_val, thr_ndx = -1, f_ndx, bad;
    unsigned long *dlut;
    unsigned char *dd;
    unsigned long *cc;
    double d;

    wwptr = solo_return_wwptr(frme);
    ww = wwptr->lead_sweep->window_num;
    dgi = dd_window_dgi(ww, "");

    if (batch_threshold_field) { /* batch thresholding to be applied */
      thr_ndx = dd_ndx_name(dgi, batch_threshold_field->str);
      if (thr_ndx >= 0) {
	d =  dgi->dds->parm[thr_ndx]->parameter_scale;
	scaled_thr_val = (int)(d * batch_threshold_value);
	bad = dgi->dds->parm[wwptr->parameter_num]->bad_data;
	tt = (short *)dgi->dds->qdat_ptrs[thr_ndx];
      }
    }

    cc = wwptr->cell_colors;	/* where the colors will ultimately reside */
    dlut = wwptr->data_color_lut; /* lookup tables for colors*/
    clut = wwptr->data_cell_lut; 

    /* pointer to start of data */
    dd = (unsigned char *)dgi->dds->qdat_ptrs[wwptr->parameter_num];
    ng = wwptr->number_cells;

    if(dgi->dds->parm[wwptr->parameter_num]->binary_format == DD_16_BITS) {
      ss = (short *)dd;
      if (thr_ndx >= 0) {
	for(ii=0; ii < ng; ii++) {
	  cn = *(clut+ii);	/* which real cell number? */
	  sval = *(ss+cn);
	  if (tt[cn] < scaled_thr_val)
	    { sval = bad; }
	  *cc++ = *(dlut +sval);
	}
      }
      else {
	for(ii=0; ii < ng; ii++) {
	  cn = *(clut+ii);	/* which real cell number? */
	  *cc++ = *(dlut +(*(ss+cn)));
	}
      }
    }

    else if(dgi->dds->parm[wwptr->parameter_num]->binary_format == DD_8_BITS) {
      for(ii=0; ii < ng; ii++) {
	cn = *(clut+ii);	/* which real cell number? */
	*cc++ = *(dlut +(*(dd+cn)));
      }
    }
}
/* c------------------------------------------------------------------------ */

