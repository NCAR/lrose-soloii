/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

#define NU_TSTAMP

/*
 * This file contains
 * 
 * solo_cpy_lock_info
 * solo_cpy_parameter_info
 * solo_cpy_sweep_info
 * solo_cpy_view_info
 * solo_gen_file_info
 * solo_parameter_colors_check
 * solo_parameter_numbers_check
 * solo_set_lock_info
 * solo_set_parameter_info
 * solo_set_sweep_info
 * solo_set_view_info
 * 
 * 
 * 
 */

# include <dorade_headers.h>
# include <solo_window_structs.h>
# include <seds.h>
# include <solo_list_widget_ids.h>
# include <solo_editor_structs.h>
# include <dd_files.h>
# include <sed_shared_structs.h>
# include <dd_math.h>
# include <function_decl.h>
# include <dgi_func_decl.h>


/* external routines */
int solo_color_check();		/* sp_colors.c */
int solo_pixelate_color();	/* sp_colors.c */
void solo_message();		/* solo.c */
void solo_check_zap_parms();	/* sp_pals.c */
void solo_subscribe_palette();	/* sp_pals.c */
int solo_get_files();		/* sp_lists.c */
int solo_hardware_color_table(); /* sp_colors.c */
void solo_palette_message();	/* sp_pals.c */
void solo_trigger_dir_rescans(); /* sp_basics.c */
void dd_razel_to_xyz();		/* ddb_common.c */



/* internal routines */
void sp_cpy_lmrk_info();
void sp_cpy_ctr_info();
void solo_cpy_lock_info();
void solo_cpy_parameter_info();
void solo_cpy_sweep_info();
void solo_cpy_view_info();
char *solo_gen_file_info();
int solo_parameter_colors_check();
void solo_parameter_numbers_check();
int solo_set_lock_info();
int solo_set_parameter_info();
int solo_set_sweep_info();
void sp_set_landmark_info();
void sp_set_center_of_view_info();
int solo_set_view_info();





/* c------------------------------------------------------------------------ */

void sp_cpy_lmrk_info(src_frme, dst_frme)
  int dst_frme, src_frme;
{
    WW_PTR wwptrs, wwptrd, solo_return_wwptr();

    if (src_frme == dst_frme)
      { return; }
    wwptrs = solo_return_wwptr(src_frme);
    wwptrd = solo_return_wwptr(dst_frme);

    memcpy(wwptrd->landmark_info, wwptrs->landmark_info
	   , sizeof(struct landmark_info));
    wwptrd->landmark_info->window_num = dst_frme;
    if(wwptrd->landmark_info->landmark_options != SOLO_LINKED_POSITIONING) {
	wwptrd->landmark_info->reference_frame = dst_frme+1;
    }
    wwptrd->landmark_info->changed = YES;
    dd_copy_pisp(wwptrs->landmark, wwptrd->landmark);
}
/* c------------------------------------------------------------------------ */

void sp_cpy_ctr_info(src_frme, dst_frme)
  int dst_frme, src_frme;
{
    WW_PTR wwptrs, wwptrd, solo_return_wwptr();

    if (src_frme == dst_frme)
      { return; }
    wwptrs = solo_return_wwptr(src_frme);
    wwptrd = solo_return_wwptr(dst_frme);

    memcpy(wwptrd->frame_ctr_info, wwptrs->frame_ctr_info
	   , sizeof(struct frame_ctr_info));
    wwptrd->frame_ctr_info->window_num = dst_frme;
    if(wwptrd->frame_ctr_info->centering_options != SOLO_LINKED_POSITIONING) {
	wwptrd->frame_ctr_info->reference_frame = dst_frme+1;
    }
    wwptrd->frame_ctr_info->changed = YES;
    dd_copy_pisp(wwptrs->center_of_view, wwptrd->center_of_view);
}
/* c------------------------------------------------------------------------ */

void solo_cpy_lock_info(src_frme, dst_frme)
  int dst_frme, src_frme;
{
    WW_PTR wwptrs, wwptrd, solo_return_wwptr();

    if (src_frme == dst_frme)
      { return; }
    wwptrs = solo_return_wwptr(src_frme);
    wwptrd = solo_return_wwptr(dst_frme);

    memcpy(wwptrd->lock, wwptrs->lock, sizeof(struct solo_plot_lock));
    wwptrd->lock->window_num = dst_frme;
    wwptrd->lock->changed = YES;
}
/* c------------------------------------------------------------------------ */

void solo_cpy_parameter_info(src_frme, dst_frme)
  int dst_frme, src_frme;
{
    WW_PTR wwptrs, wwptrd, solo_return_wwptr();

    if (src_frme == dst_frme)
      { return; }

    wwptrs = solo_return_wwptr(src_frme);
    wwptrd = solo_return_wwptr(dst_frme);

    memcpy(wwptrd->parameter, wwptrs->parameter
	   , sizeof(struct solo_parameter_info));
    wwptrd->parameter->window_num = dst_frme;
    wwptrd->palette = wwptrs->palette;
    wwptrd->parameter->changed = YES;
    wwptrd->color_bar_location = wwptrs->color_bar_location;
    wwptrd->color_bar_symbols = wwptrs->color_bar_symbols;
}
/* c------------------------------------------------------------------------ */

void solo_cpy_sweep_info(src_frme, dst_frme)
  int dst_frme, src_frme;
{
    WW_PTR wwptrs, wwptrd, solo_return_wwptr();

    if (src_frme == dst_frme)
      { return; }

    wwptrs = solo_return_wwptr(src_frme);
    wwptrd = solo_return_wwptr(dst_frme);

    memcpy(wwptrd->sweep, wwptrs->sweep, sizeof(struct solo_sweep_file));
    wwptrd->sweep->window_num = dst_frme;
    wwptrd->ddir_radar_num = wwptrs->ddir_radar_num;
    strcpy(wwptrd->radar->radar_name, wwptrs->sweep->radar_name);
    strcpy(wwptrd->show_file_info, wwptrs->show_file_info);
    wwptrd->sweep->changed = YES;
    wwptrd->xy_plane_horizontal = wwptrs->xy_plane_horizontal;
    dd_copy_pisp(wwptrs->radar_location, wwptrd->radar_location);
    strcpy(wwptrd->start_time_text, wwptrs->start_time_text);
    strcpy(wwptrd->stop_time_text, wwptrs->stop_time_text);

    wwptrd->d_prev_time_stamp = wwptrs->d_prev_time_stamp;
    wwptrd->d_sweepfile_time_stamp = wwptrs->d_sweepfile_time_stamp;
    wwptrd->swpfi_time_sync_set = wwptrs->swpfi_time_sync_set;
    wwptrd->swpfi_filter_set = wwptrs->swpfi_filter_set;
    strcpy (wwptrd->filter_scan_modes, wwptrs->filter_scan_modes);
    wwptrd->filter_fxd_ang = wwptrs->filter_fxd_ang;
    wwptrd->filter_tolerance = wwptrs->filter_tolerance;
}
/* c------------------------------------------------------------------------ */

void solo_cpy_view_info(src_frme, dst_frme)
  int dst_frme, src_frme;
{
    WW_PTR wwptrs, wwptrd, solo_return_wwptr();

    if (src_frme == dst_frme)
      { return; }
    wwptrs = solo_return_wwptr(src_frme);
    wwptrd = solo_return_wwptr(dst_frme);

    memcpy(wwptrd->view, wwptrs->view
	   , sizeof(struct solo_view_info));
    wwptrd->view->window_num = dst_frme;
    wwptrd->view->changed = YES;
    wwptrd->magic_rng_annot = wwptrs->magic_rng_annot;
    
}
/* c------------------------------------------------------------------------ */

char *solo_gen_file_info(frme)
  int frme;
{
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);
    return(wwptr->show_file_info);
}
/* c------------------------------------------------------------------------ */

int solo_set_lock_info(swi)
  struct sweep_widget_info *swi;
{
    /* this routine examines the info from the lock widget
     *
     */
    int frme=swi->frame_num;
    int ww, it, ii, i, j, k, n, mark, changed=NO, ok;
    int changed_center=NO;
    int dangling_links[SOLO_MAX_WINDOWS];
    WW_PTR wwptr, wwptrc, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    char mess[256];

    if(frme < 0 || frme >= SOLO_MAX_WINDOWS) {
	sprintf(mess, "Bad frame number: %d\n", frme);
	solo_message(mess);
	return(SOLO_BAD_DIR_NAME);
    }
    wwptr = solo_return_wwptr(frme);
    wwptr->lock->changed = YES;
    spi = solo_return_winfo_ptr();

    /* deal with the links
     */
    swi->lockstep_links[frme] = YES;	/* make sure this window is linked */

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(wwptr->lock->linked_windows[ww] && !swi->lockstep_links[ww])
	      dangling_links[ww] = YES;
	else
	      dangling_links[ww] = NO;
	wwptr->lock->linked_windows[ww]  = swi->lockstep_links[ww];
    }
    /* now copy this lock struct to the other linked windows
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	wwptrc = solo_return_wwptr(ww); /* pointer to next linked frame */
	wwptrc->lock->changed = YES;
	if(ww == frme || !wwptr->lock->linked_windows[ww])
	      continue;
	memcpy(wwptrc->lock, wwptr->lock
	       , sizeof(struct solo_plot_lock));
	wwptrc->lock->window_num = ww;
    }
    /* now deal with the dangling links
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(!dangling_links[ww])
	      continue;
	/* the dangling list becomes the links for the dangling frames
	 */
	wwptrc = solo_return_wwptr(ww);
	for(j=0; j < SOLO_MAX_WINDOWS; j++) 
	      wwptrc->lock->linked_windows[j] = dangling_links[j];
    }

    return(0);

}
/* c------------------------------------------------------------------------ */

int solo_set_parameter_info(pwi)
  struct parameter_widget_info *pwi;
{
    /* this routine examines the info from the parameter and
     * palette widgets and
     *
     */
    int frme=pwi->frame_num;
    int ww, it, ii, jj, nn, mark, changed=NO;
    int dangling_links[SOLO_MAX_WINDOWS];
    int directory_changed, filename_changed=NO, ok=YES;
    int changed_parameter, changed_palette, check_widget_info=YES;
    int changed_center, changed_increment, changed_min_max = NO;
    int push_palette = NO, changed_color_table = NO;
    int dd_tokenz();
    WW_PTR wwptr, wwptrc, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_list_mgmt *lm;
    char *aa;

    struct solo_palette_ptr
	  *pal=NULL, *top
		, *solo_get_palette()
		      , *solo_malloc_palette()
			    , *solo_push_palette()
				  , *solo_topof_palettes()
					, *solo_this_palette();

    double d, fabs(), d_min, d_max, atof();
    float f_min, f_max;
    char mess[256], directory[128], filename[128];
    char string_space[64], *strptrs[16];
    XColor xcolor;


    if(frme < 0 || frme >= SOLO_MAX_WINDOWS) {
	sprintf(mess, "Bad frame number: %d\n", frme);
	solo_message(mess);
	return(SOLO_BAD_DIR_NAME);
    }
    wwptr = solo_return_wwptr(frme);
    wwptr->parameter->changed = YES;
    spi = solo_return_winfo_ptr();


# ifdef FIXME

    pal = solo_this_palette(wwptr->palette->palette_name
			    , solo_topof_palettes());
    changed_center = FABS(pal->at->center - pwi->center) > .001;
    changed_increment = FABS(pal->at->increment - pwi->increment) > .001;
    changed_palette = strcmp
	  (pal->at->palette_name, pwi->palette_name)
		|| pwi->palette_num >= 0;
    changed_color_table = pwi->color_file_num >= 0 ||
      strcmp(pwi->color_table_dir, pal->at->color_table_dir);

    if(pwi->palette_num >= 0) {
	top = solo_topof_palettes();
	for(ii=0; top; ii++, top=top->next)
	      if(ii == pwi->palette_num)
		    break;
	strcpy(pwi->palette_name, top->at->palette_name);
	check_widget_info = NO;
	pwi->palette_num = -1;
    }
    changed_parameter = strcmp
	  (wwptr->parameter->parameter_name, pwi->parameter_name)
		|| pwi->parameter_num >= 0;
    if(pwi->parameter_num >= 0) {
	strcpy(pwi->parameter_name
	       , *(spi->list_parameters->list+pwi->parameter_num));
	pwi->parameter_num = -1;
    }

    if(pwi->color_file_num >= 0) {
	strcpy(pwi->color_table_name
	       , *(spi->list_color_tables->list+pwi->color_file_num));
	pwi->color_file_num = -1;
	push_palette = YES;
    }
    slash_path(directory, pwi->color_table_dir);
    strcpy(filename, pwi->color_table_name);


    if(changed_parameter && !changed_palette) {
	if(!pwi->add_parameter_to_palette) {
	    /* find or create a palette for this parameter
	     */
	    pal = solo_get_palette(pwi->parameter_name, frme);
	    check_widget_info = NO;
	    if(!changed_color_table) {
	       strcpy(directory, pal->at->color_table_dir);
	       strcpy(filename, pal->at->color_table_name);
	    }
	    pwi->add_parameter_to_palette = NO;
	}
	solo_check_zap_parms(pwi->parameter_name, pal);
	solo_subscribe_palette(frme, pal, pwi->parameter_name);
    }
    strcpy(wwptr->parameter->parameter_name, pwi->parameter_name);

    if(changed_palette) {
	if(!(pal = solo_this_palette(pwi->palette_name
				     , solo_topof_palettes()))) {
	    /* create a new palette from the current palette
	     */
	    pal = solo_malloc_palette(pwi->palette_name);
	    memcpy(pal->at, wwptr->palette, sizeof(struct solo_palette));
	    strcpy(pal->at->palette_name, pwi->palette_name);
	    pal = solo_push_palette(pal);
	    *pal->at->usual_parms = '\0';
	    sprintf(mess, "Generated new palette %s\n", pwi->palette_name);
	    solo_message(mess);
	}
	solo_check_zap_parms(pwi->parameter_name, pal);
	solo_subscribe_palette(frme, pal, pwi->parameter_name);
	push_palette = YES;
	if(!changed_color_table) {
	   strcpy(directory, pal->at->color_table_dir);
	   strcpy(filename, pal->at->color_table_name);
	}
    }

    changed_min_max = strcmp(wwptr->palette->message, pwi->min_max_string);

    if(changed_min_max) {
	strcpy(string_space, pwi->min_max_string);
	if((nn = dd_tokenz(string_space, strptrs, ", <\t\n")) < 2) {
	    changed_min_max = NO;
	}
	else if(nn == 2) {
	    ii = 0; jj = 1;
	}
	else if(nn == 4) {
	    ii = 2; jj = 3;
	}
	else {
	    changed_min_max = NO;
	}
	if(changed_min_max) {
	    if(sscanf(strptrs[ii], "%f", &f_min) < 1)
		  changed_min_max = NO;
	    if(sscanf(strptrs[jj], "%f", &f_max) < 1)
		  changed_min_max = NO;

	    if(changed_min_max && f_min < f_max) {
		wwptr->color_bar_min_val = f_min;
		wwptr->color_bar_max_val = f_max;
	    }
	    else {
		changed_min_max = NO;
	    }
	}
    }

    if(!changed_min_max && changed_center) {
	pal->at->center = pwi->center;
    }
    if(!changed_min_max && changed_increment) {
	pal->at->increment = pwi->increment;
    }
    if(changed_min_max || changed_center || changed_palette
       || changed_parameter || changed_increment || push_palette) {
	solo_push_palette(pal);	/* move it to the top */
    }

    if(check_widget_info) {
	/*
	 * see what numbers and colors have changed
	 */
	solo_parameter_numbers_check(pwi, pal);
	if(solo_parameter_colors_check(pwi, pal) == NO)
	      ok = NO;
    }
    /* check color table info
     */
    directory_changed = strcmp(wwptr->palette->color_table_dir, directory);
    filename_changed = strcmp(wwptr->palette->color_table_name, filename);

    if(directory_changed) {
	nn = solo_get_files(directory, spi->list_color_tables);
	if(nn < 1) {
	    sprintf(mess, "No files in dir: %s\n", directory);
	    solo_message(mess);
	    ok = NO;
	}
	else
	      strcpy(pal->at->color_table_dir, directory);
    }

    if(ok && filename_changed) { /* try to generate the new color table */
	if((nn = solo_hardware_color_table(frme, directory, filename)) < 1)
	      ok = NO;
	else
	      strcpy(pal->at->color_table_name, filename);
    }
    wwptr->palette = pal->at;
    strcpy(wwptr->parameter->palette_name, wwptr->palette->palette_name);
    solo_palette_message(frme);

# else
    strcpy(wwptr->parameter->parameter_name, pwi->parameter_name);
    strcpy(wwptr->parameter->palette_name, pwi->palette_name);
    wwptr->emphasis_min = pwi->emphasis_zone_lower;
    wwptr->emphasis_max = pwi->emphasis_zone_upper;
# endif 




    /* see what other windows use this palatte and be sure to
     * mark them as changed
     */
    wwptr->parameter->changed = YES;
    aa = wwptr->parameter->palette_name;

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {

      wwptrc = solo_return_wwptr(ww);
      if (strcmp (aa, wwptrc->parameter->palette_name) == 0)
	{
	  wwptrc->parameter->changed = YES;
	}
    }    


    /*
     * deal with the links
     */

    pwi->linked_windows[frme] = YES;	/* make sure this window is linked */

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(wwptr->parameter->linked_windows[ww] && !pwi->linked_windows[ww])
	      dangling_links[ww] = YES;
	else
	      dangling_links[ww] = NO;
	wwptr->parameter->linked_windows[ww]  = pwi->linked_windows[ww];
    }

    /* now copy this parameter struct to the other linked windows
     */

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(ww == frme || !wwptr->parameter->linked_windows[ww])
	      continue;
	wwptrc = solo_return_wwptr(ww); /* pointer to next linked frame */
	memcpy(wwptrc->parameter, wwptr->parameter
	       , sizeof(struct solo_parameter_info));
	wwptrc->parameter->window_num = ww;
    }
    /* now deal with the dangling links
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(!dangling_links[ww])
	      continue;
	/* the dangling list becomes the links for the dangling frames
	 */
	wwptrc = solo_return_wwptr(ww);
	for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) 
	      wwptrc->parameter->linked_windows[jj] = dangling_links[jj];
    }

    return(0);
}
/* c------------------------------------------------------------------------ */

int solo_set_sweep_info(swi)
  struct sweep_widget_info *swi;
{
    /* this routine examines the info from the sweep widget and
     * updates the window sweep info if it is ok
     */

    int frme=swi->frame_num;
    long time, time_now(), lnn;
    double dtime, ddfnp_list_entry(), time_span, d_start, d_stop;
    double d, d_time_stamp(), dd_relative_time();
    long mddir_file_info_v3();
    double d_mddir_file_info_v3();
    int ww, ii, j, n, mark, rn, num_swps, version
	  , dangling_links[SOLO_MAX_WINDOWS];
    int dir_changed, radar_changed=NO, rnx, vv, dd_crack_datime();
    int time_series, time_sync;
    char *aa, *bb, *cc, str[128], *mddir_radar_name(), *dts_print();
    char line[88], radar_name[16], mess[256], *mddir_radar_name_v3();
    struct dd_file_name **fnp, **mddir_return_swp_list();
    WW_PTR wwptr, wwptrc, wwptrx, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    DD_TIME dts, *d_unstamp_time();
    struct solo_list_mgmt *slm;


    if(frme < 0 || frme >= SOLO_MAX_WINDOWS) {
	sprintf(mess, "Bad frame number: %d\n", frme);
	solo_message(mess);
	return(SOLO_BAD_DIR_NAME);
    }
    solo_set_lock_info(swi);

    wwptr = solo_return_wwptr(frme);
    spi = solo_return_winfo_ptr();
    time_series = wwptr->view->type_of_plot & SOLO_TIME_SERIES;

    wwptr->swpfi_time_sync_set = time_sync = swi->timesync_flag;
    wwptr->swpfi_filter_set = swi->filter_flag;
    
    slash_path(str, swi->directory_name);
    dir_changed = strcmp(str, wwptr->sweep->directory_name);

    if(!strcmp(swi->start_time_text, wwptr->start_time_text)) {
	/* clear out this text, we don't want to analyse this string
	 * in the time series logic below unless it's been changed
	 */
	*swi->start_time_text = 0;
    }
    else if (!time_series) {
      dts.time_stamp = wwptr->d_sweepfile_time_stamp;
      d_unstamp_time(&dts);
      dts.day_seconds = 0;
      dts.hour = dts.minute = dts.second = dts.millisecond = 0;
      cc = swi->start_time_text;
      
      if(d = dd_relative_time(cc)) {
	wwptr->d_sweepfile_time_stamp += d;
      }
      else if(dd_crack_datime(cc, strlen(cc), &dts)) {
	wwptr->d_sweepfile_time_stamp = d_time_stamp(&dts);
      }
    }
    if(!strcmp(swi->stop_time_text, wwptr->stop_time_text)) {
	*swi->stop_time_text = 0;
    }
    
    if(1) {
	solo_trigger_dir_rescans(wwptr->sweep->directory_name);

	if((n = mddir_file_list_v3(frme, str)) < 1) {
	   sprintf(mess, "No sweep files in dir: %s\n", str);
	   sii_message (mess);
	   return(SOLO_BAD_DIR_NAME);
	}
	strcpy(wwptr->sweep->directory_name, str);
	/*
	 * see if the current radar exists in the new directory
	 */
	if((rn = mddir_radar_num_v3(frme, swi->radar_name)) < 0) {
	   rn = 0;
	   sprintf(mess, "No data exists for radar %s in %s\n"
		   , radar_name, str);
	   sprintf(mess+strlen(mess), "switching to radar %s\n"
		   , mddir_radar_name_v3(frme, rn));
	   g_message (mess);
	}
	wwptr->sweep->radar_num = wwptr->ddir_radar_num = rn;
	strcpy(wwptr->radar->radar_name, mddir_radar_name_v3(frme, rn));
	strcpy(wwptr->sweep->radar_name, mddir_radar_name_v3(frme, rn));
	wwptr->sweep->version_num = LATEST_VERSION;

	wwptr->sweep->time_stamp = wwptr->sweep->start_time =
	  wwptr->d_sweepfile_time_stamp = 
	  d_mddir_file_info_v3
	      (frme, wwptr->sweep->radar_num
	       , wwptr->d_sweepfile_time_stamp
	       , TIME_NEAREST, LATEST_VERSION, &vv
	       , wwptr->show_file_info
	       , wwptr->sweep->file_name);
    }
    if((time_span = wwptr->sweep->stop_time - wwptr->sweep->start_time)
       < .001) {
	time_span = 20;
    }
    wwptr->sweep->stop_time = wwptr->sweep->start_time + time_span;

    if(swi->sweep_num >= 0) {
      /* select this sweep
       */
      wwptr->sweep->start_time = dtime = wwptr->d_sweepfile_time_stamp = 
	ddfnp_list_entry(frme, wwptr->sweep->radar_num
			 , swi->sweep_num
			 , &wwptr->sweep->version_num
			 , wwptr->show_file_info
			 , wwptr->sweep->file_name);
      wwptr->sweep->stop_time = (time_series) ? dtime +time_span : dtime;
      wwptr->sweep->time_stamp = dtime;
    }
    
    if(time_series) {
	/* look at start and end time text
	 */
	dts.time_stamp = wwptr->sweep->start_time;
	d_unstamp_time(&dts);
	dts.day_seconds = 0;
	dts.hour = dts.minute = dts.second = dts.millisecond = 0;
	cc = swi->start_time_text;

	if(d = dd_relative_time(cc)) {
	    wwptr->sweep->start_time += d;
	}
	else if(dd_crack_datime(cc, strlen(cc), &dts)) {
	    wwptr->sweep->start_time = d_time_stamp(&dts);
	}

	wwptr->sweep->stop_time = wwptr->sweep->start_time + time_span;
	dts.time_stamp = wwptr->sweep->start_time;
	d_unstamp_time(&dts);
	dts.day_seconds = 0;
	dts.hour = dts.minute = dts.second = dts.millisecond = 0;
	cc = swi->stop_time_text;

	if(d = dd_relative_time(cc)) {
	    wwptr->sweep->stop_time = wwptr->sweep->start_time + d;
	}
	else if(dd_crack_datime(cc, strlen(cc), &dts)) {
	    wwptr->sweep->stop_time = d_time_stamp(&dts);
	}
	if(wwptr->sweep->stop_time <= wwptr->sweep->start_time) {
	    wwptr->sweep->stop_time = wwptr->sweep->start_time + time_span;
	}
	dts.time_stamp = wwptr->sweep->start_time;
	aa = dts_print(d_unstamp_time(&dts));
	strcpy(wwptr->start_time_text, aa);

	dts.time_stamp = wwptr->sweep->stop_time;
	aa = dts_print(d_unstamp_time(&dts));
	strcpy(wwptr->stop_time_text, aa);
    }
    swi->radar_num = -1;
    swi->sweep_num = -1;
    wwptr->sweep->changed = YES;
    wwptr->sweep->time_modified = time_now();
    strcpy(swi->filename, wwptr->show_file_info);

    if(swi->sweep_interval > 1)
	  wwptr->sweep->sweep_skip = swi->sweep_interval;
    else
	  wwptr->sweep->sweep_skip = 1;

    /*
     * deal with the links
     */

    swi->linked_windows[frme] = YES;	/* make sure this window is linked */

    for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	if(!(spi->active_windows[ii]))
	      continue;
	if(wwptr->sweep->linked_windows[ii] && !swi->linked_windows[ii])
	      dangling_links[ii] = YES;
	else
	      dangling_links[ii] = NO;
	wwptr->sweep->linked_windows[ii]  = swi->linked_windows[ii];
    }
    /* now copy this sweep struct to the other linked windows
     */
    for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	if(!(spi->active_windows[ii]))
	      continue;
	if(ii == frme || !wwptr->sweep->linked_windows[ii])
	      continue;
	solo_cpy_sweep_info(frme, ii);
    }
    /* now deal with the dangling links
     */
    for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	if(!(spi->active_windows[ii]))
	      continue;
	if(!dangling_links[ii])
	      continue;
	/* the dangling list becomes the links for the dangling frames
	 */
	wwptrc = solo_return_wwptr(ii);
	for(j=0; j < SOLO_MAX_WINDOWS; j++) 
	      wwptrc->sweep->linked_windows[j] = dangling_links[j];
	mark = 0;
    }
    solo_worksheet ();

    return(0);
}
/* c------------------------------------------------------------------------ */

void sp_set_landmark_info(lwi)
  struct landmark_widget_info *lwi;
{
    /* this routine examines the info from the view widget
     *
     */
    int frme=lwi->frame_num;
    int ww, it, ii, jj, kk, nn, mark, changed=NO, ok;
    int dangling_links[SOLO_MAX_WINDOWS];
    WW_PTR wwptr, wwptrc, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    double d, fabs();
    char mess[256];

    wwptr = solo_return_wwptr(frme);
    spi = solo_return_winfo_ptr();

    wwptr->landmark_info->landmark_options = lwi->options;
    wwptr->landmark_info->linked_windows[frme] = YES;
    wwptr->landmark_info->reference_frame =
	  wwptr->landmark_info->landmark_options != SOLO_LINKED_POSITIONING
		? frme + 1 : lwi->reference_frame;

    if(SOLO_FIXED_POSITIONING SET_IN lwi->options) {
	wwptr->landmark->latitude = lwi->latitude;
	wwptr->landmark->longitude = lwi->longitude;
	wwptr->landmark->altitude = lwi->altitude;
    }

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww])) {
	    wwptr->landmark_info->linked_windows[ww] = NO;
	    continue;
	}
	dangling_links[ww] =
	      wwptr->landmark_info->linked_windows[ww]
		    && !lwi->linked_windows[ww]
			  ? YES : NO;
	wwptr->landmark_info->linked_windows[ww]  =
	      lwi->linked_windows[ww];
    }

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(ww == frme || !wwptr->landmark_info->linked_windows[ww])
	      continue;
	sp_cpy_lmrk_info(frme, ww);
    }

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	wwptrc = solo_return_wwptr(ww);
# ifdef obsolete
	if(wwptr->landmark_info->linked_windows[ww])
	      if(wwptr->landmark_info->landmark_options ==
		 SOLO_LOCAL_POSITIONING)
		    wwptrc->landmark_info->reference_frame = ww+1;
# endif
	if(!dangling_links[ww])
	      continue;
	/* the dangling list becomes the links for the dangling frames
	 */
	for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) 
	      wwptrc->landmark_info->linked_windows[jj] = dangling_links[jj];
    }
}
/* c------------------------------------------------------------------------ */

void sp_set_center_of_view_info(pvi, cwi)
  struct view_widget_info *pvi;
  struct centering_widget_info *cwi;
{
    /* this routine examines the info from the view widget
     *
     */
    int frme=pvi->frame_num, time_series;
    int ww, it, ii, jj, kk, nn, mark, changed=NO, ok;
    int dangling_links[SOLO_MAX_WINDOWS], linked_windows[SOLO_MAX_WINDOWS];
    int ref_frme, option;
    WW_PTR wwptr, wwptrc, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    double d, fabs();
    char mess[256];

    wwptr = solo_return_wwptr(frme);
    time_series = wwptr->view->type_of_plot & SOLO_TIME_SERIES;
    spi = solo_return_winfo_ptr();

    ref_frme = cwi ? cwi->reference_frame :
	  wwptr->frame_ctr_info->reference_frame;

    if(ref_frme < 1 || ref_frme > SOLO_MAX_WINDOWS ||
       !spi->active_windows[ref_frme-1]) {
	sprintf(mess, "Illegal reference frame number: %d\n", ref_frme);
	sii_message (mess);
	return;
    }
    option = cwi ? cwi->options :
	  wwptr->frame_ctr_info->centering_options;

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	linked_windows[ww] = cwi ? cwi->linked_windows[ww] :
	    wwptr->frame_ctr_info->linked_windows[ww];  
    }

    if(pvi->ctr_from_last_click && time_series) {
	for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	    if(!spi->active_windows[ww] || !linked_windows[ww])
		  continue;
	    wwptrc = solo_return_wwptr(ww);
	    wwptrc->clicked_ctr_of_frame = YES;
	    wwptrc->clicked_ctr->latitude =
		  wwptrc->field_vals->next->lat;
	    wwptrc->clicked_ctr->longitude =
		  wwptrc->field_vals->next->lon;
	    wwptrc->clicked_ctr->altitude =
			wwptrc->field_vals->next->alt;
	    wwptrc->view->ts_ctr_km = wwptrc->clicked_range;
	}
	if(!cwi) {
	    return;
	}
    }
    else if(pvi->ctr_from_last_click) {
	for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	    if(!spi->active_windows[ww] || !linked_windows[ww])
		  continue;
	    wwptrc = solo_return_wwptr(ww);
	    wwptrc->clicked_ctr_of_frame = YES;
	    wwptrc->clicked_ctr->latitude =
		  wwptrc->field_vals->next->lat;
	    wwptrc->clicked_ctr->longitude =
		  wwptrc->field_vals->next->lon;
	    wwptrc->clicked_ctr->altitude =
			wwptrc->field_vals->next->alt;
	    if(cwi) {
		cwi->latitude = wwptrc->clicked_ctr->latitude;
		cwi->longitude = wwptrc->clicked_ctr->longitude;
		cwi->altitude = wwptrc->clicked_ctr->altitude;
	    }
	    wwptrc->center_of_view->rotation_angle = wwptrc->clicked_angle;
	    wwptrc->center_of_view->range =
		  wwptrc->clicked_ctr->range = wwptrc->clicked_range;
	    wwptrc->clicked_ctr->azimuth = wwptrc->field_vals->next->az;
	    wwptrc->clicked_ctr->elevation = wwptrc->field_vals->next->el;
	    dd_razel_to_xyz(wwptrc->clicked_ctr);
	}
	if(!cwi) {
	    return;
	}
    }
    else if(cwi) {
	if(fabs((double)(wwptr->center_of_view->rotation_angle
			 - cwi->az_of_ctr)) > .008) {
	    wwptr->center_of_view->rotation_angle = cwi->az_of_ctr;
	}
	if(fabs((double)(wwptr->center_of_view->range
			 - cwi->rng_of_ctr)) > .0008) {
	    wwptr->center_of_view->range = cwi->rng_of_ctr;
	}
    }
    else			/* widget was not popped up
				 */
	  return;
    /*
     * code to process centering widget info
     */
    wwptr->frame_ctr_info->centering_options = cwi->options;
    wwptr->frame_ctr_info->reference_frame =
	  wwptr->frame_ctr_info->centering_options != SOLO_LINKED_POSITIONING
		? frme + 1 : cwi->reference_frame;

    cwi->linked_windows[frme] = YES;
    if(cwi->options == SOLO_LINKED_POSITIONING)
	  cwi->linked_windows[cwi->reference_frame-1] = YES;

    if(SOLO_FIXED_POSITIONING SET_IN cwi->options) {
	wwptr->center_of_view->latitude = cwi->latitude;
	wwptr->center_of_view->longitude = cwi->longitude;
	wwptr->center_of_view->altitude = cwi->altitude;
    }
    pvi->ctr_from_last_click = 0;

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww])) {
	    wwptr->frame_ctr_info->linked_windows[ww] = NO;
	    continue;
	  }
	dangling_links[ww] =
	      wwptr->frame_ctr_info->linked_windows[ww]
		    && !cwi->linked_windows[ww]
			  ? YES : NO;
	wwptr->frame_ctr_info->linked_windows[ww]  =
	      cwi->linked_windows[ww];
    }

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(ww == frme || !wwptr->frame_ctr_info->linked_windows[ww])
	      continue;
	sp_cpy_ctr_info(frme, ww);
    }

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	wwptrc = solo_return_wwptr(ww);
# ifdef obsolete
	if(wwptr->frame_ctr_info->linked_windows[ww])
	      if(wwptr->frame_ctr_info->centering_options ==
		 SOLO_LOCAL_POSITIONING)
		    wwptrc->frame_ctr_info->reference_frame = ww+1;
# endif
	if(!dangling_links[ww])
	      continue;
	/* the dangling list becomes the links for the dangling frames
	 */
	for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) 
	      wwptrc->frame_ctr_info->linked_windows[jj] = dangling_links[jj];
    }
}
/* c------------------------------------------------------------------------ */

int solo_set_view_info(pvi, lwi, cwi)
  struct view_widget_info *pvi;
  struct centering_widget_info *cwi;
  struct landmark_widget_info *lwi;
{
    /* this routine examines the info from the view widget
     *
     */
    int frme=pvi->frame_num, time_series;
    int ww, it, ii, i, j, k, n, mark, changed=NO, ok;
    int changed_center=NO;
    int dangling_links[SOLO_MAX_WINDOWS];
    WW_PTR wwptr, wwptrc, wwptrx, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_list_mgmt *lm;
    struct solo_field_vals *sfv;
    double d, fabs();
    char mess[256];

    if(frme < 0 || frme >= SOLO_MAX_WINDOWS) {
	sprintf(mess, "Bad frame number: %d\n", frme);
	solo_message(mess);
	return(SOLO_BAD_DIR_NAME);
    }
    wwptr = solo_return_wwptr(frme);
    wwptr->magic_rng_annot = pvi->magic_rng_annot;
    wwptr->view->type_of_plot = pvi->type_of_plot;
    time_series = wwptr->view->type_of_plot & SOLO_TIME_SERIES;

    if (time_series) {		/* set it in all windows */
      for (ii=0; ii < SOLO_MAX_WINDOWS; ii++)
	{ solo_return_wwptr(ii)->view->type_of_plot |= SOLO_TIME_SERIES; }
    }
    else {			/* clear it in all windows */
      for (ii=0; ii < SOLO_MAX_WINDOWS; ii++)
	{ solo_return_wwptr(ii)->view->type_of_plot &= ~SOLO_TIME_SERIES; }
    }
    spi = solo_return_winfo_ptr();

    if(lwi) {
	lwi->frame_num = frme;
	sp_set_landmark_info(lwi);
    }
    
    sp_set_center_of_view_info(pvi, cwi);
    /*
     * see what numbers have changed
     */
    if(FABS((wwptr->view->magnification -pvi->magnification)) >= .001) {
	wwptr->view->magnification = pvi->magnification > .001 ?
	      pvi->magnification : 1.0;
	changed = YES;
    }
    /* time series stuff
     */
    if(FABS((wwptr->view->ts_magnification -pvi->ts_magnification)) >= .001) {
	wwptr->view->ts_magnification = pvi->ts_magnification > .001 ?
	      pvi->ts_magnification : 1.0;
	changed = YES;
    }
    if (pvi->angular_fill_pct >= 0 && pvi->angular_fill_pct <= 500.) {
       wwptr->view->angular_fill_pct = pvi->angular_fill_pct;
    }
    if(FABS(wwptr->view->ts_ctr_km - pvi->ts_ctr_km) > .001) {
	wwptr->view->ts_ctr_km = pvi->ts_ctr_km;
    }

    if(pvi->az_annot_at_km <= 0) {
	wwptr->view->az_annot_at_km = -1;
    }
    else if(FABS((wwptr->view->az_annot_at_km
		     -pvi->az_annot_at_km)) > .001) {
	wwptr->view->az_annot_at_km = pvi->az_annot_at_km;
	changed = YES;
    }
    if(pvi->az_line_int_deg <= 0) {
      wwptr->view->az_line_int_deg = -1;
    }
    else if(FABS((wwptr->view->az_line_int_deg -pvi->az_line_int_deg))
       > .001) {
	wwptr->view->az_line_int_deg = pvi->az_line_int_deg;
	changed = YES;
    }

    if(pvi->rng_annot_at_deg <= 0) {
	wwptr->view->rng_annot_at_deg = -1;
    }
    else if(FABS((wwptr->view->rng_annot_at_deg
		     -pvi->rng_annot_at_deg)) > .001) {
	wwptr->view->rng_annot_at_deg = pvi->rng_annot_at_deg;
	changed = YES;
    }
    if(pvi->rng_ring_int_km <= 0) {
      wwptr->view->rng_ring_int_km = -1;
    }
    else if(fabs((double)(wwptr->view->rng_ring_int_km -pvi->rng_ring_int_km))
       > .001) {
	wwptr->view->rng_ring_int_km = pvi->rng_ring_int_km;
	changed = YES;
    }

    if(pvi->vert_tic_mark_km < .001) {
	wwptr->view->vert_tic_mark_km = 0;
    }
    else {
	wwptr->view->vert_tic_mark_km = pvi->vert_tic_mark_km;
    }
    if(pvi->horiz_tic_mark_km < .001) {
      wwptr->view->horiz_tic_mark_km = 0;
    }
    else {
      wwptr->view->horiz_tic_mark_km = pvi->horiz_tic_mark_km;
    }
    wwptr->view->changed = YES;

    /*
     * deal with the links
     */

    pvi->linked_windows[frme] = YES;	/* make sure this window is linked */

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww])) {
	    wwptr->view->linked_windows[ww] = NO;
	    continue;
	}
	if(wwptr->view->linked_windows[ww] && !pvi->linked_windows[ww])
	      dangling_links[ww] = YES;
	else
	      dangling_links[ww] = NO;
	wwptr->view->linked_windows[ww]  = pvi->linked_windows[ww];
    }
    /* now copy this view struct to the other linked windows
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(ww == frme || !wwptr->view->linked_windows[ww])
	      continue;
	solo_cpy_view_info(frme, ww);
    }
    /* now deal with the dangling links
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	if(!dangling_links[ww])
	      continue;
	/* the dangling list becomes the links for the dangling frames
	 */
	wwptrc = solo_return_wwptr(ww);
	for(j=0; j < SOLO_MAX_WINDOWS; j++) 
	      wwptrc->view->linked_windows[j] = dangling_links[j];
    }

    return(0);

}
/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

