/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

#define NU_TSTAMP

# include <ui.h>
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

# include <stdarg.h>

# ifndef SOLO_HALT
# define SOLO_EXIT 0x1
# define SOLO_HALT 0x2
# endif

extern int LittleEndian;
extern int Solo_state;

static int Solo_Busy_Signal=NO;
static struct solo_perusal_info *solo_perusal_info=NULL;

struct solo_perusal_info *solo_return_winfo_ptr();

/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */

char *sp_ts_ascii_time(frme, xpos, atime)
  int frme, xpos;
  char *atime;
{
    int width;
    char *aa, str[32], *dts_print();
    DD_TIME dts, *d_unstamp_time();
    double d, d_time_stamp(), time_span;
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);
    time_span = wwptr->sweep->stop_time - wwptr->sweep->start_time;
    width = wwptr->view->width_in_pix;

    if(wwptr->view->type_of_plot & TS_PLOT_RIGHT_TO_LEFT) {
	xpos = width -xpos -1;
    }
    dts.time_stamp = wwptr->sweep->start_time +
	  ((double)xpos/(double)width) * time_span;
    d_unstamp_time(&dts);
    sprintf(atime, "%02d:%02d:%02d", dts.hour, dts.minute, dts.second);
    return(atime);
}
/* c------------------------------------------------------------------------ */

void sp_ui_plot_cmds(arg, cmds)
  int arg;
  struct ui_command *cmds;	
{
# ifndef notyet
    /* "^" is the exclusive or symbol
     */
    int ii, nn, ww;
    long ltime;
    char *aa, *bb, str[64], *cc, *dir, *ftype, command[256], *getenv();
    struct ui_command *cmdq=cmds+1; /* point to the first argument */
    WW_PTR wwptr, solo_return_wwptr();
    struct dd_general_info *dgi, *dd_window_dgi();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    Window windowID=0, get_window();
    DD_TIME dts;
    float f;
    double d;
    char *suffix=NULL, mne[8], *dd_scan_mode_mne();


    spi = solo_return_winfo_ptr();
    wwptr = solo_return_wwptr((int)0);
    ww = wwptr->lead_sweep->window_num;
    dgi = dd_window_dgi(ww, "");


    if(!strncmp(cmds->uc_text, "set-x-tic-mag", 7)) {
	f = cmdq->uc_v.us_v_float;
	
	for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    wwptr = solo_return_wwptr(ii);
	    wwptr->view->x_tic_mag = f > 0 ? f : 0;
	}
    }
    else if(!strncmp(cmds->uc_text, "set-y-tic-mag", 7)) {
	f = cmdq->uc_v.us_v_float;
	
	for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    wwptr = solo_return_wwptr(ii);
	    wwptr->view->y_tic_mag = f > 0 ? f : 0;
	}
    }
    else if(!strncmp(cmds->uc_text, "set-angular-fill", 7)) {
	f = cmdq->uc_v.us_v_float;
	
	for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    wwptr = solo_return_wwptr(ii);
	    wwptr->view->angular_fill_pct = f > 0 ? f : 0;
	}
    }
    else if(!strncmp(cmds->uc_text, "panner-toggle", 7)) {
	nn = cmdq->uc_v.us_v_int;
	if(nn > SOLO_MAX_WINDOWS) {
	    for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
		if(!spi->active_windows[ii])
		      continue;
		wwptr = solo_return_wwptr(ii);
		wwptr->constrained_rasterz = wwptr->constrained_rasterz
		      ? NO : YES;
	    }
	}
	else {
	    wwptr = solo_return_wwptr(nn-1);
	    wwptr->constrained_rasterz = wwptr->constrained_rasterz ? NO : YES;
	}
    }
    else if(!strncmp(cmds->uc_text, "screen-clear-mode", 11)) {
# ifdef fixme       
       set_clear_screen_toggle();
# endif
    }
    else if(!strncmp(cmds->uc_text, "nexrad-mode", 7)) {
	spi->defeat_nexrad_mode = spi->defeat_nexrad_mode ? NO : YES;
	uii_printf("\n**** NEXRAD mode toggled %s! ****\n\n"
		  , spi->defeat_nexrad_mode ? "OFF" : "ON");
    }
    else if(!strncmp(cmds->uc_text, "gif-file", 5) ||
	!strncmp(cmds->uc_text, "jpeg-file", 5) ||
	!strncmp(cmds->uc_text, "png-file", 5) ||
	!strncmp(cmds->uc_text, "xwd-file", 5) ||
	!strncmp(cmds->uc_text, "postscript-file", 5) ||
	!strncmp(cmds->uc_text, "pnm-file", 5)) {

	cc = bb = command;
# ifdef fixme
	windowID = get_window(wwptr->view->frame_config +1);
# endif

# ifdef obsolete
	if(!strncmp(cmds->uc_text, "gif-file", 5)) {
	    ftype = "gif";
	    sprintf(command
		    , "xwd -id %d -nobdrs | xwdtopnm | ppmtogif > ", windowID);
	}
	else if(!strncmp(cmds->uc_text, "jpeg-file", 5) ||
# else
	if(!strncmp(cmds->uc_text, "jpeg-file", 5) ||
# endif
	   !strncmp(cmds->uc_text, "png-file", 5) ||
	   !strncmp(cmds->uc_text, "gif-file", 5)) {

	    if( !strncmp(cmds->uc_text, "jpeg-file", 5) ) {
	      ftype = "jpeg";
	      suffix = ".jpeg";
	    }
	    else if( !strncmp(cmds->uc_text, "png-file", 5)) {
	      ftype = "png";
	      suffix = ".png";
	    }
	    else if( !strncmp(cmds->uc_text, "gif-file", 5)) {
	      ftype = "gif";
	      suffix = ".gif";
	    }
	    sprintf(command
		    , "xwd -id %d -nobdrs | convert xwd:-  ", windowID);
	}
	else if(!strncmp(cmds->uc_text, "pnm-file", 5)) {
	    ftype = "pnm";
	    sprintf(command
		    , "xwd -id %d -nobdrs | xwdtopnm > ", windowID);
	}
	else if(!strncmp(cmds->uc_text, "postscript-file", 5)) {
	    ftype = "ps";
	    sprintf(command
		    , "xwd -id %d -nobdrs | xwd2ps > ", windowID);
	}
	else {
	    ftype = "xwd";
	    sprintf(command, "xwd -id %d -out ", windowID);
	}
	bb += strlen(bb);
	if(!(dir = getenv("WINDOW_DUMP_DIR")))
	    dir = wwptr->sweep->directory_name;
	slash_path(bb, dir);
	bb += strlen(bb);

# ifdef NU_TSTAMP
	ltime = (long)wwptr->sweep->start_time;
# else
	ltime = wwptr->view->type_of_plot & SOLO_TIME_SERIES
	      ? (long)wwptr->sweep->start_time : wwptr->sweep->time_stamp;
# endif

	dd_file_name(ftype, ltime, wwptr->sweep->radar_name, 0, bb);

	sprintf( bb+strlen(bb), "_%.1f_%s", dgi->dds->swib->fixed_angle
		 , dd_scan_mode_mne(dgi->dds->radd->scan_mode, mne));

	if( suffix )
	  { strcat( bb, suffix ); }
	ii = system(command);
    }
    else if(!strncmp(cmds->uc_text, "xwd-file", 5)) {
    }
# endif	
}
/* c------------------------------------------------------------------------ */

int sp_ts_service(arg, cmds)
  int arg;
  struct ui_command *cmds;	
{
# ifdef notyet
    /* "^" is the exclusive or symbol
     */
    int ii, nn, start, relative=NO, ok=YES;
    char *aa, *bb, str[64], *dts_print();
    struct ui_command *cmdq=cmds+1; /* point to the first argument */
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    DD_TIME dts, *d_unstamp_time();
    float f;
    double d, d_time_stamp(), dd_relative_time(), span, d_start, d_stop;
    double dd_relative_time();


    spi = solo_return_winfo_ptr();
    wwptr = solo_return_wwptr((int)0);
    dd_clear_dts(&dts);

    if(!strncmp(cmds->uc_text, "time-series", 6)) {
	for(nn=ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    wwptr = solo_return_wwptr(ii);
	    wwptr->view->type_of_plot ^= SOLO_TIME_SERIES;
	    if(!nn++) {
		aa = wwptr->view->type_of_plot & SOLO_TIME_SERIES
		      ? "ON" : "OFF";
	    }
	}
	uii_printf("\nSolo time-series plots toggled %s\n", aa);
    }
    else if(!strncmp(cmds->uc_text, "ts-up-down", 5)) {
	for(nn=ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    wwptr = solo_return_wwptr(ii);
	    wwptr->view->type_of_plot ^= TS_PLOT_DOWN;
	    if(!nn++) {
		aa = wwptr->view->type_of_plot & TS_PLOT_DOWN
		      ? "DOWNWARD" : "UPWARD";
	    }
	    uii_printf("\nSolo time-series plotting %s\n", aa);
	}
    }
    else if(!strncmp(cmds->uc_text, "ts-contiguous", 5)) {
	for(nn=ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    wwptr = solo_return_wwptr(ii);
	    wwptr->view->type_of_plot ^= TS_CONTIGUOUS_TIME;
	    if(!nn++) {
		aa = wwptr->view->type_of_plot & TS_CONTIGUOUS_TIME
		      ? "ON" : "OFF";
	    }
	    uii_printf("\nSolo time-series contiguous time toggled %s\n", aa);
	}
    }
    else if(!strncmp(cmds->uc_text, "ts-ground-relative", 5)) {
	for(nn=ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(!spi->active_windows[ii])
		  continue;
	    wwptr = solo_return_wwptr(ii);
# ifdef obsolete
	    wwptr->view->type_of_plot ^= TS_MSL_RELATIVE;
	    if(!nn++) {
		aa = wwptr->view->type_of_plot & TS_MSL_RELATIVE
		      ? "ON" : "OFF";
	    }
	    uii_printf("\nSolo time-series ground relative toggled %s\n", aa);
# else
	    wwptr->view->type_of_plot ^= TS_PLOT_RIGHT_TO_LEFT;
	    if(!nn++) {
		aa = wwptr->view->type_of_plot & TS_PLOT_RIGHT_TO_LEFT
		      ? "ON" : "OFF";
	    }
	    uii_printf("\nSolo time-series right-left toggled %s\n", aa);
# endif
	}
    }
    else if(!strncmp(cmds->uc_text, "ts-start-time", 7) ||
	    !strncmp(cmds->uc_text, "ts-stop-time", 7)) {
	if(cmdq->uc_ctype != UTT_END) {
	    dd_unquote_string(str, cmdq->uc_text);
	    start = strstr(cmds->uc_text, "ts-start") != NULL;

	    if(!wwptr->sweep->start_time) {
		wwptr->sweep->start_time = wwptr->sweep->time_stamp;
	    }
	    dts.time_stamp = wwptr->sweep->start_time;
	    d_unstamp_time(&dts);
	    dts.day_seconds = 0;
	    dts.hour = dts.minute = dts.second = dts.millisecond = 0;
	    d_start = wwptr->sweep->start_time;
	    d_stop = wwptr->sweep->stop_time;
	    if((span = d_stop - d_start) < 0) {
		span = 0;
	    }
	    ii = sscanf(str, "%d", &nn);

	    if(ii == 1 && nn == 0) {
		if(start) {
		    d = 0;
		    d_stop = span;
		}
		else {
		    d = d_start + span;
		}
	    }
	    else if(d = dd_relative_time(str)) {
		relative = YES;
	    }
	    else if(ok = dd_crack_datime(str, strlen(str), &dts)) {
		d = d_time_stamp(&dts);
	    }
	    if(ok) {
		if(start) {
		    if(relative) {
			d_start += d;
		    }
		    else {
			d_start = d;
		    }
		    if(span) {
			d_stop = d_start + span;
		    }
		}
		else {
		    if(relative) {	/* stop time relative */
			d_stop = wwptr->sweep->start_time + d;
		    }
		    else {
			d_stop = d;
		    }
		}
		for(nn=ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
		    if(!spi->active_windows[ii])
			  continue;
		    wwptr = solo_return_wwptr(ii);
		    wwptr->sweep->start_time = d_start;
		    wwptr->sweep->stop_time = d_stop;
		}
	    }
	}
    }
    else {
	uii_printf("\nNo match for %s\n", cmds->uc_text);
    }
# endif
    return(1);
}  
/* c------------------------------------------------------------------------ */

void solo_clear_busy_signal()
{
    Solo_Busy_Signal = NO;
}
/* c------------------------------------------------------------------------ */

void solo_set_busy_signal()
{
    Solo_Busy_Signal = YES;
}
/* c------------------------------------------------------------------------ */

int solo_busy()
{
    int ii;
    ii = Solo_Busy_Signal ? YES : NO;
    return(ii);
}
/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

void solo_return_radar_name(frme, name)
  int frme;
  char *name;
{
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);
    strcpy(name, wwptr->sweep->radar_name);
}
/* c------------------------------------------------------------------------ */

struct point_in_space *
solo_return_radar_pisp(frme)
  int frme;
{
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);
    return(wwptr->radar_location);
}
/* c------------------------------------------------------------------------ */

void solo_activate_frame(dst_frme)
  int dst_frme;
{
    /* current thinking is just to clone the last active window
     */
    int ii, jj, kk, nn, mark, ww, src_frme;
    WW_PTR wwptrd, wwptrb, wwptrs, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();

    wwptrd = solo_return_wwptr(dst_frme);
    spi = solo_return_winfo_ptr();
    spi->active_windows[dst_frme] = YES;
    /*
     * find the last active window
     */
    for(src_frme=dst_frme-1; src_frme >= 0; src_frme--) {
	if((spi->active_windows[src_frme]))
	      break;
    }
    wwptrs = solo_return_wwptr(src_frme);
    /*
     * before copying, fix links to this and other windows
     */
    for(ww=0; ww <= src_frme; ww++) {
	wwptrb = solo_return_wwptr(ww);
	if(wwptrs->parameter->linked_windows[ww]) {
	    wwptrb->parameter->linked_windows[dst_frme] = YES;
	}
	if(wwptrs->view->linked_windows[ww]) {
	    wwptrb->view->linked_windows[dst_frme] = YES;
	}
	if(wwptrs->sweep->linked_windows[ww]) {
	    wwptrb->sweep->linked_windows[dst_frme] = YES;
	}
	if(wwptrs->lock->linked_windows[ww]) {
	    wwptrb->lock->linked_windows[dst_frme] = YES;
	}
	if(wwptrs->landmark_info->linked_windows[ww]) {
	    wwptrb->landmark_info->linked_windows[dst_frme] = YES;
	}
	if(wwptrs->frame_ctr_info->linked_windows[ww]) {
	    wwptrb->frame_ctr_info->linked_windows[dst_frme] = YES;
	}
    }
    /*
     * parameter info
     */
    solo_cpy_parameter_info(src_frme, dst_frme);
    /*
     * view info
     */
    solo_cpy_view_info(src_frme, dst_frme);
    /*
     * sweep info
     */
    solo_cpy_sweep_info(src_frme, dst_frme);
    /*
     * lock info
     */
    solo_cpy_lock_info(src_frme, dst_frme);
    /*
     * landmark info
     */
    sp_cpy_lmrk_info(src_frme, dst_frme);
    /*
     * frame center info
     */
    sp_cpy_ctr_info(src_frme, dst_frme);
}
/* c------------------------------------------------------------------------ */

int solo_read_open(wwptr)
  struct solo_window_ptrs *wwptr;
{
    char str[256];
    char mess[256];

    slash_path(str, wwptr->sweep->directory_name);
    strcat(str, wwptr->sweep->file_name);
    if(wwptr->file_id > 0)
	  close(wwptr->file_id);
    if((wwptr->file_id = open(str, O_RDONLY, 0)) < 0) {
	sprintf(mess, "Unable to open sweep %s\n", str);
	solo_message(mess);
    }
    return(wwptr->file_id);
}
/* c------------------------------------------------------------------------ */

int solo_return_state()
{
    return(Solo_state);
}
/* c------------------------------------------------------------------------ */

struct solo_perusal_info *
solo_return_winfo_ptr()
{
    struct solo_list_mgmt *solo_malloc_list_mgmt();
    struct solo_perusal_info *next;
    /* c...mark */

    if(solo_perusal_info)
	  return(solo_perusal_info);

    solo_perusal_info = next = (struct solo_perusal_info *)
	  malloc(sizeof(struct solo_perusal_info));
    memset(next, 0, sizeof(struct solo_perusal_info));

    /* set up list managers */
    
    next->defeat_nexrad_mode = YES;
    next->list_sweeps = solo_malloc_list_mgmt(56);
    next->list_radars = solo_malloc_list_mgmt(11);
    next->list_parameters = solo_malloc_list_mgmt(11);
    next->list_color_tables = solo_malloc_list_mgmt(47);
    next->list_palettes =solo_malloc_list_mgmt(63);
    next->list_winfo_files = solo_malloc_list_mgmt(127);
    next->list_ed_info_files = solo_malloc_list_mgmt(127);
    next->list_boundary_files = solo_malloc_list_mgmt(127);
    next->list_lock_colors = solo_malloc_list_mgmt(SIZEOF_COLOR_NAMES);
    
    solo_add_list_entry(next->list_lock_colors, "yellow3", sizeof("yellow3"));
    solo_add_list_entry(next->list_lock_colors, "cyan3", sizeof("cyan3"));
    solo_add_list_entry(next->list_lock_colors, "green3", sizeof("green3"));
    solo_add_list_entry(next->list_lock_colors, "blue3", sizeof("blue3"));
    solo_add_list_entry(next->list_lock_colors, "magenta3", sizeof("magenta3"));
    solo_add_list_entry(next->list_lock_colors, "red3", sizeof("red3"));

    next->list_a_message = solo_malloc_list_mgmt(87);
    next->list_scratch = solo_malloc_list_mgmt(87);
    next->list_sample_colors = solo_malloc_list_mgmt(SIZEOF_COLOR_NAMES);
    
    solo_add_list_entry(next->list_sample_colors, "red", sizeof("red"));
    solo_add_list_entry(next->list_sample_colors, "green", sizeof("green"));
    solo_add_list_entry(next->list_sample_colors, "blue", sizeof("blue"));
    solo_add_list_entry(next->list_sample_colors, "cyan", sizeof("cyan"));
    solo_add_list_entry(next->list_sample_colors, "magenta", sizeof("magenta"));
    solo_add_list_entry(next->list_sample_colors, "yellow", sizeof("yellow"));

    next->list_text = solo_malloc_list_mgmt(87);
    next->list_all_files = solo_malloc_list_mgmt(87);
    next->list_select_files = solo_malloc_list_mgmt(87);
    
    strcpy(next->winfo_comment, "no_comment");
# ifdef fixme
    solo_init_palette();
# endif
    return(solo_perusal_info);
}
/* c------------------------------------------------------------------------ */

void sp_copy_tsri(a, b)
  struct ts_ray_info *a, *b;
{
    if(a && b) {
	b->ray_num = a->ray_num;
	b->sweep_num = a->sweep_num;
    }
    return;
}
/* c------------------------------------------------------------------------ */

struct solo_window_ptrs *
solo_return_wwptr(ww_num)
  int ww_num;
{
    int i, j, mm;
    struct solo_window_ptrs *next;
    struct point_in_space *solo_malloc_pisp();
    struct solo_field_vals *sfv, *last_sfv;
    long time_now(), time2;
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_list_mgmt *solo_malloc_list_mgmt();
    struct ts_ray_table *tsrt;
    struct ts_sweep_info *tssi;
    /* c...mark */


    spi = solo_return_winfo_ptr();

    if(ww_num < 0 || ww_num > SOLO_TOTAL_WINDOWS-1) {
	return(NULL);
    }

    if(spi->solo_windows[ww_num]) 
	return(spi->solo_windows[ww_num]);

    /*
     * initialize pointers and structs for this window
     */

    time2 = time_now();

    spi->solo_windows[ww_num] = next =
	  (struct solo_window_ptrs *)
		malloc(sizeof(struct solo_window_ptrs));
    memset((char *)next, 0, sizeof(struct solo_window_ptrs));
    next->window_num = ww_num;
    next->time_modified = time2;
    next->pisp_click = solo_malloc_pisp();
    
    next->lock = (struct solo_plot_lock *)
	  malloc(sizeof(struct solo_plot_lock));
    memset((char *)next->lock, 0, sizeof(struct solo_plot_lock));
    
    strncpy(next->lock->name_struct, "SPTL", 4);
    next->lock->sizeof_struct = sizeof(struct solo_plot_lock);
    next->lock->window_num = ww_num;
    next->lock->time_modified = time2;
    next->lock->linked_windows[ww_num] = YES;
    
    
    next->sweep = (struct solo_sweep_file *)
	  malloc(sizeof(struct solo_sweep_file));
    memset((char *)next->sweep, 0, sizeof(struct solo_sweep_file));
    
    strncpy(next->sweep->name_struct, "SSFI", 4);
    next->sweep->sizeof_struct = sizeof(struct solo_sweep_file);
    next->sweep->window_num = ww_num;
    next->sweep->time_modified = time2;
    next->sweep->linked_windows[ww_num] = YES;
    next->sweep->version_num = LATEST_VERSION;
    next->sweep->latest_version = YES;
    next->sweep->sweep_skip = 1;
    next->sweep->sweep_keep_count = 7;
    
    
    next->radar = (struct solo_radar_name *)
	  malloc(sizeof(struct solo_radar_name));
    memset((char *)next->radar, 0, sizeof(struct solo_radar_name));
    
    strncpy(next->radar->name_struct, "SRDN", 4);
    next->radar->sizeof_struct = sizeof(struct solo_radar_name);
    next->radar->window_num = ww_num;
    next->radar->time_modified = time2;
    next->radar->linked_windows[ww_num] = YES;
    
    
    next->parameter = (struct solo_parameter_info *)
	  malloc(sizeof(struct solo_parameter_info));
    memset((char *)next->parameter, 0, sizeof(struct solo_parameter_info));
    
    strncpy(next->parameter->name_struct, "SPMI", 4);
    next->parameter->sizeof_struct = sizeof(struct solo_parameter_info);
    next->parameter->window_num = ww_num;
    next->parameter->time_modified = time2;
    next->parameter->linked_windows[ww_num] = YES;
    
    
    next->view = (struct solo_view_info *)
	  malloc(sizeof(struct solo_view_info));
    memset((char *)next->view, 0, sizeof(struct solo_view_info));
    
    strncpy(next->view->name_struct, "SWVI", 4);
    next->view->sizeof_struct = sizeof(struct solo_view_info);
    next->view->window_num = ww_num;
    next->view->time_modified = time2;
    next->view->linked_windows[ww_num] = YES;

    next->view->magnification = 1.;
    next->view->az_line_int_deg = 30.;
    next->view->az_annot_at_km = 45.;
    next->view->rng_ring_int_km = 15.;
    next->view->rng_annot_at_deg = 45.;
    next->view->ts_magnification = 
	  next->view->x_tic_mag = next->view->y_tic_mag = 1.;
    next->view->angular_fill_pct = 120.;
    
    next->radar_location = solo_malloc_pisp();
    strcpy(next->radar_location->id, "SOL_RAD_LOC");
    
    next->center_of_view = solo_malloc_pisp();
    strcpy(next->center_of_view->id, "SOL_WIN_LOC");
    
    next->landmark = solo_malloc_pisp();
    strcpy(next->landmark->id, "SOL_LANDMARK");
    
    /* set up for field values */
    for(i=0; i < 3; i++) {
	sfv = (struct solo_field_vals *)
	      malloc(sizeof(struct solo_field_vals));
	memset(sfv, 0, sizeof(struct solo_field_vals));
	sfv->state = 123;
	sfv->num_vals = 5;
	if(!i) {
	    next->field_vals = sfv;
	}
	else {
	    sfv->last = last_sfv;
	    last_sfv->next = sfv;
	    }
	last_sfv = sfv;
	sfv->next = next->field_vals;
	next->field_vals->last = sfv;
    }

    next->list_colors = (struct solo_list_mgmt *)
	  malloc(sizeof(struct solo_list_mgmt));
    memset(next->list_colors, 0, sizeof(struct solo_list_mgmt));
    next->list_colors->sizeof_entries = SIZEOF_COLOR_NAMES;
    
    /*
     * data coloration parameters
     */
    next->xcolors = (XColor *)malloc(256*sizeof(XColor));
    if(!next->xcolors) {
	uii_printf("Unable to malloc next->xcolors for frame: %d\n"
		  , ww_num);
	exit(1);
    }
    memset(next->xcolors, 0, 256*sizeof(XColor));
    
    next->data_color_lut = (long *)malloc(K64*sizeof(long));
    if(!next->data_color_lut) {
	uii_printf("Unable to malloc next->data_color_lut for frame: %d\n"
		  , ww_num);
	exit(1);
    }
    memset (next->data_color_lut, 0, K64*sizeof (long));
    /* now center it */
    next->data_color_lut += K32;
    
    next->data_cell_lut = (short *)malloc(4096*sizeof(short));
    if(!next->data_cell_lut) {
	uii_printf("Unable to malloc next->data_cell_lut for frame: %d\n"
		  , ww_num);
	exit(1);
    }
    memset (next->data_cell_lut, 0, 4096*sizeof(short));
    next->cell_colors = (unsigned long *)malloc(2048*sizeof(int));
    if(!next->cell_colors) {
	uii_printf("Unable to malloc next->cell_colors for frame: %d\n"
		  , ww_num);
	exit(1);
    }
    memset (next->cell_colors, 0, 2048*sizeof(int));
    next->landmark_info = (struct landmark_info *)
	  malloc(sizeof(struct landmark_info));
    memset(next->landmark_info, 0, sizeof(struct landmark_info));
    strncpy(next->landmark_info->name_struct, "SLMK", 4);
    next->landmark_info->sizeof_struct = sizeof(struct landmark_info);
    next->landmark_info->reference_frame = ww_num +1;
    next->landmark_info->reference_frame = 1;
    next->landmark_info->linked_windows[ww_num] = 1;
    next->landmark_info->window_num = ww_num;

    next->frame_ctr_info = (struct frame_ctr_info *)
	  malloc(sizeof(struct frame_ctr_info));
    memset(next->frame_ctr_info, 0, sizeof(struct frame_ctr_info));
    strncpy(next->frame_ctr_info->name_struct, "SCTR", 4);
    next->frame_ctr_info->sizeof_struct = sizeof(struct frame_ctr_info);
    next->frame_ctr_info->reference_frame = ww_num +1;
    next->frame_ctr_info->reference_frame = 1;
    next->frame_ctr_info->linked_windows[ww_num] = 1;
    next->frame_ctr_info->window_num = ww_num;

    next->clicked_ctr = solo_malloc_pisp();

    next->examine_info = (struct solo_examine_info *)
	  malloc(sizeof(struct solo_examine_info));
    memset(next->examine_info, 0, sizeof(struct solo_examine_info));
    strncpy(next->examine_info->name_struct, "SXMN", 4);
    next->examine_info->sizeof_struct = sizeof(struct solo_examine_info);
    next->examine_info->window_num = ww_num;
    next->examine_info->scroll_increment = 19;
    next->examine_info->ray_count = 5;
    next->examine_info->cell_count = 21;
    next->examine_info->whats_selected = EX_RADAR_DATA;
    next->examine_info->typeof_change = EX_DELETE;
    strcpy(next->examine_info->display_format, "6.1f");

    next->examine_line_len = 256;
    next->examine_list = solo_malloc_list_mgmt(next->examine_line_len);
    next->examine_control = (struct examine_control *)
	  malloc(sizeof(struct examine_control));
    memset(next->examine_control, 0, sizeof(struct examine_control));

    next->list_field_vals= solo_malloc_list_mgmt(80);

    next->tsrt = (struct ts_ray_table *)malloc(sizeof(struct ts_ray_table));
    memset(next->tsrt, 0, sizeof(struct ts_ray_table));
    next->tsrt->max_sweep_entries = 3;
    mm = next->tsrt->max_sweep_entries * sizeof(struct ts_sweep_info);
    next->tsrt->tssi_top = (struct ts_sweep_info *)malloc(mm);
    memset(next->tsrt->tssi_top, 0, mm);

    next->clicked_tsri = (struct ts_ray_info *)
	  malloc(sizeof(struct ts_ray_info));
    memset(next->clicked_tsri, 0, sizeof(struct ts_ray_info));
    next->magic_rng_annot = 1;
    strcpy (next->filter_scan_modes, "PPI,SUR");
    next->filter_fxd_ang = 0.5;
    next->filter_tolerance = .25;

    /*
     * now return the pointer to the window struct
     */
    return(spi->solo_windows[ww_num]);
}
/* c------------------------------------------------------------------------ */

void se_set_sfic_from_frame(frme)
  int frme;
{
    /* this routine nabs the sweep info from the frame info
     * this routine is called when the editor widget is popped up
     *
     * and is called at the beginning of an editing pass just to
     * make sure the editor info is current
     */
    long lnn;
    double d_mddir_file_info_v3(), dd_ts_start();
    int ii, nn, mark, vv, lead_frame;
    char *a, *b, *c, info[128], name[128], *mddir_radar_name_v3();
    char *se_string_time(), ts[24];
    struct swp_file_input_control * sfic, *return_swp_fi_in_cntrl();
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_edit_stuff *seds, *return_sed_stuff();


    seds = return_sed_stuff();
    sfic = return_swp_fi_in_cntrl();
    wwptr = solo_return_wwptr(frme);
    seds->focus_frame = 
	  sfic->frame = wwptr->lead_sweep->window_num;
    seds->popup_frame = frme;
    sfic->clicked_frame = frme +1; /* a non-zero value here implies
				    * that the editor will operate on
				    * the sweep plotted in this frame
				    */
    sfic->radar_num = wwptr->sweep->radar_num;
    strcpy(sfic->directory_text, wwptr->sweep->directory_name);
    strcpy(sfic->radar_names_text, wwptr->sweep->radar_name);


    if(wwptr->view->type_of_plot & SOLO_TIME_SERIES) {
	sfic->start_time = dd_ts_start
	      (sfic->frame, wwptr->sweep->radar_num
	       , wwptr->sweep->start_time, &vv, info, name);
	sfic->stop_time = wwptr->sweep->stop_time;
    }
    else {
	sfic->start_time = sfic->stop_time = 
	      d_mddir_file_info_v3
# ifdef NU_TSTAMP
		    (sfic->frame, wwptr->sweep->radar_num
		     , wwptr->sweep->start_time, TIME_NEAREST
		     , wwptr->sweep->version_num, &vv, info, name);
# else
		    (sfic->frame, wwptr->sweep->radar_num
		     , (double)wwptr->sweep->time_stamp, TIME_NEAREST
		     , wwptr->sweep->version_num, &vv, info, name);
# endif

    }
    strcpy(sfic->first_sweep_text, se_string_time
	   (sfic->start_time, ts));
    strcpy(sfic->last_sweep_text, sfic->first_sweep_text);
    if(wwptr->sweep->version_num == LATEST_VERSION) {
	strcpy(sfic->version_text, "last");
    }
    else if(wwptr->sweep->version_num == EARLIEST_VERSION) {
	strcpy(sfic->version_text, "first");
    }
    else {
	sprintf(sfic->version_text, "%ld", wwptr->sweep->version_num);
    }
    sfic->version = wwptr->sweep->version_num;
}
/* c------------------------------------------------------------------------ */

int solo_set_state(new_state)
  int new_state;
{
    return(Solo_state = new_state);
}
/* c------------------------------------------------------------------------ */

void solo_trigger_dir_rescans(dir)
  char *dir;
{
    int ii, nn, ww, mark;
    char *a, *b, dirx[128], diry[128];
    WW_PTR wwptr, wwptrc, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();

    spi = solo_return_winfo_ptr();
    slash_path(dirx, dir);

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	wwptr = solo_return_wwptr(ww);
	slash_path(diry, wwptr->sweep->directory_name);
	if(strcmp(dirx, diry) == 0) {
	    ddir_rescan_urgent(ww);
	}
    }
}
/* c------------------------------------------------------------------------ */

void solo_update_active_info()
{
    int ii, jj, ww, frme;
    WW_PTR wwptr, wwptrc, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();

    spi = solo_return_winfo_ptr();

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	wwptr = solo_return_wwptr(ww);
	for(ii=0; ii < SOLO_MAX_WINDOWS; ii++) {
	    if(spi->active_windows[ii])
		  continue;
	    wwptr->view->linked_windows[ii] = NO;
	    wwptr->lock->linked_windows[ii] = NO;
	    wwptr->sweep->linked_windows[ii] = NO;
	    wwptr->parameter->linked_windows[ii] = NO;
	    wwptr->landmark_info->linked_windows[ii] = NO;
	    wwptr->frame_ctr_info->linked_windows[ii] = NO;
	}
    }
}
/* c------------------------------------------------------------------------ */

void solo_ww_top_line(frme)
  int frme;
{
    int ww;
    struct dd_general_info *dgi, *dd_window_dgi();
    DD_TIME dtx, *dts, *d_unstamp_time();
    WW_PTR wwptr, solo_return_wwptr(), wwptrx;
    char *aa, *bb, str[16], *str_terminate(), *dd_scan_mode_mne(), mne[16];
    char *dts_print();

    wwptr = solo_return_wwptr(frme);
    wwptrx = wwptr->lead_sweep;
    aa = wwptr->top_line;
    ww = wwptr->lead_sweep->window_num;
    dgi = dd_window_dgi(ww, "");
    dtx.time_stamp = wwptrx->view->type_of_plot & SOLO_TIME_SERIES
	  ? wwptrx->sweep->start_time : dgi->time;
# ifdef obsolete
    dts = dgi->dds->dts;
# else
    dts = &dtx;
# endif
    d_unstamp_time(dts);
    sprintf(aa, "%d %02d/%02d/%d %02d:%02d:%02d %s %.1f %s %s"
	    , frme+1
	    , dts->month
	    , dts->day
	    , dts->year
	    , dts->hour
	    , dts->minute
	    , dts->second
	    , str_terminate(str, dgi->dds->swib->radar_name, 8)
	    , dgi->dds->swib->fixed_angle
	    , dd_scan_mode_mne(dgi->dds->radd->scan_mode, mne)
	    , wwptr->parameter->parameter_name
	    );
}

/* c------------------------------------------------------------------------ */

int sp_max_frames()
{
    return(SOLO_MAX_WINDOWS);
}
/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

