/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "sii_utils.h"
# include "help_view.h"

# include <solo_window_structs.h>
# include <sed_shared_structs.h>
# include <seds.h>
# include <sii_widget_content_structs.h>
# include <math.h>


enum {
   VIEW_ZERO,

   VIEW_AZRNG,
   VIEW_NO_AZRNG,
   VIEW_AZ_ONLY,
   VIEW_RNG_ONLY,
   VIEW_AZRNG_LABELS,
   VIEW_MAGIC_RNG_LBLS,

   VIEW_XY_TICS,
   VIEW_NO_XY_TICS,
   VIEW_X_TICS_ONLY,
   VIEW_Y_TICS_ONLY,
   VIEW_XY_TIC_LABELS,

   VIEW_LINKS,
   VIEW_RADAR_LAT_LON_ALT,

   CENTER_ON_ONE,
   CENTER_ON_TWO,
   CENTER_ON_FOUR,
   CENTER_ON_RADAR,

   CENTER_NO_LINKS,
   CENTER_FIXED,
   CENTER_OTHER,
   CENTER_LINKS,

   LANDMARK_NO_LINKS,
   LANDMARK_FIXED,
   LANDMARK_OTHER,
   LANDMARK_LINKS,

   VIEW_REPLOT_THIS,
   VIEW_REPLOT_LINKS,
   VIEW_REPLOT_ALL,

   VIEW_OK,
   VIEW_CANCEL,
   VIEW_CLOSE,

   VIEW_ZOOM,
   VIEW_VALUES_AZRNG,
   VIEW_VALUES_XYTICS,
   VIEW_AZ_LBL_RNG,
   VIEW_RNG_LBL_AZ,
   VIEW_ANG_FILL,

   VIEW_CTR_RNG_AZ,
   VIEW_CTR_LAT_LON,
   VIEW_CTR_ALT,
   VIEW_CTR_REF_FRAME,

   VIEW_LMRK_RNG_AZ,
   VIEW_LMRK_LAT_LON,
   VIEW_LMRK_ALT,
   VIEW_LMRK_REF_FRAME,

   VIEW_TS_MODE,
   VIEW_TS_LR,
   VIEW_TS_RL,
   VIEW_TS_RADAR,
   VIEW_TS_MSL,
   VIEW_TS_UP,
   VIEW_TS_DOWN,
   VIEW_TS_AUTO,

   VIEW_HLP_OVERVIEW,
   VIEW_HLP_OPTS,
   VIEW_HLP_LINKS,
   VIEW_HLP_CTR,
   VIEW_HLP_LMRK,
   VIEW_HLP_TS,
   VIEW_HLP_FILL,
   VIEW_LAST_ENUM,

}ViewWidgetIds;


# define VIEW_MAX_WIDGETS VIEW_LAST_ENUM

typedef enum
{
  VW_RINGS        = 1 << 0,
  VW_SPOKES       = 1 << 1,
  VW_RA_LABELS    = 1 << 2,
  VW_RA_AUTO      = 1 << 3,

} ViewRngAzTasks;

typedef enum
{
  VW_X_TICS       = 1 << 0,
  VW_Y_TICS       = 1 << 1,
  VW_XY_LABELS    = 1 << 2,

} ViewXYticTasks;


typedef struct {
   gboolean changed;

   GtkWidget *data_widget[VIEW_MAX_WIDGETS];

   gint entry_flag[VIEW_MAX_WIDGETS];
   gboolean change_flag[VIEW_MAX_WIDGETS];

   gfloat orig_values[VIEW_MAX_WIDGETS][2];
   gfloat values[VIEW_MAX_WIDGETS][2];

   GString *orig_txt[VIEW_MAX_WIDGETS];
   GString *txt[VIEW_MAX_WIDGETS];
   guint precision[VIEW_MAX_WIDGETS];

   gboolean toggle[VIEW_MAX_WIDGETS];

   guint rng_az_state;
   gboolean rng_az_labels;

   guint xy_tics_state;
   gboolean xy_labels;

   guint ctr_state;
   guint lmrk_state;

   LinksInfo *view_links;
   LinksInfo *center_links;
   LinksInfo *landmark_links;

   guint ts_lr_state;
   guint ts_relative_state;
   guint ts_point_state;

   GtkWidget *radar_lat_lon;

  gboolean ctr_from_last_click;

}ViewData;

typedef struct {
   GtkWidget *w;
   guint wid;
} SII_WIDGET_INFO;

static ViewData *vdata;
static GSList *radio_group = NULL;

void sii_view_menubar2( GtkWidget  *window, GtkWidget **menubar
		       , guint frame_num);
void sii_view_widget( guint frame_num );
void sii_update_view_widget (guint frame_num);
void sii_update_linked_view_widgets (guint frame_num);

/* c---------------------------------------------------------------------- */
/* c------------------------------------------------------------------------ */

gboolean sii_view_tic_info (guint frame_num, gdouble *xinc, gdouble *yinc
			    , gboolean *xtics, gboolean *ytics
			    , gboolean *annot)
{
  gboolean doit;
  ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
  WW_PTR wwptr = solo_return_wwptr(frame_num);

  *xinc = wwptr->view->horiz_tic_mark_km;
  *yinc = wwptr->view->vert_tic_mark_km;
  *xtics = *xinc > 0;
  *ytics = *yinc > 0;

  doit = *xtics || *ytics;

  if (*xinc <= 0)
    { *xinc = vd->orig_values[VIEW_VALUES_XYTICS][0]; }
  if (*yinc <= 0)
    { *yinc = vd->orig_values[VIEW_VALUES_XYTICS][1]; }

  *annot = vd->toggle[VIEW_XY_TIC_LABELS];

  return doit;
}

/* c------------------------------------------------------------------------ */

void sii_default_center (gint frame_num)
{
  WW_PTR wwptr;
  guint jj = 0, nn = SOLO_MAX_WINDOWS;

  if( frame_num >= 0) {
    jj = frame_num;
    nn = jj+1;
  }

  for(; jj < nn; jj++) {
    wwptr = solo_return_wwptr(jj);
    wwptr->center_of_view->rotation_angle = 
      wwptr->center_of_view->range = 0;
  }
  sii_plot_data (0, REPLOT_ALL);
  for(jj=0; jj < sii_return_frame_count(); jj++) {
     sii_update_view_widget (jj);
  }
  return;
}

/* c------------------------------------------------------------------------ */

void sii_blanket_zoom_change (double zoom_factor)
{
  WW_PTR wwptr;
  guint jj;

  for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) {
    wwptr = solo_return_wwptr(jj);
    if (zoom_factor > 0) {
       wwptr->view->magnification *= zoom_factor;
       wwptr->view->ts_magnification = wwptr->view->magnification;
    }
    else
      { wwptr->view->ts_magnification = wwptr->view->magnification = 1.0; }
  }
  sii_plot_data (0, REPLOT_ALL);
  for(jj=0; jj < sii_return_frame_count(); jj++) {
     sii_update_view_widget (jj);
  }
  return;
}

/* c------------------------------------------------------------------------ */

void sii_center_on_clicks (gint num_clicks)
{
  LinksInfo *li;
  ViewData *vd;
  SiiFrameConfig *sfc;
  WW_PTR wwptrc, wwptr, wwptrx;
  struct frame_ctr_info *fci;
  SiiPoint *pt, *prev_pt;
  guint jj, kk, fn, frame_num;
  SiiLinkedList *scq = sii_click_que;
  double minx = 0, maxx = 0, miny = 0, maxy = 0, xx, yy, zz, rr, theta, rotang;
  double lat0, lon0, alt0, lat1, lon1, alt1, R_earth, zero = 0;
  double dx, dy, factor, ppk, sp_meters_per_pixel();
  float f_angle, f_range;
  struct point_in_space radar, *center, pisp;

  if (!num_clicks || num_clicks >= TASK_MODULO) {
    /* zero => frame:0 & center on radar */
    frame_num = num_clicks/TASK_MODULO;
    num_clicks %= TASK_MODULO;
  }
  else if (!sii_click_que) {
      sii_message ("Not enough clicks in the same frame");
      return;
  }
  else if (sii_click_que) {
    prev_pt = pt = (SiiPoint *)sii_click_que->data;
    if (num_clicks < click_count) {
      sii_message ("Not enough clicks in the same frame");
      return;
    }
    frame_num = pt->frame_num;
  }

  vd = (ViewData *)frame_configs[frame_num]->view_data;
  wwptr = solo_return_wwptr(frame_num);

  if (num_clicks < 2 && frame_num >= 0) {
    li = frame_configs[frame_num]->link_set[LI_CENTER];

    for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) {
      if(!li->link_set[jj])
	{ continue; }
      wwptr = solo_return_wwptr(jj);
      f_angle = (num_clicks < 1) ? 0 : wwptr->clicked_angle;
      f_range = (num_clicks < 1) ? 0 : wwptr->clicked_range;

      /* the following angle and range are relative to the radar
       * and are used by "sp_rtheta_screen_to_xyz()"
       */
      wwptr->center_of_view->rotation_angle = f_angle;
      wwptr->center_of_view->range = f_range;
      wwptr->view->ts_ctr_km = f_range;
    }
    sii_plot_data (frame_num, REPLOT_ALL);
    for(jj=0; jj < sii_return_frame_count(); jj++) {
       sii_update_view_widget (jj);
    }
    return;
  }



  minx = maxx = pt->xx;
  miny = maxy = pt->yy;
  scq = sii_click_que->previous;
  sfc = frame_configs[frame_num];

  for (jj=0; jj < num_clicks -1; jj++, scq = scq->previous) {
    pt = (SiiPoint *)scq->data;
    if (pt->frame_num != frame_num) {
      sii_message ("Not enough clicks in the same frame");
      return;
    }
    if (pt->xx < minx)
      { minx = pt->xx; }
    if (pt->xx > maxx)
      { maxx = pt->xx; }
    if (pt->yy < miny)
      { miny = pt->yy; }
    if (pt->yy > maxy)
      { maxy = pt->yy; }
  }
  dx = (maxx - minx);
  dy = (maxy - miny);
  wwptr = solo_return_wwptr(frame_num);
  ppk = wwptr->view->magnification/sp_meters_per_pixel() * 1000;

  if (wwptr->view->type_of_plot & SOLO_TIME_SERIES)
    { factor = (sfc->height/ppk)/dy; }
  else if( dx/dy > sfc->width/sfc->height)
    { factor = (sfc->width/ppk)/dx; }
  else
    { factor = (sfc->height/ppk)/dy; }

  li = frame_configs[frame_num]->link_set[LI_VIEW];
  wwptrc = wwptr;

  for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) {
    if(!li->link_set[jj])
      { continue; }
    wwptr = solo_return_wwptr(jj);
    wwptr->view->magnification *= factor;
    wwptr->view->ts_magnification = wwptr->view->magnification;
  }

  xx = minx + .5 * dx;
  yy = miny + .5 * dy;
  zz = 0;

  wwptr = solo_return_wwptr(frame_num);
  center = wwptr->center_of_view;

  /* set range and rot. angle of center relative to radar */
  theta = atan2 (yy, xx);
  wwptr->view->ts_ctr_km = center->range = sqrt (xx*xx + yy*yy);
  center->rotation_angle = CART_ANGLE (DEGREES (theta));

  dd_copy_pisp(wwptr->lead_sweep->radar_location, &radar);
  sp_rtheta_screen_to_xyz(frame_num, &radar, center);
  dd_latlon_shift(&radar, center);

  li = frame_configs[frame_num]->link_set[LI_CENTER];
  wwptrc = wwptr;

  for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) {
    if(!li->link_set[jj] || jj == wwptrc->window_num)
      { continue; }
    wwptr = solo_return_wwptr(jj);

    if (wwptrc->sweep->linked_windows[jj]) {
      wwptr->center_of_view->rotation_angle = center->rotation_angle;
      wwptr->view->ts_ctr_km =
	wwptr->center_of_view->range = center->range;
    }
    else {
      dd_copy_pisp(wwptr->lead_sweep->radar_location, &radar);
      dd_latlon_relative (center, &radar); /* set xyz in "radar" */
      sp_xyz_to_rtheta_screen(jj, &radar, wwptr->center_of_view);
				/* set new (r,theta) in "center_of_view" */
    }
  }
  
  sii_plot_data (frame_num, REPLOT_ALL);
  for(jj=0; jj < sii_return_frame_count(); jj++) {
     sii_update_view_widget (jj);
  }
  return;
}

/* c---------------------------------------------------------------------- */

gboolean
sii_set_view_info (guint frame_num)
{
  LinksInfo *li;
  ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
  char str[256], *sptrs[32];
  const gchar *aa, *bb;
  gint nt, nn, jj;
  gfloat ff,gg;
  gboolean time_series = FALSE;
  struct landmark_widget_info lwi;
  struct centering_widget_info cwi;
  struct view_widget_info vwi;


  memset (&vwi, 0, sizeof (vwi));
  memset (&cwi, 0, sizeof (cwi));
  memset (&lwi, 0, sizeof (lwi));

  vwi.frame_num = cwi.frame_num = lwi.frame_num = frame_num;
  vwi.type_of_plot = 0;
  
  time_series = vd->toggle[VIEW_TS_MODE];

  if (time_series)
    { vwi.type_of_plot |= SOLO_TIME_SERIES; }

  if (vd->toggle[VIEW_TS_RL])
    { vwi.type_of_plot |= TS_PLOT_RIGHT_TO_LEFT; }

  if (vd->toggle[VIEW_TS_MSL])
    { vwi.type_of_plot |= TS_MSL_RELATIVE; }

  if (vd->toggle[VIEW_TS_DOWN])
    { vwi.type_of_plot |= TS_PLOT_DOWN; }

  if (vd->toggle[VIEW_TS_AUTO])
    { vwi.type_of_plot |= TS_AUTOMATIC; }

  vwi.ctr_from_last_click = (vd->ctr_from_last_click) ? 1 : 0;
  vd->ctr_from_last_click = FALSE;

  jj = VIEW_ZOOM;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 1, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
  }
  vwi.magnification = vwi.ts_magnification = vd->orig_values[jj][0];

  jj = VIEW_ANG_FILL;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 1, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
  }
  vwi.angular_fill_pct = vd->orig_values[jj][0];

  jj = VIEW_VALUES_AZRNG;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 2, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
    vd->orig_values[jj][1] = gg;
  }

  if (time_series) {
    vwi.az_line_int_deg = vwi.rng_ring_int_km = -1;
  }
  else if (vd->rng_az_state == VIEW_AZRNG) {
    vwi.rng_ring_int_km = vd->orig_values[jj][0];
    vwi.az_line_int_deg = vd->orig_values[jj][1];
  }
  else if (vd->rng_az_state == VIEW_NO_AZRNG) {
    vwi.az_line_int_deg = vwi.rng_ring_int_km = -1;
  }
  else if (vd->rng_az_state == VIEW_AZ_ONLY) {
    vwi.az_line_int_deg = vd->orig_values[jj][1];
    vwi.rng_ring_int_km = -1;
  }
  else {
    vwi.rng_ring_int_km = vd->orig_values[jj][0];
    vwi.az_line_int_deg = -1;
  }

  vwi.az_annot_at_km = vwi.rng_annot_at_deg = -1;

  if (vd->rng_az_labels = vd->toggle[VIEW_AZRNG_LABELS]) {
    jj = VIEW_AZ_LBL_RNG;
    aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
    if (sii_str_values ( aa, 1, &ff, &gg )) {
      vd->orig_values[jj][0] = ff;
    }
    vwi.az_annot_at_km = vd->orig_values[jj][0];
    
    jj = VIEW_RNG_LBL_AZ;
    aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
    if (sii_str_values ( aa, 1, &ff, &gg )) {
      vd->orig_values[jj][0] = ff;
    }
    vwi.rng_annot_at_deg = vd->orig_values[jj][0];
  }
  vwi.magic_rng_annot = (vd->toggle[VIEW_MAGIC_RNG_LBLS]) ? 1 : 0;

  jj = VIEW_VALUES_XYTICS;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 2, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
    vd->orig_values[jj][1] = gg;
  }

  if (time_series || vd->xy_tics_state == VIEW_XY_TICS) {
    vwi.horiz_tic_mark_km = vd->orig_values[jj][0];
    vwi.vert_tic_mark_km = vd->orig_values[jj][1];
  }
  else if (vd->xy_tics_state == VIEW_NO_XY_TICS) {
    vwi.horiz_tic_mark_km = 0;
    vwi.vert_tic_mark_km = 0;
  }
  else if (vd->xy_tics_state == VIEW_X_TICS_ONLY) {
    vwi.horiz_tic_mark_km = vd->orig_values[jj][0];
    vwi.vert_tic_mark_km = 0;
  }
  else {
    vwi.horiz_tic_mark_km = 0;
    vwi.vert_tic_mark_km = vd->orig_values[jj][1];
  }

  li = frame_configs[frame_num]->link_set[LI_VIEW];
  for (jj=0; jj < maxFrames; jj++) {
    vwi.linked_windows[jj] = (li->link_set[jj]) ? 1 : 0;
  }


  /* centering */

  cwi.reference_frame = 1;
  cwi.options = SOLO_LOCAL_POSITIONING;
  if (vd->ctr_state == CENTER_OTHER) {
    cwi.options = SOLO_LINKED_POSITIONING;
  }
  else if (vd->ctr_state == CENTER_FIXED) {
    cwi.options = SOLO_FIXED_POSITIONING;
  }

  jj = VIEW_CTR_RNG_AZ;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (!strlen (aa))
    { vd->orig_values[jj][0] = vd->orig_values[jj][1] = 0; }
  else if (sii_str_values ( aa, 2, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
    vd->orig_values[jj][1] = gg;
  }
  cwi.rng_of_ctr = vwi.ts_ctr_km = vd->orig_values[jj][0];
  cwi.az_of_ctr = vd->orig_values[jj][1];

  jj = VIEW_CTR_LAT_LON;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 2, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
    vd->orig_values[jj][1] = gg;
  }
  cwi.latitude = vd->orig_values[jj][0];
  cwi.longitude = vd->orig_values[jj][1];

  jj = VIEW_CTR_ALT;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 1, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
  }
  cwi.altitude = vd->orig_values[jj][0];

# ifdef obsolete
  jj = VIEW_CTR_REF_FRAME;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 1, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
  }
  cwi.reference_frame = vd->orig_values[jj][0];
# endif

  li = frame_configs[frame_num]->link_set[LI_CENTER];
  for (jj=0; jj < maxFrames; jj++) {
    cwi.linked_windows[jj] = (li->link_set[jj]) ? 1 : 0;
  }


  /* landmark */

  lwi.reference_frame = 1;
  lwi.options = SOLO_LOCAL_POSITIONING;
  if (vd->lmrk_state == LANDMARK_OTHER) {
    lwi.options = SOLO_LINKED_POSITIONING;
  }
  else if (vd->lmrk_state == LANDMARK_FIXED) {
    lwi.options = SOLO_FIXED_POSITIONING;
  }

# ifdef notyet
  jj = VIEW_LMRK_RNG_AZ;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 2, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
    vd->orig_values[jj][1] = gg;
  }
  lwi.rng_of_lmrk = vd->orig_values[jj][0];
  lwi.az_of_lmrk = vd->orig_values[jj][1];
# endif

  jj = VIEW_LMRK_LAT_LON;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 2, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
    vd->orig_values[jj][1] = gg;
  }
  lwi.latitude = vd->orig_values[jj][0];
  lwi.longitude = vd->orig_values[jj][1];

  jj = VIEW_LMRK_ALT;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 1, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
  }
  lwi.altitude = vd->orig_values[jj][0];

# ifdef obsolete
  jj = VIEW_LMRK_REF_FRAME;
  aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[jj]));
  if (sii_str_values ( aa, 1, &ff, &gg )) {
    vd->orig_values[jj][0] = ff;
  }
  lwi.reference_frame = vd->orig_values[jj][0];
# endif

  li = frame_configs[frame_num]->link_set[LI_LANDMARK];
  for (jj=0; jj < maxFrames; jj++) {
    lwi.linked_windows[jj] = (li->link_set[jj]) ? 1 : 0;
  }

 /* send to old solo code */
  jj = solo_set_view_info(&vwi, &lwi, &cwi);

  return (!jj) ? TRUE : FALSE;
}

/* c---------------------------------------------------------------------- */

void sii_update_linked_view_widgets (guint frame_num)
{
  LinksInfo *li;
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
  guint jj;
  GtkWidget *widget, *entry;

  li = frame_configs[frame_num]->link_set[LI_VIEW];
  for (jj=0; jj < maxFrames; jj++) {
    if(!li->link_set[jj])
      { continue; }
    widget = sii_get_widget_ptr (jj, FRAME_VIEW);
    if (widget) {
      sii_set_view_info (jj);
      sii_update_view_widget (jj);
    }
  }
}

/* c---------------------------------------------------------------------- */

void sii_view_update_links (guint frame_num, int li_type)
{
  LinksInfo *li;
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
  guint jj;
  long *lwins = 0;

  if (!vd)
    { return; }

  switch (li_type) {
   case LI_VIEW:
     lwins = wwptr->view->linked_windows;
     break;
   case LI_CENTER:
     lwins = wwptr->frame_ctr_info->linked_windows;
     break;
   case LI_LANDMARK:
     lwins = wwptr->landmark_info->linked_windows;
     break;
  };

  sii_set_links_from_solo_struct (frame_num, li_type, lwins);

}

/* c---------------------------------------------------------------------- */

void sii_update_view_info (guint frame_num)
{
  LinksInfo *li;
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
  guint jj;
  GtkWidget *widget, *entry, *check_item;
  gchar *aa, str[64], fmt[16];
  GString *gs;
  gfloat ff, gg;
  gboolean active;

  /* assume already initialized */

  vd->rng_az_state = VIEW_AZRNG;

  if (wwptr->view->rng_ring_int_km <= 0 && wwptr->view->az_line_int_deg <= 0) {
    vd->rng_az_state = VIEW_NO_AZRNG;
  }
  else if (wwptr->view->rng_ring_int_km <= 0) {
    vd->rng_az_state = VIEW_AZ_ONLY;
  }
  else if (wwptr->view->az_line_int_deg <= 0) {
    vd->rng_az_state = VIEW_RNG_ONLY;
  }

  vd->rng_az_labels = TRUE;
  if (wwptr->view->az_annot_at_km <= 0 && vd->rng_az_state == VIEW_AZ_ONLY) {
    vd->rng_az_labels = FALSE;
  }
  else if (wwptr->view->rng_annot_at_deg < 0
	   && vd->rng_az_state == VIEW_RNG_ONLY) {
    vd->rng_az_labels = FALSE;
  }
  else if (wwptr->view->az_annot_at_km <= 0 &&
	   wwptr->view->rng_annot_at_deg < 0) {
    vd->rng_az_labels = FALSE;
  }
  

  vd->xy_tics_state = VIEW_XY_TICS;

  if (wwptr->view->horiz_tic_mark_km <= 0
      && wwptr->view->vert_tic_mark_km <= 0 ) {
    vd->xy_tics_state = VIEW_NO_XY_TICS;
  }
  else if (wwptr->view->horiz_tic_mark_km > 0
	   && wwptr->view->vert_tic_mark_km <= 0) {
    vd->xy_tics_state = VIEW_X_TICS_ONLY;
  }
  else if (wwptr->view->vert_tic_mark_km > 0
	   && wwptr->view->horiz_tic_mark_km <= 0) {
    vd->xy_tics_state = VIEW_Y_TICS_ONLY;
  }

  if (wwptr->view->type_of_plot & SOLO_TIME_SERIES) {
    check_item = vd->data_widget[VIEW_TS_MODE];
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  }

  check_item = (wwptr->view->type_of_plot & TS_PLOT_RIGHT_TO_LEFT)
    ? vd->data_widget[VIEW_TS_RL] : vd->data_widget[VIEW_TS_LR];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  check_item = (wwptr->view->type_of_plot & TS_MSL_RELATIVE)
    ? vd->data_widget[VIEW_TS_MSL] : vd->data_widget[VIEW_TS_RADAR];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  check_item = vd->data_widget[VIEW_TS_UP];
  if (wwptr->view->type_of_plot & TS_PLOT_DOWN)
    { check_item = vd->data_widget[VIEW_TS_DOWN]; }
  else if (wwptr->view->type_of_plot & TS_AUTOMATIC)
    { check_item = vd->data_widget[VIEW_TS_AUTO]; }

  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  jj = VIEW_ZOOM;
  ff = wwptr->view->magnification;
  vd->orig_values[jj][0] = ff;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 1, ff, gg, vd->precision[jj]);
  
  jj = VIEW_VALUES_AZRNG;
  ff = wwptr->view->rng_ring_int_km;
  if (ff > 0)
    { vd->orig_values[jj][0] = ff; }
  gg = wwptr->view->az_line_int_deg;
  if (gg > 0)
    { vd->orig_values[jj][1] = gg; }

  aa = sii_set_string_from_vals (vd->orig_txt[jj], 2
				 , vd->orig_values[jj][0]
				 , vd->orig_values[jj][1]
				 , vd->precision[jj]);

  jj = VIEW_VALUES_XYTICS;
  ff = wwptr->view->horiz_tic_mark_km;
  if (ff > 0)
    { vd->orig_values[jj][0] = ff; }
  gg = wwptr->view->vert_tic_mark_km;
  if (gg > 0)
    { vd->orig_values[jj][1] = gg; }
  aa = sii_set_string_from_vals (vd->orig_txt[jj], 2
				 , vd->orig_values[jj][0]
				 , vd->orig_values[jj][1]
				 , vd->precision[jj]);

  jj = VIEW_AZ_LBL_RNG;
  ff = wwptr->view->az_annot_at_km;
  if (ff > 0) {
    vd->orig_values[jj][0] = ff;
    aa = sii_set_string_from_vals (vd->orig_txt[jj], 1, ff, gg
				   , vd->precision[jj]);
  }
  jj = VIEW_RNG_LBL_AZ;
  ff = wwptr->view->rng_annot_at_deg;
  if (ff >= 0) {
    vd->orig_values[jj][0] = ff;
    aa = sii_set_string_from_vals (vd->orig_txt[jj], 1, ff, gg
				   , vd->precision[jj]);
  }
  jj = VIEW_ANG_FILL;
  ff = wwptr->view->angular_fill_pct;
  vd->orig_values[jj][0] = ff;
  aa = sii_set_string_from_vals (vd->orig_txt[jj], 1, ff, gg
				 , vd->precision[jj]);

  jj = VIEW_RADAR_LAT_LON_ALT;
  g_string_sprintf (vd->orig_txt[jj]
		    , "Radar Lat: %.4f  Lon: %.4f  Alt: %.3f"
		    , wwptr->radar_location->latitude
		    , wwptr->radar_location->longitude
		    , wwptr->radar_location->altitude);

  sii_set_links_from_solo_struct (frame_num, LI_VIEW
				  , wwptr->view->linked_windows);

  /* wwptr->frame_ctr_info->centering_options */

  switch (wwptr->frame_ctr_info->centering_options) {
  case SOLO_LOCAL_POSITIONING:
    vd->ctr_state = CENTER_NO_LINKS;
    break;
  case SOLO_FIXED_POSITIONING:
    vd->ctr_state = CENTER_FIXED;
    break;
  case SOLO_LINKED_POSITIONING:
    vd->ctr_state = CENTER_OTHER;
    break;
  };

  jj = VIEW_CTR_RNG_AZ;
  ff = wwptr->center_of_view->range;
  gg = wwptr->center_of_view->rotation_angle;
  vd->orig_values[jj][0] = ff;
  vd->orig_values[jj][1] = gg;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 2, ff, gg, vd->precision[jj]);

  jj = VIEW_CTR_LAT_LON;
  ff = (float)wwptr->center_of_view->latitude;
  gg = (float)wwptr->center_of_view->longitude;
  vd->orig_values[jj][0] = ff;
  vd->orig_values[jj][1] = gg;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 2, ff, gg, vd->precision[jj]);


  jj = VIEW_CTR_ALT;
  ff = (float)wwptr->center_of_view->altitude;
  vd->orig_values[jj][0] = ff;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 1, ff, gg, vd->precision[jj]);

  jj = VIEW_CTR_REF_FRAME;
  ff = wwptr->frame_ctr_info->reference_frame;
  vd->orig_values[jj][0] = ff;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 1, ff, gg, vd->precision[jj]);

  sii_set_links_from_solo_struct (frame_num, LI_CENTER
				  , wwptr->frame_ctr_info->linked_windows);

  jj = VIEW_LMRK_RNG_AZ;
  ff = wwptr->landmark->range;
  gg = wwptr->landmark->azimuth;
  vd->orig_values[jj][0] = ff;
  vd->orig_values[jj][1] = gg;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 2, ff, gg, vd->precision[jj]);

  jj = VIEW_LMRK_LAT_LON;
  ff = (float)wwptr->landmark->latitude;
  gg = (float)wwptr->landmark->longitude;
  vd->orig_values[jj][0] = ff;
  vd->orig_values[jj][1] = gg;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 2, ff, gg, vd->precision[jj]);


  jj = VIEW_LMRK_ALT;
  ff = (float)wwptr->landmark->altitude;
  vd->orig_values[jj][0] = ff;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 1, ff, gg, vd->precision[jj]);

  jj = VIEW_LMRK_REF_FRAME;
  ff = wwptr->landmark_info->reference_frame;
  vd->orig_values[jj][0] = ff;
  aa = sii_set_string_from_vals
    (vd->orig_txt[jj], 1, ff, gg, vd->precision[jj]);

  sii_set_links_from_solo_struct (frame_num, LI_LANDMARK
				  , wwptr->landmark_info->linked_windows);

  switch (wwptr->landmark_info->landmark_options) {
  case SOLO_LOCAL_POSITIONING:
    vd->lmrk_state = LANDMARK_NO_LINKS;
    break;
  case SOLO_FIXED_POSITIONING:
    vd->lmrk_state = LANDMARK_FIXED;
    break;
  case SOLO_LINKED_POSITIONING:
    vd->lmrk_state = LANDMARK_OTHER;
    break;
  };

}

/* c---------------------------------------------------------------------- */

void sii_update_view_widget (guint frame_num)
{
  LinksInfo *li;
  WW_PTR wwptr = solo_return_wwptr(frame_num)->lead_sweep;
  ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
  guint jj;
  GtkWidget *widget, *entry, *check_item;
  gchar *aa;
  gdouble lat = 34.5678, lon = -123.4567, alt = 1.234;
  gboolean active;
  
  if (!sii_get_widget_ptr (frame_num, FRAME_VIEW))
    { return; }

  sii_update_view_info (frame_num);

  check_item = vd->data_widget[vd->rng_az_state];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  active = vd->rng_az_labels;
  check_item = vd->data_widget[VIEW_AZRNG_LABELS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), active );

  active = (wwptr->magic_rng_annot) ? TRUE : FALSE;
  check_item = vd->data_widget[VIEW_MAGIC_RNG_LBLS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), active );

  jj = VIEW_XY_TICS;
  jj = VIEW_X_TICS_ONLY;
  jj = VIEW_Y_TICS_ONLY;
  jj = VIEW_NO_XY_TICS;
  jj = vd->xy_tics_state;
  check_item = vd->data_widget[vd->xy_tics_state];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  
   jj = VIEW_ZOOM;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);
  
   jj = VIEW_ANG_FILL;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_VALUES_AZRNG;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_VALUES_XYTICS;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_AZ_LBL_RNG;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_RNG_LBL_AZ;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_RADAR_LAT_LON_ALT;
   aa = vd->orig_txt[jj]->str;
   gtk_label_set_text (GTK_LABEL (vd->data_widget[jj]), aa);

   jj = VIEW_CTR_RNG_AZ;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_CTR_LAT_LON;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_CTR_ALT;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

# ifdef obsolete
   jj = VIEW_CTR_REF_FRAME;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);
# endif

   check_item = vd->data_widget[vd->ctr_state];
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

   jj = VIEW_LMRK_RNG_AZ;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_LMRK_LAT_LON;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

   jj = VIEW_LMRK_ALT;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);

# ifdef obsolete
   jj = VIEW_LMRK_REF_FRAME;
   aa = vd->orig_txt[jj]->str;
   gtk_entry_set_text (GTK_ENTRY (vd->data_widget[jj]), aa);
# endif

   check_item = vd->data_widget[vd->lmrk_state];
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

}

/* c---------------------------------------------------------------------- */

gint sii_radio_inventory ( GtkWidget *rmi, gint *item_count )
{
   /* This routine appears to be necessary because if you change
    * a radio menu item selection you get two callbacks.
    * The first one from the unselected button and the second
    * from the selected button.
    * However, if you click on an already selected button you get
    * only on callback from the already selected button.
    *
    */
   GSList *group;
   gint jj, kk= -1, active;

   g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));
   group = gtk_radio_menu_item_group ( GTK_RADIO_MENU_ITEM (rmi));
   g_return_if_fail (group != NULL);
   *item_count = 0;

   for (jj=0; group; jj++, group = group->next) {
      (*item_count)++;

      if (active = GTK_CHECK_MENU_ITEM (group->data)->active)
	{ kk = jj; }
   }
   return kk;

   /* the group list appears to be a stack
    * so the first item encountered is
    * the last radio item created
    * at least in the menu item factory
    */
}

/* c---------------------------------------------------------------------- */

void sii_initialize_view (guint frame_num)
{
   ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
   guint jj;
   gchar *aa;
   gdouble lat = 34.5678, lon = -123.4567, alt = 1.234;


  if( !frame_configs[frame_num]->view_data ) {
     vd = frame_configs[frame_num]->view_data =
       g_malloc0 (sizeof( ViewData ));

     frame_configs[frame_num]->link_set[LI_VIEW] = 
       vd->view_links = sii_new_links_info
	 ( "View Links", frame_num, FRAME_VIEW_LINKS, FALSE );
     
     frame_configs[frame_num]->link_set[LI_CENTER] = 
       vd->center_links = sii_new_links_info
	 ( "Center Links", frame_num, FRAME_CTR_LINKS, FALSE );
     
     frame_configs[frame_num]->link_set[LI_LANDMARK] = 
       vd->landmark_links = sii_new_links_info
	 ( "Landmark Links", frame_num, FRAME_LMRK_LINKS, FALSE );
  }

   vd->changed = TRUE;

   for (jj=0; jj < VIEW_MAX_WIDGETS; jj++) {
      vd->precision[jj] = 1;
      vd->change_flag[jj] = TRUE;
      vd->orig_txt[jj] = g_string_new ("");
      vd->txt[jj] = g_string_new ("");
   }

   jj = VIEW_ZOOM;
   vd->entry_flag[jj] = ENTRY_ONE_FLOAT;
   vd->precision[jj] = 3;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 1, 1, 0, vd->precision[jj]);

   jj = VIEW_VALUES_AZRNG;
   vd->entry_flag[jj] = ENTRY_TWO_FLOATS;
   vd->precision[jj] = 1;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 2, 10, 30, vd->precision[jj]);

   vd->toggle[VIEW_XY_TIC_LABELS] = TRUE;

   jj = VIEW_VALUES_XYTICS;
   vd->entry_flag[jj] = ENTRY_TWO_FLOATS;
   vd->precision[jj] = 1;
   vd->orig_values[jj][0] = vd->orig_values[jj][1] = 10.;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 2
      , vd->orig_values[jj][0]
      , vd->orig_values[jj][1]
      , vd->precision[jj]);

   jj = VIEW_AZ_LBL_RNG;
   vd->entry_flag[jj] = ENTRY_ONE_FLOAT;
   vd->precision[jj] = 1;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 1, 45, 0, vd->precision[jj]);

   jj = VIEW_RNG_LBL_AZ;
   vd->entry_flag[jj] = ENTRY_ONE_FLOAT;
   vd->precision[jj] = 1;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 1, 35, 0, vd->precision[jj]);

   jj = VIEW_CTR_RNG_AZ;
   vd->entry_flag[jj] = ENTRY_TWO_FLOATS;
   vd->precision[jj] = 3;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 2, 0, 0, vd->precision[jj]);

   jj = VIEW_CTR_LAT_LON;
   vd->entry_flag[jj] = ENTRY_TWO_FLOATS;
   vd->precision[jj] = 4;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 2, 0, 0, vd->precision[jj]);

   jj = VIEW_CTR_ALT;
   vd->entry_flag[jj] = ENTRY_ONE_FLOAT;
   vd->precision[jj] = 3;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 1, 0, 0, vd->precision[jj]);

   jj = VIEW_LMRK_RNG_AZ;
   vd->entry_flag[jj] = ENTRY_TWO_FLOATS;
   vd->precision[jj] = 3;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 2, 0, 0, vd->precision[jj]);

   jj = VIEW_LMRK_LAT_LON;
   vd->entry_flag[jj] = ENTRY_TWO_FLOATS;
   vd->precision[jj] = 4;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 2, 0, 0, vd->precision[jj]);

   jj = VIEW_LMRK_ALT;
   vd->entry_flag[jj] = ENTRY_ONE_FLOAT;
   vd->precision[jj] = 3;
   aa = sii_set_string_from_vals
     (vd->orig_txt[jj], 1, 0, 0, vd->precision[jj]);

   jj = VIEW_RADAR_LAT_LON_ALT;
   g_string_truncate (vd->orig_txt[jj], 0);
   g_string_sprintfa (vd->orig_txt[jj]
		      , "Radar Lat: %.4f  Lon: %.4f  Alt: %.3f"
		      , lat, lon, alt);
}

/* c---------------------------------------------------------------------- */

void sii_view_dup_orig (guint frame_num)
{
   ViewData *vd = (ViewData *)frame_configs[frame_num]->view_data;
   guint size, jj, entry_flag;
   size = sizeof (*vd->values[jj]);

   for (jj=0; jj < VIEW_MAX_WIDGETS; jj++) {

      entry_flag = vd->entry_flag[jj];
				/*
      if (!entry_flag)
	{ continue; }
      g_message ("jj:%d  ef:%d", jj, entry_flag);
				 */
      switch (entry_flag) {

       case ENTRY_TXT_ONLY:
	 break;

       default:
	 g_string_assign (vd->txt[jj], vd->orig_txt[jj]->str);
	 g_memmove (vd->values[jj], vd->orig_values[jj], sizeof (*vd->values[jj]));
	 break;
      };
   }
}

/* c---------------------------------------------------------------------- */

void sii_view_entry_info
( GtkWidget *w, guint wid, guint frame_num, guint entry_flag )
{
   vdata->data_widget[wid] = w;
}

/* c---------------------------------------------------------------------- */

void sii_view_menu_cb ( GtkWidget *w, gpointer   data )
{
  guint num = GPOINTER_TO_UINT (data);
  guint frame_num, task, wid, active, jj, task2;
  gfloat f1, f2, rnd = .000001;
  const gchar *aa;
  ViewData *vd;
  GtkWidget *widget, *check_item, *rmi;
  gint flag, nn;
   
  frame_num = num/TASK_MODULO;
  wid =
    task = num % TASK_MODULO;

  vd = (ViewData *)frame_configs[frame_num]->view_data;
  widget =
    check_item =
    rmi = vd->data_widget[task];

  switch (task) {

  case VIEW_OK:
  case VIEW_CANCEL:
  case VIEW_CLOSE:
    widget = sii_get_widget_ptr (frame_num, FRAME_VIEW);
    if( widget )
      { gtk_widget_hide (widget); }
    widget = sii_get_widget_ptr (frame_num, FRAME_VIEW_LINKS);
    if( widget )
      { gtk_widget_hide (widget); }
    widget = sii_get_widget_ptr (frame_num, FRAME_CTR_LINKS);
    if( widget )
      { gtk_widget_hide (widget); }
    widget = sii_get_widget_ptr (frame_num, FRAME_LMRK_LINKS);
    if( widget )
      { gtk_widget_hide (widget); }

    if (task == VIEW_OK) {	/* send info to old solo */
      sii_set_view_info (frame_num);
      sii_update_linked_view_widgets (frame_num);
    }
    break;
      
    /* Radio menu items: If you change a radio menu item selection,
     * you get two callbacks. The first one from the 
     * unselected button and the second from the selected button.
     * If you click on an already selected button you get
     * only one callback from the already selected button.
     *
     * This is apparently only true on Solaris!
     */
  case VIEW_AZRNG:
  case VIEW_NO_AZRNG:
  case VIEW_AZ_ONLY:
  case VIEW_RNG_ONLY:
    g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

    if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
      { vd->rng_az_state = task; }

# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    vd->toggle[task] = active;
    break;


  case VIEW_XY_TICS:
  case VIEW_X_TICS_ONLY:
  case VIEW_Y_TICS_ONLY:
  case VIEW_NO_XY_TICS:
    g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

    if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
      { vd->xy_tics_state = task; }

# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    vd->toggle[task] = active;
    break;


  case CENTER_NO_LINKS:
  case CENTER_FIXED:
  case CENTER_OTHER:
    g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

    if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
      { vd->ctr_state = task; }

# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    vd->toggle[task] = active;
    break;


  case LANDMARK_NO_LINKS:
  case LANDMARK_FIXED:
  case LANDMARK_OTHER:
    g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

    if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
      { vd->lmrk_state = task; }

# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    vd->toggle[task] = active;
    break;

    /* Check menu items */
  case VIEW_AZRNG_LABELS:
  case VIEW_MAGIC_RNG_LBLS:
  case VIEW_XY_TIC_LABELS:
  case VIEW_TS_MODE:

    vd->toggle[task] = GTK_CHECK_MENU_ITEM (rmi)->active;
# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    break;
      

  case CENTER_ON_FOUR:
  case CENTER_ON_TWO:
  case CENTER_ON_ONE:
  case CENTER_ON_RADAR:
    nn = TASK_MODULO * frame_num;

    switch (task) {
    case CENTER_ON_FOUR:
      nn += 4;
      break;
    case CENTER_ON_TWO:
      nn += 2;
      break;
    case CENTER_ON_ONE:
      nn += 1;
      break;
    };
    sii_center_on_clicks (nn);
    break;

  case VIEW_REPLOT_THIS:
  case VIEW_REPLOT_LINKS:
  case VIEW_REPLOT_ALL:
    flag = REPLOT_THIS_FRAME;
      
    if (task == VIEW_REPLOT_LINKS)
      { flag = REPLOT_LOCK_STEP; }
    else if (task == VIEW_REPLOT_ALL)
      { flag = REPLOT_ALL; }
      
    if ( sii_set_view_info (frame_num))
      { sii_plot_data (frame_num, flag); }
    sii_update_linked_view_widgets (frame_num);
    break;

  case VIEW_TS_LR:
  case VIEW_TS_RL:
    g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

    if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
      { vd->ts_lr_state = task; }

# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    vd->toggle[task] = active;
    break;

  case VIEW_TS_RADAR:
  case VIEW_TS_MSL:
    g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

    if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
      { vd->ts_relative_state = task; }

# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    vd->toggle[task] = active;
    break;

  case VIEW_TS_UP:
  case VIEW_TS_DOWN:
  case VIEW_TS_AUTO:
    g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

    if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
      { vd->ts_point_state = task; }

# ifdef obsolete
    g_message ( "View Task: %d  State: %d  Active: %d"
		, task, vd->toggle[task], active );
# endif
    vd->toggle[task] = active;
    break;

  case VIEW_ZOOM:
  case VIEW_AZ_LBL_RNG:
  case VIEW_RNG_LBL_AZ:
  case VIEW_CTR_ALT:
  case VIEW_CTR_REF_FRAME:
  case VIEW_LMRK_ALT:
  case VIEW_LMRK_REF_FRAME:
				/* one value */
    aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[task]));
    if( !sii_str_values ( aa, 1, &f1, &f2 )) {
      sii_bad_entry_message (aa, 1);
      break;
    }
    vd->values[task][0] = f1;
    vd->values[task][1] = f2;
    break;

  case VIEW_VALUES_AZRNG:
  case VIEW_VALUES_XYTICS:
  case VIEW_CTR_RNG_AZ:
  case VIEW_CTR_LAT_LON:
  case VIEW_LMRK_RNG_AZ:
  case VIEW_LMRK_LAT_LON:
				/* two values */
    aa = gtk_entry_get_text( GTK_ENTRY (vd->data_widget[task]));
    if( !sii_str_values ( aa, 2, &f1, &f2 )) {
      sii_bad_entry_message (aa, 2);
      break;
    }
    vd->values[task][0] = f1;
    vd->values[task][1] = f2;
    g_string_assign (vd->orig_txt[task], aa);
    break;


  case CENTER_LINKS:
    show_links_widget (vd->center_links); 
    break;

  case LANDMARK_LINKS:
    show_links_widget (vd->landmark_links); 
    break;

  case VIEW_LINKS:
    show_links_widget (vd->view_links); 
    break;

  case VIEW_HLP_OVERVIEW:
    nn = sizeof (hlp_view_overview)/sizeof (char *);
    sii_glom_strings (hlp_view_overview, nn, gen_gs);
    sii_message (gen_gs->str);

    break;

  case VIEW_HLP_OPTS:
    nn = sizeof (hlp_view_options)/sizeof (char *);
    sii_glom_strings (hlp_view_options, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case VIEW_HLP_LINKS:
    nn = sizeof (hlp_view_viewlinks)/sizeof (char *);
    sii_glom_strings (hlp_view_viewlinks, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case VIEW_HLP_CTR:
    nn = sizeof (hlp_view_center)/sizeof (char *);
    sii_glom_strings (hlp_view_center, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case VIEW_HLP_LMRK:
    nn = sizeof (hlp_view_landmark)/sizeof (char *);
    sii_glom_strings (hlp_view_landmark, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case VIEW_HLP_TS:
    nn = sizeof (hlp_view_timeseries)/sizeof (char *);
    sii_glom_strings (hlp_view_timeseries, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  };
}

/* c---------------------------------------------------------------------- */

void sii_view_entry_paste_cb ( GtkWidget *w, gpointer   data )
{
   GtkWidget *widget;
   guint num = GPOINTER_TO_UINT (data);
   guint frame_num, wid;
   ViewData *vdata;
   
   frame_num = num/TASK_MODULO;
   wid = num % TASK_MODULO;
   vdata = (ViewData *)frame_configs[frame_num]->view_data;

				/* clear out the entry befor pasting */
   gtk_entry_set_text (GTK_ENTRY (vdata->data_widget[wid]), "");

}

/* c---------------------------------------------------------------------- */

void show_view_widget (GtkWidget *text, gpointer data )
{
  GtkWidget *widget;
  guint afi, frame_num = GPOINTER_TO_UINT (data);
  GdkPoint *ptp;
  gint x, y;
  gchar str[128];


  widget = sii_get_widget_ptr (frame_num, FRAME_VIEW);

  if( !widget )
    { sii_view_widget( frame_num ); }
  else {
     ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
     x = ptp->x; y = ptp->y;
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     sii_update_view_widget (frame_num);
     gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */

void sii_view_widget( guint frame_num )
{
  GtkWidget *label;

  GtkWidget *window;
  GtkWidget *hbox0;
  GtkWidget *hbox;
  GtkWidget *hbox1;
  GtkWidget *hbox2;
  GtkWidget *hbox3;

  GtkWidget *hbbox;

  GtkWidget *vbox;
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *vbox3;

  GtkWidget *table;

  GtkWidget *entry;
  GtkWidget *menubar;
  GtkWidget *menu;
  GtkWidget *menuitem;

  GtkWidget *button;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 0;
  guint ypadding = 0;
  guint padding = 0, row = 0;

  char *aa;
  gchar str[256], str2[256], *bb, *ee;
  gint event_flags = GDK_BUTTON_PRESS_MASK;
  const gchar *cc;
  gint length, width, max_width = 0, jj, nn;
  gfloat upper, f1, f2;
  GSList *entries = NULL;
  GdkPoint *ptp;
  gint x, y;

				/* c...code starts here */

  vdata = (ViewData *)frame_configs[frame_num]->view_data;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  sii_set_widget_ptr ( frame_num, FRAME_VIEW, window );
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num * TASK_MODULO + FRAME_VIEW));

  /* --- Title and border --- */
  bb = g_strdup_printf ("Frame %d  View, Center & Landmark Widget"
			, frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  sii_view_menubar2 (window, &menubar, frame_num);
  gtk_container_add (GTK_CONTAINER(vbox), menubar);

  hbox0 = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (vbox), hbox0);

  strcpy (str, "  View, Center & Landmark Settings   " );
  label = gtk_label_new ( str );
  gtk_box_pack_start (GTK_BOX (hbox0), label, TRUE, TRUE, 0 );

  hbbox = gtk_hbutton_box_new ();
  gtk_hbutton_box_set_spacing_default (4);
  gtk_box_pack_end (GTK_BOX (hbox0), hbbox, TRUE, TRUE, 0 );

  button = gtk_button_new_with_label ("Replot");
  gtk_box_pack_start (GTK_BOX (hbbox), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + VIEW_REPLOT_THIS;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_view_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("OK");
  gtk_box_pack_start (GTK_BOX (hbbox), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + VIEW_OK;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_view_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("Cancel");
  gtk_box_pack_start (GTK_BOX (hbbox), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + VIEW_CANCEL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_view_menu_cb
		      , (gpointer)nn
		      );
  /*
   * set an hbox to contain two vertical boxes of entries
   */
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_container_border_width (GTK_CONTAINER (hbox), 4);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (hbox), vbox2);

  label = gtk_label_new ( "View Settings" );
  gtk_container_add (GTK_CONTAINER (vbox2), label);

  table = gtk_table_new (2, 5, FALSE);
  gtk_box_pack_start (GTK_BOX (vbox2), table, TRUE, TRUE, 0 );
  row = -1;

  row++;
  label = gtk_label_new ( " Zoom " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_ZOOM, frame_num, ENTRY_ONE_FLOAT );

  row++;
  label = gtk_label_new ( " Rings(km) & Spokes(deg) " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_VALUES_AZRNG, frame_num, ENTRY_TWO_FLOATS );

  row++;
  label = gtk_label_new ( " X & Y Tic Marks (km) " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_VALUES_XYTICS, frame_num
		      , ENTRY_TWO_FLOATS );

  row++;
  label = gtk_label_new ( " Azimuth Labels (km) " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_AZ_LBL_RNG, frame_num, ENTRY_ONE_FLOAT );

  row++;
  label = gtk_label_new ( " Range Labels (deg) " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_RNG_LBL_AZ, frame_num, ENTRY_ONE_FLOAT );

  row++;
  label = gtk_label_new ( " Angular Fill (%) " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_ANG_FILL, frame_num, ENTRY_ONE_FLOAT );


# ifdef obsolete
  aa = vdata->orig_txt[VIEW_RADAR_LAT_LON_ALT]->str;
  vdata->data_widget[VIEW_RADAR_LAT_LON_ALT] = label =
    gtk_label_new ( aa );
  gtk_container_add (GTK_CONTAINER (vbox2), label);
# endif

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (hbox), vbox3);

  table = gtk_table_new (2, 9, FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 0 );
  row = -1;


  row++;
  label = gtk_label_new ( " Center Range (km) & Az " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_CTR_RNG_AZ, frame_num, 2 );

  row++;
  label = gtk_label_new ( " Center Lat & Lon " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_CTR_LAT_LON, frame_num, ENTRY_TWO_FLOATS );

  row++;
  label = gtk_label_new ( " Center Altitude (km) " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_CTR_ALT, frame_num, ENTRY_ONE_FLOAT );

# ifdef obsolete
  row++;
  label = gtk_label_new ( " Center Ref. Frame " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_CTR_REF_FRAME, frame_num, ENTRY_ONE_INT );
# endif


  row++;
  label = gtk_label_new ( " " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  row++;
  aa = vdata->orig_txt[VIEW_RADAR_LAT_LON_ALT]->str;
  vdata->data_widget[VIEW_RADAR_LAT_LON_ALT] = label =
    gtk_label_new ( aa );
  gtk_table_attach (GTK_TABLE (table), label, 0, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  row++;
  label = gtk_label_new ( " " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  row++;
  label = gtk_label_new ( " Landmark Range & Az " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_LMRK_RNG_AZ, frame_num, ENTRY_TWO_FLOATS );
  gtk_entry_set_editable (GTK_ENTRY (entry), FALSE ); 

  row++;
  label = gtk_label_new ( " Landmark Lat & Lon " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_LMRK_LAT_LON, frame_num, ENTRY_TWO_FLOATS );

  row++;
  label = gtk_label_new ( " Landmark Altitude " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_LMRK_ALT, frame_num, ENTRY_ONE_FLOAT );

# ifdef obsolete
  row++;
  label = gtk_label_new ( " Landmark Ref. Frame " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_view_entry_info( entry, VIEW_LMRK_REF_FRAME, frame_num, ENTRY_ONE_INT );
# endif

  gtk_container_foreach (GTK_CONTAINER (vbox),
			 sii_set_widget_frame_num,
			 (gpointer)frame_num );

  for (jj=0; jj < VIEW_MAX_WIDGETS; jj++ ) {

     if (vdata->entry_flag[jj]) {
	nn = frame_num * TASK_MODULO + jj;
	gtk_signal_connect (GTK_OBJECT(vdata->data_widget[jj])
			    ,"activate"
			    , (GtkSignalFunc) sii_view_menu_cb
			    , (gpointer)nn );
	gtk_signal_connect (GTK_OBJECT(vdata->data_widget[jj])
			    ,"paste_clipboard"
			    , (GtkSignalFunc) sii_view_entry_paste_cb
			    , (gpointer)nn );
     }
  }

  sii_update_view_widget (frame_num);

  /* --- Make everything visible --- */
  gtk_widget_show_all (window);
}

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

static GtkItemFactoryEntry view_menu_items[] = {
  { "/Options",                  NULL,             NULL,                 0,  "<Branch>" },
  { "/Options/Close",            NULL, sii_view_menu_cb,        VIEW_CLOSE,  NULL },
  { "/Options/sep3",             NULL,             NULL,                 0,  "<Separator>" },
  { "/Options/Az-Rng Overlay",   NULL, sii_view_menu_cb,        VIEW_AZRNG,  "<RadioItem>" },
  { "/Options/NO Az-Rng",        NULL, sii_view_menu_cb,     VIEW_NO_AZRNG,  "/Options/Az-Rng Overlay" },
  { "/Options/Azimuth Only",     NULL, sii_view_menu_cb,      VIEW_AZ_ONLY,  "/Options/Az-Rng Overlay" },
  { "/Options/Range Only",       NULL, sii_view_menu_cb,     VIEW_RNG_ONLY,  "/Options/Az-Rng Overlay" },
  { "/Options/sep3",             NULL,             NULL,                 0,  "<Separator>" },
  { "/Options/Az-Rng Labels",    NULL, sii_view_menu_cb, VIEW_AZRNG_LABELS,  "<CheckItem>" },
  { "/Options/sep3",             NULL,             NULL,                 0,  "<Separator>" },
  
  { "/Options/X-Y Tics Marks",   NULL, sii_view_menu_cb,       VIEW_XY_TICS,  "<RadioItem>" },
  { "/Options/NO X-Y Tics",      NULL, sii_view_menu_cb,    VIEW_NO_XY_TICS,  "/Options/X-Y Tics Marks" },
  { "/Options/sep3",             NULL,             NULL,                  0,  "<Separator>" },
  { "/Options/X-Y Tic Labels",   NULL, sii_view_menu_cb, VIEW_XY_TIC_LABELS,  "<CheckItem>" },
  { "/Options/sep3",             NULL,             NULL,                  0,  "<Separator>" },

  { "/ViewLinks",                NULL,             NULL,                  0,  "<Branch>" },
  { "/ViewLinks/Set Links",      NULL, sii_view_menu_cb,                  0,  NULL },
  
  { "/Center",                   NULL,             NULL,                  0,  "<Branch>" },
  { "/Center/Center on",         NULL,             NULL,                  0,  "<Title>" },
  { "/Center/Last click",        NULL, sii_view_menu_cb,      CENTER_ON_ONE,  NULL },
  { "/Center/Last 2 clicks",     NULL, sii_view_menu_cb,      CENTER_ON_TWO,  NULL },
  { "/Center/Last 4 clicks",     NULL, sii_view_menu_cb,     CENTER_ON_FOUR,  NULL },
  { "/Center/Local Radar",       NULL, sii_view_menu_cb,    CENTER_ON_RADAR,  NULL },
  { "/Center/sep3",              NULL,             NULL,                  0,  "<Separator>" },
  { "/Center/Based on",          NULL,             NULL,                  0,  "<Title>" },
  { "/Center/Local Radar Only",  NULL, sii_view_menu_cb,    CENTER_NO_LINKS,  "<RadioItem>" },
  { "/Center/Fixed Lat Lon Alt", NULL, sii_view_menu_cb,       CENTER_FIXED,  "/Center/Local Radar Only" },
  { "/Center/On Ref. Frame",     NULL, sii_view_menu_cb,       CENTER_OTHER,  "/Center/Local Radar Only" },
  { "/Center/sep3",              NULL,             NULL,                  0,  "<Separator>" },
  { "/Center/Links",             NULL, sii_view_menu_cb,       CENTER_LINKS,  NULL },
       
  { "/Landmark",                 NULL,             NULL,                  0,  "<Branch>" },
  { "/Landmark/Based on",        NULL,             NULL,                  0,  "<Title>" },
  { "/Landmark/Local Radar Only", NULL, sii_view_menu_cb, LANDMARK_NO_LINKS,  "<RadioItem>" },
  { "/Landmark/Fxd Lat Lon Alt", NULL, sii_view_menu_cb,     LANDMARK_FIXED,  "/Landmark/Local Radar Only" },
  { "/Landmark/On Ref. Frame",   NULL, sii_view_menu_cb,     LANDMARK_OTHER,  "/Landmark/Local Radar Only" },
  { "/Landmark/sep3",            NULL,             NULL,                  0,  "<Separator>" },
  { "/Landmark/Links",           NULL, sii_view_menu_cb,     LANDMARK_LINKS,  NULL },

  { "/TimeSeries",               NULL,             NULL,                  0,  "<Branch>" },
  { "/TimeSeries/TimeSeries Mode", NULL, sii_view_menu_cb,     VIEW_TS_MODE,  "<CheckItem>" },
  { "/TimeSeries/sep4",          NULL,             NULL,                  0,  "<Separator>" },
  { "/TimeSeries/Plot",          NULL,             NULL,                  0,  "<Title>" },
  { "/TimeSeries/Left-Right",    NULL, sii_view_menu_cb,         VIEW_TS_LR,  "<RadioItem>" },
  { "/TimeSeries/Right-Left",    NULL, sii_view_menu_cb,         VIEW_TS_RL,  "/TimeSeries/Left-Right" },
  { "/TimeSeries/sep3",          NULL,             NULL,                  0,  "<Separator>" },
  { "/TimeSeries/Relative to",   NULL,             NULL,                  0,  "<Title>" },
  { "/TimeSeries/Radar",         NULL, sii_view_menu_cb,      VIEW_TS_RADAR,  "<RadioItem>" },
  { "/TimeSeries/MSL",           NULL, sii_view_menu_cb,        VIEW_TS_MSL,  "/TimeSeries/Radar" },
  { "/TimeSeries/sep3",          NULL,             NULL,                  0,  "<Separator>" },
  { "/TimeSeries/Pointing",      NULL,             NULL,                  0,  "<Title>" },
  { "/TimeSeries/Up",            NULL, sii_view_menu_cb,         VIEW_TS_UP,  "<RadioItem>" },
  { "/TimeSeries/Down",          NULL, sii_view_menu_cb,       VIEW_TS_DOWN,  "/TimeSeries/Up" },
  { "/TimeSeries/Automanic",     NULL, sii_view_menu_cb,       VIEW_TS_AUTO,  "/TimeSeries/Up" },
  { "/TimeSeries/sep3",          NULL,             NULL,                  0,  "<Separator>" },

  { "/ ",                        NULL,             NULL,                  0,  "<Title>" },

  { "/Replot",                   NULL,             NULL,                  0,  "<Branch>" },
  { "/Replot/This Frame",        NULL, sii_view_menu_cb,   VIEW_REPLOT_THIS,  NULL },
  { "/Replot/Linked Frames",     NULL, sii_view_menu_cb,  VIEW_REPLOT_LINKS,  NULL },
  { "/Replot/All Frames",        NULL, sii_view_menu_cb,    VIEW_REPLOT_ALL,  NULL },

  { "/Help",                     NULL,             NULL,                  0,  "<LastBranch>" },
  { "/Help/Overview",            NULL, sii_view_menu_cb,  VIEW_HLP_OVERVIEW,  NULL },
  { "/Help/Center",              NULL, sii_view_menu_cb,       VIEW_HLP_CTR,  NULL },
  { "/Help/Landmark",            NULL, sii_view_menu_cb,      VIEW_HLP_LMRK,  NULL },
  { "/Help/TimeSeries",          NULL, sii_view_menu_cb,        VIEW_HLP_TS,  NULL },

};

/* c---------------------------------------------------------------------- */

void sii_view_menubar2( GtkWidget  *window, GtkWidget **menubar
		       , guint frame_num)
{
   gint jj, nn, pdi, task, active, widget_id;
   guint radio_num = 0;
   GtkWidget *check_item, *mbar, *menuitem, *submenu, *next_item;

   radio_group = NULL;
   
   *menubar = mbar = gtk_menu_bar_new ();

   submenu = sii_submenu ( "File", mbar );
   
   widget_id = VIEW_CLOSE;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Close", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

# ifdef obsolete
   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
   radio_group = NULL;
   radio_num = 0;
# endif

   submenu = sii_submenu ( "Options", mbar );
   
   widget_id = VIEW_AZRNG;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Az-Rng Overlay", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_NO_AZRNG;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "NO Az-Rng", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_AZ_ONLY;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Azimuth Only", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_RNG_ONLY;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Range Only", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = VIEW_AZRNG_LABELS;	/* check item */
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Az-Rng Labels", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, 0
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = VIEW_MAGIC_RNG_LBLS;	/* check item */
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Magic Rng Lbls", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, 0
			       , &radio_group);


   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
   radio_group = NULL;
   radio_num = 0;
   
   widget_id = VIEW_XY_TICS;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "X-Y Tics Marks", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_X_TICS_ONLY;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "X Tics Only", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_Y_TICS_ONLY;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Y Tics Only", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_NO_XY_TICS;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "NO X-Y Tics", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = VIEW_XY_TIC_LABELS;	/* check item */
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "X-Y Tic Labels", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, 0
			       , &radio_group);

   submenu = sii_submenu ( "ViewLinks", mbar );

   widget_id = VIEW_LINKS;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Set Links", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   submenu = sii_submenu ( "Center", mbar );

   widget_id = VIEW_ZERO;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Center On", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );
   
   widget_id = CENTER_ON_ONE;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Last click", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );
   
   widget_id = CENTER_ON_TWO;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Last 2 clicks", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );
   
   widget_id = CENTER_ON_FOUR;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Last 4 clicks", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );
   
   widget_id = CENTER_ON_RADAR;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Local Radar", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );
   
   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   radio_group = NULL;
   radio_num = 0;

   widget_id = VIEW_ZERO;	/* Label */
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Location", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = CENTER_NO_LINKS;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Local Radar", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = CENTER_FIXED;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Fixed Lat Lon Alt", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = CENTER_OTHER;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Same as Frame 1", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = CENTER_LINKS;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Set Links", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );


   submenu = sii_submenu ( "Landmark", mbar );

   radio_group = NULL;
   radio_num = 0;

   widget_id = VIEW_ZERO;	/* Label */
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Location", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = LANDMARK_NO_LINKS;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Local Radar", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = LANDMARK_FIXED;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Fixed Lat Lon Alt", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = LANDMARK_OTHER;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Same as Frame 1", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = LANDMARK_LINKS;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Set Links", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );


   submenu = sii_submenu ( "TimeSeries", mbar );
   
   widget_id = VIEW_TS_MODE;	/* Check Item */
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "TimeSeries Mode", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, 0
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
   radio_group = NULL;
   radio_num = 0;

   widget_id = VIEW_ZERO;	/* Label */
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Plot", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = VIEW_TS_LR;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Left-Right", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_TS_RL;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Right-Left", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = VIEW_ZERO;	/* Label */
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Relative to", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );
   radio_group = NULL;
   radio_num = 0;
   
   widget_id = VIEW_TS_RADAR;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Radar", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_TS_MSL;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "MSL", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = VIEW_ZERO;	/* Label */
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Pointing", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );
   radio_group = NULL;
   radio_num = 0;
   
   widget_id = VIEW_TS_UP;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Up", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_TS_DOWN;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Down", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = VIEW_TS_AUTO;
   vdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Automatic", submenu, widget_id
			       , (GtkSignalFunc)sii_view_menu_cb, frame_num, ++radio_num
			       , &radio_group);


   submenu = sii_submenu ( "Replot", mbar );

   widget_id = VIEW_REPLOT_LINKS;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Linked Frames", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = VIEW_REPLOT_ALL;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "All Frames", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );


   submenu = sii_submenu ( "Help", mbar );

   widget_id = VIEW_HLP_OVERVIEW;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Overview", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = VIEW_HLP_OPTS;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Options", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = VIEW_HLP_LINKS;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "ViewLinks", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = VIEW_HLP_CTR;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Center", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = VIEW_HLP_LMRK;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Landmark", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );

   widget_id = VIEW_HLP_TS;
   vdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "TimeSeries", submenu, widget_id
		       , (GtkSignalFunc)sii_view_menu_cb, frame_num );



  gtk_widget_show_all (*menubar);

  check_item = vdata->data_widget[VIEW_AZRNG];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  vdata->rng_az_state = VIEW_AZRNG;

  check_item = vdata->data_widget[VIEW_AZRNG_LABELS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  check_item = vdata->data_widget[VIEW_MAGIC_RNG_LBLS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  check_item = vdata->data_widget[VIEW_NO_XY_TICS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  vdata->xy_tics_state = VIEW_NO_XY_TICS;

  check_item = vdata->data_widget[VIEW_XY_TIC_LABELS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  check_item = vdata->data_widget[CENTER_NO_LINKS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  vdata->ctr_state = CENTER_NO_LINKS;

  check_item = vdata->data_widget[LANDMARK_NO_LINKS];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  vdata->lmrk_state = CENTER_NO_LINKS;

  check_item = vdata->data_widget[VIEW_TS_LR];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  vdata->ts_lr_state = VIEW_TS_LR;

  check_item = vdata->data_widget[VIEW_TS_RADAR];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  vdata->ts_relative_state = VIEW_TS_RADAR;

  check_item = vdata->data_widget[VIEW_TS_UP];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  vdata->ts_point_state = VIEW_TS_UP;
}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */




