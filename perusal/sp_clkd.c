/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

/*
 * This file contains routines:
 * 
 * sp_data_click
 * sp_locate_this_point
 * sp_rtheta_screen_to_xyz
 * sp_seek_field_vals
 * sp_xyz_to_rtheta_screen
 * 
 */

# include <errno.h>
# include <sys/types.h>
# include <dirent.h>
# include <dorade_headers.h>
# include "solo_window_structs.h"
# include <dd_files.h>
# include <sed_shared_structs.h>
# include <dd_math.h>
# include <function_decl.h>
# include <dgi_func_decl.h>
# include "soloii.h"
# include "sii_externals.h"


/* external functions */
void print_fieldvalues();
void dd_clear_pisp(struct point_in_space *pisp);
void dd_copy_pisp();		/* ddb_common.c */
void dd_razel_to_xyz();		/* ddb_common.c */
void dd_latlon_shift();		/* ddb_common.c */
void dd_latlon_relative();	/* ddb_common.c */
void dd_range_gate();		/* ddb_common.c */
int dd_givfld();		/* ddb_common.c */
void dd_rng_info();		/* ddb_common.c */
void solo_set_busy_signal();	/* sp_basics.c */
void solo_cpy_sweep_info();	/* sp_accepts.c */
double sp_meters_per_pixel();	/* sp_xyraster.c */
void solo_message();		/* solo.c */
int solo_nab_next_file();	/* solo_perusal.c */
void sp_copy_tsri();		/* sp_basics.c */
void sxm_set_click_frame();	/* sxm_examine.c */
void solo_clear_busy_signal();	/* sp_basics.c */


/* internal functions */
void solo_new_list_field_vals();
void solo_put_field_vals();
void sp_sweep_file_check();
void sp_ts_seek_field_vals();
int sp_ts_ray_list();
void sp_ts_data_click();
void sp_locate_this_point();
void sp_rtheta_screen_to_xyz ();
void sp_seek_field_vals ();
void sp_xyz_to_rtheta_screen ();
void sii_exam_click_in_data (guint frame_num);


/* c------------------------------------------------------------------------ */

void solo_new_list_field_vals(frme)
  int frme;
{
    static int count=0;
    long *boom=NULL;
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_field_vals *sfv;
    struct point_in_space *pisp, radar, clicked;
    struct solo_list_mgmt *slm;
    char str[256];
    double sin(), cos(), dd_earth_radius(), dlat, dlon, dalt, daz, del;
    double fabs(), drng, drng1, drng2, drng3, dcost;
    double d, x, y, z;
    int ii, jj, kk, hilite=YES, mark, ctr_val, entry_num=0, ts;
    char *aa, *bb, *dts_print(), tmp_str[32];
    DD_TIME dts, *d_unstamp_time();

    count++;
    wwptr = solo_return_wwptr(frme);
    ts = wwptr->view->type_of_plot & SOLO_TIME_SERIES;
    slm = wwptr->list_field_vals;
    solo_reset_list_entries(slm);
    sfv = wwptr->field_vals;
    ctr_val = sfv->num_vals/2;
    dd_copy_pisp(wwptr->radar_location, &radar);

    radar.azimuth = sfv->next->az;
    radar.elevation = sfv->next->el;
    radar.range = sfv->rng;
    dd_razel_to_xyz(&radar);

    if(ts) {
	dd_copy_pisp(wwptr->radar_location, &clicked);
	clicked.altitude = wwptr->clicked_range;
    }
    else {
	dd_latlon_shift(&radar, &clicked);
    }

    if (!ts) {
	dts.time_stamp = sfv->next->time;
	aa = dts_print(d_unstamp_time(&dts));
        sprintf(str, "%s:  %s\n", wwptr->parameter->parameter_name, aa);
    } else {
        sprintf(str, "   %s\n", wwptr->parameter->parameter_name);
    }
        
    
    solo_add_list_entry(slm, str, strlen(str));

    switch(wwptr->lead_sweep->scan_mode) {
    case RHI:
	dcost = cos(fabs(RADIANS(sfv->next->el)));
	break;
    default:
	dcost = cos(fabs(RADIANS(sfv->next->tilt)));
	break;
    }
    d = ts ? sfv->next->alt :
	  sfv->next->agl + (clicked.altitude - radar.altitude);

    sprintf(str, " Hdg. %9.1f   Agl: %7.3f\n"
	    , sfv->next->heading, d);
    solo_add_list_entry(slm, str, strlen(str));

    d = ts ? sfv->next->lon : clicked.longitude;
    sprintf(str, " Lon. %9.4f     x: %7.3f\n"
	    , d, radar.x); 
    solo_add_list_entry(slm, str, strlen(str));

    d = ts ? sfv->next->lat : clicked.latitude;
    sprintf(str, " Lat. %9.4f     y: %7.3f\n"
	    , d, radar.y);
    solo_add_list_entry(slm, str, strlen(str));

    sprintf(str, " Alt. %9.4f     z: %7.3f\n"
	    , clicked.altitude, radar.z); 
    solo_add_list_entry(slm, str, strlen(str));

    sprintf(str, " Angle    %7.2f %7.2f %7.2f\n", sfv->rot_ang
	    , sfv->next->rot_ang, sfv->next->next->rot_ang); 

    if(ts) {
	dts.time_stamp = sfv->next->time;
	aa = dts_print(d_unstamp_time(&dts));
	aa = strchr(aa, ' ') +1;
	sprintf(str, "Time          %s", aa);
    }
    solo_add_list_entry(slm, str, strlen(str));

    for(ii=0; ii < sfv->num_vals; ii++) {
	d = M_TO_KM(sfv->ranges[ii]);
	if(!ts)
	      d *= dcost;

	sprintf(str, "%7.2fkm.%7.2f %7.2f %7.2f\n"
		, d
		, sfv->field_vals[ii]
		, sfv->next->field_vals[ii]
		, sfv->next->next->field_vals[ii]); 
	hilite = ii == ctr_val ? YES : NO;
	solo_add_list_entry(slm, str, strlen(str));
    }
    print_fieldvalues(frme, slm);
}
/* c------------------------------------------------------------------------ */

void solo_put_field_vals(dgi, param_num, angle, range, sfv, state)
  struct dd_general_info *dgi;
  struct solo_field_vals *sfv;
  int param_num, state; 
  float angle, range;
{
    long *boom=NULL;
    struct radar_angles *ra=dgi->dds->ra;
    double sin(), cos(), dd_earthr();
    double dd_rotation_angle(), dd_tilt_angle(), dd_azimuth_angle()
	  , dd_elevation_angle(), dd_nav_tilt_angle();
    double dd_latitude(), dd_longitude(), dd_altitude();
    int gg, i, j, n, nc, gate, pn=param_num, five=5, mark;
    float rng, cell_val, f_bad, f_data[7], f_rngs[7];
    float r, max_r, min_r, delta_r;

    rng = KM_TO_M(range);
    nc = dgi->dds->celvc->number_cells;
    min_r = dgi->dds->celvc->dist_cells[0];
    max_r = dgi->dds->celvc->dist_cells[nc-1];
    sfv->state = state;
    for(j=0; j < 5; j++) 
	  sfv->ranges[j] = 
		sfv->field_vals[j] = -999;
    /*
     * get the data into a floating point array
     * for this routine gate # begins at 1 instead of 0
     */
    dd_range_gate( dgi, &rng, &gate, &cell_val);
    gate -= 2;
    if (pn < dgi->num_parms)
      { n = dd_givfld(dgi, &pn, &gate, &five, f_data, &f_bad); }
    dd_rng_info(dgi, &gate, &five, f_rngs, &n);
    gg = rng > max_r ? nc+1 : gate;
    
    for(j=0; j < 5; j++,gg++) {
	if(gg > nc)
	      break;
	if(gg < 1)
	      continue;
	if(sfv->state != SFV_GARBAGE)
	      sfv->field_vals[j] = f_data[j];

	sfv->ranges[j] = f_rngs[j];
    }
    sfv->num_vals = 5;
    sfv->bad_flag = f_bad;
    sfv->rng = range;
    sfv->rot_ang = dgi->dds->radd->scan_mode == RHI
	  ? dd_elevation_angle(dgi) : dd_rotation_angle(dgi);
    sfv->tilt = dd_tilt_angle(dgi);
    sfv->az = dd_azimuth_angle(dgi);
    sfv->el = dd_elevation_angle(dgi);
    sfv->lat = dd_latitude(dgi);
    sfv->lon = dd_longitude(dgi);
    sfv->alt = dd_altitude(dgi);
    sfv->agl = dgi->dds->asib->altitude_agl;
    sfv->earth_r = dd_earthr((double)sfv->lat);
    sfv->time = dgi->time;
    sfv->heading = dgi->dds->asib->heading;
}
/* c------------------------------------------------------------------------ */

void sp_sweep_file_check(dir, file)
  char *dir, *file;
{
    /* the purpose of this routine is to determine if this file is being
     * used in any of the windows and set a flag that says it might
     * have changed since the last access
     */
    int ii, ww;
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    
    spi = solo_return_winfo_ptr();

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	wwptr = solo_return_wwptr(ww);
	if(wwptr->lead_sweep != wwptr)
	      continue;		/* this is not a lead sweep */

	if(strcmp(dir, wwptr->sweep->directory_name) == 0
	   && strcmp(file, wwptr->sweep->file_name) == 0)
	      wwptr->sweep_file_modified = YES;
    }
}
/* c------------------------------------------------------------------------ */

void sp_ts_seek_field_vals(frme)
  int frme;
{
    /* 
     * routine to printout relevent field values corresponding to
     * the clicked location for time series data
     */
    int ii, jj, kk, ll, nn, ww, state, ray_num, top_down;
    int file_action=TIME_NEAREST, version=LATEST_VERSION;
    int sweep_skip=1, replot=YES;
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptr, wwptrx, solo_return_wwptr();
    struct solo_field_vals *sfv;
    struct rot_table_entry *entry1, *dd_return_rotang1();
    struct rot_ang_table *rat;
    struct ts_ray_table *tsrt;
    struct ts_ray_info *tsri, *tsri_right, *tsrix, **tlist;
    struct ts_sweep_info *tssi;
    double angdiff(), fabs();
    int indices[3], solo_busy(), in_sector();
    float angles[3], angle_k, range, theta=0;
    char str[256], *aa, mess[256];


    wwptrx = solo_return_wwptr(frme);
    wwptr = wwptrx->lead_sweep;
    tsrt = wwptr->tsrt;
    if(!tsrt || !tsrt->tsri_top) {
	sprintf (mess, "NULL tsrt in sp_ts_seek_field_vals() for frame %d\n"
		 , frme);
	g_message (mess);
	return;
    }
    tlist = tsrt->tsri_list;

    dgi = dd_window_dgi(wwptr->window_num, "");
    rat = dgi->source_rat;	/* rotation angle table */
    if(!rat)
	  return;		/* bad lead sweep info...probably */
    state = SFV_CENTER;


    for(ray_num=0; ray_num < 3; ray_num++, sfv = sfv->next) {
	if(wwptr->file_id > 0)
	      close(wwptr->file_id);
	tsri = *(tlist +ray_num);
	tssi = tsrt->tssi_top + tsri->sweep_num;
	slash_path(str, tssi->directory);
	strcat(str, tssi->filename);

	if((wwptr->file_id = open(str, O_RDONLY, 0)) < 0) {
	    sprintf(mess, "Unable to open sweep %s\n", str);
	    solo_message(mess);
	    return;
	}
	slash_path(dgi->directory_name, tssi->directory);
	dgi->in_swp_fid = wwptr->file_id;
	dd_absorb_header_info(dgi);
	entry1 = dd_return_rotang1(rat);
	dgi_buf_rewind(dgi);
	nn = lseek(dgi->in_swp_fid
		  , (long)(entry1+tsri->ray_num)->offset, 0L);
	dd_absorb_ray_info(dgi);
	for(wwptrx = wwptr ; wwptrx; wwptrx = wwptrx->next_sweep) {
	    if(!(wwptrx->view->type_of_plot & SOLO_TIME_SERIES))
		  continue;
	    if (wwptrx->window_num >= sii_return_frame_count())
	      { continue; }
	    
	    if(wwptr->view->type_of_plot & TS_MSL_RELATIVE) {
		if(wwptr->view->type_of_plot & TS_AUTOMATIC) {
		    top_down = in_sector((float)dd_rotation_angle(dgi)
					 , (float)135., (float)225.);
		}
		else {
		    top_down = wwptr->view->type_of_plot & TS_PLOT_DOWN;
		}
		if(top_down) {
		    range = dd_altitude(dgi) - wwptr->clicked_range;
		}
		else {
		    range = wwptr->clicked_range - dd_altitude(dgi);
		}
	    }
	    else if(wwptr->view->type_of_plot & TS_PLOT_DOWN) {
		range = 2. * wwptr->view->ts_ctr_km - wwptr->clicked_range;
	    }
	    else {
		range = wwptr->clicked_range;
	    }
	    /* range is now radar relative */
	    
	    switch(ray_num) {
	    case 0:
		sfv = wwptrx->field_vals;
		break;
	    case 1:
		sfv = wwptrx->field_vals->next;
		break;
	    case 2:
		sfv = wwptrx->field_vals->next->next;
		break;
	    }
	    solo_put_field_vals(dgi, wwptrx->parameter_num, (float)theta
				, (float)range, sfv, state);
	}
    }
    for(wwptrx = wwptr ; wwptrx; wwptrx = wwptrx->next_sweep) {
      if (wwptrx->window_num >= sii_return_frame_count())
	{ continue; }
	solo_new_list_field_vals(wwptrx->window_num);
    }
}
/* c------------------------------------------------------------------------ */

int sp_ts_ray_list(frme, xpos, nr)
  int frme, xpos, nr;
{
    /* construct a list of "nr" rays of time series data
     * centered on xpos.
     */
    int ii, jj, kk, mm, nn, ww, width, x_pxl, nr2 = 2*nr;
    int nurl=0, nurr=0, nrhalf;
    int last_sweep_num=-1;
    double d, scale, range, height;
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptrx, wwptr, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct ts_ray_table *tsrt;
    struct ts_sweep_info *tssi;
    struct ts_ray_info *tsri, *tsri_right, *tsrix, **tlist;

    spi = solo_return_winfo_ptr();
    wwptrx = solo_return_wwptr(frme)->lead_sweep;
    tsrt = wwptrx->tsrt;
    if(!tsrt || !tsrt->tsri_top) {
	uii_printf("NULL tsri in sp_ts_ray_list() for frame: %d\n"
		  , frme);
	return((int)0);
    }
    

    if(tsrt->max_list_entries < nr2) { /* check the list to be sure
					* it's large enough */
	tsrt->max_list_entries = nr2;
	if(tsrt->tsri_list) free(tsrt->tsri_list);
	tsrt->tsri_list = (struct ts_ray_info **)malloc
	      (tsrt->max_list_entries*sizeof(struct ts_ray_info *));
	memset(tsrt->tsri_list, 0, tsrt->max_list_entries*
	       sizeof(struct ts_ray_info *));
    }
    tsri_right = tsrt->tsri_top + wwptrx->view->width_in_pix;
    tsri = tsrix = tsrt->tsri_top + xpos;
    tlist = tsrt->tsri_list + nr -1; /* the list has 2 * nr entries */

    if(tsrix->ray_num >=0) {	/* really clicked on some data */
	*tlist-- = tsrix = tsri--;
	nurl++;
    }

    /* look to the left for nr unique rays
     */
    for(; nurl < nr && tsri >= tsrt->tsri_top;) {
	for(; tsri >= tsrt->tsri_top; tsri--) {
	    if(tsri->ray_num < 0)
		  continue;
	    if(tsri->ray_num != tsrix->ray_num ||
	       tsri->sweep_num != tsrix->sweep_num) { /* new entry */
		*tlist-- = tsrix =  tsri--;
		nurl++;
		break;
	    }
	}
    }
    tlist = tsrt->tsri_list + nr;
    tsri = tsrix = tsrt->tsri_top + xpos;
    /*
     * look to the right for nr rays
     */
    for(; nurr < nr && tsri < tsri_right;) {
	for(; tsri < tsri_right; tsri++) {
	    if(tsri->ray_num < 0)
		  continue;
	    if(tsri->ray_num != tsrix->ray_num ||
	       tsri->sweep_num != tsrix->sweep_num) {
		*tlist++ = tsrix = tsri++;
		nurr++;
		break;
	    }
	}
    }
    nn = (nr +1)/2;		/* nr odd => nn = nr/2 +1 */
    tlist = tsrt->tsri_list;
    if((mm = nurl + nurr) <= nr) { /* found only nr or less rays */
	jj = nr - nurl;
	for(; mm-- ; tlist++) {
	    *tlist = *(tlist +jj);
	}
	return(nurl + nurr);
    }
    jj = nr - nn;

    if(nurl < nn) {		/* not symmetric (less on the left) */
	jj += nn - nurl;
    }
    else if(nurr < nr/2) {	/* not symmetric (less on the right) */
	jj -= nr/2 - nurr;
    }
    if(jj) {
	for(mm=nr; mm-- ; tlist++) {
	    *tlist = *(tlist +jj);
	}
    }    
    return(nr);
}
/* c------------------------------------------------------------------------ */

void sp_ts_data_click(frme, xpos, ypos)
  int frme, xpos, ypos;
{
    int ii, mm, nn, ww, width, x_pxl, checked_off[SOLO_MAX_WINDOWS];
    int last_sweep_num=-1, nr;
    double d, scale, range, height;
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptrx, wwptr, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct ts_ray_table *tsrt;
    struct ts_sweep_info *tssi;
    struct ts_ray_info *tsri, *tsrix, *tsri_click, *tsri_right;
    char mess[256];
    SiiPoint *pt;


    spi = solo_return_winfo_ptr();
    wwptrx = solo_return_wwptr(frme)->lead_sweep;
    tsrt = wwptrx->tsrt;
    if(!tsrt || !tsrt->tsri_top) {
	sprintf (mess, "NULL tsri in sp_ts_ray_list() for frame: %d\n"
		 , frme);
	g_message (mess);
	return;
    }
    pt = (SiiPoint *)sii_click_que->data;
    xpos = pt->x;
    ypos = pt->y;
    scale = M_TO_KM(sp_meters_per_pixel()/wwptrx->view->ts_magnification);
    height = wwptrx->view->height_in_pix;
    width = wwptrx->view->width_in_pix;
# ifdef obsolete
    ypos = height -1 -ypos;
# endif
    /* range in this case will be an altitude
     */
    range = wwptrx->view->ts_ctr_km + (.5*height -ypos) * scale;
    wwptrx->clicked_x_pxl = xpos;
    wwptrx->clicked_y_pxl = ypos;
    if((wwptrx->view->type_of_plot & SOLO_TIME_SERIES)) {
       d = wwptrx->sweep->stop_time - wwptrx->sweep->start_time;
       wwptrx->clicked_time = wwptrx->sweep->start_time +
	 d * ((double)xpos/wwptrx->view->width_in_pix);
    }
    sxm_set_click_frame(frme, (float)0, (float)range);
    for(ii=0; ii < SOLO_MAX_WINDOWS; checked_off[ii++] = NO);

    /* you need to do the above based on xpos for now in all the
     * active time series windows
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]) || checked_off[ww])
	      continue;
	if (ww >= sii_return_frame_count())
	  { continue; }

	wwptr = solo_return_wwptr(ww);
	if(!(wwptr->view->type_of_plot & SOLO_TIME_SERIES))
	      continue;
	/* get a list of 3 rays around xpos */
	if((nr = sp_ts_ray_list(ww, xpos, 3)) > 0) {
	    /* center ray info struct */
	    tsri = *(wwptr->tsrt->tsri_list +1);
	}
	for(; wwptr ; wwptr = wwptr->next_sweep) {
	    checked_off[wwptr->window_num] = YES;
	    if (wwptr->window_num >= sii_return_frame_count())
	      { continue; }
	    wwptr->clicked_x_pxl = xpos;
	    wwptr->clicked_y_pxl = ypos;
	    wwptr->clicked_range = range;
	    if(nr > 0)
		  sp_copy_tsri(tsri, wwptr->clicked_tsri);
	}
	sp_ts_seek_field_vals(ww);
	sii_exam_click_in_data (ww);
	sii_edit_click_in_data (ww);
    }
}
/* c------------------------------------------------------------------------ */

void sp_data_click(int frme, double x, double y)
{
    /* (x,y) are in km. relative to the radar
     * (fieldvalues)
     * routine to printout relevent field values corresponding to
     * the clicked location
     */
    int ii, jj, kk, ll, nn, ww, state;
    long *sweep_list;
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptrx, wwptr, solo_return_wwptr();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_field_vals *sfv;
    struct rot_table_entry *entry1, *dd_return_rotang1();
    struct rot_ang_table *rat;
    double d, range, theta;
    double sp_meters_per_pixel(), angdiff();
    int indices[3], solo_busy();
    float angles[3], angle_k;
    struct point_in_space radarx, radar, pisp;


    if(solo_busy())
	  return;

    dd_clear_pisp (&pisp);
    wwptrx = solo_return_wwptr(frme);
    solo_set_busy_signal();

    if(wwptrx->view->type_of_plot & SOLO_TIME_SERIES) {
	sp_ts_data_click(frme, 0,0);
	solo_clear_busy_signal();
	return;
    }

    dd_copy_pisp(wwptrx->radar_location, &radarx);

    theta = atan2(y, x);
    pisp.rotation_angle = 
	  theta = fmod((double)(450. -DEGREES(theta)), (double)360.);
    pisp.range = 
	  range = sqrt(SQ(x) + SQ(y));

# ifdef obsolete
    printf ("spclk:%d)(%.4f,%.4f)(%.4f,%.4f)\n"
	    , frme, x, y, pisp.rotation_angle, pisp.range);
# endif
    sxm_set_click_frame(frme, (float)theta, (float)range);
    /*
     * calculate (x,y,z) relative to this radar
     */
    sp_rtheta_screen_to_xyz(frme, &radarx, &pisp);
    /* calculate the absolute lat/lon/alt of this point
     */
    dd_latlon_shift(&radarx, &pisp);
    se_rain_gauge_info( pisp.latitude, pisp.longitude );

    spi = solo_return_winfo_ptr();
    /*
     * convert this into a range and angle for all the other windows
     */
    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(!(spi->active_windows[ww]))
	      continue;
	wwptr = solo_return_wwptr(ww);
	if(wwptrx->sweep->linked_windows[ww]) {
	    wwptr->clicked_angle = theta;
	    wwptr->clicked_range = range;
	}
	else {
	    dd_copy_pisp(wwptr->radar_location, &radar);
	    /* calculate (x,y,z) relative to this radar
	     */
	    dd_latlon_relative(&pisp, &radar);
	    /* calculate range and rotation angle relatvie to this radar
	     */
	    sp_xyz_to_rtheta_screen(ww, &radar, &pisp);
	    wwptr->clicked_angle = pisp.rotation_angle;
	    wwptr->clicked_range = pisp.range;
	}
	dd_copy_pisp(&pisp, wwptr->pisp_click);
    }

    for(ww=0; ww < SOLO_MAX_WINDOWS; ww++) {
	if(spi->active_windows[ww]) {
	  sp_seek_field_vals(ww);
	  sii_exam_click_in_data (ww);
	  sii_edit_click_in_data (ww);
	}
    }    
    solo_clear_busy_signal();
}
/* c------------------------------------------------------------------------ */

void sp_locate_this_point(sci, bpm)
  struct solo_click_info *sci;
  struct bnd_point_mgmt *bpm;
{
    /* this routine loads up a pisp struct with all the positioning
     * info for this boundary point
     */
    int frme, ww, ii, jj, kk, nn, nr, ypos;
    int file_action=TIME_NEAREST, version=LATEST_VERSION;
    int sweep_skip=1, replot=YES;
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptrld, wwptr, solo_return_wwptr();
    struct ts_ray_info *tsri;
    struct ts_sweep_info *tssi;
    struct ts_ray_table *tsrt;
    double theta, range, x, y, scale;
    double dd_rotation_angle(), dd_tilt_angle(), dd_azimuth_angle()
	  , dd_elevation_angle(), dd_earthr(), dd_nav_tilt_angle();
    double dd_latitude(), dd_longitude(), dd_altitude();
    double dd_heading(), dd_roll(), dd_pitch(), dd_drift();
    struct rot_table_entry *entry1, *dd_return_rotang1();
    struct rot_ang_table *rat;
    struct point_in_space *pisp=bpm->pisp;
    char str[256];


    frme = sci->frame;
    wwptr = solo_return_wwptr(frme);
    wwptrld = wwptr->lead_sweep;
    ww = wwptrld->window_num;
    dgi = dd_window_dgi(ww, "");
    rat = dgi->source_rat;	/* rotation angle table */
    entry1 = dd_return_rotang1(rat);

    strcpy(pisp->id, "BND_PT_V1");
    pisp->state = PISP_AZELRG | PISP_EARTH;


    if(wwptr->view->type_of_plot & SOLO_TIME_SERIES) {
	/*
	 * for time series the center of the screen is assumed to be
	 * the specified number in km. above msl
	 */
	scale = M_TO_KM(sp_meters_per_pixel())/wwptr->view->ts_magnification;
	ypos = wwptr->view->height_in_pix -1 - sci->y;

	/* set msl altitude of clicked location */
	pisp->range = 
	  pisp->z = wwptr->view->ts_ctr_km -
	    ((.5*wwptr->view->height_in_pix -.5) -ypos) * scale;
	pisp->x = sci->x;	/* pixel values */
	pisp->y = sci->y;
	pisp->state |= PISP_TIME_SERIES;
	nr = sp_ts_ray_list(ww, (int)sci->x, 3);
	tsrt = wwptrld->tsrt;
	if(!tsrt || !tsrt->tsri_top) {
	    uii_printf("NULL tsrt pointer in sp_locate_this_point()  frme: %d\n"
		      , frme);
	    return;
	}
	tsri = *(tsrt->tsri_list + nr/2); /* center ray info struct */
	kk = tsri->ray_num;
	tssi = tsrt->tssi_top + tsri->sweep_num;
	slash_path(str, tssi->directory);
	strcat(str, tssi->filename);
	if(wwptrld->file_id)
	      close(wwptrld->file_id);
	if((wwptrld->file_id = open(str, O_RDONLY, 0)) < 0) {
	    uii_printf("Unable to open sweep %s\n", str);
	    return;
	}
	slash_path(dgi->directory_name, tssi->directory);
	dgi->in_swp_fid = wwptrld->file_id;
	dd_absorb_header_info(dgi);
	wwptrld->sweep_file_modified = NO;
    }
    else {
	x = sci->x;
	y = sci->y;
	theta = atan2(y, x);
	theta = FMOD360(CART_ANGLE(DEGREES(theta)));
	range = M_TO_KM(sqrt(SQ(x) + SQ(y))); /* km. */
	
	if(wwptr->lead_sweep->sweep_file_modified) {
	    solo_nab_next_file(ww, file_action, version, sweep_skip, replot);
	    wwptr->lead_sweep->sweep_file_modified = NO;
	}
	kk = dd_rotang_seek(rat, (float)theta);
	pisp->x = M_TO_KM(bpm->x);
	pisp->y = M_TO_KM(bpm->y);
	pisp->range = range;
	bpm->r = KM_TO_M(range);

	if(wwptr->lead_sweep->scan_mode == AIR)
	      pisp->state |= PISP_PLOT_RELATIVE;
    }

    dgi_buf_rewind(dgi);
    nn = lseek(dgi->in_swp_fid, (long)(entry1+kk)->offset, 0L);
    dd_absorb_ray_info(dgi);

    pisp->time = dgi->time;
    pisp->rotation_angle = dd_rotation_angle(dgi);
    pisp->tilt = dd_tilt_angle(dgi);
    pisp->tilt = dgi->dds->swib->fixed_angle;
    
    pisp->azimuth = dd_azimuth_angle(dgi);
    pisp->elevation = dd_elevation_angle(dgi);

    pisp->latitude = dd_latitude(dgi);
    pisp->longitude = dd_longitude(dgi);
    pisp->altitude = dd_altitude(dgi);

    if(wwptr->view->type_of_plot & SOLO_TIME_SERIES) {
	/* true range of clicked location from radar
	 */
	if(wwptr->view->type_of_plot & TS_MSL_RELATIVE) {
	    pisp->range = (pisp->z - pisp->altitude) * sin(pisp->elevation);
	}
	else if(wwptr->view->type_of_plot & TS_PLOT_DOWN) {
	    pisp->range = 2. * wwptr->view->ts_ctr_km -pisp->z;
	}
	else {			/* plot up */
	    pisp->range = pisp->z;
	}
	bpm->r = KM_TO_M(pisp->range);
    }
    else {
	/* msl altitude of clicked location */
	pisp->z = (pisp->altitude + range *
		   sin((double)RADIANS(pisp->elevation)));
    }
    pisp->earth_radius = dd_earthr(pisp->latitude);

    pisp->heading = dd_heading(dgi);
    pisp->roll = dd_roll(dgi);
    pisp->pitch = dd_pitch(dgi);
    pisp->drift = dd_drift(dgi);
}
/* c------------------------------------------------------------------------ */

void sp_rtheta_screen_to_xyz(frme, radar, pisp)
  int frme;
    struct point_in_space *radar, *pisp;
{
    /* calculate absolute (x,y,z) relative to the radar
     * for the point on the screen
     * pisp->range (slant range)
     * and pisp->rotation_angle are relative to the radar
     */
    double d, r, rxy, rota, tilt, H, x, y, z, az;
    double sqrt(), atan2(), fmod(), sin(), cos(), fabs();
    double sinH, cosH;
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);

    if((r = pisp->range) < .0005) {	/* half a meter */
	radar->x = radar->y = radar->z = 0;
	return;
    }
    tilt = RADIANS(radar->tilt);
    rota = RADIANS(CART_ANGLE(pisp->rotation_angle));

    switch(wwptr->lead_sweep->scan_mode) {

    case AIR:
	x = r * cos(tilt) * cos(rota);	/* aircraft relative */
	y = r * sin(tilt);	/* aircraft relative */
	sinH = sin(H = RADIANS(radar->heading));
	cosH = cos(H);
	radar->x = x * cosH - y * sinH;
	radar->y = x * sinH + y * cosH;
	radar->z = r * cos(tilt) * sin(rota);
	break;

    case RHI:
	radar->z = r * sin(rota); /* rota = elevation in this case */
	rxy = r * cos(rota);
	radar->x = rxy * cos(tilt);
	radar->y = rxy * sin(tilt);
	break;

    default:
	radar->z = r * sin(tilt);
	rxy = r * cos(tilt);
	radar->x = rxy * cos(rota);
	radar->y = rxy * sin(rota);
	break;
    }
}
/* c------------------------------------------------------------------------ */

void sp_seek_field_vals(frme)
  int frme;
{
    /* 
     * routine to printout relevent field values corresponding to
     * the clicked location
     */
    int ii, jj, kk, ll, nn, ww, state;
    int file_action=TIME_NEAREST, version=LATEST_VERSION;
    int sweep_skip=1, replot=YES;
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_field_vals *sfv;
    struct rot_table_entry *entry1, *dd_return_rotang1();
    struct rot_ang_table *rat;
    double angdiff(), fabs();
    int indices[3], solo_busy();
    float angles[3], angle_k, range, theta;


    wwptr = solo_return_wwptr(frme);
    range = wwptr->clicked_range;
    theta = wwptr->clicked_angle;

    dgi = dd_window_dgi(wwptr->lead_sweep->window_num, "");
    if(wwptr->lead_sweep->sweep_file_modified) {
	ww = wwptr->lead_sweep->window_num;
	solo_nab_next_file(ww, file_action, version, sweep_skip, replot);
	wwptr->lead_sweep->sweep_file_modified = NO;
    }
    rat = dgi->source_rat;	/* rotation angle table */
    if(!rat)
	  return;		/* bad lead sweep info...probably */
    entry1 = dd_return_rotang1(rat);

    
    /* get the index of entry closest to theta
     */
    indices[1] = kk = dd_rotang_seek(rat, (float)theta); 
    angles[1] = angle_k = (entry1+kk)->rotation_angle;
    
    indices[0] = jj = DEC_NDX(kk, rat->num_rays); /* index of ray before */
    angles[0] = (entry1+jj)->rotation_angle;
    wwptr->clicked_tsri->ray_num = jj +1;
    wwptr->clicked_tsri->sweep_num = 0;
    
    indices[2] = ll = INC_NDX(kk, rat->num_rays); /* index of ray after */
    angles[2] = (entry1+ll)->rotation_angle;

    for(ii=0; ii < 3; ii++) {
	if(fabs(angdiff((float)theta, angles[ii])) > 5.) {
	    state = SFV_GARBAGE;
	}
	else {
	    state = SFV_CENTER;
	}
	dgi_buf_rewind(dgi);
	nn = lseek(dgi->in_swp_fid
		  , (long)(entry1+indices[ii])->offset, 0L);
	dd_absorb_ray_info(dgi);

	sfv = wwptr->field_vals;
	if(ii == 1) {
	    sfv = sfv->next;
	}
	else if(ii == 2) {
	    sfv = sfv->next->next;
	}
	solo_put_field_vals(dgi, wwptr->parameter_num, angles[ii]
			    , (float)range, sfv, state);
    }

    solo_new_list_field_vals(frme);

}
/* c------------------------------------------------------------------------ */

void
sp_xyz_to_rtheta_screen(frme, radar, pisp)
  int frme;
  struct point_in_space *radar, *pisp;
{
    /* calculate r & theta for the point of the screen
     * relative to the radar
     */
    double tol = 1.e-4;
    double d, r, rxy, rxz, rota, tilt, H, x, y, z;
    double sqrt(), atan2(), fmod(), asin(), sin(), cos(), fabs();
    double sinH, cosH, cosT, sinT;
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);
    tilt = RADIANS(radar->tilt);

    r = sqrt(SQ(radar->x) + SQ(radar->y)+ SQ(radar->z));
    if(r < .0005) {	/* half a meter */
	pisp->range = pisp->rotation_angle = 0;
	return;
    }

    switch(wwptr->lead_sweep->scan_mode) {
	
    case AIR:
	/*
	 * take into account the heading of the aircraft and the fact
	 * that the screen is vertical rather than horizontal
	 */
	H = -RADIANS(radar->heading);
	x = radar->x * cos(H) - radar->y * sin(H);
	z = radar->z;

	if(SQ(x) > tol) {
	    rota = atan2(z, x);
	    r = fabs(sqrt(SQ(x) + SQ(z))/cos(tilt));
	}
	else if(SQ(z) > tol) {
	    rota = z > 0 ? PIOVR2 : 3*PIOVR2;
	    r = fabs(z/cos(tilt));
	}
	else {
	    pisp->rotation_angle = 0;
	    pisp->range = 0;
	    return;
	}
	break;

    case RHI:
	if((r = sqrt(SQ(radar->x) + SQ(radar->y) + SQ(radar->z))) > tol) {
	    rota = asin(radar->z/r);
	}
	else {
	    rota =  radar->z >= 0 ? PIOVR2 : 3*PIOVR2;
	}
	break;

    default:
	x = radar->x;
	y = radar->y;
	
	if(SQ(x) > tol) {
	    rota = atan2(y, x);
	    r = fabs(sqrt(SQ(x) + SQ(y))/cos(tilt));
	}
	else if(SQ(y) > tol) {
	    rota =  y > 0 ? PIOVR2 : 3*PIOVR2;
	    r = fabs(y/cos(tilt));
	}
	else {
	    pisp->rotation_angle = 0;
	    pisp->range = 0;
	    return;
	}
	break;
    }

    pisp->range = r;
    pisp->rotation_angle = fmod((double)450.-DEGREES(rota)
				  , (double)360.);
}
/* c------------------------------------------------------------------------ */

