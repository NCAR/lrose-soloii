/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

# define new_stuff

# include <dd_defines.h>
# include <solo_window_structs.h>
# include <dorade_headers.h>
# include <dd_files.h>
# include <input_sweepfiles.h>
# include <dd_math.h>
# include <function_decl.h>
# include <dgi_func_decl.h>


/*
 * tic marks become manditory
 * 
 * 
 * What other criteria are there for determining what gets plotted?
 *    Data characteristics
 */

struct times_que {
    struct times_que *last;
    struct times_que *next;
    int x_pxl;
    double time;
    double delta;
    double delta2;
    double av_delta;
    double t_pxl;
};

struct ts_linked_frames {
    struct ts_linked_frames *next;
    WW_PTR wwptr;
    WW_PTR lead_sweep;

    double time_span;
    double cell_num;
    double cells_per_pixel;
    double sweep_start;

    int sweep_count;
    int width;
    int height;
    int py0;
    int py_count;
    int type_of_plot;
    
    unsigned char *frame_image_ptr;
    unsigned char *at;

    struct times_que *tq;
};

# define TS_NUM_DELTAS 9

static struct ts_linked_frames *original_sweep_set[SOLO_MAX_WINDOWS];
static struct ts_linked_frames *linked_sweep_set[SOLO_MAX_WINDOWS];
static struct ts_linked_frames *linked_frames[2*SOLO_MAX_WINDOWS];

void sp_change_cursor();
void sii_xfer_images ();
void sii_update_frame_info (guint frame_num);
guint sii_frame_sync_num (guint frame_num);
guint sii_config_sync_num (guint frame_num);


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
int solo_worksheet();		/* solo_perusal.c */
int solo_parameter_setup();	/* solo_perusal.c */
int solo_busy();		/* sp_basics.c */


/* internal functions */
int sp_ts_data_loop();
void sp_ts_line();
int tsDisplay();
void ts_ray_clone();
void sp_tsYpixel_info();


/* c------------------------------------------------------------------------ */

int sp_ts_data_loop(tsLink0, usi)
  struct ts_linked_frames *tsLink0;
  struct unique_sweepfile_info_v3 *usi;
{
   static int dia_count=0, dia_trip=54;
   static int count=0, trip=0;
   static int ray_count=0, ray_trip = 240;
   static int line_count=0, line_trip = 123;

   struct ts_linked_frames *tsLink=tsLink0, *tsl;
   int lead_frame=tsLink0->lead_sweep->window_num;
   int ii, jj, kk, mm, nn, mark, mark2, ww, rayindex=0, halted=NO, advance=0;
   int px_count, seed_deltas=TS_NUM_DELTAS, mark0;
   int rc_thr=TS_NUM_DELTAS;
   int top_down=NO, msl_relative=NO, right_left=NO, automatic=NO;
   int x_pxl, width, xinc, xstop, xmax;
   WW_PTR wwptr, solo_return_wwptr();
   struct dd_general_info *dgi, *dd_window_dgi();
   double d, d_nn, delta0, delta, delta2, avg_delta=0, sum_deltas=0;
   double rcp_num_deltas=1./(double)TS_NUM_DELTAS;
   double t_pxl, pxl_span, rcp_pxl_span, dd_rotation_angle();
   double sstime;
   float rotang;
   struct times_que *tq;
   char *aa, *bb, *ee;
   struct ts_ray_table *tsrt;
   struct ts_sweep_info *tssi;
   struct ts_ray_info *tsri, *tsri_right;
   guint frame_sync_nums[SOLO_MAX_WINDOWS];


   wwptr = solo_return_wwptr(lead_frame);
   sii_update_frame_info(lead_frame);
   dgi = dd_window_dgi(lead_frame, "UNKNOWN");
   top_down = tsLink0->type_of_plot & TS_PLOT_DOWN;
   right_left = tsLink0->type_of_plot & TS_PLOT_RIGHT_TO_LEFT;
   automatic = tsLink0->type_of_plot & TS_AUTOMATIC;
   msl_relative = tsLink0->type_of_plot & TS_MSL_RELATIVE;

   for(tsl = tsLink0; tsl; tsl = tsl->next) {
      ww = tsl->wwptr->window_num;
      if (ww >= sii_return_frame_count())
	{ break; }
      frame_sync_nums[ww] = sii_frame_sync_num (ww);
      sii_update_frame_info(ww);
      tsl->width = tsl->wwptr->view->width_in_pix;
      tsl->height = tsl->wwptr->view->height_in_pix;
      tsl->frame_image_ptr = (char *)tsl->wwptr->image->data;
      tsl->type_of_plot = tsl->wwptr->view->type_of_plot;
      /*
       * calculate the starting pixel number, the gate num of this pixel
       * and the gate increment for each subsequent pixel in the Y direction
       */
      sp_tsYpixel_info(tsl, top_down);
   }
   tsLink0->tq->time = dgi->time -1.; /* starts avg. delta at one sec.
				       */
   tsLink0->tq->av_delta = .2;
   if((pxl_span = (wwptr->sweep->stop_time - wwptr->sweep->start_time)
       /((double)tsLink0->width -1.)) <= 0) { /* time between pixels */
      return(0);
   }
   rcp_pxl_span = 1./pxl_span;

   /*
    * start time is assummed to be in the center of the first pixel
    * and stop time is assumed to be in the center of the last pixel
    * "t_pxl" is the trip time for associating the current ray with
    * the next pixel
    */
   sstime = t_pxl = wwptr->sweep->start_time - .5 * pxl_span;
   xmax = tsLink0->width -1;
   xinc = right_left ? -1 : 1;
   x_pxl = right_left ? xmax +1 : -1;

   if((delta = dgi->time - t_pxl) > 0) {
      /* skip to the first pixel that should contain data
       */
      advance = (int)(delta * rcp_pxl_span);
   }
   t_pxl += (double)advance * pxl_span;
   px_count = tsLink0->width -advance;
   tsrt = tsLink0->wwptr->tsrt;
   tsrt->sweep_count = -1;

   if(tsrt->max_ray_entries < tsLink0->width) {
      if(tsrt->tsri_top)
	free(tsrt->tsri_top);
      tsrt->max_ray_entries = tsLink0->width;
      mm = tsrt->max_ray_entries * sizeof(struct ts_ray_info);
      tsri = tsrt->tsri_top = (struct ts_ray_info *)malloc(mm);
      memset(tsri, 0, mm);
   }
   tssi = tsrt->tssi_top;
   tsri = tsrt->tsri_top;
   tsri_right = tsrt->tsri_top + tsrt->max_ray_entries;
    

   for(; tsri < tsri_right; tsri++) { /* initialize pixel/ray info */
      tsri->ray_num = -1;
   }
   x_pxl += (right_left ?  -advance : advance);
   dia_count = 0;

   /*
    * The pointers and pixel num are set up this way so when it is time 

    * to replicate they are refering to the last ray.
    *
    * now really loop through the data
    */

   for(;;) {			/* for each ray */
      mark0 = ray_count++;
      if(ray_count >= ray_trip) {
	 mark = 0;
      }
      mark2 = dia_count++;
      if (dia_count >= dia_trip)
	{ mark = 0; }

      delta = dgi->time - t_pxl;

      if (delta >= 0) {
	 line_count++;
	 if(line_count >= line_trip) {
	    mark = 0;
	 }
	 mm = nn = (int)(delta * rcp_pxl_span);
	 if (mm > 10)
	   { mark = 0; }
	 /*
	  * make sure there isn't a gap in the data
	  */
	 if(ray_count > rc_thr && mm > 0 && delta > 1.1 * avg_delta) {
	    /* fill in the prev ray to the avg_delta time
	     */
	    mm = avg_delta * rcp_pxl_span;
	 }
	 /* do any replication that's needed
	  */
	 if(nn > 0 && line_count == 1) {
	    /* first time through but the data starts beyond the first
	     * pixel becaue the time resolution is higher than the
	     * data resolution
	     */
	    mm = 0;
	 }

	 if(mm > 0) {
	    if (mm > 10)
	      { mark = 0; }
	    mark |= 8; mark = mm;
	    /* mm is the number of times we repeat the ray info
	     */
	    if((xstop = x_pxl + (mm * xinc)) < 0) {
	       mm = x_pxl;
	    }
	    else if(xstop > xmax) {
	       mm = xmax - x_pxl;
	    }
	    jj = mm;
	    for(kk=x_pxl+xinc;  jj-- ; kk += xinc) {
	       memcpy(tsrt->tsri_top + kk
		      , tsri, sizeof(struct ts_ray_info));
	    }
	    for(tsl = tsLink0; tsl; tsl=tsl->next) {
	       ts_ray_clone(tsl->at, tsl->at + xinc, tsl->py_count
			    , tsl->width, mm, top_down, right_left);
	    }
	 }
	 if(nn > 0) {
	    x_pxl += nn * xinc;
	    px_count -= nn;
	    t_pxl += nn * pxl_span;
	 }

	 x_pxl += xinc;
	 if(x_pxl < 0 || x_pxl > xmax) {
	    break;
	 }
	 tsri = tsrt->tsri_top + x_pxl;

	 for(tsl = tsLink0; tsl; tsl=tsl->next) {
	    /* for each frame using this sweep
	     */
	    count++;
	    if(count >= trip) {
	       ii=0;
	    }
	    if(automatic) {
	       top_down = in_sector((float)dd_rotation_angle(dgi)
				    , (float)135., (float)225.);
	    }
	    if(msl_relative || automatic) {
	       sp_tsYpixel_info(tsl, top_down);
	    }
	    ww = tsl->wwptr->window_num;
	    if (frame_sync_nums[ww] != sii_config_sync_num ((guint)ww))
	      { return -1; }
	    solo_color_cells(ww); /* assign colors */
	    tsl->at = tsl->frame_image_ptr + x_pxl +
	      tsl->py0 * tsl->width;
	    sp_ts_line(tsl, top_down);
	 }
	 if(tsrt->sweep_count != usi->swp_count) {
	    tsrt->sweep_count = usi->swp_count;
	    if(tsrt->sweep_count +1 > tsrt->max_sweep_entries) {
	       tsrt->max_sweep_entries += 11;
	       tsrt->tssi_top = (struct ts_sweep_info *)
		 realloc(tsrt->tssi_top, tsrt->max_sweep_entries *
			 sizeof(struct ts_sweep_info));
	    }
	    tssi = tsrt->tssi_top + tsrt->sweep_count;
	    tssi->dir_num = usi->dir_num;
	    tssi->ray_count = usi->num_rays;
	    strcpy(tssi->directory, usi->directory);
	    strcpy(tssi->filename, usi->filename);
	    mark |= 2;
	 }
	 t_pxl += pxl_span;
	 tsri->ray_num = usi->ray_num-1;
	 tsri->sweep_num = tsrt->sweep_count;
	 if(tsri->ray_num >= tssi->ray_count) {
	    mark |= 4;
	 }
      }	/* if (1) { */
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
      if(ddswp_next_ray_v3(usi) == END_OF_TIME_SPAN) {
	 break;
      }
      ee = (dgi->new_sweep) ? "new" : "same";
      if(dgi->new_sweep) {
	 for(tsl = tsLink0; tsl; tsl=tsl->next) {
	    tsl->wwptr->uniform_cell_spacing = 0;
	    solo_cell_lut(tsl->wwptr->window_num);
	    sp_tsYpixel_info(tsl, top_down);
	 }
      }
      dgi->new_sweep = dgi->new_vol = NO;
      /*
       * compute the average time diff between rays so we can detect
       * time gaps
       */
      tsl = tsLink0;
      delta2 = dgi->time -tq->time;
      tq = tsl->tq = tsl->tq->next;
      tq->time = dgi->time;
      tq->x_pxl = x_pxl;
      tq->t_pxl = t_pxl;
      tq->delta2 = delta2;
      tq->av_delta = .2;

      if (delta2 > 0) {

	 if (TS_NUM_DELTAS -seed_deltas > 3 && tq->last->delta2 < 2.2 && 
	     tq->last->last->delta2 > 2.2) {
	    mark = TS_NUM_DELTAS;
	    /* we have a forward time glitch */
	    t_pxl = tq->t_pxl = tq->last->last->t_pxl;
	    x_pxl = tq->x_pxl = tq->last->last->x_pxl;
	 }

	 d = delta2 > .2 ? .2 : delta2;

	 if(seed_deltas) {
	    seed_deltas--;
	    sum_deltas += d;
	    avg_delta = sum_deltas * rcp_num_deltas;
	 }
	 else {
	    sum_deltas -= tq->last->av_delta;
	    sum_deltas += d;
	    avg_delta = sum_deltas * rcp_num_deltas;
	 }
	 tq->av_delta = d;
	 tq->delta2 = delta2;
      }
   }

   for(tsl=tsLink0;  tsl;  tsl=tsl->next) {
      ww = tsl->wwptr->window_num;
      sii_xfer_images(ww, 0);	
      sii_reset_reconfig_flags(ww);
   }

   return(0);
}
/* c------------------------------------------------------------------------ */

void sp_ts_line(tsl, top_down)
  struct ts_linked_frames *tsl;
  int top_down;
{
    /* color in the line
     * remember y is really upside down
     */
    int ii=0, fw;
    unsigned long *cell_color = tsl->wwptr->cell_colors;
    int nn = tsl->py_count;
    double cn = tsl->cell_num, cinc = tsl->cells_per_pixel;
    unsigned char *at = tsl->at;

    fw = top_down ? tsl->width : -tsl->width;

    for(; nn--; at += fw, cn += cinc) {
	ii = (int)cn;
	*at = (unsigned char)(*(cell_color +ii));
    }
}
/* c------------------------------------------------------------------------ */

int tsDisplay(click_frme, command)
  int click_frme, command;
{
    static int plot_count=0;
    int ii, jj, nn, ok, vv, ww, frme, mark, broke=NO, lead_frame, one_pass;
    int contiguous=NO, return_flag=0, loop_count;
    WW_PTR wwptr, wwptrx, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct dd_general_info *dgi, *dd_window_dgi();
    char *aa, *bb, *solo_gen_file_info(), *getenv();
    char mess[256], *get_tagged_string();
    int original_sweep_count, original_sweep_set_count, sset;
    int frame_list[SOLO_MAX_WINDOWS], frame_count=0;
    int checked_off[SOLO_MAX_WINDOWS];
    long time_now();
    double d_mddir_file_info_v3(), t1, t2, t3, t4, dd_ts_start();
    struct dd_input_sweepfiles_v3 *dis, *dd_return_sweepfile_structs_v3();
    struct unique_sweepfile_info_v3 *usi;
    struct dd_input_filters *difs, *dd_return_difs_ptr();
    struct ts_linked_frames *tsLink0, *tsLink, *tsl;
    struct times_que *tq;
    DD_TIME dts, *d_unstamp_time();
    double d_time_stamp();
    char *dts_print();
    struct solo_list_mgmt *slm;
    struct solo_perusal_info *solo_return_winfo_ptr();


    if(solo_halt_flag()) {
	solo_clear_halt_flag();
	return(1);
    }
    if(solo_busy())
	  return(0);

    spi = solo_return_winfo_ptr();

    if(plot_count++ < 1) {
	for(ii=0; ii < 2*SOLO_MAX_WINDOWS; ii++) {

	    /* produce an array of linked frame structs
	     * to be used in sweep sets
	     */
	    tsLink = linked_frames[ii] = (struct ts_linked_frames *)
		  malloc(sizeof(struct ts_linked_frames));
	    memset(tsLink, 0, sizeof(struct ts_linked_frames));
	    for(jj=0; jj < TS_NUM_DELTAS; jj++) {
		tq = (struct times_que *)malloc(sizeof(struct times_que));
		memset(tq, 0, sizeof(struct times_que));
		if(!jj) {
		    tsLink->tq = tq;
		    tsLink->tq->last = tq;
		}
		else {
		    tq->last = tsLink->tq->last;
		    tsLink->tq->last->next = tq;
		}		    
		tq->next = tsLink->tq;
		tsLink->tq->last = tq;
	    }
	}
    }    
    solo_set_busy_signal();
    sp_change_cursor(NO);

    wwptr = solo_return_wwptr(click_frme);
    contiguous = wwptr->view->type_of_plot & TS_CONTIGUOUS_TIME;
    wwptrx = wwptr->lead_sweep;
    lead_frame = wwptrx->window_num;


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

    solo_worksheet();

    for(ii=0; ii < frame_count; ii++) {
	checked_off[frame_list[ii]] = NO;
    }
    original_sweep_count = original_sweep_set_count = 0;
    /*
     * set up links between frames using the same sweep plus
     * an array of unique sweeps in case we are plotting more than
     * one time series
     */
    for(ii=0; ii < frame_count; ii++) {
	if(checked_off[ww=frame_list[ii]])
	      continue;
	checked_off[ww] = YES;
	/*
	 * we are at the lead sweep for the next sweep set
	 */
	tsLink = original_sweep_set[original_sweep_set_count++] =
	      linked_frames[original_sweep_count++];
	tsLink->wwptr = wwptr = solo_return_wwptr(ww);
	tsLink->type_of_plot = tsLink->wwptr->view->type_of_plot;
	tsLink->sweep_count = -1;
	tsLink->lead_sweep = wwptr->lead_sweep;
	tsLink->next = NULL;
	if(frame_count == 1)
	      break;
	for(wwptr=wwptr->next_sweep;  wwptr;  wwptr=wwptr->next_sweep) {

	  if( wwptr->window_num < sii_return_frame_count()) {
	    tsLink->next = linked_frames[original_sweep_count++];
	    tsLink = tsLink->next;
	    tsLink->wwptr = wwptr;
	    tsLink->lead_sweep = wwptr->lead_sweep;
	    tsLink->next = NULL;
	  }
	    checked_off[wwptr->window_num] = YES;
	}
    }
    one_pass = command == FORWARD_FOREVER || command == BACKWARDS_FOREVER
	  ? NO : YES;


    for(;;) {
	for(sset=0; sset < original_sweep_set_count; sset++) {

	    tsLink = tsLink0 = original_sweep_set[sset];
	    lead_frame = tsLink->lead_sweep->window_num;
	    wwptrx = tsLink->lead_sweep;

	    switch(command) {
	    
	    case BACKWARDS_FOREVER:
	    case BACKWARDS_ONCE:
		if((tsLink->time_span = wwptrx->sweep->stop_time -
		    wwptrx->sweep->start_time) > 0) {
		    wwptrx->sweep->start_time -= tsLink->time_span;
		    wwptrx->sweep->stop_time -= tsLink->time_span;
		}
		break;
	    
	    case FORWARD_FOREVER:
	    case FORWARD_ONCE:
		if((tsLink->time_span = wwptrx->sweep->stop_time -
		    wwptrx->sweep->start_time) > 0) {
		    wwptrx->sweep->start_time += tsLink->time_span;
		    wwptrx->sweep->stop_time += tsLink->time_span;
		}
		break;
	    
	    default:
		break;
	    }

	    if((nn = mddir_file_list_v3
		(lead_frame, wwptrx->sweep->directory_name)) < 1 )
	      {
		 sprintf(mess, "Directory %s contains no sweep files\n"
			 , wwptrx->sweep->directory_name);
		 sii_message (mess);
		 broke = YES;
		 return_flag = 1;
		 break;
	      }
	    if((wwptrx->sweep->radar_num =
		mddir_radar_num_v3(lead_frame, wwptrx->sweep->radar_name)) < 0)
		  {
		     sprintf(mess, "No sweep files for radar %s\n"
			     , wwptrx->sweep->radar_name);
		     sii_message (mess);
		     broke = YES;
		     return_flag = 1;
		     break;
		  }

	    if((tsLink->time_span = wwptrx->sweep->stop_time -
		wwptrx->sweep->start_time) <= 0) {
		tsLink->time_span = 20;
	    }
	    if(!wwptrx->sweep->start_time) {
		wwptrx->sweep->start_time = wwptrx->sweep->time_stamp;
	    }
	    wwptrx->sweep->stop_time = wwptrx->sweep->start_time
		  + tsLink->time_span;
	
	    dts.time_stamp = wwptrx->sweep->start_time;
	    aa = strcpy(wwptrx->lead_sweep->start_time_text
			, dts_print(d_unstamp_time(&dts)));
	    dts.time_stamp = wwptrx->sweep->stop_time;
	    bb = strcpy(wwptrx->lead_sweep->stop_time_text
			, dts_print(d_unstamp_time(&dts)));
	    /*
	     * first see if there are any sweeps in the current
	     * time span
	     */
	    dis = dd_return_sweepfile_structs_v3();
	    if(!(usi = dis->usi_top)) {
		dis->usi_top = usi = (struct unique_sweepfile_info_v3 *)
		      malloc(sizeof(struct unique_sweepfile_info_v3));
		memset(usi, 0, sizeof(struct unique_sweepfile_info_v3));
	    }
	    usi->dir_num = usi->radar_num = lead_frame;
	    usi->ddir_radar_num = wwptrx->sweep->radar_num;
	    usi->version = LATEST_VERSION;
	    usi->new_version_flag = NO;
	    strcpy(usi->directory, wwptrx->sweep->directory_name);
	    nn = dd_sweepfile_list_v3(usi, wwptrx->sweep->start_time
				      , wwptrx->sweep->stop_time);
	    if(nn <= 0) {
	       sprintf(mess,
		       "Unable to produce sweep file list for radar: %s\nin %s\nFrom %s to %s"
		       , wwptrx->sweep->radar_name
		       , wwptrx->sweep->directory_name
		       , aa, bb
		       );
	       sii_message (mess);
	       broke = YES;
	       return_flag = 1;
	       break;
	    }

	    wwptrx->sweep->time_stamp = tsLink->sweep_start = 
		  dd_ts_start(lead_frame, wwptrx->sweep->radar_num
			      , wwptrx->sweep->start_time, &vv
			      , wwptrx->show_file_info
			      , wwptrx->sweep->file_name);


	    nn = dd_sweepfile_list_v3(usi, tsLink->sweep_start
				      , wwptrx->sweep->stop_time);
	    if(nn <= 0) {
	      sprintf(mess,
		      "Unable to produce sweep file list for radar: %s in %s From %s to %s"
		      , wwptrx->sweep->radar_name
		      , wwptrx->sweep->directory_name
		      , aa, bb
		      );
	       sii_message (mess);
	       broke = YES;
	       return_flag = 1;
	       break;
	    }
	    /* get the first ray
	     */
	    if(ddswp_next_ray_v3(usi) == END_OF_TIME_SPAN) {
		sprintf (mess, "Problems reading first ray of %s/%s\n"
			  , usi->directory, usi->filename);
		sii_message (mess);
		broke = YES;
		return_flag = 1;
		break;
	    }

	    sprintf (mess, "plot: %s to %s  %.3f - %.3f\n"
		     , wwptrx->lead_sweep->start_time_text
		     , wwptrx->lead_sweep->stop_time_text
		     , wwptrx->sweep->start_time
		     , wwptrx->sweep->stop_time
		     );
	    uii_printf (mess);
	
	    for(; tsLink; tsLink=tsLink->next) {
		/* for each linked sweep
		 */
		wwptr = tsLink->wwptr;
		ww = wwptr->window_num;
		solo_parameter_setup(ww);
		solo_ww_top_line(ww);
		wwptr->uniform_cell_spacing = 0;
		solo_cell_lut(ww);
	    }
	    /*
	     * loop through the data for this sweep set
	     */
	    sp_ts_data_loop(tsLink0, usi);

	    /*
	     * update sweep info in all frames just plotted
	     */
	    tsLink = tsLink0->next;
	    for(; tsLink; tsLink = tsLink->next) {
		solo_cpy_sweep_info(lead_frame, tsLink->wwptr->window_num);
		sii_update_swpfi_widget (tsLink->wwptr->window_num);
		sii_update_param_widget (tsLink->wwptr->window_num);
		sii_update_view_widget (tsLink->wwptr->window_num);
		if(tsLink->wwptr->window_num != lead_frame) {
		    strcpy(tsLink->wwptr->start_time_text
			   , tsLink->wwptr->lead_sweep->start_time_text);
		    strcpy(tsLink->wwptr->stop_time_text
			   , tsLink->wwptr->lead_sweep->stop_time_text);
		}
	    }
	    if(solo_halt_flag()) {
		solo_clear_halt_flag();
		one_pass = YES;
		break;
	    }
	}
	if(broke || one_pass)
	      break;
    }
    solo_clear_busy_signal();
    sp_change_cursor(YES);

    return(return_flag);
}
/* c------------------------------------------------------------------------ */

void ts_ray_clone(aa, cc, y_count, frame_width, nlines, top_down, right_left)
  char *aa, *cc;
  int y_count, frame_width, nlines, top_down, right_left;
{
    /* copies a vertical line of data from left to right and bottom up
     * "aa" points to the bottom pixel of the vertical line to be cloned
     * "cc" points to the bottom pixel along the same horizontal line where
     *     the cloning is to begin
     */
    int nn, fw, rl;
    char *bb, *dd;
    /*
     * 
     */
    fw = top_down ? frame_width : -frame_width;
    rl = right_left ? -1 : 1;

    for(; nlines--; cc += rl) {
	for(bb=aa,dd=cc,nn=y_count;  nn--;  bb += fw, dd += fw) {
	    *dd = *bb;
	}
    }
}
/* c------------------------------------------------------------------------ */

void sp_tsYpixel_info(tsLink, top_down)
  struct ts_linked_frames *tsLink;
  int top_down;
{
    int ii, jj, nc, ww, pn, ymax, mark, ucc;
    struct ts_linked_frames *tsl=tsLink;
    struct dd_general_info *dgi, *dd_window_dgi();
    double d, h, rmax, rmin, gs, rcp_gs, sp_meters_per_pixel();
    double rloc, rctr, rbottom, d_y0, ppm, mpp, rtop, dd_altitude();
    char mess[256];

    /* first pretend that (0,0) is in the lower left corner and
     * we are plotting increasing range from bottom to top
     */
    dgi = dd_window_dgi(tsl->lead_sweep->window_num, "UNKNOWN");
    nc = tsl->wwptr->number_cells -1;
    gs = tsl->wwptr->uniform_cell_spacing;
    rcp_gs = 1./gs;
    rmin = tsl->wwptr->uniform_cell_one;
    rmax = rmin + nc * gs;
    mpp = sp_meters_per_pixel()/tsl->wwptr->view->ts_magnification;
    ppm = 1./mpp;
    rctr = KM_TO_M(tsl->wwptr->view->ts_ctr_km);
    tsl->cells_per_pixel = mpp * rcp_gs;
    tsl->py_count = tsl->height;
    rbottom = rctr - .5 * (double)(tsl->py_count-1) * mpp;
    rtop = rctr + .5 * (double)(tsl->py_count-1) * mpp;
    ymax = tsl->height -1;


    if(tsl->type_of_plot & TS_MSL_RELATIVE) {
	d = KM_TO_M(dd_altitude(dgi));
	rloc = top_down ? d - rmin : d + rmin;
    }
    else {
	rloc = top_down ? 2. * rctr : 0;
    }

    if(top_down) {
	if((d = rtop - rloc) > 0) {
	    /* data starts below the top pixel
	     */
	    tsl->py0 = (int)(d * ppm);
	    tsl->py_count -= tsl->py0 +1;
	    tsl->cell_num = 0.5;
	}
	else {
	    tsl->py0 = 0;
	    tsl->cell_num = (rloc - rtop) * rcp_gs +.5;
	}
	if((d = (rloc - rmax) - rbottom) > 0) {	/* data stops before bottom */
	    tsl->py_count -= (int)(d * ppm +.5);
	}
    }
    else {			/* bottom up...reverse y coord */
	if((d = rloc - rbottom) > 0) {
	    /* data starts above the bottom pixel
	     */
	    ii = d * ppm;
	    tsl->py0 = ymax -ii;
	    tsl->py_count -= ii +1;
	    tsl->cell_num = 0.5;
	}
	else {
	    tsl->py0 = ymax;
	    tsl->cell_num = (rbottom -rloc) * rcp_gs +.5;
	}
	if((d = rtop - (rloc + rmax)) > 0) {
	    tsl->py_count -= (int)(d * ppm +.5);
	}
    }

    if(tsl->py_count < 0 || tsl->py0 < 0 || tsl->py0 > ymax) {
	sprintf (mess, "tsl->py_count < 0 : %d %.3f %.3f %.3f %.3f\n"
		  , tsl->py_count, dd_altitude(dgi)
		  , rmin, rmax, rloc);
	uii_printf (mess);
	tsl->py_count = 0;
	mark = 0;
    }
}
/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */







