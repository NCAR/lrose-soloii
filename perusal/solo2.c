/* 	$Id$	 */

# define config_debug

# ifdef info_only
# endif

# include "soloii.h"
# include "sii_externals.h"
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
# include <errno.h>
# include <dirent.h>

# ifdef LITTLENDIAN
    int LittleEndian = 1;
# else
    int LittleEndian = 0;
# endif

/* c------------------------------------------------------------------------ */
				/* displayq stuff */
/* c------------------------------------------------------------------------ */

static int diag_flag=0;
static gint bup_factor = 1;

struct linked_frames {
    struct linked_frames *next;
    WW_PTR wwptr;
    WW_PTR lead_sweep;
};

struct sii_arc {
   int id;
   int num_intxns;
   double angle1;
   double angle2;
   int nxt_ndx;
   Dxyz xy[2];
   struct sii_arc *next;
};

typedef struct sii_arc SiiArc;

static SiiArc *sii_arcs = NULL;

static struct linked_frames *original_sweep_set[SOLO_MAX_WINDOWS];
static struct linked_frames *linked_sweep_set[SOLO_MAX_WINDOWS];
static struct linked_frames *linked_frames[2*SOLO_MAX_WINDOWS];
static int original_sweep_set_count;
static int original_sweep_count;
static int real_sweep_set_count, nex_sweep_count;
static double nex_tol=.2;
static gboolean debug_flag = FALSE;

# define HIGH_PRF 0
# define LOW_PRF 1
const gchar *set_cb_labeling_info (guint frame_num, gdouble *relative_locs);
void sii_plot_data (guint frame_num, guint plot_function);
void sii_xfer_images (int frame_num, GdkRectangle *area);
GdkColor *sii_boundary_color (guint frame_num, gint exposed);
const gchar *sii_param_palette_name (guint frame_num);

gboolean sii_view_tic_info (guint frame_num, gdouble *xinc, gdouble *yinc
			    , gboolean *xtics, gboolean *ytics
			    , gboolean *annot);

void sii_blow_up_expose_event (GtkWidget *frame, GdkEvent *event
			       , gpointer data );
void sii_blow_up_config_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
gint sii_blow_up_keyboard_event(GtkWidget *frame, GdkEvent *event
				, gpointer data );
void sii_blow_up_mouse_button_event (GtkWidget *frame, GdkEvent *event
				     , gpointer data );
gint sii_rng_ring_arcs (guint frame_num, gdouble range);

gboolean sii_lmrk_inside (guint frame_num);

gboolean sii_magic_rng_lbls (guint frame_num);

/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */

static int Solo_state=0;
static int Solo_Busy_Signal=NO;
static struct solo_perusal_info *solo_perusal_info=NULL;
static GString *gs = NULL;
static gboolean redraw_bnd = TRUE;
static struct solo_list_mgmt *tmp_slm = NULL;

				/* c...mark */
/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */

void sii_png_image_prep (char *dir)
{
  char *aa, *bb, fname[256], mne[16];
  WW_PTR wwptr = solo_return_wwptr(0);
  struct dd_general_info *dgi, *dd_window_dgi();

  dgi = dd_window_dgi(0);
  if (!dir)
    { dir = wwptr->sweep->directory_name; }
  slash_path (fname, dir);
  bb = fname + strlen (fname);
  dd_file_name("png", (long)wwptr->sweep->start_time
	       , wwptr->sweep->radar_name, 0, bb);
  bb += strlen (bb);
  sprintf (bb, "_%.1f_%s_%s", dgi->dds->swib->fixed_angle
	   , dd_scan_mode_mne(dgi->dds->radd->scan_mode, mne)
	   , wwptr->parameter->parameter_name
	   );
  sii_png_image (fname);
}

/* c------------------------------------------------------------------------ */

gboolean sii_reset_frame_sizes ()
{
  SiiFrameConfig *sfc = frame_configs[0];
  GtkWidget *widget;
  gint x, y, width, height, mm, nn, dw, dh;

  width = sfc->ncols * sii_table_widget_width;
  height = sfc->nrows * sii_table_widget_height;

  gdk_window_get_origin ( sfc->frame->window, &x, &y );
  dw = screen_width_px -x;
  dh = screen_height_px -y;
  if (width < dw && height < dh)
    { return TRUE; }
  if (width >= dw) {
    mm = dw/sfc->ncols;
    sii_table_widget_width = (mm/4) *4;
  }
  if (height >= dh) {
    nn = dh/sfc->nrows;
    sii_table_widget_height = (mm/4) *4;
  }
}

/* c------------------------------------------------------------------------ */

void sii_reset_reconfig_flags(int frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  /*
   * Record the frame size for reference later if
   * not a drag resize
   */
  if (!(sfc->reconfig_flag & FrameDragResize)) {
     sfc->x_drag_ref = sfc->width;
     sfc->y_drag_ref = sfc->height;
  }
  sfc->reconfig_count = 0;
  sfc->local_reconfig = 0;
  sfc->reconfig_flag = 0;
}

/* c------------------------------------------------------------------------ */

void sii_reset_reconfig_count(int frame_num)
{
  SiiFrameConfig *sfc;
  frame_configs[frame_num]->reconfig_count = 0;
}

/* c------------------------------------------------------------------------ */

guint sii_frame_sync_num (guint frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  return (sfc->sync_num);
}

/* c------------------------------------------------------------------------ */

guint sii_config_sync_num (guint frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  return (sfc->config_sync_num);
}

/* c------------------------------------------------------------------------ */

void sii_set_debug_flag(gint flag)
{
   debug_flag = (flag) ? TRUE : FALSE;
}

/* c------------------------------------------------------------------------ */

gboolean sii_debug_flag()
{
   return debug_flag;
# ifdef never
    if (sii_debug_flag()) {
    }
# endif
}
/* c------------------------------------------------------------------------ */

int d_inSector( double ang, double ang1, double ang2)
{
    /* assumes sector defined from ang1 clockwise to ang2 */

    if(ang1 > ang2)		// crosses 360.
	{ return(ang >= ang1 || ang < ang2); }

    return(ang >= ang1 && ang < ang2);
}

/* c------------------------------------------------------------------------ */

int se_dump_sfic_widget(struct swp_file_input_control *sfic, int frame_num)
{
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  double t1,t2;

  strcpy (sfic->directory_text, wwptr->sweep->directory_name);
  strcpy (sfic->directory, wwptr->sweep->directory_name);
  strcpy (sfic->radar_names_text, wwptr->sweep->radar_name);
  sfic->start_time = sfic->stop_time = wwptr->d_sweepfile_time_stamp;

  sii_edit_return_start_stop (frame_num,  &sfic->start_time, &sfic->stop_time);
  strcpy (sfic->version_text, "last");
  sfic->version = 99999;
  sfic->new_version_flag = NO;

 if (!se_edit_start_stop (frame_num, sfic))
    { return 0; }

  return 1;
}

/* c------------------------------------------------------------------------ */

void sii_check_colors ()
{
  GdkColor *gcolor;
  gcolor = (GdkColor *)g_tree_lookup( colors_tree, (gpointer)"midnightblue" );
}

/* c------------------------------------------------------------------------ */

int sii_return_frame_count ()
{ return sii_frame_count; }

/* c------------------------------------------------------------------------ */

struct solo_perusal_info *
solo_return_winfo_ptr();

void
sii_set_links_from_solo_struct (int frame_num, int links_id,
				     long *linked_windows);
void
sii_initialize_parameter (guint frame_num, gchar *name);

gchar *
sii_set_param_names_list (guint frame_num, GString *gs_list, guint count);

void sp_replot_all ();
int displayq (int click_frme, int command);

void print_fieldvalues(int frame, struct solo_list_mgmt  *slm);
char *solo_list_entry (struct solo_list_mgmt  *slm, int jj);
struct dd_general_info *dd_window_dgi(int ww, char *name);
int solo_worksheet();

void sii_set_geo_coords ( int frame_num, gdouble dx, gdouble dy);

void sp_data_click(int frme, double xpos, double ypos);

double sp_meters_per_pixel ();

void dd_clear_pisp (struct point_in_space *pisp);

const gchar *sii_return_swpfi_dir (guint frame_num);
const gchar *sii_return_swpfi_radar (guint frame_num);

const struct solo_list_mgmt *sii_return_swpfi_radar_list (guint frame_num);
const struct solo_list_mgmt *sii_return_swpfi_list (guint frame_num);
double sii_return_swpfi_time_stamp (guint frame_num);

DD_TIME dts, *d_unstamp_time();
double d_time_stamp(), dd_relative_time();
char *dts_print (DD_TIME *dts);

double relative_time (char *aa);
int dd_crack_datime(char *cc, int nn, DD_TIME *dts);

const GString *sii_slm_to_gs (struct solo_list_mgmt  *slm);

gboolean
sii_nab_command_set (int frame_num, const char *dir, const char *file);

gboolean
sii_nab_boundary (int frame_num, const char *dir, const char *file);

struct boundary_stuff *return_se_bnd_struct();
struct solo_click_info *clear_click_info();
struct sed_command_files *se_return_cmd_files_struct();
struct solo_frame_state_files *se_return_state_files_struct();
struct solo_edit_stuff *return_sed_stuff();
struct solo_list_mgmt *solo_malloc_list_mgmt (int nn);
double sii_nearest_file_time (int frame_num, double target_time);
const gchar *sii_return_swpfi_label (int frame_num);
void sii_solo_set_filter (int frame_num, int set_it);
gboolean sii_time_series (guint frame_num, gdouble *tstart, gdouble *tspan);

GdkColor *sii_grid_color (guint frame_num, gint exposed);
GdkColor *sii_background_color (guint frame_num, gint exposed);
GdkColor *sii_annotation_color (guint frame_num, gint exposed);
void sii_do_annotation (guint frame_num, gint exposed, gboolean blow_up);

/* c------------------------------------------------------------------------ */

gboolean sii_time_series (guint frame_num, gdouble *tstart, gdouble *tspan)
{
  int ii = SOLO_TIME_SERIES;
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  *tstart = wwptr->sweep->start_time;
  *tspan = wwptr->sweep->stop_time - wwptr->sweep->start_time;
  ii = wwptr->view->type_of_plot & SOLO_TIME_SERIES;
  return ii != 0;
}

/* c------------------------------------------------------------------------ */

void sii_solo_set_filter (int frame_num, int set_it)
{
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  wwptr->swpfi_filter_set = (set_it) ? frame_num +1 : 0;
}

/* c------------------------------------------------------------------------ */

int sii_split_file_comment (const char *filename, float *fxd, char *scan_mode)
{
  char str[256], *sptrs[32], *aa, *bb;
  int jj, nt, nn, vn = -1;

  strcpy (str, filename);
  nt = dd_tokenz (str, sptrs, "_");
  bb = sptrs[0] + strlen (sptrs[0]) -1;

  for (nn=0; sptrs[0] < bb; --bb) {
    if (*(bb-1) == '.') {
      if (++nn > 1)
	{ break; }		/* on the second '.' */
    }
  }

  nn = sscanf (bb, "%f", fxd);
  for (jj =1; jj < nt; jj++) {
    if (*sptrs[jj] == 'v')
      { break; }
  }
  if (jj < nt) {
    nn = sscanf (sptrs[jj]+1, "%d", &vn);
    strcpy (scan_mode, sptrs[jj-1]);
  }

  return vn;
}

/* c------------------------------------------------------------------------ */

const gchar *sii_return_swpfi_label (int frame_num)
{
   WW_PTR wwptr = solo_return_wwptr(frame_num);
   return wwptr->lead_sweep->show_file_info;
}
/* c------------------------------------------------------------------------ */

double sii_nearest_file_time (int frame_num, double target_time)
{
   WW_PTR wwptr;
   double d, d_mddir_file_info_v3();
   int nn, rn, vv;
   char str[256], str2[256];

    wwptr = solo_return_wwptr(frame_num);

    ddir_rescan_urgent(frame_num);
    nn = mddir_file_list_v3
      (frame_num, wwptr->lead_sweep->sweep->directory_name);
    rn = mddir_radar_num_v3(frame_num, wwptr->lead_sweep->sweep->radar_name);
    d = d_mddir_file_info_v3(frame_num, rn, target_time
			       , TIME_NEAREST, LATEST_VERSION, &vv
			       , str, str2);
   return d;
 }

/* c---------------------------------------------------------------------- */

const GString *sii_return_config_files (const char *dir)
{
   struct solo_edit_stuff *seds = return_sed_stuff();
   struct solo_list_mgmt *slm;
   char *aa = "wds";

  slm = seds->list_winfo_files;
   if(!slm) {
      slm = solo_malloc_list_mgmt(SE_MAX_STRING);
      seds->list_winfo_files = slm;
   }
  se_nab_files(dir, slm, aa, strlen(aa));
  solo_sort_strings(slm->list, slm->num_entries);

  return sii_slm_to_gs (slm);
}

/* c---------------------------------------------------------------------- */

gboolean
sii_nab_state_set (int frame_num, const char *dir, const char *filename)
{
   struct solo_frame_state_files *sfsf = se_return_state_files_struct();
   struct solo_click_info *sci = clear_click_info();

   strcpy (sfsf->directory_text, dir); 
   strcpy (sfsf->file_name_text, filename);
   sci->frame = frame_num;
   sci->which_list = FRAME_STATES_LIST;
   se_process_click(sci);
}

/* c---------------------------------------------------------------------- */

gboolean
sii_save_commands (int frame_num, const char *dir, const char *comment)
{
   struct sed_command_files *scf = se_return_cmd_files_struct();
   struct solo_click_info *sci = clear_click_info();

   strcpy (scf->directory_text, dir);
   strcpy (scf->comment_text, comment);
   sci->frame = frame_num;
   sci->which_widget_button = SAVE_SED_CMD_FILES;
   se_process_click(sci);
}

/* c---------------------------------------------------------------------- */

gboolean
sii_save_boundary (int frame_num, const char *dir, const char *comment)
{
   struct boundary_stuff *sebs = return_se_bnd_struct();
   struct solo_click_info *sci = clear_click_info();

   strcpy (sebs->directory_text, dir);
   strcpy (sebs->comment_text, comment);
   sci->frame = frame_num;
   sci->which_widget_button = SAVE_BOUNDARY;
   se_process_click(sci);
}

/* c---------------------------------------------------------------------- */

gboolean
sii_nab_command_set (int frame_num, const char *dir, const char *filename)
{
   struct sed_command_files *scf = se_return_cmd_files_struct();
   struct solo_click_info *sci = clear_click_info();

   strcpy (scf->directory_text, dir); 
   strcpy (scf->file_name_text, filename);
   sci->frame = frame_num;
   sci->which_list = SED_CMD_FILES_LIST;
   se_process_click(sci);
}

/* c---------------------------------------------------------------------- */

gboolean
sii_nab_boundary (int frame_num, const char *dir, const char *filename)
{
   struct boundary_stuff *sebs = return_se_bnd_struct();
   struct solo_click_info *sci = clear_click_info();

   strcpy (sebs->directory_text, dir);
   strcpy (sebs->file_name_text, filename);

   sci->frame = frame_num;
   sci->which_list = BND_FILES_LIST;
   se_process_click(sci);
}

/* c---------------------------------------------------------------------- */

double sii_dtime_from_string (char *tstr)
{
  double d, d_time_stamp ();
  int jj;

  jj = dd_crack_datime(tstr, strlen(tstr), &dts);
  if (jj) {
    return d_time_stamp (&dts);
  }
  return 0;
}

/* c---------------------------------------------------------------------- */

const char *sii_string_from_dtime (double dtime)
{
  static char str[32];

  dts.time_stamp = dtime;
  d_unstamp_time (&dts);
  strcpy (str, dts_print (&dts));
  return (const char *)str;
}

/* c---------------------------------------------------------------------- */

double sii_return_swpfi_time_stamp (guint frame_num)
{
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  return wwptr->lead_sweep->d_sweepfile_time_stamp;
}

/* c---------------------------------------------------------------------- */

const gchar *sii_return_swpfi_dir (guint frame_num)
{

  WW_PTR wwptr = solo_return_wwptr(frame_num);
  wwptr = wwptr->lead_sweep;
  return (const gchar *)wwptr->sweep->directory_name;
}

/* c---------------------------------------------------------------------- */

const gchar *sii_return_swpfi_radar (guint frame_num)
{
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  wwptr = wwptr->lead_sweep;
  return (const gchar *)wwptr->sweep->radar_name;
}

/* c---------------------------------------------------------------------- */

const struct solo_list_mgmt *sii_return_swpfi_radar_list (guint frame_num)
{
  struct solo_perusal_info *spi = solo_return_winfo_ptr();
  WW_PTR wwptr = solo_return_wwptr(frame_num);

  solo_gen_radar_list(wwptr->lead_sweep->window_num);
  return (spi->list_radars);
}

/* c---------------------------------------------------------------------- */

const struct solo_list_mgmt *sii_return_swpfi_list (guint frame_num)
{
  struct solo_perusal_info *spi = solo_return_winfo_ptr();
  WW_PTR wwptr = solo_return_wwptr(frame_num);

  solo_gen_swp_list(wwptr->lead_sweep->window_num);
  return (spi->list_sweeps);
}

/* c---------------------------------------------------------------------- */

const GString *sii_return_edit_cmd_files (guint frame_num, const char *dir)
{
  struct solo_perusal_info *spi = solo_return_winfo_ptr();
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  struct solo_list_mgmt *slm;
  char *aa = "sed";

  slm = spi->list_ed_info_files;
  se_nab_files(dir, slm, aa, strlen(aa));
  solo_sort_strings(slm->list, slm->num_entries);

  return sii_slm_to_gs (slm);
}

/* c---------------------------------------------------------------------- */

const GString *sii_return_edit_bnd_files (guint frame_num, const char *dir)
{
  struct solo_perusal_info *spi = solo_return_winfo_ptr();
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  struct solo_list_mgmt *slm;
  char *aa = "bnd";

  slm = spi->list_boundary_files;
  se_nab_files(dir, slm, aa, strlen(aa));
  solo_sort_strings(slm->list, slm->num_entries);

  return sii_slm_to_gs (slm);
}

/* c---------------------------------------------------------------------- */

const GString *sii_return_llb_bnd_files (guint frame_num, const char *dir)
{
  struct solo_perusal_info *spi = solo_return_winfo_ptr();
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  struct solo_list_mgmt *slm;
  char *aa = "llb";

  if (!tmp_slm)
    { tmp_slm = solo_malloc_list_mgmt (256); }

  slm = tmp_slm;
  se_nab_files(dir, slm, aa, strlen(aa));
  solo_sort_strings(slm->list, slm->num_entries);

  return sii_slm_to_gs (slm);
}

/* c------------------------------------------------------------------------ */

void sii_set_geo_coords ( int frame_num, gdouble dx, gdouble dy)
{
  struct point_in_space radarx, radar, pisp, center;
  gdouble lat0, lon0, alt0, lat1, lon1, alt1;
  double R_earth, zero = 0, xx, yy, zz, scale, theta;
  double xxCtr, yyCtr;
  gint xpos, ypos, zpos;
  WW_PTR wwptrc, wwptrx = solo_return_wwptr(frame_num);
  SiiPoint *pt, *prev_pt;
  gchar mess[256];

  pt = (SiiPoint *)sii_click_que->data;
  scale = M_TO_KM (sp_meters_per_pixel())/wwptrx->view->magnification;

  theta = RADIANS (CART_ANGLE (wwptrx->center_of_view->rotation_angle));
  xxCtr = wwptrx->center_of_view->range * cos (theta);
  yyCtr = wwptrx->center_of_view->range * sin (theta);
  xx = xxCtr + scale * dx;
  yy = yyCtr + scale * dy;

  pt->xx = xx;
  pt->yy = yy;
  sp_data_click(frame_num, xx, yy);

  if (sii_electric_centering && pt->button_mask & GDK_BUTTON2_MASK) {
    /* clicked to recenter plot!
     */
     sii_center_on_clicks (1);
  }
}
/* c------------------------------------------------------------------------ */

double sii_range_in_pixels (int frame_num, double dist_km)
{
   WW_PTR wwptr = solo_return_wwptr (frame_num);
   double d;
				/* get to pixels per km */
   d = wwptr->view->magnification/(sp_meters_per_pixel() * .001);
   return d * dist_km;
}

/* c------------------------------------------------------------------------ */

void sp_cell_range_info (int frame_num, gfloat *r0, gfloat *gs, gint *ncells)
{
   struct dd_general_info *dgi;
   WW_PTR wwptr = solo_return_wwptr (frame_num);
   int ww;
   
   ww = wwptr->lead_sweep->window_num;
   ww = 0;
   dgi = dd_window_dgi(ww, "");
   *ncells = dgi->dds->celvc->number_cells;
   *r0 = dgi->dds->celvc->dist_cells[0] * .001;
   *gs = (dgi->dds->celvc->dist_cells[1]
	  - dgi->dds->celvc->dist_cells[0]) *.001;
}

/* c------------------------------------------------------------------------ */

gfloat sp_nyquist_vel (int frame_num)
{
   struct dd_general_info *dgi;
   WW_PTR wwptr = solo_return_wwptr (frame_num);
   int ww;
   
   ww = wwptr->lead_sweep->window_num;
   dgi = dd_window_dgi(ww, "");
   return dgi->dds->radd->eff_unamb_vel;
}

/* c------------------------------------------------------------------------ */

const GString *sii_slm_to_gs (struct solo_list_mgmt  *slm)
{
  int jj;
  gchar *aa, str[256];

  if (!gs)
    { gs = g_string_new (""); }
  g_string_truncate (gs, 0);

  for (jj=0; jj < slm->num_entries; jj++) {
    aa = solo_list_entry (slm, jj);
    strcpy (str, aa);
    g_strchomp (str);
    g_string_append (gs, str);
    g_string_append (gs, "\n");
  }
  return gs;
}
/* c------------------------------------------------------------------------ */

void print_fieldvalues(int frame_num, struct solo_list_mgmt  *slm)
{
  int nn;
  sii_slm_to_gs (slm);
  /* send field values minus the final linefeed */
  nn = gs->len -1;
  g_string_truncate (gs, nn);
  sii_set_click_data_text (frame_num, gs);

}

/* c------------------------------------------------------------------------ */
void popup_message (struct solo_list_mgmt *slm)
{
   sii_slm_to_gs (slm);
   sii_message (gs->str);
}
/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */

void yes_exit() {
  gtk_main_quit ();
}

/* c------------------------------------------------------------------------ */

void sii_set_boundary_pt_count (int frame_num, int num_points)
{
  frame_configs[0]->points_in_bnd = num_points;
  return;
}

/* c------------------------------------------------------------------------ */
void se_clear_segments(int frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  GdkRectangle area;

  area.x = 0;
  area.y = 0;
  area.width = sfc->width;
  area.height = sfc->height;
  sii_xfer_images (frame_num, &area);
}

/* c------------------------------------------------------------------------ */

int sii_radar_coords_to_frame_coords (int frame_num
				      , gdouble xx_km, gdouble yy_km
				      , gint *x, gint *y)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  gdouble ppk, xx, yy;

  ppk = wwptr->view->magnification/M_TO_KM (sp_meters_per_pixel());
  xx = (xx_km -sfc->ulc_radar.xx) * ppk;
  yy = -(yy_km -sfc->ulc_radar.yy) * ppk;
  *x = (xx < 0) ? xx -.5 : xx +.5;
  *y = (yy < 0) ? yy -.5 : yy +.5;
}

/* c------------------------------------------------------------------------ */

gdouble sii_tic1 (gdouble tic, gdouble ticinc)
{
  gdouble d = fabs (tic), tic1, rem;

  rem = fmod (d, ticinc);
  tic1 = (tic < 0) ? tic + rem : tic + ticinc -rem;
  return tic1;
}

/* c------------------------------------------------------------------------ */

int sii_lmrk_coords_to_frame_coords (int frame_num
				      , gdouble xx_km, gdouble yy_km
				      , gint *x, gint *y)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  gdouble ppk, xx, yy;

  ppk = wwptr->view->magnification/M_TO_KM (sp_meters_per_pixel());
  xx = (xx_km -sfc->corner[0].xx) * ppk;
  yy = -(yy_km -sfc->corner[0].yy) * ppk;
  *x = (xx < 0) ? xx -.5 : xx +.5;
  *y = (yy < 0) ? yy -.5 : yy +.5;
}

/* c------------------------------------------------------------------------ */

void se_erase_segments (int frame_num, int num_points
			, struct bnd_point_mgmt *bpm)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  GdkRectangle area;
  gint x1, y1, x2, y2, x, y;
  

  if (num_points < 2 || frame_num >= sii_return_frame_count()) {
    return;
  }
  if (num_points > 2) {
    se_clear_segments (frame_num);
  }
    sii_radar_coords_to_frame_coords (frame_num
				      , (gdouble)M_TO_KM (bpm->_x)
				      , (gdouble)M_TO_KM (bpm->_y)
				      , &x1, &y1);
    sii_radar_coords_to_frame_coords (frame_num
				      , (gdouble)M_TO_KM (bpm->last->_x)
				      , (gdouble)M_TO_KM (bpm->last->_y)
				      , &x2, &y2);
  area.x = (x1 < x2) ? x1 : x2;
  area.y = (y1 < y2) ? y1 : y2;
  x = (x1 < x2) ? x2 : x1;
  y = (y1 < y2) ? y2 : y1;
  area.width = x -area.x +1;
  area.height = y -area.y +1;
  redraw_bnd = FALSE;
  sii_xfer_images (frame_num, &area);
  redraw_bnd = TRUE;
}

/* c------------------------------------------------------------------------ */

void se_draw_segments (int frame_num, int num_points,
		       struct bnd_point_mgmt *bpm)
{
  /* Draw the segments of the boundary
   * what comes in are endpoints relative to the local radar
   * in meters.
   * the list is a stack and is drawn from bottom to top
   */
  SiiFrameConfig *sfc = frame_configs[frame_num];
  SiiFrameConfig *sfc0 = frame_configs[0];
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  gint x1, y1, x2, y2, jj, kk, nn, xlate;
  GtkWidget *frame;
  GdkColor *gcolor;
  GdkGC *gc;
  GdkRectangle rect, clip;
  SiiPoint *pt;
  GdkPoint p1, p2;
  gdouble ppk, xx, yy;
  gboolean in1, in2;
  gchar str[256];

  if (frame_num >= sii_return_frame_count() || num_points < 2)
    { return; }

  gcolor = sii_boundary_color (frame_num, 1);
  gc = sfc->gc;
  gdk_gc_set_foreground (sfc->gc, gcolor );
  ppk = wwptr->view->magnification/M_TO_KM (sp_meters_per_pixel());
  pt = &sfc->ulc_radar;	  /* contains coordinates of the ulc per the radar */
  xlate = sfc->height -1;

  for(jj = 0; jj < num_points-1; jj++, bpm = bpm->last){
    /* convert to screen coordinates */
    
    if((wwptr->view->type_of_plot & SOLO_TIME_SERIES)) {
      /* already in screen coordinates */
      x1 = bpm->_x;
      x2 = bpm->last->_x;
      y1 = bpm->_y;
      y2 = bpm->last->_y;
    }
    else {
      sii_radar_coords_to_frame_coords (frame_num
					, (gdouble)M_TO_KM (bpm->_x)
					, (gdouble)M_TO_KM (bpm->_y)
					, &x1, &y1);
      sii_radar_coords_to_frame_coords (frame_num
					, (gdouble)M_TO_KM (bpm->last->_x)
					, (gdouble)M_TO_KM (bpm->last->_y)
					, &x2, &y2);
    }
# ifdef obsolete
    g_string_sprintf (gs_complaints,
		       "bnd:(%.4f,%.4f)(%d,%d) - (%.4f,%.4f)(%d,%d)"
		       , (float)bpm->_x, (float)bpm->_y, x1, y1
		       , (float)bpm->last->_x, (float)bpm->last->_y, x2, y2
		       );
    g_message (gs_complaints->str);
# endif

    in1 = x1 >= 0 && x1 < sfc->width && y1 >= 0 && y1 < sfc->height;
    in2 = x2 >= 0 && x2 < sfc->width && y2 >= 0 && y2 < sfc->height;

    if (in1 || in2) {
      
      /* define the corners of a rectangle containing the boundary */
      if (jj == 0) {
	sfc->ulc_bnd.x = sfc->lrc_bnd.x = x1;
	sfc->ulc_bnd.y = sfc->lrc_bnd.y = y1;
      }
      if (x2 < sfc->ulc_bnd.x)
	{ sfc->ulc_bnd.x = x2; }
      if (x2 > sfc->lrc_bnd.x)
	{ sfc->lrc_bnd.x = x2; }
      if (y2 < sfc->ulc_bnd.y)
	{ sfc->ulc_bnd.y = y2; }
      if (y2 > sfc->lrc_bnd.y)
	{ sfc->lrc_bnd.y = y2; }
      
      gdk_draw_line (sfc->frame->window, gc, x1, y1, x2, y2);
      
      if (sfc->blow_up) {
	gdk_draw_line (sfc0->blow_up_frame->window, gc
		       , x1*2, y1*2, x2*2, y2*2);
      }
    }
  }

  return;

# ifdef fyi
  gdk_gc_set_function (gc, GDK_COPY);
  gdk_gc_set_function (gc, GDK_XOR);
  gdk_gc_set_function (gc, GDK_INVERT);
# endif

}

/* c------------------------------------------------------------------------ */

void sii_really_xfer_images
(int frame_num, GdkRectangle *area, gboolean blow_up);

/* c------------------------------------------------------------------------ */

gboolean sii_blow_up (guint frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  SiiFrameConfig *sfc0 = frame_configs[0];
  GtkWidget *frame, *window, *vbox;
  static GString *gs = NULL;
  gint jj;
  guchar *img;
  gint event_flags = GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK
    | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK
      | GDK_FOCUS_CHANGE_MASK;
  
  if (!uniform_frame_shape)
    { return FALSE; }

  if (frame_num >= sii_frame_count)
    { return FALSE; }

  if (2 * sfc->width >= screen_width_px ||
     2 * sfc->height >= screen_height_px -32)
    { return FALSE; }
    
  if (sfc->blow_up) {		/* consecutive hits on this frame. Hide it! */
    if (sfc0->blow_up_frame)
      { gtk_widget_hide (sfc0->blow_up_window); }
    sfc->blow_up = FALSE;
    sfc0->blow_up_frame_num = 0;
    return FALSE;
  }

  if (sfc0->big_width != sfc->width *2 ||
      sfc0->big_height != sfc->height *2)
    {
      if (!gs)
	{ gs = g_string_new (""); }

      if (sfc0->blow_up_window) {
	g_free (sfc0->big_image->data);
	gtk_widget_destroy(sfc0->blow_up_window);
      }
      else {
	sfc0->big_image = (SiiImage *)g_malloc0 (sizeof (SiiImage));
      }
      sfc0->big_reconfig_count = 0;
      sfc0->big_width = sfc->width *2;
      sfc0->big_height = sfc->height *2;
      sfc0->big_image_size = sfc0->big_width * sfc0->big_height * 3;
      sfc0->big_image->data = (guchar *)g_malloc0 (sfc0->big_image_size);

      sfc0->blow_up_window = window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
				/*
      gtk_widget_set_uposition (window, 0, 0);
				 */
      vbox = gtk_vbox_new (FALSE,0);
      gtk_container_add (GTK_CONTAINER (window), vbox);

      sfc0->blow_up_frame = frame = gtk_drawing_area_new ();

      gtk_widget_set_events (frame, event_flags );
      gtk_container_add (GTK_CONTAINER(vbox), frame);
      gtk_drawing_area_size (GTK_DRAWING_AREA (frame)
			     , sfc0->big_width
			     , sfc0->big_height );
      
      gtk_signal_connect (GTK_OBJECT (frame), "expose_event",
			  (GtkSignalFunc) sii_blow_up_expose_event
			  , (gpointer)frame_num);
      gtk_signal_connect (GTK_OBJECT(frame),"configure_event",
			  (GtkSignalFunc) sii_blow_up_config_event
			  , (gpointer)frame_num);
      gtk_signal_connect (GTK_OBJECT(sfc0->blow_up_window),"key_press_event",
			  (GtkSignalFunc) sii_blow_up_keyboard_event
			  , (gpointer)frame_num);
      gtk_signal_connect (GTK_OBJECT(frame),"button_press_event",
			  (GtkSignalFunc) sii_blow_up_mouse_button_event
			  , (gpointer)frame_num);
   }

  g_string_sprintf (gs, "Frame %d", frame_num+1);
  gtk_window_set_title (GTK_WINDOW (sfc0->blow_up_window), gs->str);

  frame = sfc0->blow_up_frame;
  gtk_object_set_data (GTK_OBJECT(frame),
		       "frame_num",
		       (gpointer)frame_num);
  gtk_object_set_data (GTK_OBJECT(sfc0->blow_up_window),
		       "frame_num",
		       (gpointer)frame_num);

  gtk_widget_show_all (sfc0->blow_up_window);
  sii_double_colorize_image (frame_num);
  sfc->big_sync_num = sfc->sync_num;

  for (jj=0; jj < maxFrames; frame_configs[jj++]->blow_up = FALSE);
  sfc->blow_up = TRUE;
  sfc0->blow_up_frame_num = frame_num+1;

  sii_really_xfer_images (frame_num, NULL, TRUE);
  return TRUE;
}

/* c------------------------------------------------------------------------ */

void sii_xfer_images (int frame_num, GdkRectangle *area)
{
  gboolean blow_up = FALSE;
  SiiFrameConfig *sfc = frame_configs[frame_num];

  sii_really_xfer_images (frame_num, area, blow_up);

  if (sfc->blow_up) {
    if (sfc->big_sync_num != sfc->sync_num) {
      sfc->blow_up = FALSE;
      sii_blow_up (frame_num);
    }
    else
      { sii_really_xfer_images (frame_num, area, TRUE); }
  }
}

/* c------------------------------------------------------------------------ */

void sii_really_xfer_images (int frame_num, GdkRectangle *area
			     , gboolean blow_up)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  SiiFrameConfig *sfc0 = frame_configs[0];
  WW_PTR wwptrx, wwptr = solo_return_wwptr(frame_num);
  gint x, y, lbearing, rbearing, width, height, ascent, descent, font_height;
  gint stride, xclip, yclip, num_arcs;
  GtkWidget *frame;
  gint jj, kk, mark = 0, x1,y1,x2,y2, nl, exposed, nxtics, nytics;
  GdkGC *gc;
  GdkFont *font;
  GdkColor *gcolor;
  guint ix=0, iy=0;
  gint8 dash_list[] = {5,3};
  gchar *aa, annot[16], str[512], *sptrs[32], *fmt;
  guchar *img = NULL;
  const gchar *cc;
  sii_table_parameters *stp;
  GdkRectangle *rect, clip;
  gdouble d, xctr, yctr, xlmrk, ylmrk, rng, rri, xx, yy, zz, angle, theta;
  gdouble lat, lon, alt, clat, clon, calt, min, max;
  gdouble xx2ctr, yy2ctr, zz2ctr, ts_start, ts_span, dtime, magic_angle;
  gdouble cb_locs[16];
  gdouble ppm, ppk;			/* pixels per meter */
  double xtic, ytic, ytica, xtic1, ytic1, xtic2, ytic2, xtici, ytici, tval;
  struct point_in_space pisp, landmark, center, radar;
  double dd_earthr (double lat);
  SiiPoint *pt, *ptlr, *pt_max;
  gint ticlen = 13;
  gboolean xyannot, xtics, ytics, time_series, inside;
  SiiArc *arc;
  gint iangle, iarc_len, loop_count;
  struct dd_general_info *dgi, *dd_window_dgi();

  /*
   * transfer the image
   * label the plot 
   * label the color bar
   * draw boundaries
   * draw border
   */

  if (frame_num >= sii_return_frame_count ())
    { return; }

  time_series = sii_time_series (frame_num, &ts_start, &ts_span);
  frame = sfc->frame;
  bup_factor = 1;
  exposed = (area) ? 1 : 0;
  font = med_pro_font;
  aa = title_tmplt;
  gdk_text_extents (font, aa, strlen (aa)
		    , &lbearing, &rbearing, &width, &ascent, &descent);

  if (width > sfc->width)
    { font = small_pro_font; }
  sfc->font = font;
  sfc->font_height = 
    font_height = font->ascent + font->descent+3;

  if (sfc->frame && sfc->image && sfc->width * sfc->height > 99) {
    img = (guchar *)sfc->image->data;
    img += sfc->width * sfc->height;
  }

  if (img && !sfc->gc)
    { sfc->gc = gdk_gc_new (sfc->frame->window); }
  gc = sfc->gc;

				/* c...really */
  if (blow_up) {

    sfc0->big_font = font = big_pro_font;
    sfc0->big_font_height = font_height = font->ascent + font->descent+2;
    bup_factor = 2;
    frame = sfc0->blow_up_frame;

    if (!sfc0->big_gc)
      { sfc0->big_gc = gdk_gc_new (frame->window); }
    gc = frame_configs[0]->big_gc;

    if (img = sfc0->big_image->data) {
      gdk_draw_rgb_image (frame->window
			  , sfc->frame->style->white_gc
			  , 0, 0, sfc0->big_width, sfc0->big_height
			  , GDK_RGB_DITHER_NONE
			  , img
			  , sfc0->big_width * 3);
    }
  }

  else if (!area && img) {
    set_color_bar (frame_num, font_height);
    sii_colorize_image (frame_num);
    gdk_draw_rgb_image (sfc->frame->window,
			sfc->frame->style->white_gc,
			0, 0, sfc->width, sfc->height
			, GDK_RGB_DITHER_NONE
			, img, sfc->width * 3);
  }

  else if (img && sfc->colorize_count) {

    stride = sfc->width * 3;
    if (area) {
       img += area->y * stride;
       img += area->x * 3;
       gdk_draw_rgb_image (sfc->frame->window
			  , sfc->frame->style->white_gc
			  , area->x, area->y
			  , area->width, area->height
			  , GDK_RGB_DITHER_NONE
			  , img, stride);
    }
    else {
      gdk_draw_rgb_image (sfc->frame->window,
			  sfc->frame->style->white_gc,
			  0, 0, sfc->width, sfc->height
			  , GDK_RGB_DITHER_NONE
			  , img, stride);
    }
  }

  

  /*
   *
   * To draw range rings, find the location of the landmark
   * and turn it into screen coordinates
   *
   * Since the range rings are slant range, when the landmark is the
   * radar we have to factor in the fixed angle.
   *
   * When the landmark is not the radar, we project the range rings
   * onto the surface.
   *
   */
  if (wwptr->landmark_info->landmark_options == SOLO_LINKED_POSITIONING) {
    wwptrx =  solo_return_wwptr(wwptr->landmark_info->reference_frame-1);
    dd_copy_pisp(wwptrx->landmark, &landmark);
  }
  else {
    dd_copy_pisp(wwptr->landmark, &landmark);
  }
  dd_copy_pisp(wwptr->center_of_view, &center);
  dd_copy_pisp(wwptr->radar_location, &radar);

  /* calculate (x,y,z) of center relative to the landmark
   * the values are placed in (x,y,z) of the landmark
   */
  dd_latlon_relative(&center, &landmark);

  if (landmark.latitude != radar.latitude ||
      landmark.longitude != radar.longitude ) {
    xx2ctr = landmark.x;
    yy2ctr = landmark.y;
  }
  else {
    theta = RADIANS(CART_ANGLE(center.rotation_angle));
    xx2ctr = center.range * cos (theta);
    yy2ctr = center.range * sin (theta);
  }
  
  xctr = sfc->width *.5;
  yctr = sfc->height *.5;
  ppm = wwptr->view->magnification/sp_meters_per_pixel();
  ppk = ppm * 1000;		/* pixel per km */

  if (time_series) {
    xx2ctr = xctr/ppk;
    yy2ctr = wwptr->center_of_view->range;
  }
  else if (wwptr->lead_sweep->scan_mode == RHI || wwptr->scan_mode == AIR) {
     theta = RADIANS (CART_ANGLE (wwptr->center_of_view->rotation_angle));
     xx2ctr = wwptr->center_of_view->range * cos (theta);
     yy2ctr = wwptr->center_of_view->range * sin (theta);
  }

  pt = sfc->corner;
  /* set the corners relative to the landmark (ul, ur, lr, ll)
   */
  pt->dtime = ts_start;
  pt->x = pt->y = 0;
  pt->xx = xx2ctr - xctr/ppk;
  pt->yy = yy2ctr + yctr/ppk;
  pt->zz = 0;

  /* set the upper left corner lat/lon to facilitate overlays*/
  lat = center.latitude;
  lon = center.longitude;
  alt = center.altitude;


  pt++;				/* next corner (upper right) */
  pt->dtime = ts_start +ts_span;
  pt->x = sfc->width -1;
  pt->y = 0;
  pt->xx = xx2ctr + xctr/ppk;
  pt->yy = yy2ctr + yctr/ppk;
  xx = xctr/ppk;
  yy = yctr/ppk;
  zz = 0;
  loop_xy2ll_v3( &pt->lat, &pt->lon, &pt->alt, &xx, &yy, &zz
		 , lat, lon, alt , dd_earthr(lat), 1 );

  pt++;				/* next corner (lower right) */
  pt->dtime = ts_start +ts_span;
  pt->x = sfc->width -1;
  pt->y = sfc->height -1;
  pt->xx = xx2ctr + xctr/ppk;
  pt->yy = yy2ctr - yctr/ppk;
  xx = xctr/ppk;
  yy = -yctr/ppk;
  zz = 0;
  loop_xy2ll_v3( &pt->lat, &pt->lon, &pt->alt, &xx, &yy, &zz
		 , lat, lon, alt , dd_earthr(lat), 1 );

  pt++;				/* next corner (lower left) */
  pt->dtime = ts_start;
  pt->x = 0;
  pt->y = sfc->height -1;
  pt->xx = xx2ctr - xctr/ppk;
  pt->yy = yy2ctr - yctr/ppk;
  xx = -xctr/ppk;
  yy = -yctr/ppk;
  zz = 0;
  loop_xy2ll_v3( &pt->lat, &pt->lon, &pt->alt, &xx, &yy, &zz
		 , lat, lon, alt , dd_earthr(lat), 1 );

  min = 2.e22;
  max = 0;
  pt = frame_configs[frame_num]->corner;

  /* find the min and max range;
   */
  for (jj=0; jj < 4; jj++, pt++) {
    pt->rng_km = sqrt (pt->xx*pt->xx + pt->yy*pt->yy);
    if (pt->rng_km > max)
      { max = pt->rng_km; pt_max = pt; }
    if (pt->rng_km < min)
      { min = pt->rng_km; }
  }
  theta = atan2 (pt_max->yy, pt_max->xx);
  magic_angle = FMOD360 (CART_ANGLE (DEGREES (theta)));

  if (max > 500) {
    sprintf (str, "sii_xfer_images: Max: %.3f > 500", max);
    g_message (str);
    max = 500;
  }

  gcolor = sii_grid_color(frame_num, exposed);
  if (!gcolor)
    { return; }

  sii_do_annotation (frame_num, exposed, blow_up);
  sii_get_clip_rectangle (frame_num, &clip, blow_up);

  gdk_gc_set_foreground (gc, gcolor );

  /* center crosshairs */

  if (sii_center_crosshairs) {
    x1 = x2 = xctr;
    y1 = yctr -7;
    y2 = yctr +7;
    gdk_draw_line( frame->window, gc, x1*bup_factor, y1*bup_factor
		   , x2*bup_factor, y2*bup_factor);
    y1 = y2 = yctr;
    x1 = xctr -7;
    x2 = xctr +7;
    gdk_draw_line( frame->window, gc, x1*bup_factor, y1*bup_factor
		 , x2*bup_factor, y2*bup_factor);
  }    
  gdk_gc_set_clip_rectangle (gc, &clip);
  gdk_gc_set_dashes (gc, 0, dash_list, sizeof (dash_list));
  gdk_gc_set_line_attributes (gc, 1, GDK_LINE_ON_OFF_DASH, 0, 0);

  angle = (wwptr->magic_rng_annot) ?
    magic_angle : wwptr->view->rng_annot_at_deg;

  theta = RADIANS (CART_ANGLE (angle));
  xlmrk = xctr - xx2ctr*ppk;
  ylmrk = yctr + yy2ctr*ppk;
  
  aa = annot;
  rng = wwptr->view->rng_ring_int_km;
  inside = sii_lmrk_inside (frame_num);

  max *= .9999;

  if (!sii_arcs) {
     sii_arcs = (SiiArc *)g_malloc0 (4 * sizeof (SiiArc));
     for (jj=0; jj < 4; jj++) {
	(sii_arcs+jj)->id = jj;
	(sii_arcs+jj)->next = sii_arcs +jj +1;
     }
     (sii_arcs+3)->next = sii_arcs; /* make it circular */
  }

  /* range rings */

  for (; !time_series && rng > 0 && rng < max;
       rng += wwptr->view->rng_ring_int_km) {

    xx = xlmrk - rng*ppk;
    yy = ylmrk - rng*ppk;
    x = (gint)((xx > 0) ? xx +.5 : xx -.5) * bup_factor;
    y = (gint)((yy > 0) ? yy +.5 : yy -.5) * bup_factor;
    width = height = (gint)(2 * rng*ppk +.5) * bup_factor;
# ifdef obsolete
    sprintf (str, "%.1f %d %d %d %.6f", rng, x, y, width, ppk);
    g_message (str);
# endif

    if (rng > wwptr->view->rng_ring_int_km) {
       /* look for partial circles
	* drawing full circles gets very slow at high zooms
	*/
       num_arcs = 0;
       num_arcs = sii_rng_ring_arcs (frame_num, rng);
       if (!inside && num_arcs == 0)
	 { continue; }
       arc = sii_arcs;
       loop_count = (num_arcs > 0) ? 4 : 1;
       iangle = 0; iarc_len = 360*64;

       for (; loop_count--; arc++) {
	  if (num_arcs && arc->angle1 >= 0) {
	     iangle = (int)(64 * arc->angle1);
	     d = arc->angle2 - arc->angle1;
	     if (d < 0 )
	       { d += 360; }
	     iarc_len = (int)(64 * d);
	  }
	  else if (num_arcs)
	    { continue; }

	  gdk_draw_arc( frame->window
		       , gc, FALSE, x, y, width, height, iangle, iarc_len);
       }
    }
    if (angle < 0)
      { continue; }
    xx = xlmrk + rng*ppk * cos (theta);
    yy = ylmrk - rng*ppk * sin (theta);
    x = (gint)((xx > 0) ? xx +.5 : xx -.5) * bup_factor;
    y = (gint)((yy > 0) ? yy +.5 : yy -.5) * bup_factor;
    fmt = (wwptr->view->rng_ring_int_km < 3) ? "%.1f" : "%.0f";
    sprintf (aa, fmt, rng);
    gdk_text_extents (font, aa, strlen (aa)
		      , &lbearing, &rbearing, &width, &ascent, &descent);
    x = (angle > 180) ? x : x -width;
    y = (angle > 90 && angle < 270) ? y 
      : y +(ascent+descent);
    gdk_draw_text (frame->window
		   , font, gc
		   , x, y
		   , aa, strlen (aa));
  }
  dash_list[0] = 4; dash_list[1] = 5;
  gdk_gc_set_dashes (gc, 0, dash_list, sizeof (dash_list));

  rng = wwptr->view->az_annot_at_km;
  rri = wwptr->view->rng_ring_int_km;
  if (rri <= 0)
    { rri = 5; }

  for (angle = 0; !time_series && wwptr->view->az_line_int_deg > 0 &&
	 angle < 360; angle += wwptr->view->az_line_int_deg) {
    theta = RADIANS (CART_ANGLE (angle));

    xx = xlmrk + cos (theta) * rri * ppk;
    yy = ylmrk - sin (theta) * rri * ppk;
    x1 = (gint)((xx > 0) ? xx +.5 : xx -.5) * bup_factor;
    y1 = (gint)((yy > 0) ? yy +.5 : yy -.5) * bup_factor;

    xx = xlmrk + cos (theta) * max * ppk;
    yy = ylmrk - sin (theta) * max * ppk;
    x2 = (gint)((xx > 0) ? xx +.5 : xx -.5) * bup_factor;
    y2 = (gint)((yy > 0) ? yy +.5 : yy -.5) * bup_factor;

    gdk_draw_line( frame->window, gc, x1, y1, x2, y2);

    if (rng <= 0)
      { continue; }
    xx = xlmrk + cos (theta) * rng*ppk;
    yy = ylmrk - sin (theta) * rng*ppk;
    x = (gint)((xx > 0) ? xx +.5 : xx -.5) * bup_factor;
    y = (gint)((yy > 0) ? yy +.5 : yy -.5) * bup_factor;

    sprintf (aa, "%.0f", angle);
    gdk_text_extents (font, aa, strlen (aa)
		      , &lbearing, &rbearing, &width, &ascent, &descent);
    x = (angle > 180) ? x : x -width;

    if (angle > 315 || angle < 45)
      { y -= (ascent+descent)/2; }
    else if (angle > 135 && angle < 225)
      { y += (ascent+descent)/2; }

    gdk_draw_text (frame->window
		   , font, gc
		   , x, y
		   , aa, strlen (aa));
 }
  gdk_gc_set_line_attributes (gc, 1, GDK_LINE_SOLID, 0, 0);

  /* set coords of ulc relative to radar */
  lat = wwptr->radar_location->latitude;
  lon = wwptr->radar_location->longitude;
  alt = wwptr->radar_location->altitude;
  sfc->ulc_radar = sfc->corner[0];/* get lat/lon/alt and absolute pixels */
  pt = &sfc->ulc_radar; /* upper left */

  loop_xy2ll_v3( &pt->lat, &pt->lon, &pt->alt, &pt->xx, &pt->yy, &pt->zz
		 , lat, lon, alt, dd_earthr(lat), 1 );


  sii_view_tic_info (frame_num, &xtici, &ytici, &xtics, &ytics, &xyannot);

  if (xtics || ytics) {
    pt = sfc->corner; /* upper left */
    ptlr = &sfc->corner[2];
    if (time_series) {
      nxtics = 7;
      xtici = (sfc->corner[1].xx - sfc->corner[0].xx)/(nxtics+1);
      xtic1 = sfc->corner[0].xx + xtici;
    }
    else {
      xtic1 = sii_tic1 (pt->xx, xtici);
      nxtics = (ptlr->xx -xtic1)/xtici +1;
    }
    ytic1 = sii_tic1 (ptlr->yy, ytici);
    nytics = (pt->yy -ytic1)/ytici +1;
    ytic = ytic1;

    for (kk=0; kk < nytics; kk++, ytic += ytici) {
      xtic = xtic1;
      for (jj=0; jj < nxtics; jj++, xtic += xtici) {
	sii_lmrk_coords_to_frame_coords (frame_num, xtic, ytic, &x, &y);
	x *= bup_factor;
	y *= bup_factor;

	if (xtics) {		/* draw x tic marks */
	  x1 = x -ticlen/2;
	  x2 = x1 +ticlen -1;
	  gdk_draw_line( frame->window, gc, x1, y, x2, y);
	}
	if (ytics) {		/* draw y tic marks */
	  y1 = y -ticlen/2;
	  y2 = y1 +ticlen -1;
	  gdk_draw_line( frame->window, gc, x, y1, x, y2);
	}
      }
    }
    ytica = ytic -ytici;		/* last tic drawn */

    if (xyannot) {
      aa = annot;
      xtic = xtic1;
      ytic = ytic1;
      for (xtic = xtic1 ;; xtic += xtici) {
	sii_lmrk_coords_to_frame_coords (frame_num, xtic, ytic, &x, &y);
	if (x >= clip.x)
	  { break; }
      }
      for (;; ytica -= ytici) {
	sii_lmrk_coords_to_frame_coords (frame_num, xtic, ytica, &x, &y);
	if (y > clip.y -ticlen/2)
	  { break; }
      }
      for (ytic = ytic1;; ytic += ytici) {
	sii_lmrk_coords_to_frame_coords (frame_num, xtic, ytic, &x, &y);
	if (y <= clip.y +clip.height)
	  { break; }
      }
      xtic1 = xtic;
      ytic1 = ytic;
      for (jj=0; jj < nxtics; jj++, xtic += xtici) {
	sii_lmrk_coords_to_frame_coords (frame_num, xtic, ytica, &x, &y);
	x *= bup_factor;
	y *= bup_factor;
	if (time_series) {	/* seconds offset from start time */
	  tval = ts_span * xtic/(sfc->corner[1].xx - sfc->corner[0].xx) +.5;
	}
	else
	  { tval = xtic; }
	
	fmt =  (xtici < 2.) ? "%.1f" : "%.0f";
	sprintf (aa, fmt, tval);
	gdk_text_extents (font, aa, strlen (aa)
			  , &lbearing, &rbearing, &width, &ascent, &descent);
	x -= width/2;
	y += ticlen/2 +font->ascent + font->descent;
	gdk_draw_text (frame->window
		       , font, gc
		       , x, y
		       , aa, strlen (aa));
      }
      xtic = xtic1;
      for (jj=0; jj < nytics; jj++, ytic += ytici) {
	sii_lmrk_coords_to_frame_coords (frame_num, xtic, ytic, &x, &y);
	x *= bup_factor;
	y *= bup_factor;
	fmt =  (ytici < 2.) ? "%.1f" : "%.0f";
	sprintf (aa, fmt, ytic);
	gdk_text_extents (font, aa, strlen (aa)
			  , &lbearing, &rbearing, &width, &ascent, &descent);
	x += ticlen/2 +1;
	y += (ascent+descent)/2;
	gdk_draw_text (frame->window
		       , font, gc
		       , x, y
		       , aa, strlen (aa));
      }
    }
  }



  gdk_gc_set_clip_rectangle (gc, NULL);

  stp = &sfc->tbl_parms;
  width = frame->allocation.width;
  height = frame->allocation.height;

# ifdef notyet
  width -=2;
  height -=2;

  if( stp->left_attach == 0 )
    { ix++; width--; }
  if( stp->top_attach == 0 )
    { iy++; height--; }
# endif
  
  /* draw a two pixel width border */
  gdk_gc_set_foreground (gc, frame_border_colors[wwptr->lock_num] );

  gdk_draw_rectangle (frame->window
		      , gc
		      , FALSE
		      , ix, iy
		      , width, height );
  
  ix++; iy++; width -= 2; height -= 2;
  gdk_draw_rectangle (frame->window
		      , gc
		      , FALSE
		      , ix, iy
		      , width, height );


  if (redraw_bnd)
    { se_redraw_all_bnds_for_frame (frame_num); }
  /*
   *
  gdk_flush();
   */
}

/* c------------------------------------------------------------------------ */

gboolean sii_lmrk_inside (guint frame_num)
{
   SiiFrameConfig *sfc = frame_configs[frame_num];
   SiiPoint *pt, *pt2;
   gboolean inside;
   pt = &sfc->corner[0];	/* upper left km. relative to landmark*/
   pt2 = &sfc->corner[2];	/* lower right */

   inside = pt->xx <= 0 && pt2->xx >= 0 && pt2->yy <= 0 && pt->yy >= 0;
   return inside;
}

/* c------------------------------------------------------------------------ */

gint sii_rng_ring_arcs (guint frame_num, gdouble range)
{
   SiiFrameConfig *sfc = frame_configs[frame_num];
   SiiPoint *pt, *pt2;
   Rectangle *rct;
   gdouble d, xx, xx2, yy, yy2, theta, theta2, xmin, xmax, ymin, ymax, dg, dg2;
   gboolean inside, swap_xy;
   gint jj, kk, nn, num_intxns = 0, num_arcs = 0, ndx;
   gint mark, count = 0, xns_sides = 0;
   SiiArc *side = sii_arcs, *side2;
   Dxyz xy, *xy1, *xy2;
   static double rng_trip = 21;
   
				/* c...mark */

   pt = &sfc->corner[0];	/* upper left km. relative to landmark*/
   xmin = pt->xx;
   ymax = pt->yy;
   pt = &sfc->corner[2];	/* lower right */
   xmax = pt->xx;
   ymin = pt->yy;
   
   inside = xmin < 0 && xmax > 0 && ymin < 0 && ymax > 0;
   /*
    * Go around the boundaries starting with the top
    */
   if (range > rng_trip)
     { mark = 0; }

   for (jj=0; jj < 4; jj++, side++) {

      pt = &sfc->corner[jj];
      side->num_intxns = 0;
      side->nxt_ndx = 0;
      side->angle1 = side->angle2 = -360;

      if (jj & 1) {		/* consider the x-axis */
	 xx = pt->xx;
	 if (inside && range < fabs(xx))
	   { continue; }
	 else if (!inside && fabs (xx) > range)
	   { continue; }
	 theta = acos (xx/range);
	 yy = range * sin (theta);
	 yy2 = -yy;
	 kk = 0;
	 
	 if (yy > ymin && yy < ymax ) {
	    side->xy[kk].x = xx;
	    side->xy[kk].y = yy;
	    kk = ++side->num_intxns;
	    num_intxns++;
	 }
	 if (yy2 > ymin && yy2 < ymax ) {
	    side->xy[kk].x = xx;
	    side->xy[kk].y = yy2;
	    side->num_intxns++;
	    num_intxns++;
	 }
      }
      else {			/* y-axis */
	 yy = pt->yy;
	 if (inside && range < fabs(yy))
	   { continue; }
	 else if (!inside && fabs (yy) > range)
	   { continue; }
	 theta = asin (yy/range);
	 xx = range * cos (theta);
	 xx2 = -xx;
	 kk = side->num_intxns = 0;
	 if (xx > xmin && xx < xmax ) {
	    side->xy[kk].x = xx;
	    side->xy[kk].y = yy;
	    kk = ++side->num_intxns;
	    num_intxns++;
	 }
	 if (xx2 > xmin && xx2 < xmax ) {
	    side->xy[kk].x = xx2;
	    side->xy[kk].y = yy;
	    side->num_intxns++;
	    num_intxns++;
	 }
      }	/* end if */

      if (side->num_intxns == 2) {

	 if (num_intxns == 2) {
	    /* first side with intersections and
	     * there are two intersection; start with the second one
	     */
	    side->nxt_ndx = 1;
	 }
	 /*
	  * order the intxns to move clockwise around the borders
	  */
	 swap_xy = FALSE;

	 switch (jj) {
	  case 0:		/* top border */
	    if (side->xy[0].x > side->xy[1].x)
	      { swap_xy = TRUE; }
	    break;
	  case 1:		/* right border */
	    if (side->xy[0].y < side->xy[1].y)
	      { swap_xy = TRUE; }
	    break;
	  case 2:		/* bottom border */
	    if (side->xy[0].x < side->xy[1].x)
	      { swap_xy = TRUE; }
	    break;
	  case 3:		/* left border */
	    if (side->xy[0].y > side->xy[1].y)
	      { swap_xy = TRUE; }
	    break;
	 };
	 if (swap_xy) {
	    xy = side->xy[0];
	    side->xy[0] = side->xy[1];
	    side->xy[1] = xy;
	 }
      }	/* end if (side->num_intxns */
   } /* end for */

   if (num_intxns & 1)
     { printf ("num_intxns:%d is odd!\n", num_intxns); return 0; }

   if (num_intxns < 2)
     { return 0; }


   /*
    * Go around clockwise starting at the top and define the arcs
    */

   for (side = sii_arcs ;;) {			/* c...mark */
      count++;
# ifdef diag
      printf (" r:%.1f c:%d ai:%d an:%d a2i:%d a2n:%d %.2f %.2f %.2f %.2f %d\n"
	      , range , count, side->id, side->num_intxns
	      , side->next->id, side->next->num_intxns
	      , side->xy[0].x, side->xy[0].y
	      , side->xy[1].x, side->xy[1].y
	      , side->nxt_ndx
	      );
# endif
      if (!side->num_intxns) {
	 side = side->next;
	 if (side->id == 0)
	   { return num_arcs; }
	 continue;
      }

      if (num_intxns == 2 && side->num_intxns == 2) {
	 num_arcs = 1;		/* intersects only one boundary */
	 xy1 = &side->xy[0];
	 xy2 = &side->xy[1];
	 theta = atan2 (xy1->y, xy1->x);
	 side->angle1 = 
	   (theta < 0) ? DEGREES (theta) +360. : DEGREES (theta);
	 theta2 = atan2 (xy2->y, xy2->x);
	 side->angle2 = 
	   (theta2 < 0) ? DEGREES (theta2) +360. : DEGREES (theta2);
	 return num_arcs;
      }

      xy1 = &side->xy[side->nxt_ndx];
      side2 = side->next;
      nn = 3;
      
      if (side->num_intxns == 2)
	{ side->nxt_ndx = (side->nxt_ndx == 1) ? 0 : 1; }

      /* find the next intersection
       */
      for ( ; nn--; ) {
# ifdef diag
	 printf ("  srch: a2i:%d a2n:%d %.2f %.2f %.2f %.2f %d\n"
		 , side2->id, side2->num_intxns
		 , side2->xy[0].x, side2->xy[0].y
		 , side2->xy[1].x, side2->xy[1].y
		 , side2->nxt_ndx
		 );
# endif
	 if (side2->num_intxns)
	   { break;}
	 side2 = side2->next;
      }
# ifdef obsolete
      if (nn)
	{ return num_arcs; }
# endif
      xy2 = &side2->xy[side2->nxt_ndx];
      side2->nxt_ndx = (side2->nxt_ndx == 1) ? 0 : 1;
      num_arcs++;

      /* set the angle limits of the arc
       */
      theta = atan2 (xy1->y, xy1->x);
      dg = (theta < 0) ? DEGREES (theta) +360. : DEGREES (theta);
      theta2 = atan2 (xy2->y, xy2->x);
      dg2 = (theta2 < 0) ? DEGREES (theta2) +360. : DEGREES (theta2);
      /*
       * arcs are counter clockwise from the x-axis
       */
      switch (side->id) {
       case 0:			/* top border */

	 switch (side2->id) {
	  case 1:		/* right border */
	    if (xy2->y < 0 ) {	/* arc in lower-left quadrant*/
	       /* arc from xy1 -> xy2 */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    else {		/* upper-right quad */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    break;
	  case 2:		/* bottom border */
	    if (xy1->x <= 0) {	/* left hemisphere */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    else {		/* right */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    break;
	  case 3:		/* left border */
	    if (xy2->y <= 0) {	/* lower hemisphere */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    else {		/* upper hemisphere */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    break;
	 };
	 break;

       case 1:

	 /* can only have an unprocessed
	  * intersections with the bottom or left border (2,3)
	  */
	 switch (side2->id) {
	  case 0:		/* top line */
	    if (xy1->y <= 0) {	/*   */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    else {		/*  */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    break;
	  case 2:		/* bottom */
	    if (xy1->y <= 0) {	/* lower right quad */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    else {		/* upper left quad */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    break;
	  case 3:
	    if (xy1->y <= 0) {	/* lower hemisphere */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    else {		/* upper */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    break;
	 };
	 break;

       case 2:			/* bottom */

	 switch (side2->id) {
	  case 0:		/* top */
	    /* if there are two intersections, this is the left one */
	    side->angle1 = dg2;
	    side->angle2 = dg;
	    break;
	  case 1:		/* right border */
	    if (xy1->y < 0) {	/*  */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    else {		/* upper */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    break;
	  case 3:		/* left border */
	    if (xy1->x >= 0) {	/* right hemisphere */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    else {		/* upper */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    break;
	 };
	 break;

       case 3:			/* left border */
	 switch (side2->id) {
	  case 0:
	    side->angle1 = dg2;
	    side->angle2 = dg;
	    break;
	  case 1:		/* right */
	    /* if there are two intersections, this is the top one */
	    side->angle1 = dg2;
	    side->angle2 = dg;
	    break;
	  case 2:		/* bottom border */
	    if (xy1->y < 0) {	/*  */
	       side->angle1 = dg;
	       side->angle2 = dg2;
	    }
	    else {		/*  */
	       side->angle1 = dg2;
	       side->angle2 = dg;
	    }
	    break;
	 };
	 break;
      };
# ifdef diag
      printf ("   a2i:%d a2n:%d a1:%.2f a2:%.2f %.2f %.2f %.2f %.2f\n"
	      , side2->id, side2->num_intxns
	      , side->angle1, side->angle2
	      , side2->xy[0].x, side2->xy[0].y
	      , side2->xy[1].x, side2->xy[1].y
	      );
      printf ("    xy1:%.2f %.2f xy2:%.2f %.2f\n"
	      , xy1->x, xy1->y
	      , xy2->x, xy2->y
	      );
# endif
      if (num_arcs *2 >= num_intxns)
	{ return num_arcs; }

      if (side2->num_intxns == 1) {
	 side = side2->next;
      }
      else {
	 side = side2;
      }

      if (count > 5)
	{ printf ("exit c:%d\n", count); return 0; }

   }

   return num_arcs;
}

/* c------------------------------------------------------------------------ */

void sp_do_lock_color(int frame)
{
    WW_PTR wwptr;
    int color;

    wwptr = solo_return_wwptr(frame);
    color = wwptr->lock_color_num;
}

/* c------------------------------------------------------------------------ */

void sii_update_frame_info (guint frame_num)
{
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  SiiFrameConfig *sfc = frame_configs[frame_num];

  wwptr->view->width_in_pix = sfc->width;
  wwptr->view->height_in_pix = sfc->height;

  wwptr->big_width_in_pix = sfc->big_width;
  wwptr->big_height_in_pix = sfc->big_height;
  wwptr->pixel_depth = 8;

  wwptr->big_image = sfc->big_image;
  wwptr->image = sfc->image;

  sii_reset_image (frame_num);
}

/* c------------------------------------------------------------------------ */

void sii_check_image_size (guint frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  guint size;
  gchar str[256];


  size = sfc->width * sfc->height;

  if (size > sfc->max_image_size) {
# ifdef config_debug
     sprintf (str, "Enlarge Image %d %d %d -> %d"
	      , frame_num, sii_frame_count, sfc->max_image_size, size);    
# endif
     if (sfc->image) {
	if (sfc->image->data)
	  { g_free (sfc->image->data); }
     }
     else {
	sfc->image = (SiiImage *)g_malloc0 (sizeof (SiiImage));
     }
     sfc->image->data = g_malloc0 (size * 4);
# ifdef config_debug
     sprintf (str+strlen(str), " data:%d", sfc->image->data);
     /*
     g_message (str);
      */
     sii_append_debug_stuff (str);
# endif
     sfc->max_image_size = size;
  }
  sfc->sync_num = sfc->config_sync_num;
  sfc->image_size = size;
}

/* c------------------------------------------------------------------------ */

void sii_plot_data2 (guint frame_num, guint plot_function)
{
  if (plot_function < 0)
    { return; }

  switch (plot_function) {
    case BACKWARDS_FOREVER:
    case BACKWARDS_ONCE:
    case REPLOT_LOCK_STEP:
    case REPLOT_THIS_FRAME:
    case FORWARD_FOREVER:
    case FORWARD_ONCE:
     displayq (frame_num, plot_function);
     break;

    case REPLOT_ALL:
     sp_replot_all ();
     break;
  };
}

/* c------------------------------------------------------------------------ */

void sii_recalc_default_frame ()
{
   gint fn, mm, nn, ndx;
   gdouble d, multiple, mx_x_change = 0, mx_y_change = 0;
   sii_table_parameters *stp;
   SiiFrameConfig *sfc;

   /* find the max changes in the frames */

   for (fn=0; fn < sii_frame_count; fn++) {
      sfc = frame_configs[fn];
      stp = &sfc->tbl_parms;
      ndx = sfc->cfg_que_ndx;
      
      multiple = stp->right_attach - stp->left_attach;
      d = (sfc->cfg_width[ndx] - sfc->x_drag_ref)/multiple;
      if (d > mx_x_change) mx_x_change = d;
      
      multiple = stp->bottom_attach - stp->top_attach;
      d = (sfc->cfg_height[ndx] - sfc->y_drag_ref)/multiple;
      if (d > mx_y_change) mx_y_change = d;
   }
   sii_table_widget_width += (guint)(mx_x_change);
   sii_table_widget_height += (guint)(mx_y_change);
   sii_check_def_widget_sizes ();
}

/* c------------------------------------------------------------------------ */

void sii_plot_data (guint frame_num, guint plot_function)
{
  /* Update and or modify image sizes */
  SiiFrameConfig *sfc = frame_configs[frame_num];
  guint fn;
  gint idx, idy, mark, mm, nn;
  GtkWidget *frame;
  gchar str[256];
    
  if (sfc->local_reconfig) {
     mark = 0;
  }
  else if (sfc->drag_resize_count > 0 &&
	   !(sfc->reconfig_flag & FrameDragResize)) {

     /* next plot after a drag resize. Do a local reconfig
      * in case the frame sizes are not consistant
      */
     sii_new_frames ();
  }

  
  sii_plot_data2 (frame_num, plot_function);

}

/* c---------------------------------------------------------------------- */

gboolean sii_batch (gpointer argu)
{
  int jj;
  gchar *png_dir;
  gfloat ff;
				/*
  g_message ("sii_batch");
				 */
  for (jj=0; jj < sii_frame_count; jj++) {
    if (frame_configs[jj]->expose_count == 0) {
      gdk_flush();
      return TRUE;
    }
  }
  if (batch_mode) {
				/* get png destination dir */
    png_dir = (gs_image_dir) ? gs_image_dir->str : NULL;
    
    for (;;) {
      sii_png_image_prep (png_dir);
      if(displayq(0, FORWARD_ONCE))
	{ break; }		/* non-zero reply => it's over */
    }
    gtk_main_quit();
  }
				/*
  g_message ("Returning FALSE");
				 */
  return FALSE;
}
/* c------------------------------------------------------------------------ */

void uii_printf(char *message, ...)
{
    printf("%s", message);
}
/* c------------------------------------------------------------------------ */

void solo_message(char *message, ...)
{
    printf("%s", message);
}
/* c------------------------------------------------------------------------ */

void solo_def_params(dgi, list, num_parms)
  struct dd_general_info *dgi;
  int *list;
  int num_parms;
{
    static int count=0;
    int ii, jj, mark;

    count++;

    /*
     * return a list of parameter numbers to start the first plot
     */
    for(;;) {
	for(ii=0; ii < dgi->num_parms; ii++) {
	    if(!dgi->dds->field_present[ii])
		  continue;
	    *list++ = ii;
	    if(--num_parms <= 0)
		  break;
	}
	if(num_parms <= 0)
	      break;
    }
}
/* c---------------------------------------------------------------------- */

gchar *sii_get_swpfi_dir (gchar *dir, int argc, char **argv)
{
   gint nn;
   int ii;
   static char fpath[4096];
   char *fname = 0;

   if (argc > 1) {
     for (ii = 1; ii < argc; ii++) {
       strcpy(fpath, argv[ii]);
       fname = strstr(fpath, "swp."); /* only want sweep files */
       if (fname != NULL) {
         /* terminate the dir with a NULL */
         *fname = '\0';
         dir = fpath;
         break;
       }
     } /* ii */
   }

   if (!dir) {
     dir = getenv ("DORADE_DIR");
   }
   if (!dir) {
     dir = "./"; 
   }

   if (argc > 1) {
     nn = mddir_file_list_from_command_line(0, dir, argc, argv);
   } else {
     nn = mddir_file_list_v3(0, dir);
   }
   return (nn < 1) ? NULL : dir;
}

/* c---------------------------------------------------------------------- */

int sii_default_startup (const gchar *swpfi_dir)
{
   int mark = 0, frme, jj, nn, nfiles;
   int param_list[64];
   struct solo_perusal_info *spi, *solo_return_winfo_ptr ();
   struct solo_frame_state_files *sfsf, *se_return_state_files_struct();
   WW_PTR wwptr, wwptrc, solo_return_wwptr();
   struct dd_general_info *dgi, *dd_window_dgi();
   char *aa, *mddir_radar_name_v3();
   const gchar *cc;
   GString *gstr = NULL;

  
  if (gstr == NULL)
    { gstr = g_string_new (""); }

   spi =solo_return_winfo_ptr (); 
   
    for(frme=0; frme < SOLO_MAX_WINDOWS; frme++) {
	/*
	 * just set it up so that the default plot is to
	 * plot variables from the same radar
	 * for the same time
	 */
	spi->active_windows[frme] = YES;
	wwptr = solo_return_wwptr(frme); /* activate a window */
	wwptr->frame_ctr_info->reference_frame = frme + 1;
	wwptr->landmark_info->reference_frame = frme + 1;
	wwptr->parameter->linked_windows[frme] = YES;

	for(jj=0; jj < SOLO_MAX_WINDOWS; jj++) {
	    wwptr->lock->linked_windows[jj] = YES;
	    wwptr->sweep->linked_windows[jj] = YES;
	    wwptr->view->linked_windows[jj] = YES;
	    wwptr->landmark_info->linked_windows[jj] = YES;
	    wwptr->frame_ctr_info->linked_windows[jj] = YES;
	}
	wwptr->active = wwptr->changed =
	      wwptr->sweep->changed = 
		    wwptr->lock->changed = 
			  wwptr->parameter->changed = 
				wwptr->view->changed = YES;
    }
   solo_worksheet ();

   wwptr = solo_return_wwptr(0);
   slash_path(wwptr->sweep->directory_name, swpfi_dir);
   solo_trigger_dir_rescans(wwptr->sweep->directory_name);

   nn = mddir_file_list_v3(0, wwptr->sweep->directory_name);
   if (nn < 1)
     { return 0; }
    /*
     * nab a radar name
     */
    if(aa = mddir_radar_name_v3(0,0)) {
	strcpy(wwptr->sweep->radar_name, aa);
    }
    else
      { return 0; }
    /*
     * see if we can nab a sweep file for this directory
     */
    if(!solo_nab_next_file(0, TIME_NEAREST, LATEST_VERSION, 1, YES)) {
       mark = 0;
    }
    /*
     * figure out what parameters you are going to plot
     */
    dgi = dd_window_dgi(0, "");
    solo_def_params(dgi, param_list, SOLO_MAX_WINDOWS);

    for(frme=0; frme < SOLO_MAX_WINDOWS; frme++) {
	wwptr = solo_return_wwptr(frme); 
	wwptr->parameter_num = param_list[frme];
	str_terminate
	      (wwptr->parameter->parameter_name
	       , dgi->dds->parm[wwptr->parameter_num]->parameter_name, 8);
	/*
	 * find the appropriate color palette
	 */
	if (frme < dgi->num_parms) {
	  g_string_append (gstr, wwptr->parameter->parameter_name);
	  g_string_append (gstr, "\n");
	}
	if (frme) {
	  dd_copy_pisp(wwptr->lead_sweep->radar_location
		       , wwptr->radar_location);
	}
	sp_landmark_offsets(frme);
	sp_align_center(frme);

	sii_initialize_parameter (frme, wwptr->parameter->parameter_name);
	cc = sii_param_palette_name (frme);
	strcpy (wwptr->parameter->palette_name, cc);
	mark = 0;
    }
    for(frme=0; frme < SOLO_MAX_WINDOWS; frme++) {
      sii_set_param_names_list (frme, gstr, dgi->num_parms);   
    }
    g_string_free (gstr, TRUE);	/* free string struct and char data */
    return nn;

}

/* c------------------------------------------------------------------------ */

void solo_data_color_lut(frme)
  int frme;
{
    /* create the data coloration lookup table
     * this is the master lookup table that is in effect
     * until the parameter changes or color information changes
     * this is a 64K lookup table that works for raw data <= 16-bits
     */
    int i, j, kk, n, nc, min, max;
    WW_PTR wwptr, solo_return_wwptr();
    double f_inc, f_min, f_max, f_color_num=0;
    float ctr, inc, emin, emax;

    nc = sii_get_ctr_and_inc (frme, &ctr, &inc);

    wwptr = solo_return_wwptr(frme);

    f_min = (double)ctr -.5*(double)nc * inc;
    f_max = -.000001 +(double)f_min + nc * inc;
    min = DD_SCALE(f_min, wwptr->parameter_scale, wwptr->parameter_bias);
    max = DD_SCALE(f_max, wwptr->parameter_scale, wwptr->parameter_bias);
    min = min >= (-K32) ? min : -K32;
    max = max < K32 ? max : K32-1;

    for(i=(-K32); i < min; i++) {
	*(wwptr->data_color_lut+i) = wwptr->exceeded_color_num;
    }
    for(i=max+1; i < K32; i++) {
	*(wwptr->data_color_lut+i) = wwptr->exceeded_color_num;
    }
    f_inc = (double)wwptr->num_colors_in_table/(double)(max-min+1);
    /*
     * colors for the data
     */
    for(i=min; i <= max; i++,f_color_num+=f_inc) {
	kk = f_color_num;
	*(wwptr->data_color_lut+i) = (wwptr->xcolors+kk)->pixel;
    }
    sii_get_emph_min_max (frme, &emin, &emax);

    if(emin < emax) {
	/*
	 * do the emphasis zone
	 */
	min = DD_SCALE(emin, wwptr->parameter_scale, wwptr->parameter_bias);
	max = DD_SCALE(emax, wwptr->parameter_scale, wwptr->parameter_bias);
	min = min >= (-K32) ? min : -K32;
	max = max < K32 ? max : K32-1;

	for(i=min; i <= max; i++) {
	    *(wwptr->data_color_lut+i) = wwptr->emphasis_color_num;
	}
    }
    /* missing data color
     */
    *(wwptr->data_color_lut+wwptr->parameter_bad_val) =
	  wwptr->missing_data_color_num;
}
/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */







