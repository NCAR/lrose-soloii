
/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */


#define NU_TSTAMP


# include "soloii.h"
# include "solo_window_structs.h"
# include "sii_externals.h"
# include "xyraster.h"
# include <dorade_headers.h>
# include <seds.h>
# include <solo_list_widget_ids.h>
# include <solo_editor_structs.h>
# include <dd_files.h>
# include <sed_shared_structs.h>
# include <dd_math.h>
# include <function_decl.h>
# include <dgi_func_decl.h>

# include <stdarg.h>
# include <errno.h>
# include <dirent.h>

static int diag_flag=0;

struct linked_frames {
    struct linked_frames *next;
    WW_PTR wwptr;
    WW_PTR lead_sweep;
};

static struct linked_frames *original_sweep_set[SOLO_MAX_WINDOWS];
static struct linked_frames *linked_sweep_set[SOLO_MAX_WINDOWS];
static struct linked_frames *linked_frames[2*SOLO_MAX_WINDOWS];
static int original_sweep_set_count;
static int original_sweep_count;
static int real_sweep_set_count, nex_sweep_count;
static double nex_tol=.2;

# define HIGH_PRF 0
# define LOW_PRF 1

/* Sherries routines */
void sp_change_cursor();

/* external routines */
int tsDisplay();		/* color_bscan.c */
int solo_halt_flag();		/* sp_basics.c */
void solo_clear_halt_flag();	/* sp_basics.c */
void solo_set_busy_signal();	/* sp_basics.c */
void solo_cpy_sweep_info();	/* sp_accepts.c */
void dd_copy_pisp();		/* ddb_common.c */
void init_xyraster();		/* sp_xyraster.c */
void solo_ww_top_line();	/* sp_basics.c */
void solo_cell_lut();		/* sp_dorade.c */
void solo_clear_busy_signal();	/* sp_basics.c */
int ray_raster_setup();		/* sp_xyraster.c */
void solo_color_cells();	/* sp_dorade.c */
void xx_inner_loop();		/* sp_xyraster.c */
void yy_inner_loop();		/* sp_xyraster.c */
void solo_set_halt_flag();	/* sp_basics.c */
int solo_read_open();		/* sp_basics.c */
void solo_def_params();		/* solo_io.c */
void solo_subscribe_palette();	/* sp_pals.c */
int solo_hardware_color_table(); /* sp_colors.c */
int solo_palette_color_numbers(); /* sp_pals.c */
void solo_data_color_lut();	/* sp_colors.c */
int solo_color_check();		/* sp_colors.c */
int solo_pixelate_color();	/* sp_colors.c */
void solo_message();		/* solo.c */
void solo_palette_message();	/* sp_pals.c */
void sp_xyz_to_rtheta_screen (); /* sp_clkd.c */
void sp_rtheta_screen_to_xyz();	/* sp_clkd.c */
void yes_exit();		/* solo.c */
int solo_busy();		/* sp_basics.c */


/* internal routines */
int displayq();
void sp_nex_sweep_set();
int sp_data_loop();
int sp_diag_flag();
void sp_set_diag_flag();
void sp_replot_all();
void sp_batch_plots();
int sp_batch_mode();
int solo_worksheet();
int solo_nab_next_file();
int sp_nab_fixed_angle_from_fn();
int solo_parameter_setup();
void sp_landmark_offsets();
void sp_align_center();
void mouse_control_case();

/* c------------------------------------------------------------------------ */

void sii_xfer_images ();
void sii_update_frame_info (guint frame_num);


/* c------------------------------------------------------------------------ */

int displayq(click_frme, command)
  int click_frme, command;
{
    static int plot_count=0;
    static int no_access_count = 0;
    long mddir_file_info_v3();
    double d_mddir_file_info_v3();
    struct linked_frames *flink0, *flink, *flinkx, *last_flink;
    struct linked_frames *dz_sweeps[SOLO_MAX_WINDOWS];
    struct linked_frames *ve_sweeps[SOLO_MAX_WINDOWS];
    int frame_list[SOLO_MAX_WINDOWS], frame_count=0, loop_count;
    int checked_off[SOLO_MAX_WINDOWS];

    int ii, jj, kk, nn, ww, frme, mark, last_sweep_type, new_lead_sweep;
    int sset, pass_count, num_dz, num_ve, set_num, sweep_type=HIGH_PRF;

    int original_version=LATEST_VERSION;
    int original_replot=NO;
    int original_file_action=TIME_AFTER;
    int original_lead_frame, original_sweep_skip;
    int version, replot, backwards, forward, dz_next;
    int lead_frame, sweep_skip, file_action;

    int broke=NO, one_pass=YES, nexrad_mode;
    int double_break, triple_break;
    int new_worksheet=YES, ve_next, dz_only, ve_only, newer_sweeps, low_prf;
    /* c...mark */
    WW_PTR wwptr, wwptrc, wwptrx, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct dd_general_info *dgi, *dd_window_dgi();
    int solo_busy();
    char *aa, *solo_gen_file_info(), *getenv();
    char mess[256], *get_tagged_string();
    double d, dfxd0, dfxd1, dfxd2, d0, d2, fabs();
    double tstamp0, tstamp1, tstamp2, d_sweepfile_time_stamp;
    char file_info[88], file_name[88], str[256], *sptrs[32];
    int tvn, return_flag = 0, nt;
    float ff;

/*
 * forward declarations
 */
    void sp_nex_sweep_set ();
    void sp_landmark_offsets ();
    void sp_align_center ();


    if(solo_halt_flag()) {
	solo_clear_halt_flag();
	return(1);
    }
    if(solo_busy())
	  return(0);

    wwptr = solo_return_wwptr(click_frme);

    if(wwptr->view->type_of_plot & SOLO_TIME_SERIES) {
	ii = tsDisplay(click_frme, command);
# ifdef notyet
# endif
	return(ii);
    }

    sprintf (mess, "Enter displayq(%d,%d)", click_frme, command);
    sii_append_debug_stuff (mess);
    solo_set_busy_signal();
    sp_change_cursor(NO);
    spi = solo_return_winfo_ptr();
    /*
     * a sweep set is the set of frames that are plotting the same sweep
     */
    if(plot_count++ < 1) {
	for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    /* produce an array of linked frame structs
	     * to be used in sweep sets
	     */
	    linked_frames[ii] = (struct linked_frames *)
		  malloc(sizeof(struct linked_frames));
	    memset(linked_frames[ii], 0, sizeof(struct linked_frames));
	}
	if (1 || batch_mode) {
	  if (aa = getenv ("BATCH_THRESHOLD")) {
	    strcpy (str, aa);
	    nt = dd_tokens (str, sptrs);
	    if (nt >= 2) {
	      jj = sscanf (sptrs[1], "%f", &ff);
	      if (jj == 1) {
		batch_threshold_value = ff;
		batch_threshold_field = g_string_new (sptrs[0]);
	      }
	    }
	    else {
	      batch_threshold_field = g_string_new ("DM");
	      batch_threshold_value = -111;
	    }
	  }
	}
    }
    frame_count = 0;

    if(command == REPLOT_THIS_FRAME) {
	frame_list[frame_count++] = click_frme;
    }
    else {
        loop_count = sii_return_frame_count();
	for(frme=0; frme < loop_count; frme++) {
	    if(spi->active_windows[frme] && wwptr->lock->linked_windows[frme])
	      frame_list[frame_count++] = frme;
	}
    }

    switch(command) {

    case BACKWARDS_FOREVER:
	one_pass = NO;
	original_file_action = TIME_BEFORE;
	break;

    case BACKWARDS_ONCE:
	original_file_action = TIME_BEFORE;
	break;

    case REPLOT_LOCK_STEP:
    case REPLOT_THIS_FRAME:
	original_sweep_skip = 1;
	original_replot = YES;
	original_file_action = TIME_NEAREST;
	original_version = wwptr->sweep->version_num;
	break;

    case FORWARD_FOREVER:
	one_pass = NO;
	break;

    case FORWARD_ONCE:
	break;

    default:
	uii_printf("Illegal command to displayq: %d\n", command);
	return(1);
    }
    backwards = original_file_action == TIME_BEFORE;
    forward = original_file_action == TIME_AFTER;
    wwptr->sweep->version_num = original_version;
    solo_worksheet();

    for(ii=0; ii < frame_count; ii++) {
	checked_off[frame_list[ii]] = NO;
    }
    original_sweep_count = original_sweep_set_count = 0;

    for(ii=0; ii < frame_count; ii++) {
	if(checked_off[ww=frame_list[ii]])
	      continue;
	checked_off[ww] = YES;
	/*
	 * we are at the lead sweep for the next sweep set
	 */
	set_num = original_sweep_set_count++;
	flink0 = flink = original_sweep_set[set_num] =
	      linked_frames[original_sweep_count++];
	flink->wwptr = wwptr = solo_return_wwptr(ww);
	flink->lead_sweep = wwptr->lead_sweep;
	flink->next = NULL;
	if(frame_count == 1)
	      break;
	for(wwptr=wwptr->next_sweep;  wwptr;  wwptr=wwptr->next_sweep) {
	    if (wwptr->window_num >= loop_count)
	      { break; }
	    flink->next = linked_frames[original_sweep_count++];
	    flink = flink->next;
	    flink->wwptr = wwptr;
	    flink->lead_sweep = wwptr->lead_sweep;
	    flink->next = NULL;
	    checked_off[wwptr->window_num] = YES;
	}
    }



    for(;;) {			/* be ready to plot this lockstep forever
				 */
	real_sweep_set_count = nex_sweep_count = 0;
	if(new_worksheet) {
	    new_worksheet = NO;
	    /*
	     * put all the sweep links back in case
	     * they've been twiddled by nexrad
	     */
	    solo_worksheet();
	}

	for(sset=0; sset < original_sweep_set_count; sset++) {

	    flink0 = original_sweep_set[sset];
	    original_lead_frame = lead_frame =
		  flink0->lead_sweep->window_num;
	    sweep_skip = original_replot ? 1 :
		  flink0->lead_sweep->sweep->sweep_skip;
	    file_action = original_file_action;
	    version = original_version;
	    replot = original_replot;
	    num_dz = num_ve = 0;
	    nexrad_mode = ve_next = dz_next = NO;

	    for(pass_count=0;; pass_count++) {
		/* loop because we may have to move ahead
		 * to accommondate a nexrad split sweep
		 */
# ifndef obsolete
		if(pass_count > 2) {
		  aa = "Caught in an infinit loop! Disable NEXRAD mode.";
		  sii_message (aa);
		  return 0;
		}
# endif

		if(!solo_nab_next_file(lead_frame, file_action
				       , version, sweep_skip, replot)) {
		    /* could not access data for this plot
		     */
		    return_flag = 1;
		    if (!no_access_count++)
		    { sii_message ("Could not access data for the next plot"); }
		    broke = YES;
		    break;
		}
		no_access_count = 0;
		if(spi->defeat_nexrad_mode)
		      break;
		/*
		 * from to the end of the pass_count loop is
		 * code to try and plot two sweeps instead of one
		 * if each sweep just contains reflectivity or
		 * just velocity data
		 */
		sweep_skip = flink0->lead_sweep->sweep->sweep_skip;
		file_action = original_file_action;
		version = original_version;
		replot = original_replot;

		wwptr = solo_return_wwptr(lead_frame);
		dgi = dd_window_dgi(wwptr->window_num, "");

		if(strncmp(dgi->dds->vold->proj_name, "NEXRAD", 6))
		      break;
		newer_sweeps = !strncmp
		      (dgi->dds->vold->proj_name, "NEXRAD_", 7)
		      ? YES : NO;
		if(newer_sweeps && dgi->dds->radd->num_ipps_trans > 1) {
		    break;	/* both DZ and VE present */
		}
		sweep_type = dgi->dds->radd->interpulse_per1 > .00123 ?
		      LOW_PRF : HIGH_PRF;
		
		if(!pass_count) {	/* see what we need */
		    for(flink=flink0;  flink;  flink=flink->next) {
			if(strstr(flink->wwptr->parameter->parameter_name
				  , "DZ")) {
			    dz_sweeps[num_dz++] = flink;
			}
			else {
			    ve_sweeps[num_ve++] = flink;
			}
		    }
		}
		/* get the fixed angles before, after and for this scan
		 */
		tstamp1 = wwptr->d_sweepfile_time_stamp;
		if(!sp_nab_fixed_angle_from_fn
		   (wwptr->sweep->file_name, &dfxd1))
		      break;
		tstamp0 = d_mddir_file_info_v3
		      (lead_frame, wwptr->sweep->radar_num
		       , wwptr->d_sweepfile_time_stamp
		       , (int)TIME_BEFORE, (int)LATEST_VERSION, &tvn
		       , file_info, file_name);
		if(tstamp0 == tstamp1)  /* we're at the first sweep */
		      dfxd0 = -999.;
		else if(!sp_nab_fixed_angle_from_fn(file_name, &dfxd0))
		      break;

		tstamp2 = d_mddir_file_info_v3
		      (lead_frame, wwptr->sweep->radar_num
		       , wwptr->d_sweepfile_time_stamp
		       , (int)TIME_AFTER, (int)LATEST_VERSION, &tvn
		       , file_info, file_name);
		if(tstamp2 == tstamp1) /* we're at the last sweep */
		      dfxd2 = -999.;
		else if(!sp_nab_fixed_angle_from_fn(file_name, &dfxd2))
		      break;

		if(dfxd0 == -999. && dfxd2 == -999.)
		      break;	/* only one sweep */

		if(newer_sweeps) {
		    dz_only = sweep_type == LOW_PRF;
		    ve_only = sweep_type != LOW_PRF;
		}
		else {
		    /* if current fixed angle and previous fixed angle
		     * are close
		     */
		    ve_only = fabs(dfxd1 -dfxd0) <= nex_tol;
		    /*
		     * if current fixed angle and next fixed angle are close
		     */
		    dz_only = fabs(dfxd1 -dfxd2) <= nex_tol;
		}
		if((dz_only && ve_next) ||
		   !(dz_only || ve_only) ||
		   (dz_only && ve_only)) {
		    /*
		     * we are not in or near or cannot detect a split sweep
		     */
		    break;
		}

		if(ve_only && ve_next) {
		    /* we've already set up DZ for the previous
		     * sweep now set up to plot the velocities
		     */
		    sp_nex_sweep_set(num_ve, ve_sweeps);
		    break;
		}
		if(dz_only && dz_next) {
		    sp_nex_sweep_set(num_dz, dz_sweeps);
		    break;
		}

		if(dz_only) {
		    /* this sweep is the first sweep (DZ only)
		     * of a split sweep
		     */
# ifdef obsolete
		    if(num_dz && backwards && pass_count == 0) {
			/* at this point we have moved back one sweep and
			 * we are in a DZ only sweep;
			 * Assume the following sweep
			 * was a VE only sweep in the current split sweep and
			 * we need to move back into the previous sweep */
			sweep_skip = 1;
			file_action = TIME_BEFORE;
			version = LATEST_VERSION;
		    }
# else
		    if(num_dz && backwards && pass_count == 0) {
			/* at this point we have moved back one sweep and
			 * we are in a DZ only sweep;
			 * We should have encountered a velocity only sweep
			 * but didn't
			 */
			break;
		    }
# endif
		    else if(num_dz && lead_frame !=
		       dz_sweeps[0]->wwptr->window_num) {
			/* we're in the right DZ sweep but the lead frame
			 * is for velocity data; iterate again
			 */
			sweep_skip = 1;
			replot = YES;
			file_action = TIME_NEAREST;
			version = LATEST_VERSION;
			solo_cpy_sweep_info
			      (lead_frame, dz_sweeps[0]->wwptr->window_num);
			lead_frame = dz_sweeps[0]->wwptr->window_num;
		    }
		    else {
			if(num_dz) {
			    /* set up a sweep set for the
			     * DZ sweeps */
			    nexrad_mode = new_worksheet = ve_next = YES;
			    sp_nex_sweep_set(num_dz, dz_sweeps);
			    if(!num_ve)
				  break;
			}
			/* move forward to the velocity sweep
			 */
			sweep_skip = 1;
			file_action = TIME_AFTER;
			version = LATEST_VERSION;
			solo_cpy_sweep_info
			      (lead_frame
			       , ve_sweeps[0]->wwptr->window_num);
			lead_frame = ve_sweeps[0]->wwptr->window_num;
		    }

		} /* end if(dz_only */
	    
		else {		/* this sweep is the second sweep (VE & SW)
				 * of a split sweep */

		    if(num_ve && forward && pass_count == 0) {
			sweep_skip = 1;
			file_action = TIME_AFTER;
			version = LATEST_VERSION;
		    }
		    else if(num_ve && lead_frame !=
			    ve_sweeps[0]->wwptr->window_num) {
			sweep_skip = 1;
			replot = YES;
			file_action = TIME_NEAREST;
			version = LATEST_VERSION;
			solo_cpy_sweep_info
			      (lead_frame, ve_sweeps[0]->wwptr->window_num);
			lead_frame = ve_sweeps[0]->wwptr->window_num;
		    }
		    else {
			if(num_ve) {
			    nexrad_mode = dz_next = new_worksheet = YES;
			    sp_nex_sweep_set(num_ve, ve_sweeps);
			    if(!num_dz)
				  break;
			}
			/* we need to move to the DZ sweep
			 */
			file_action = forward ? TIME_AFTER : TIME_BEFORE;
			sweep_skip = 1;
			version = LATEST_VERSION;
			solo_cpy_sweep_info
			      (lead_frame, dz_sweeps[0]->wwptr->window_num);
			lead_frame = dz_sweeps[0]->wwptr->window_num;
		    }

		} /* end else (ve only)*/
		
	    } /* end for(pass_count */
	    
	    
	    if(!nexrad_mode) {	/* normal mode
				 */
		linked_sweep_set[real_sweep_set_count++] =
		      original_sweep_set[sset];
	    }
	    
	} /* end for(sset */
	
	if(broke)
	      break;
	
	/*
	 * do some initialization for each sweep set
	 */
	for(sset=0; sset < original_sweep_set_count; sset++) {
	    
	    flink = original_sweep_set[sset];
	    lead_frame = flink->lead_sweep->window_num;

	    for(; flink; flink=flink->next) {
		/* for each linked sweep
		 */
		wwptr = flink->wwptr;
		ww = wwptr->window_num;
		if(!solo_parameter_setup(ww)) {
		    broke = YES;
		    g_message ("Broke at solo_parameter_setup");
		    solo_clear_busy_signal();
		    sp_change_cursor(YES);
		    return 0;
		    break;
		}
		sii_param_set_plot_field
		  (ww, wwptr->parameter->parameter_name);
		if(ww != lead_frame) {
		    /* copy radar location
		     */
		    dd_copy_pisp(wwptr->lead_sweep->radar_location
				 , wwptr->radar_location);
		}
		sp_landmark_offsets(ww);
		sp_align_center(ww);
		sii_update_frame_info (ww);
		init_xyraster(ww);
		solo_ww_top_line(ww);
		wwptr->uniform_cell_spacing = 0;
		solo_cell_lut(ww);
		checked_off[ww] = YES;
	    }
	    if(broke)
		  break;
	}
	if(broke)
	  yes_exit();

	/*
	 * loop through the data for each sweep set
	 */

	for(sset=0; sset < real_sweep_set_count; sset++) {

	    if(sp_data_loop(linked_sweep_set[sset]) < 0) {
		one_pass = YES;
		break;
	    }
	    if(solo_halt_flag()) {
		solo_clear_halt_flag();
		one_pass = YES;
		break;
	    }
	}
	if(broke || one_pass)
	      break;

    } /* end for(;; */

    /* update sweep info in all frames plotted
     */
    for(ii=0; ii < real_sweep_set_count; ii++) {
	flink = linked_sweep_set[ii];
	lead_frame = flink->wwptr->lead_sweep->window_num;
	for(; flink; flink=flink->next) {
	    solo_cpy_sweep_info(lead_frame, flink->wwptr->window_num);
	    sii_update_swpfi_widget (flink->wwptr->window_num);
	    sii_update_param_widget (flink->wwptr->window_num);
	    sii_update_view_widget (flink->wwptr->window_num);
	    sii_edit_reset_times (flink->wwptr->window_num);
	}
    }

    solo_clear_busy_signal();
    sp_change_cursor(YES);

    return(return_flag);
}
/* c------------------------------------------------------------------------ */

void sp_nex_sweep_set(ns, sweep_list)
  int ns;
  struct linked_frames *sweep_list[];
{
    int ii, jj, kk;
    struct linked_frames *flink, *flinkx, *last_flink;

    jj = real_sweep_set_count++;

    for(ii=0; ii < ns; ii++) {
	kk = original_sweep_count + nex_sweep_count++;
	flink = linked_frames[kk];
	memcpy(flink, sweep_list[ii], sizeof(struct linked_frames));
	if(!ii) {
	    linked_sweep_set[jj] = flinkx = flink;
	}
	else {
	    last_flink->next = flink;
	    last_flink->wwptr->next_sweep = flink->wwptr;
	}
	flink->wwptr->lead_sweep = flink->lead_sweep = 
	      flinkx->wwptr;
	flink->wwptr->next_sweep = NULL;
	flink->next = NULL;
	last_flink = flink;
    }
    return;
}
/* c------------------------------------------------------------------------ */

int sp_data_loop(flink0)
  struct linked_frames *flink0;
{
    struct linked_frames *flink=flink0;
    int lead_frame=flink0->lead_sweep->window_num;

    static int count=0, trip=0;
    int ii, jj, nn, mark, ww, rayindex=0, halted=NO;
    int flag_count=0, set_flag, fd, loop = 0;
    char str[256];
    float ang;
    double d, angle0, angle1, prev_angle1, sector;
    double ang_fill;
    static float max_sector=4., ang0=220, ang1=250;
    struct dd_ray_sector *ddrc, *dd_ray_coverage();
    WW_PTR wwptr, solo_return_wwptr();
    struct xyras *rxy, *return_xyras();
    struct dd_general_info *dgi, *dd_window_dgi();



    wwptr = solo_return_wwptr(lead_frame);
    ang_fill = wwptr->view->angular_fill_pct * .01;
    dgi = dd_window_dgi(lead_frame, "UNKNOWN");
    prev_angle1 = EMPTY_FLAG;
    
    
    /*
     * now really loop through the data
     */
    
    for(;; loop++) {			/* for each ray */
       /*
	* get the sector to plot based on the rotation angle
	*/
       flink = flink0;
       ddrc = dd_ray_coverage(dgi, dgi->source_rat
			      , dgi->source_ray_num
			      , (float)0);
       
       if (ang_fill && ang_fill < 1.) {
	  sector = ddrc->sector < 0 ? -ang_fill : ang_fill;
	  angle0 = FMOD360(ddrc->rotation_angle -.5*sector);
	  angle1 = FMOD360(ddrc->rotation_angle +.5*sector);
       }
       else if(FABS(ddrc->sector) > max_sector) {
	  sector = ddrc->sector < 0 ? -max_sector*.5 : max_sector*.5;
	  angle0 = (prev_angle1 != EMPTY_FLAG) ? prev_angle1 :
	    FMOD360 (ddrc->rotation_angle -.5*sector);
	  angle1 = FMOD360 (ddrc->rotation_angle +.5*sector);
	  prev_angle1 = EMPTY_FLAG;
       }
	else {
	   sector = ddrc->sector;
	   angle0 = ddrc->angle0;
	   if (ang_fill) {
	      sector *= ang_fill;
	   }
	   else {
	      sector *= 1.2;
	   }
	   angle1 = FMOD360(angle0 + sector);
	   prev_angle1 = FMOD360(ddrc->rotation_angle +.5*ddrc->sector);
	   
	   /* all this thrashsing is to keep the plot from
	    * rotating when the angular fill is increased
	    */
	}
       
	for(ii=0; flink; flink=flink->next, ii++) {
	    count++;
	    if(count >= trip) {
		mark = 0;
	    }
	    ww = flink->wwptr->window_num;
	    rxy = return_xyras(ww);
	    ray_raster_setup(ww, angle0, angle1, rxy);
	    if(rxy->ignore_this_ray) {
# ifdef obsolete
	       g_string_sprintfa (gs_complaints, "%d)Ignore (%.2f,%.2f)"
				 , loop, ddrc->angle0, ddrc->angle1);
	       g_message (gs_complaints->str);
# endif
	       continue;
	    }
	    /*
	     * assign colors to the data and
	     * rasterize this variable for this ray
	     */
	    solo_color_cells(ww);
	    if(rxy->x_inner_loop) {
		xx_inner_loop(rxy);
	    }
	    else {
		yy_inner_loop(rxy);
	    }
	}
	if(dgi->source_ray_num >= dgi->dds->swib->num_rays) {
	    mark = 0;
	    break;
	}
        /*
         * Check for stop event.
         */
        if(solo_halt_flag()){
            uii_printf("HALTING\n");
	    halted = YES;
            break;
	 } 
	/* nab the next ray
	 */
	if((nn=dd_absorb_ray_info(dgi)) < 1 ) {
	    break;
	}
    }


    for(flink=flink0;  flink;  flink=flink->next) {
	ww = flink->wwptr->window_num;
	sii_set_sync_num (ww);
	sii_xfer_images (ww, NULL);
	sii_reset_reconfig_count(ww);
    }

    return(0);
}
/* c------------------------------------------------------------------------ */

int sp_diag_flag()
{
    return(diag_flag);
}
/* c------------------------------------------------------------------------ */

void sp_set_diag_flag(flag)
  int flag;
{
    diag_flag = flag;
}
/* c------------------------------------------------------------------------ */

void sp_replot_all()
{
    int ii, jj, nn, ww;
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    int displayq();

    solo_worksheet();
    spi = solo_return_winfo_ptr();

    for(ii=0; ii < spi->num_locksteps; ii++) {
	ww = spi->first_ww_for_lockstep[ii];
	displayq(ww, REPLOT_LOCK_STEP);
    }
}
/* c------------------------------------------------------------------------ */

int solo_worksheet()
{

  void sp_do_lock_color(int frame);

    int ww, ii, jj, kk, mm, n, nus=0, nun=0, checked_off[SOLO_MAX_WINDOWS]
	  , unlink[SOLO_MAX_WINDOWS], lock_num;
    int locks_changed=NO, current_frame_count;
    long *lock_list, *sweep_links;
    WW_PTR wwptr, wwptrx, wwptrc, solo_return_wwptr(), last_wwptr, lead_sweep;
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    char mess[256];


    spi = solo_return_winfo_ptr();
    current_frame_count = sii_return_frame_count ();

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	checked_off[ww]=NO;
	wwptr = solo_return_wwptr(ww);
	wwptr->next_lock = wwptr->next_sweep = NULL;
	if(wwptr->lock->changed) {
	    locks_changed = YES;
	}
	wwptr->lock->changed = NO;
    }

    /*
     * determine the frame links organization
     */

    for(spi->num_locksteps=ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!spi->active_windows[ww])
	      continue;
	if(checked_off[ww]) /* already examined */
	      continue;
	wwptr = solo_return_wwptr(ww);
	for(ii=ww; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    if(wwptr->lock->linked_windows[ii]) {
		/*
		 * this is the first window of a new lockstep
		 * record it and check in the rest of the frames
		 * in the lockstep
		 */
		last_wwptr = wwptr;
		lock_num = spi->num_locksteps++;
		spi->first_ww_for_lockstep[lock_num] = ww;
		for(jj=ii; jj < SOLO_MAX_WINDOWS; jj++) {
		    if(!spi->active_windows[jj])
			  continue;
		    if(wwptr->lock->linked_windows[jj]) {
			wwptrc = solo_return_wwptr(jj);
			wwptrc->lock_num = lock_num;
			last_wwptr->next_lock = wwptrc;
			last_wwptr = wwptrc;
			checked_off[jj] = YES;
		    }
		}
		break;
	    }
	}
    }
    if(locks_changed) {
	for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	    wwptr = solo_return_wwptr(ww);
	    wwptr->lock_color_num =
	      frame_border_colors[wwptr->lock_num]->pixel;
	}
    }
    for(ii=0; ii < SOLO_MAX_WINDOWS; checked_off[ii++]=NO);
    /*
     * we want to compile a list of unique sweeps for each lock step and
     * break any sweep links to frames outside the lock step
     */
    for(mm=0; mm < spi->num_locksteps; mm++) {
	/*
	 * for each lockstep
	 */
	wwptr = solo_return_wwptr(spi->first_ww_for_lockstep[mm]);
	lock_list = wwptr->lock->linked_windows;
	spi->sweeps_in_lockstep[mm] = 0;

	for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	    if(checked_off[ww] || !(*(lock_list+ww)) ||
	       !spi->active_windows[ww])
		  continue;	/* ignore! */
	    checked_off[ww] = YES;
	    /*
	     * we now have a window from the current lock list
	     * that has not been checked.
	     * record unique sweep info and
	     * break the sweep links to windows not in the
	     * current lockstep.
	     */
	    for(jj=0; jj < SOLO_MAX_WINDOWS; unlink[jj++]=NO);

	    wwptrx = solo_return_wwptr(0);
	    nus = spi->sweeps_in_lockstep[mm]++;
	    spi->work_sheet[mm][nus] = ww;
	    wwptr = solo_return_wwptr(ww);
	    last_wwptr = lead_sweep = wwptr->lead_sweep = wwptr;
	    sweep_links = wwptr->sweep->linked_windows;

	    for(nun=0,jj=ww+1; jj < SOLO_MAX_WINDOWS; jj++) {
		if(!spi->active_windows[jj])
		      continue;
		if(*(sweep_links+jj)) {
		    if(*(lock_list+jj)) {
			/* uses same source sweep */
			checked_off[jj] = YES;
			last_wwptr->next_sweep = solo_return_wwptr(jj);
			last_wwptr = last_wwptr->next_sweep;
			last_wwptr->lead_sweep = lead_sweep;
			spi->sweeps_in_lockstep[mm]++;
		    }
		    else {
			/* need to break sweep links to this frame
			 * because it is no longer part of the lock step
			 */
			*(sweep_links+jj) = NO;
			unlink[jj] = YES;
			nun++;
		    }
		}
	    }
	    if(nun) {
		/* unlink should have flags for windows that
		 * are not part of the current lock step but were linked to
		 * windows that are part of the current lock step
		 * for this sweep. So we break the links
		 */
		for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) {
		    if(!spi->active_windows[jj])
			  continue;
		    if(unlink[jj]) {
			/* links that were not part of the lock step
			 * should be preserved so
			 * unlink becomes the new set of links
			 * for these windows
			 */
			wwptr = solo_return_wwptr(jj);
			for(kk=0; kk < SOLO_MAX_WINDOWS; kk++) {
			    wwptr->sweep->linked_windows[kk] = unlink[kk];
			}
		    }
		}
	    }
	}
    }
    return(nus);
}
/* c------------------------------------------------------------------------ */

int solo_nab_next_file(frme, file_action, version, sweep_skip, replot)
  int frme, file_action, version, sweep_skip, replot;
{
    WW_PTR wwptr, solo_return_wwptr(), wwptrx;
    long mddir_file_info_v3(), time_now();
    double d_mddir_file_info_v3();
    struct dd_general_info *dgi, *dd_window_dgi();
    struct ts_sweep_info *tssi;
    char *a, mne[16], mess[256], *solo_gen_file_info(), *dd_scan_mode_mne();
    double dd_latitude(), dd_longitude(), dd_altitude(), dd_earthr();
    double dd_heading(), dd_roll(), dd_pitch(), dd_drift(), dd_tilt_angle();
    double diff;
    int ii, jj, n, rnx, vv, mark, filter_set = 0, vn, vn_ref, nsms, frame_num;
    struct solo_list_mgmt *slm;
    struct solo_perusal_info *solo_return_winfo_ptr();
    const char *smptr;
    float fxd, ffxd, tol;
    char str[256];
    char scan_modes[64], *smptrs[16];
    char scan_mode[8];
    int swpfi_time_sync = NO;


    wwptr = solo_return_wwptr(frme);
    sweep_skip = 1;

    /* The assumption is that the frame number is the first
     * frame of a set of linked frames sharing sweepfile information
     */
    swpfi_time_sync =
      wwptr->swpfi_time_sync_set && wwptr->lead_sweep->window_num != 0;

    if (swpfi_time_sync) {
      file_action = TIME_NEAREST;
# ifdef notyet
# endif
    }
    /* see if a filter has been set
     */
    if (file_action != TIME_NEAREST && wwptr->swpfi_filter_set) {
# ifdef obsolete
      filter_set = sii_return_filter_info
	(wwptr->swpfi_filter_set-1, scan_modes, &ffxd, &tol);
# else
      filter_set = 1;
      strcpy (scan_modes, wwptr->filter_scan_modes);
      ffxd = wwptr->filter_fxd_ang;
      tol = wwptr->filter_tolerance;
# endif
    }
    
    if(wwptr->sweep->changed) {
	wwptr->sweep->changed = NO;
	ddir_rescan_urgent(frme);
    }
    if(replot && version == LATEST_VERSION && wwptr->sweep_file_modified) {
	/* the editor has created new
	 * version of files for this radar
	 */
	ddir_rescan_urgent(frme);
    }
    if((n = mddir_file_list_v3(frme, wwptr->sweep->directory_name)) < 1 ) {
	sprintf(mess, "Directory %s contains no sweep files\n"
		  , wwptr->sweep->directory_name);
	g_message (mess);
	return(NO);
    }
    if((wwptr->sweep->radar_num =
	mddir_radar_num_v3(frme, wwptr->sweep->radar_name)) < 0)
	  {
	     sprintf(mess, "No sweep files for radar %s\n"
		     , wwptr->sweep->radar_name);
	     g_message (mess);
	     return(NO);
	  }

    if (filter_set) {
      d_mddir_file_info_v3(frme, wwptr->sweep->radar_num
			   , wwptr->d_sweepfile_time_stamp
			   , TIME_NEAREST, version, &vv
			   , wwptr->show_file_info
			   , wwptr->sweep->file_name);
      vn_ref = sii_split_file_comment
	(wwptr->sweep->file_name, &fxd, scan_mode);
      if (vn_ref < 0) {
	filter_set = 0;
      }
      nsms = dd_tokenz (scan_modes, smptrs, sii_item_delims());
    }

    for(ii=0; ii < sweep_skip || filter_set; ii++) {
	wwptr->d_prev_time_stamp = wwptr->d_sweepfile_time_stamp;

	if (swpfi_time_sync) {
	  wwptr->d_sweepfile_time_stamp =
	    solo_return_wwptr(0)->d_sweepfile_time_stamp;
# ifdef notyet
# endif
	}

	wwptr->sweep->time_stamp =
	  wwptr->sweep->start_time = 
	  wwptr->d_sweepfile_time_stamp = 
	  d_mddir_file_info_v3(frme, wwptr->sweep->radar_num
			       , wwptr->d_sweepfile_time_stamp
			       , file_action, version, &vv
			       , wwptr->show_file_info
			       , wwptr->sweep->file_name);

	if(!swpfi_time_sync && !replot && wwptr->d_prev_time_stamp ==
	   wwptr->d_sweepfile_time_stamp) {
				/* last sweep in the directory
				 * for this radar */
	  sprintf(mess, "Sweep list exhausted for frame: %d\n", frme);

# ifdef fixme
	  mark = sp_batch_mode();
# endif
	  if( !(mark = 1) ) {
	    sii_message (mess);
	  }
	  return(NO);
	}
	if (filter_set) {
	  vn = sii_split_file_comment
	    (wwptr->sweep->file_name, &fxd, scan_mode);
	  if (vn == vn_ref)
	    { continue; }
	  diff = FABS (fxd -ffxd);
	  if (diff > tol)
	    { continue; }
	  if (!nsms)
	    { break; }
	  for (jj=0; jj < nsms; jj++) {
	    if (strcmp (scan_mode, smptrs[jj]) == 0)
	      { break; }
	  }
	  if (jj < nsms)
	    { break; }
	}
    }
    wwptr->sweep->time_stamp =
      wwptr->sweep->start_time =
      wwptr->sweep->stop_time =
      wwptr->d_sweepfile_time_stamp;

    if(solo_read_open(wwptr) < 0) {
	/* problems with the open */
	return(NO);
    }
    dgi = dd_window_dgi(frme, "");
    slash_path(dgi->directory_name, wwptr->sweep->directory_name);
    dgi->in_swp_fid = wwptr->file_id;
    dd_absorb_header_info(dgi);
    dd_absorb_ray_info(dgi);
    wwptr->xy_plane_horizontal = dd_xy_plane_horizontal(dgi);
    wwptr->radar_location->latitude = dd_latitude(dgi);
    wwptr->radar_location->longitude = dd_longitude(dgi);
    wwptr->radar_location->altitude = dd_altitude(dgi);
    wwptr->radar_location->heading = dd_heading(dgi);
    wwptr->radar_location->tilt = dd_tilt_angle(dgi);
    wwptr->radar_location->tilt = dgi->dds->swib->fixed_angle;
    wwptr->radar_location->earth_radius =
	  dd_earthr(wwptr->radar_location->latitude);
    wwptr->radar_location->state = PISP_EARTH;
    wwptr->scan_mode = dgi->dds->radd->scan_mode;
    wwptr->sweep->radar_type = dgi->dds->radd->radar_type;
    strcpy(wwptr->sweep->scan_type
	   , dd_scan_mode_mne(dgi->dds->radd->scan_mode, mne));
    wwptr->time_modified = time_now();

    tssi = wwptr->tsrt->tssi_top +1;
    tssi->dir_num = frme;
    tssi->ray_count = dgi->dds->swib->num_rays;
    strcpy(tssi->directory, wwptr->sweep->directory_name);
    strcpy(tssi->filename, wwptr->sweep->file_name);
    wwptr->tsrt->sweep_count = 1;

    return(YES);
}
/* c------------------------------------------------------------------------ */

int sp_nab_fixed_angle_from_fn(fn, fxd)
  char *fn;
  double *fxd;
{
    float f;
    char *a=fn, *b, *c, *delim=".";

    if(!a || !strlen(a))
	  return(NO);

    for(; *a && *a != *delim; a++); /* file type */
    if(*a++ != *delim)
	  return(NO);
    for(; *a && *a != *delim; a++); /* time stamp */
    if(*a++ != *delim)
	  return(NO);
    for(; *a && *a != *delim; a++); /* radar_name */
    if(*a++ != *delim)
	  return(NO);
    for(; *a && *a != *delim; a++); /* version num */
    if(*a++ != *delim)
	  return(NO);
    if(!strstr(a, "_"))
	  return(NO);
    if(sscanf(a, "%f_", &f) != 1)
	  return(NO);
    *fxd = f;
    return(YES);
}
/* c------------------------------------------------------------------------ */

int solo_parameter_setup(frme)
  int frme;
{
    int ii, ww, pnum, ok=YES, np, nf;
    char *a, *solo_gen_file_info(), *str_terminate(), pname[12], mess[256];
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptr, wwptrc, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_palette_ptr
	  *pal=NULL, *solo_get_palette();
    double d, dinc;

    wwptr = solo_return_wwptr(frme);
    ww = wwptr->lead_sweep->window_num;
    dgi = dd_window_dgi(ww, "");

    pnum = dd_ndx_name(dgi, wwptr->parameter->parameter_name);
    if(pnum != wwptr->parameter_num 
       || wwptr->parameter_scale !=
       dgi->dds->parm[pnum]->parameter_scale
       || wwptr->parameter_bias !=
       dgi->dds->parm[pnum]->parameter_bias 
       || wwptr->parameter_bad_val != 
       dgi->dds->parm[wwptr->parameter_num]->bad_data
       )
	  {
	      wwptr->parameter->changed = YES;
	  }
    if(pnum < 0 ) {
	wwptr->parameter->changed = YES;
	sprintf(mess, "Parameter %s does not exist in this sweep"
		, wwptr->parameter->parameter_name);
	g_message (mess);
	pnum = (frme < dgi->num_parms) ? frme : frme % dgi->num_parms;
	str_terminate
	      (wwptr->parameter->parameter_name
	       , dgi->dds->parm[pnum]->parameter_name, 8);
	sprintf(mess, "Substituting %s"
		, wwptr->parameter->parameter_name);
	g_message (mess);
    }
    
    if(ok && wwptr->parameter->changed) {
	wwptr->parameter->changed = NO;
	wwptr->parameter_num = pnum;
	wwptr->parameter_scale =
	      dgi->dds->parm
		    [wwptr->parameter_num]->parameter_scale;
	wwptr->parameter_bias =
	      dgi->dds->parm
		    [wwptr->parameter_num]->parameter_bias;
	wwptr->parameter_bad_val = 
	      dgi->dds->parm
		    [wwptr->parameter_num]->bad_data;

	if((!solo_hardware_color_table (frme)))
	  { ok = NO; }

	solo_data_color_lut(frme);
    }
    return(ok);
}
/* c------------------------------------------------------------------------ */

void sp_landmark_offsets(frme)
  int frme;
{
    int ii, nn, ww;
    struct point_in_space pisp, radar, *landmark;
    struct landmark_info *lmi;
    WW_PTR wwptr, wwptrx, solo_return_wwptr();
    double d, pxm, sp_meters_per_pixel();

    wwptr = solo_return_wwptr(frme);
    pxm = wwptr->view->magnification/sp_meters_per_pixel();
    lmi = wwptr->landmark_info;
    dd_copy_pisp(wwptr->radar_location, &radar);
    landmark = wwptr->landmark;

    if(!lmi->landmark_options) { /* local radar is the landmark */
	wwptr->landmark_x_offset = 0;
	wwptr->landmark_y_offset = 0;
	dd_copy_pisp(wwptr->radar_location, landmark);
	strcpy(landmark->id, "SOL_LANDMARK");
 	return;
    }
    else if(SOLO_LINKED_POSITIONING SET_IN lmi->landmark_options) {
	ww = lmi->reference_frame -1;
	wwptrx = solo_return_wwptr(ww);
	dd_copy_pisp(wwptrx->radar_location, landmark);
	strcpy(landmark->id, "SOL_LANDMARK");
    }

    dd_latlon_relative(wwptr->landmark, &radar);

    if(strncmp("AIR", wwptr->lead_sweep->sweep->scan_type, 3) == 0) {
	wwptr->landmark_x_offset = 1000. * radar.x;
	wwptr->landmark_y_offset = 1000. * radar.z;
    }
    else {
	wwptr->landmark_x_offset = 1000. * radar.x;
	wwptr->landmark_y_offset = 1000. * radar.y;
    }
}
/* c------------------------------------------------------------------------ */

void sp_align_center(frme)
  int frme;
{
    struct frame_ctr_info *fci;
    struct point_in_space pisp, *radar, *center;
    WW_PTR wwptr, wwptrx, wwptrf, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);

    fci = wwptr->frame_ctr_info;
    radar = wwptr->radar_location;
    center = wwptr->center_of_view;
    /*
     * make sure we've got the radar location
     */
    if(frme != wwptr->lead_sweep->window_num)
	  dd_copy_pisp(wwptr->lead_sweep->radar_location, radar);


    if(SOLO_FIXED_POSITIONING SET_IN fci->centering_options) {
	/*
	 * "center" contains the lat/lon/alt of the fixed point
	 * first calcualte (x,y,z) relative the to radar
	 * which will come back in the radar pisp
	 */
	dd_latlon_relative(center, radar);
	sp_xyz_to_rtheta_screen(frme, radar, center);
	/*
	 * for possible future reference calculate the actual (x,y,z)
	 * of the center of the screen based on r, theta, and tilt
	 */
	sp_rtheta_screen_to_xyz(frme, radar, center);
    }
    else if(SOLO_LINKED_POSITIONING SET_IN fci->centering_options) {
	/* reference frame */
	wwptrf = solo_return_wwptr(fci->reference_frame-1);
	/*
	 * convert the r,theta of the screen center to and (x,y,z)
	 * value relative the lat/lon/alt of the radar
	 */
	sp_rtheta_screen_to_xyz(wwptrf->window_num, wwptrf->radar_location
				, wwptrf->center_of_view);
	/* calculate true lat/lon/alt
	 * of the center
	 */
	dd_latlon_shift(wwptrf->radar_location, center);

	if(frme != wwptrf->window_num) { /* not the reference frame */
	    /* get center (x,y,z) relative to radar in frame
	     */
	    dd_latlon_relative(center, radar);
	}
	/* calculate r,theta for this frame
	 */
	sp_xyz_to_rtheta_screen(frme, radar, center);
	sp_rtheta_screen_to_xyz(frme, radar, center);
    }
    else {			/* center is relative to the local radar
				 */
	sp_rtheta_screen_to_xyz(frme, radar, center);
	dd_latlon_shift(radar, center);
    }
    wwptr->clicked_ctr_of_frame = NO;
    return;
}
/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */












