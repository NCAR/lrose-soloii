/* $Id$ */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

# include <ctype.h>
# include <string.h>
# include <sys/types.h>
# include <errno.h>
# include <dirent.h>
# include <solo_window_structs.h>
# include <solo_defs.h>
# include <dd_files.h>
# include <dorade_share.h>
# include <dd_defines.h>
# include <dd_general_info.h>
# include <sed_shared_structs.h>
# include <function_decl.h>
# include <dgi_func_decl.h>
# include "soloii.h"
# include "sii_externals.h"

/*
 * 
 * solo_absorb_window_info
 * solo_save_window_info
 * 
 * 
 * 
 * 
 */

extern int LittleEndian;

void se_fix_comment();		/* se_utils.c */

SiiLinkedList *sii_solo_palette_stack ();

gchar *return_image_dir();
void sii_png_image_prep (char *dir);

/* c------------------------------------------------------------------------ */

struct solo_sii_extras {
    char name_struct[4];	/* "SXTR" */
    long sizeof_struct;
    long window_num;
    char attributes[256];
};
static struct solo_sii_extras *siiex, *sse = NULL;

/* c------------------------------------------------------------------------ */

int solo_absorb_window_info (char *dir, char *fname, int ignore_swpfi_info)
{
    int ww, ii, jj, kk, mm, nn, mark, count=0, pcount=0, gottaSwap=NO;
    int frame_count = 0, config_count = 0, ba, ta, ra, la, cfg, old_sos;
    int new_config = NO, no_config = NO, *ipt, *jpt;
    unsigned long gdsos, ppsos, gww;
    size_t len, lenx;
    FILE *stream;
    char *a, *b, *c, *buf, *e, str[256], mess[256], atts[256], *sptrs[32];
    char *cbuf, *aa, *bb, *cc;
    struct solo_generic_window_struct *gws, gwx;
    struct point_in_space pisp;
    struct solo_view_info view;
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_palette spal;
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    SiiFrameConfig *sfc;
    sii_table_parameters stp;
    gchar *png_dir;
    char *aptrs[16];
    int na, nt, nc, h_frames, v_frames, solo_width=372;
    int h_default, v_default;
    double d;

    struct landmark_info0 slmk0;
    struct res_landmark_info0 res_slmk0;
    struct frame_ctr_info0 sctr0;
    struct res_frame_ctr_info0 res_sctr0;
    struct solo_sweep_file0 ssfi0;
    struct res_solo_sweep_file0 res_ssfi0;

    struct solo_parameter_info0 spmi0;    
    struct res_solo_parameter_info0 res_spmi0;    
    struct solo_view_info0 swvi0;
    struct res_solo_view_info0 res_swvi0;
    struct solo_plot_lock0 sptl0;



    spi = solo_return_winfo_ptr();
    for(kk=0; kk < SOLO_MAX_WINDOWS; spi->active_windows[kk++] = NO);

    slash_path(str, dir);
    if(strncmp("wds", fname, 3)) {
	/* trashed file name! */
	sprintf(mess, "**** Illegal file name: %s ****\n"
		, fname);
	sii_message(mess);
	return(-1);
    }
    strcat(str, fname);
    
    if(!(stream = fopen(str, "r"))) {
	sprintf(mess, "Unable to open frame state file %s\n", str);
	sii_message(mess);
	return(-1);
    }
    ii = fseek(stream, 0L, (int)2); /* at end of file */
    len = ftell(stream);	/* how big is the file */
    rewind(stream);
        
    c = buf = (char *)malloc(len);
    memset (c, 0, len);
    if((lenx = fread(buf, sizeof(char), len, stream)) < len) {
	mark = 0;
	/* complain */
    }
    fclose(stream);
    e = c+len;
    sii_reset_config_cells ();
    gws = &gwx;

    /* count the number of frames
     */
    for(c = buf;;) {
	memcpy (&gwx, c, sizeof (struct solo_generic_window_struct));
	gdsos = gws->sizeof_struct;
	gww = gws->window_num;

	if(gdsos > MAX_REC_SIZE) {
	   gottaSwap = YES;
	   swack4(&gws->sizeof_struct, &gdsos);
	   swack4(&gws->window_num, &gww);
	}
	if(!strncmp(c, "SSFI", 4)) { /* sweep file info */
	    frame_count++;
	}
	c += gdsos; 
	if(c >= e) {
	    break;
	}
	if(count > 1000) {
	    mark = 0;
	    break;
	}
    }

    /* read in window info
     */
    for(c = buf;;) {
	count++;
	memcpy (&gwx, c, sizeof (struct solo_generic_window_struct));
	gdsos = gws->sizeof_struct;
	gww = gws->window_num;

	if(gdsos > MAX_REC_SIZE) {
	   gottaSwap = YES;
	   swack4(&gws->sizeof_struct, &gdsos);
	   swack4(&gws->window_num, &gww);
	}

	if(!strncmp(c, "SPAL", 4)) { /* color palette info */
	    pcount++;
	    if(gottaSwap) {
	       sp_crack_spal(c, &spal, (int)0);
	    }
	    else {
	       memcpy(&spal, c, gdsos);
	    }
	    /* turn the old style palette into an sii palette
	     *
	     */
	    sii_param_dup_opal (&spal);
	}
	else if(!strncmp(c, "SCTB", 4)) { /* ascii color table */
	  nn = gdsos -8;
	  cbuf = (char *)malloc (nn+1);
	  memcpy (cbuf, c+8, nn);
	  strncpy (str, c+8, 80);
	  cbuf[nn] = str[80] = '\0';
	  nt = dd_tokens (str, sptrs); /* color table name is second token */
	  put_ascii_color_table (sptrs[1], cbuf);
	  sii_new_color_tables_list ();
	}
	else if(!strncmp(c, "SSFI", 4)) { /* sweep file info */
	  if(!ignore_swpfi_info) {
	    wwptr = solo_return_wwptr(gww);
	    old_sos = gdsos != sizeof(struct solo_sweep_file);
	    if(gottaSwap) {
	      if (old_sos)
		{ sp_crack_ssfi0 (c, &ssfi0, (int)0); }
	      else
		{ sp_crack_ssfi (c, wwptr->sweep, (int)0); }
	    }
	    else {
	      if (old_sos)
		{ memcpy (&ssfi0, c, sizeof (ssfi0)); }
	      else
		{ memcpy(wwptr->sweep, c, gdsos); }
	    }
	    if (old_sos) {
	       memcpy (wwptr->sweep, &ssfi0, sizeof (ssfi0));
	       memcpy (&wwptr->sweep->sweep_count, &ssfi0.sweep_count
		       , sizeof (res_ssfi0));
	      for (jj = frame_count; jj < SOLO_MAX_WINDOWS; jj++)
		{ wwptr->sweep->linked_windows[jj] = 1; }
	    }
	    wwptr->sweep->sizeof_struct = sizeof(struct solo_sweep_file);
	    wwptr->sweep->changed = YES;
	    ddir_rescan_urgent(gww);
	    wwptr->d_sweepfile_time_stamp = wwptr->sweep->start_time;
	  }
	}
	else if(!strncmp(c, "SPTL", 4)) { /* lock step */
	    wwptr = solo_return_wwptr(gww);
	    old_sos = gdsos != sizeof(struct solo_plot_lock);
	    if(gottaSwap) {
	      if (old_sos)
		{ sp_crack_sptl0 (c, &sptl0, (int)0); }
	      else
		{ sp_crack_sptl (c, wwptr->lock, (int)0); }
	    }
	    else {
	      if (old_sos)
		{ memcpy (&sptl0, c, sizeof (sptl0)); }
	      else
		{ memcpy(wwptr->lock, c, gdsos); }
	    }
	    if (old_sos) {
	      for (jj = frame_count; jj < SOLO_MAX_WINDOWS; jj++)
		{ wwptr->lock->linked_windows[jj] = 1; }
	    }
	    wwptr->lock->sizeof_struct = sizeof(struct solo_plot_lock);
	    wwptr->active = 
		  spi->active_windows[gww] =
			wwptr->lock->changed = YES;
	}
	else if(!strncmp(c, "SPMI", 4)) { /* parameter info */
	    wwptr = solo_return_wwptr(gww);
	    old_sos = gdsos != sizeof(struct solo_parameter_info);
	    if(gottaSwap) {
	      if (old_sos)
		{ sp_crack_spmi0 (c, wwptr->parameter, (int)0); }
	      else
		{ sp_crack_spmi (c, wwptr->parameter, (int)0); }
	    }
	    else {
	      if (old_sos)
		{ memcpy (&spmi0, c, sizeof (spmi0)); }
	      else
		{ memcpy(wwptr->parameter, c, gdsos); }
	    }
	    if (old_sos) {
	       memcpy (wwptr->parameter, &spmi0, sizeof (spmi0));
	       memcpy (wwptr->parameter->parameter_name, spmi0.parameter_name
		       , sizeof (res_spmi0));
	       for (jj = frame_count; jj < SOLO_MAX_WINDOWS; jj++)
		 { wwptr->parameter->linked_windows[jj] = 0; }
	    }
	    wwptr->parameter->sizeof_struct =
		  sizeof(struct solo_parameter_info);
	    wwptr->parameter->changed = YES;
	}
	else if(!strncmp(c, "SWVI", 4)) { /* view info */
	    wwptr = solo_return_wwptr(gww);
	    if(gottaSwap)
	      { sp_crack_swvi (c, &view, (int)0); }
	    else
	      { memcpy(&view, c, sizeof (view)); }

	    if( gww == 0 && view.frame_config > 100)
	      { new_config = YES; }

	    /* smallest value in the new configuration is 0101
	     * and luckily the frame_config is before the linked_windows
	     * variable
	     */
	    if(gottaSwap) {
	      if (new_config)
		{ memcpy (wwptr->view, &view, sizeof (view)); }
	      else
		{ sp_crack_swvi0 (c, &swvi0, (int)0); }
	    }
	    else {
	      if (new_config)
		{ memcpy(wwptr->view, c, gdsos); }
	      else
		{ memcpy (&swvi0, c, sizeof (swvi0)); }
	    }
	    if (!new_config) {
	       memcpy (wwptr->view, &swvi0, sizeof (swvi0));
	       memcpy (&wwptr->view->type_of_annot, &swvi0.type_of_annot
		       , sizeof (res_swvi0));
	       for (jj = frame_count; jj < SOLO_MAX_WINDOWS; jj++)
		 { wwptr->view->linked_windows[jj] = 1; }
	    }
	    wwptr->view->sizeof_struct = sizeof(struct solo_view_info);
	    wwptr->view->changed = YES;

	    b = c +wwptr->view->offset_to_pisps;
	    for(ii=0; ii < wwptr->view->num_pisps; ii++) {
	        memcpy (&pisp, b, sizeof (pisp));
		ppsos = pisp.sizeof_struct;
		if( gottaSwap ) {
		   swack4(&pisp.sizeof_struct, &ppsos);
		}
		if(strstr(pisp.id, "SOL_RAD_LOC")) {
		    a = (char *)wwptr->radar_location;
		}
		else if(strstr(pisp.id, "SOL_WIN_LOC")) {
		    a = (char *)wwptr->center_of_view;
		}
		else if(strstr(pisp.id, "SOL_LANDMARK")) {
		    a = (char *)wwptr->landmark;
		}
		else {
		    a = NULL;
		}
		if(a) {
		   if(gottaSwap) {
		      se_crack_pisp (b, a, (int)0);
		   }
		   else {
		      memcpy(a, b, ppsos);
		   }
		}
		b += ppsos;
	    }
	    /* extract and setup the configuration info
	     */
	    if ((nn = wwptr->view->frame_config) > 100) { /* smallest value is 0101 */
	      stp.bottom_attach = nn % 10; nn /= 10;
	      stp.top_attach =    nn % 10; nn /= 10;
	      stp.right_attach =  nn % 10; nn /= 10;
	      stp.left_attach =   nn % 10; nn /= 10;
	    
	      mm = stp.right_attach - stp.left_attach;
	      nn = stp.bottom_attach - stp.top_attach;

	      if (gww == 0) {
		sii_table_widget_width = wwptr->view->width_in_pix/mm;
		sii_table_widget_height = wwptr->view->height_in_pix/nn;
	      }
	      ii = stp.top_attach * maxConfigCols;
	      for (; nn--; ii += maxConfigCols) {
		jj = ii +stp.left_attach;
		kk = jj +mm;
		for (; jj < kk ; config_cells[jj++]->frame_num = gww+1);
	      }
	    }
	    else if (!new_config)
	      { config_count++; }
	}
	else if(!strncmp(c, "SLMK", 4)) { /* lankmark use info */
	    wwptr = solo_return_wwptr(gww);
	    old_sos = gdsos != sizeof(struct landmark_info);
	    if(gottaSwap) {
	      if (old_sos)
		{ sp_crack_slmk0 (c, wwptr->landmark_info, (int)0); }
	      else
		{ sp_crack_slmk (c, wwptr->landmark_info, (int)0); }
	    }
	    else {
	      if (old_sos)
		{ memcpy (&slmk0, c, sizeof (slmk0)); }
	      else
		{ memcpy(wwptr->landmark_info, c, gdsos); }
	    }
	    if (old_sos) {
	       memcpy (wwptr->landmark_info, &slmk0, sizeof (slmk0));
	       memcpy (&wwptr->landmark_info->landmark_options
		       , &slmk0.landmark_options, sizeof (res_slmk0));
	      for (jj = frame_count; jj < SOLO_MAX_WINDOWS; jj++)
		{ wwptr->landmark_info->linked_windows[jj] = 1; }
	    }
	    wwptr->landmark_info->sizeof_struct =
		  sizeof(struct landmark_info);
	}
	else if(!strncmp(c, "SCTR", 4)) { /* center of frame info */
	    wwptr = solo_return_wwptr(gww);
	    old_sos = gdsos != sizeof(struct frame_ctr_info);
	    if(gottaSwap) {
	      if (old_sos)
		{ sp_crack_sctr0 (c, wwptr->frame_ctr_info, (int)0); }
	      else
		{ sp_crack_sctr (c, wwptr->frame_ctr_info, (int)0); }
	    }
	    else {
	      if (old_sos)
		{  memcpy (&sctr0, c, sizeof (sctr0)); }
	      else
		{ memcpy(wwptr->frame_ctr_info, c, gdsos); }
	    }
	    if (old_sos) {
	       memcpy (wwptr->frame_ctr_info, &sctr0, sizeof (sctr0));
	       memcpy (&wwptr->frame_ctr_info->centering_options, &sctr0.centering_options
		       , sizeof (res_sctr0));
	      for (jj = frame_count; jj < SOLO_MAX_WINDOWS; jj++)
		{ wwptr->frame_ctr_info->linked_windows[jj] = 1; }
	    }
	    wwptr->frame_ctr_info->sizeof_struct =
		  sizeof(struct frame_ctr_info);
	}
	else if(!strncmp(c, "SXMN", 4)) { /* examine info */
	    wwptr = solo_return_wwptr(gww);
	    if(gottaSwap) {
	       (c, wwptr->examine_info, (int)0);
	    }
	    else {
	       memcpy(wwptr->examine_info, c, gdsos);
	    }
	    wwptr->examine_info->sizeof_struct =
		  sizeof(struct solo_examine_info);
	    /* just in case there's a non-zero change count lurking out there */
	    wwptr->examine_info->change_count = 0;
	}
	else if(!strncmp(c, "SXTR", 4)) { /* new sii options */
	    wwptr = solo_return_wwptr(gww);
	    siiex = (struct solo_sii_extras*)c;
	    memcpy (atts, siiex->attributes, sizeof (siiex->attributes));
	    na = dd_tokenz (atts, sptrs, ";");	/* pointers to attributes */

	    for (ii = 0; ii < na; ii++) {
	       a = sptrs[ii];
	       if (strstr (a, "TSYNC")) { /* swpfi time sync of frame 1 */
		  wwptr->swpfi_time_sync_set = YES;
	       }
	       else if (strstr (a, "SWPFLT")) { /* swpfi filter */
		  wwptr->swpfi_filter_set = gww +1;
		  nt = dd_tokenz (a, aptrs, ": \t\n,");
		  jj = sscanf (aptrs[1], "%f", &wwptr->filter_fxd_ang);
		  jj = sscanf (aptrs[2], "%f", &wwptr->filter_tolerance);
		  b = wwptr->filter_scan_modes;
		  *b = '\0';
		  for (kk=3; kk < nt; kk++) {
		     strcat (b, aptrs[kk]);
		     strcat (b, ",");
		  }
	       }
	       else if (strstr (a, "CB_LOC")) { /* color bar location */
				/* default is bottom */
		  if (strstr (a, "-1"))
		    { wwptr->color_bar_location = -1; } /* left side */
		  else
		    { wwptr->color_bar_location = 1; }	/* right side */
		  param_set_cb_loc (gww, wwptr->color_bar_location);
	       }
	       else if (strstr (a, "CB_SYMS")) { /* CB symbols */
		  wwptr->color_bar_symbols = YES;
	       }
	    }
	    sii_update_swpfi_widget (gww);
	    sii_update_param_widget (gww);
	}

	else {
	    mark = 0;
	}

	c += gdsos; memcpy (str, c, 255); str[255] = '\0';
	if(c >= e) {
	    break;
	}
	if(count > 1000) {
	    mark = 0;
	    break;
	}
    }
    free(buf);

    /* setup the from the old configuration if config_count > 0
     *
     */
    if (config_count > 0) {
       h_frames = 1;
       h_default = solo_width;
       v_frames = 1;
       v_default = (int)(solo_width * .75 +.5); /* slide proportions */
       v_default = ((v_default +3)/4) * 4;
       cfg = solo_return_wwptr(0)->view->frame_config;
       if (cfg == LARGE_SQUARE_FRAME || cfg == LARGE_SLIDE_FRAME) {
	  config_count = 1;
       }
       switch (cfg) {
	case LARGE_SQUARE_FRAME:
	case SQUARE_FRAME:
	case LARGE_HALF_HEIGHT_FRAME:
	  v_default = h_default;
	  break;
       }
       
      for (ww = 0; ww < config_count; ww++) {

	sfc = frame_configs[ww];
	wwptr = solo_return_wwptr(ww);
	cfg = wwptr->view->frame_config;

	switch (cfg) {

	case SMALL_RECTANGLE:
	case SQUARE_FRAME:
	  if (ww & 1) 		/* odd => right frame */
	    { la = 1; ra = 2; }
	  else		/* left frame */
	    { la = 0; ra = 1; }

	  if (ww/2 == 0)	/* always 2 frames wide */
	    { ta = 0; ba = 1; }
	  else if (ww/2 == 1)
	    { ta = 1; ba = 2; }
	  else
	    { ta = 2; ba = 3; }
	  break;

	case LARGE_SLIDE_FRAME:
	  la = 0; ra = 3;
	  ta = 0; ba = 3;
	  break;
	case LARGE_SQUARE_FRAME:
	  la = 0; ra = 2;
	  ta = 0; ba = 2;
	  break;
	case DOUBLE_WIDE_FRAME:
	  la = 0; ra = 2;
	  if (ww == 0)		/* one frame per row */
	    { ta = 0; ba = 1; }
	  else if (ww == 1)
	    { ta = 1; ba = 2; }
	  else
	    { ta = 2; ba = 3; }
	  break;
	case LARGE_HALF_HEIGHT_FRAME:
	  la = 0; ra = 3;
	  if (ww == 0)		/* one frame per row */
	    { ta = 0; ba = 1; }
	  else
	    { ta = 1; ba = 2; }
	  break;
	};
	
	stp.bottom_attach = ba;
	stp.top_attach = ta;
	stp.left_attach = la;
	stp.right_attach = ra;
	
	mm = stp.right_attach - stp.left_attach;
	nn = stp.bottom_attach - stp.top_attach;
	
	if (ww == 0) {
	  sii_table_widget_width = h_default;
	  sii_table_widget_height = v_default;
	}
	ii = stp.top_attach * maxConfigCols;
	for (; nn--; ii += maxConfigCols) {
	  jj = ii +stp.left_attach;
	  kk = jj +mm;
	  for (; jj < kk ; config_cells[jj++]->frame_num = ww+1);
	}
      }	/* end if (config_count > 0) */
    }

    /* Set dangling frames */

    for (ww=frame_count; ww < SOLO_MAX_WINDOWS; ww++) {
      solo_cpy_sweep_info(0, ww);
      solo_cpy_lock_info(0, ww);
# ifdef notyet
      solo_cpy_parameter_info(0, ww);
# endif
      solo_cpy_view_info(0, ww);
      sp_cpy_lmrk_info(0, ww);
      sp_cpy_ctr_info(0, ww);
    }


    for(kk=0; kk < SOLO_MAX_WINDOWS; spi->active_windows[kk++] = YES);
    sii_set_config ();
    sii_new_frames ();		/* triggers plots from expose callback */


    return(1);
}
/* c------------------------------------------------------------------------ */

void solo_save_window_info (const char *dir, const char *a_comment)
{
    int ww, i, ii, j, nn, mark, size=0, frame_count;
    size_t len, lenx;
    FILE *stream;
    fpos_t pos;
    long time, time_now();
    char *a, *b, *c, *buf, *e, str[256], uid[32];
    struct solo_frame_state_files *sfsf, *se_return_state_files_struct();
    struct solo_palette *spal;
    struct solo_generic_window_struct *gws;
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_palette_ptr *pal, *solo_topof_palettes();
    
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    char mess[256];
    SiiFrameConfig *sfc;
    SiiLinkedList *sps, *top;

    spi = solo_return_winfo_ptr();
    frame_count = sii_return_frame_count ();
    /*
     * generate the file name
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	/* find first active window */
	if(spi->active_windows[ww]) {
	    break;
	}
    }
    wwptr = solo_return_wwptr(ww);
    if(!dir || !strlen(dir)) {
	dir = wwptr->sweep->directory_name;
    }

    a = str;
    slash_path(a, dir);
    a += strlen(a);
    time = time_now();
    dd_file_name("wds", time, wwptr->sweep->radar_name, getuid(), a);

    if (a_comment && strlen (a_comment)) {
	strcat(str, ".");
	a += strlen (a);
	strcpy (a, a_comment);
	se_fix_comment(a);
    }

    if(!(stream = fopen(str, "w"))) {
	sprintf(mess, "Problem opening %s for writing\n", str);
	sii_message(mess);
	return;
    }
    ii = fgetpos (stream, &pos);
    /* save the palettes in reverse order so they will pop onto the
     * stack in the current order
     */
    top = sii_solo_palette_stack ();

    for(sps=top; sps->next; sps = sps->next); /* find last */
    mark = 0;

    for(; sps; sps = sps->previous) {
      spal = (struct solo_palette *)sps->data;
      
      if((nn = fwrite(spal, sizeof(char), (size_t)spal->sizeof_struct, stream))
	 < spal->sizeof_struct)
	{
	  sprintf(mess, "Problem writing palette %s\n"
		  , spal->palette_name);
	  sii_message(mess);
	  return;
	}
      if (sps == top)
	break;
    }

    /* save the direct window descriptors
     */
    for(ww=0; ww < sii_return_frame_count(); ww++) {
	if(!spi->active_windows[ww])
	      continue;
	sfc = frame_configs[ww];
	wwptr = solo_return_wwptr(ww);

	wwptr->sweep->sizeof_struct= sizeof(struct solo_sweep_file);
	if((nn = fwrite(wwptr->sweep, sizeof(char)
		       , (size_t)wwptr->sweep->sizeof_struct, stream))
	   < wwptr->sweep->sizeof_struct)
	      {
		  sprintf(mess, "Problem writing sweep info %d\n", ww);
		  solo_message(mess);
		  return;
	      }
	wwptr->lock->sizeof_struct= sizeof(struct solo_plot_lock);
	if((nn = fwrite(wwptr->lock, sizeof(char)
		       , (size_t)wwptr->lock->sizeof_struct, stream))
	   < wwptr->lock->sizeof_struct)
	      {
		  sprintf(mess, "Problem writing lock info %d\n", ww);
		  solo_message(mess);
		  return;
	      }
	wwptr->parameter->sizeof_struct = sizeof(struct solo_parameter_info);
	if((nn = fwrite(wwptr->parameter, sizeof(char)
		       , (size_t)wwptr->parameter->sizeof_struct, stream))
	   < wwptr->parameter->sizeof_struct)
	      {
		  sprintf(mess, "Problem writing radar parameter %d\n", ww);
		  solo_message(mess);
		  return;
	      }
	wwptr->frame_ctr_info->sizeof_struct = sizeof(struct frame_ctr_info);
	if((nn = fwrite(wwptr->frame_ctr_info, sizeof(char)
		       , (size_t)wwptr->frame_ctr_info->sizeof_struct, stream))
	   < wwptr->frame_ctr_info->sizeof_struct)
	      {
		  sprintf(mess, "Problem writing frame_ctr info %d\n", ww);
		  solo_message(mess);
		  return;
	      }
	wwptr->landmark_info->sizeof_struct = sizeof(struct landmark_info);
	if((nn = fwrite(wwptr->landmark_info, sizeof(char)
		       , (size_t)wwptr->landmark_info->sizeof_struct, stream))
	   < wwptr->landmark_info->sizeof_struct)
	      {
		  sprintf(mess, "Problem writing landmark info %d\n", ww);
		  solo_message(mess);
		  return;
	      }
	wwptr->examine_info->sizeof_struct = sizeof(struct solo_examine_info);
	if((nn = fwrite(wwptr->examine_info, sizeof(char)
		       , (size_t)wwptr->examine_info->sizeof_struct, stream))
	   < wwptr->examine_info->sizeof_struct)
	      {
		  sprintf(mess, "Problem writing solo_examine_info %d\n", ww);
		  solo_message(mess);
		  return;
	      }
	/* the view struct is messier
	 */
	wwptr->view->num_pisps = 3;
	wwptr->view->offset_to_pisps = sizeof(struct solo_view_info);
	wwptr->view->sizeof_struct = sizeof(struct solo_view_info)
	      +3*sizeof(struct point_in_space);

	wwptr->view->width_in_pix = sfc->width;
	wwptr->view->height_in_pix = sfc->height;

	if (ww < frame_count) {
	  nn = sfc->tbl_parms.left_attach;
	  nn = nn *10 + sfc->tbl_parms.right_attach;
	  nn = nn *10 + sfc->tbl_parms.top_attach;
	  wwptr->view->frame_config = nn *10 + sfc->tbl_parms.bottom_attach;
	}
	else {
	  wwptr->view->frame_config = 0;
	}
	

	if((nn = fwrite(wwptr->view, sizeof(char)
		       , sizeof(struct solo_view_info), stream))
	   < sizeof(struct solo_view_info))
	      {
		  sprintf(mess, "Problem writing view info %d\n", ww);
		  solo_message(mess);
		  return;
	      }
	if((nn = fwrite(wwptr->radar_location, sizeof(char)
		       , sizeof(struct point_in_space), stream))
	   < sizeof(struct point_in_space))
	      {
		  sprintf(mess, "Problem writing radar_location info %d\n",
			  ww);
		  solo_message(mess);
		  return;
	      }
	if((nn = fwrite(wwptr->center_of_view, sizeof(char)
		       , sizeof(struct point_in_space), stream))
	   < sizeof(struct point_in_space))
	      {
		  sprintf(mess, "Problem writing center_of_view info %d\n",
			  ww);
		  solo_message(mess);
		  return;
	      }
	if((nn = fwrite(wwptr->landmark, sizeof(char)
		       , sizeof(struct point_in_space), stream))
	   < sizeof(struct point_in_space))
	      {
		  sprintf(mess, "Problem writing landmark info %d\n",
			  ww);
		  solo_message(mess);
		  return;
	      }
	/* new soloii features */
	if (!sse) {
	   sse = (struct solo_sii_extras*)malloc (sizeof (*sse));
	   memset (sse, 0, sizeof (*sse));
	   strncpy (sse->name_struct, "SXTR", 4);
	   sse->sizeof_struct = sizeof (*sse);
	}
	sse->window_num = ww;
	a = sse->attributes;
	*a = '\0';
				/* c...mark */
	if (wwptr->swpfi_time_sync_set)
	  { strcat (a, "TSYNC;"); }

	if (wwptr->swpfi_filter_set) { 
	   strcat (a, "SWPFLT:");
	   sprintf (a +strlen (a), "%.2f,%.3f,", wwptr->filter_fxd_ang
		    , wwptr->filter_tolerance);
	   strcat (a, wwptr->filter_scan_modes);
	   strcat (a, ";");
	}
	if (wwptr->color_bar_location) {
	   strcat (a, "CB_LOC:");
	   strcat (a, (wwptr->color_bar_location > 0) ? "1" : "-1");
	   strcat (a, ";");
	}
	if (wwptr->color_bar_symbols)
	  { strcat (a, "CB_SYMS;"); } 

	if((nn = fwrite(sse, sizeof(char)
			, sizeof(*sse), stream)) < sizeof(*sse))
	  {
	     sprintf(mess, "Problem writing solo_ssi_extras info %d\n",
		     ww);
	     solo_message(mess);
	     return;
	  }
     }
				/* Write color tables here (odd byte counts) */
    if (!sii_param_dump_ctbls (stream))
      { return; }

    fclose(stream);
}
/* c------------------------------------------------------------------------ */






