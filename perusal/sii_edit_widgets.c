/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "help_edit.h"
# include "sii_utils.h"
# include <gdk/gdkkeysyms.h>
# include <math.h>

# include <solo_window_structs.h>
# include <solo_editor_structs.h>
# include <sed_shared_structs.h>
# include <seds.h>
# include <solo_list_widget_ids.h>

# ifndef ETERNITY
# define DAY_ZERO 0
# define ETERNITY 0x7fffffff
# endif

/* c---------------------------------------------------------------------- */

static GdkFont *edit_font;
static GdkColor *edit_fore;
static GdkColor *edit_back;
static GTree *edit_cmds_help = NULL;
static GTree *edit_egs_help = NULL;
static GSList *edit_cmds_list_help = NULL;
static GtkWidget *vbox_hlp_cmds = NULL;
static GString *cmds_one_time_only = NULL;
static GString *cmds_for_each_ray = NULL;
static gint max_cmd_width = 0;

enum {
   EDIT_ZERO,
   EDIT_CLOSE,
   EDIT_CANCEL,
   EDIT_PREV_BND_SET,
   EDIT_ADD_NEXT_BND,
   EDIT_REDRAW_BND,
   EDIT_CLEAR_BND,
   EDIT_CLEAR_CURRENT_BND,
   EDIT_DO_IT,
   EDIT_CLEAR_FER,
   EDIT_CLEAR_OTO,

   EDIT_RESET_TIMES,
   EDIT_START_TIME,
   EDIT_STOP_TIME,
   EDIT_FIRST_SWP,
   EDIT_LAST_SWP,

   EDIT_CMD_DIR,
   EDIT_CMD_COMM,
   EDIT_BND_DIR,
   EDIT_BND_COMM,
   EDIT_CMD_FILES,
   EDIT_BND_FILES,

   EDIT_LLB_DIR,
   EDIT_LLB_FILES,

   EDIT_OTO_TEXT,
   EDIT_FER_TEXT,

   EDIT_PREV_SWP_SET,
   EDIT_SAVE_CMDS,
   EDIT_SAVE_BND,
   EDIT_FILES_CANCEL,
   EDIT_CMD_BND_FILES,
   EDIT_IMPORT_LLB_FILES,
   EDIT_EXTENDED,
   EDIT_AUTO_REPLOT,

   EDIT_EXAMPLES,
   EDIT_EG_BB_UNF,
   EDIT_EG_FLG_FRK,
   EDIT_EG_FLG_GLTCH,
   EDIT_EG_REG_HST,
   EDIT_EG_IRREG_HST,
   EDIT_EG_RSHEAR,
   EDIT_EG_THRSH,

   EDIT_BND_IN,
   EDIT_BND_OUT,
   EDIT_BND_DRAW,
   EDIT_BND_PREV,
   EDIT_BND_LIST,

   EDIT_REPLOT_THIS,
   EDIT_REPLOT_LINKS,
   EDIT_REPLOT_ALL,

   EDIT_HLP_CANCEL,
   EDIT_HLP_OVERVIEW,
   EDIT_HLP_SHORTCUTS,
   EDIT_HLP_BNDS,
   EDIT_HLP_EG ,
   EDIT_HLP_WITH ,
   EDIT_HLP_EXTEND,
   EDIT_HLP_FILE,

   EDIT_LAST_ENUM,

};


# define EDIT_MAX_WIDGETS EDIT_LAST_ENUM

typedef struct {
  GtkWidget *data_widget[EDIT_MAX_WIDGETS];

  gboolean entry_flag[EDIT_MAX_WIDGETS];
  guint num_values[EDIT_MAX_WIDGETS];

  GString *orig_txt[EDIT_MAX_WIDGETS];
  GString *txt[EDIT_MAX_WIDGETS];
  gboolean toggle[EDIT_MAX_WIDGETS];

  guint bnd_in_out_state;
  gboolean auto_replot;

  gdouble start_time;
  gdouble stop_time;

  gboolean frame_active;
  gboolean stop_drawing;

  gchar *fer_lines;
  gchar *oto_lines;

} EditData;

static EditData *edata;



void main_edit_widget( guint frame_num );
static GSList *radio_group = NULL;

void sii_edit_menubar2( GtkWidget  *window,
                    GtkWidget **menubar, guint frame_num );
void sii_edit_menubar( GtkWidget  *window,
                    GtkWidget **menubar, guint frame_num );

void edit_files_widget (guint frame_num );

void show_edit_files_widget (guint frame_num );
const gchar *sii_return_swpfi_dir (guint frame_num);
const gchar *sii_return_swpfi_radar (guint frame_num);
const GString *sii_return_edit_cmd_files (guint frame_num, const char *dir);
const GString *sii_return_edit_bnd_files (guint frame_num, const char *dir);
const GString *sii_return_llb_bnd_files (guint frame_num, const char *dir);

const struct solo_list_mgmt *sii_return_swpfi_radar_list (guint frame_num);
double sii_return_swpfi_time_stamp (guint frame_num);
struct solo_list_mgmt *slm;
const char *sii_string_from_dtime (double dtime);
double sii_return_swpfi_time_stamp (guint frame_num);
double sii_nearest_file_time (int frame_num, double target_time);
void sii_zap_quick_message (GtkWidget *w, gpointer data );
static void edit_help_widget (gchar *words);
double sii_dtime_from_string (char *tstr);
void sii_edit_eg_split (guint frame_num, const gchar **eg, int nlines);
struct solo_edit_stuff *return_sed_stuff();
char *solo_list_entry (struct solo_list_mgmt  *slm, int jj);

void put_edit_cmds_help (gchar *name, GString *words);

void sii_cmds_help_event (GtkWidget *w, GdkEvent *event
			      , gpointer data );
gboolean set_edit_help_items( gpointer a_str, gpointer words );
void edit_cmds_help_widget (guint frame_num );
void show_edit_cmd_help_widget (guint frame_num);    
void sii_edit_reset_times (guint frame_num);
char *absorb_zmap_bnd (char *, int, int *);


/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

void sii_edit_zap_last_bnd_point()
{
  struct solo_edit_stuff *seds, *return_sed_stuff();
  struct one_boundary *ob;
  struct boundary_stuff *sebs, *return_se_bnd_struct();
  
  seds = return_sed_stuff();
  sebs = return_se_bnd_struct();

  ob = sebs->current_boundary;
  if(ob->num_points > 1)
    se_draw_bnd(ob->top_bpm->last, 2, YES); /* erase it first */
  se_zap_last_bpm(&ob->top_bpm);
  if(ob->num_points > 0)
    ob->num_points--;
  sebs->last_operation = BND_POINT_DELETED;
}

/* c---------------------------------------------------------------------- */

void sii_edit_return_start_stop (guint frame_num, gdouble *stt, gdouble *stp)
{
  EditData *edd = (EditData *)frame_configs[frame_num]->edit_data;
  *stt = edd->start_time;
  *stp = edd->stop_time;
}

/* c---------------------------------------------------------------------- */

void sii_edit_reset_times (guint frame_num)
{
   EditData *edd = (EditData *)frame_configs[frame_num]->edit_data;
   WW_PTR wwptr, solo_return_wwptr();
   const char *cc;

   if (!edd)
     { return; }
   wwptr = solo_return_wwptr(frame_num);
   edd->start_time =  wwptr->sweep->start_time;
   edd->stop_time = (wwptr->view->type_of_plot & SOLO_TIME_SERIES)
     ? wwptr->sweep->stop_time : wwptr->sweep->start_time;
   
   cc = sii_string_from_dtime (edd->start_time);
   g_string_assign (edd->orig_txt[EDIT_START_TIME], cc);
   gtk_entry_set_text (GTK_ENTRY (edd->data_widget[EDIT_START_TIME]), cc );
   
   cc = sii_string_from_dtime (edd->stop_time);
   g_string_assign (edd->orig_txt[EDIT_STOP_TIME], cc);
   gtk_entry_set_text (GTK_ENTRY (edd->data_widget[EDIT_STOP_TIME]), cc );
}

/* c---------------------------------------------------------------------- */

void sii_edit_menu_cb ( GtkWidget *w, gpointer data )
{
   guint num = GPOINTER_TO_UINT (data);
   guint nn, frame_num, task, wid, active, len;
   gchar *aa, *bb, *line, str[128], *sptrs[16];
   GtkWidget *widget, *check_item, *rmi;
   GtkWidget *entry, *text;
   EditData *edd;
   gint x, y;
   GdkPoint *ptp;
   const char *cc;
   const GString *cgs;
   int arg=0;
   struct ui_command *cmds=NULL;
   int time_series=NO, automatic, down;
   gdouble dtime, d_ctr, t1, t2;
   struct solo_edit_stuff *seds, *return_sed_stuff();
   struct one_boundary *ob;
   struct boundary_stuff *sebs, *return_se_bnd_struct();
   struct swp_file_input_control *sfic;
   WW_PTR wwptr, solo_return_wwptr();


  
   frame_num = num/TASK_MODULO;
   wid = 
     task = num % TASK_MODULO;
   edd = (EditData *)frame_configs[frame_num]->edit_data;
   widget =
     check_item =
       rmi = edd->data_widget[task];

   seds = return_sed_stuff();
   sebs = return_se_bnd_struct();

   
   switch (task) {

    case EDIT_CANCEL:
    case EDIT_CLOSE:
      widget = sii_get_widget_ptr (frame_num, FRAME_EDITOR);

      if( widget ) {
	 gdk_window_get_origin ( widget->window, &x, &y );
	 gdk_window_get_root_origin ( widget->window, &x, &y );
	 ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDITOR];
	 ptp->x = x; ptp->y = y;
	 gtk_widget_hide (widget);
	 edd->frame_active = FALSE;
      }
      widget = sii_get_widget_ptr (frame_num, FRAME_EDIT_FILES);

      if( widget ) {
	 gdk_window_get_origin ( widget->window, &x, &y );
	 gdk_window_get_root_origin ( widget->window, &x, &y );
	 ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDIT_FILES];
	 ptp->x = x; ptp->y = y;
	 gtk_widget_hide (widget);
      }
      widget = sii_get_widget_ptr (frame_num, FRAME_EDIT_CMDS_HELP);

      if( widget ) {
	 gdk_window_get_origin ( widget->window, &x, &y );
	 gdk_window_get_root_origin ( widget->window, &x, &y );
	 ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDIT_CMDS_HELP];
	 ptp->x = x; ptp->y = y;
	 gtk_widget_hide (widget);
      }

    case EDIT_FILES_CANCEL:
      widget = sii_get_widget_ptr (frame_num, FRAME_EDIT_FILES);

      if( widget ) {
	 gdk_window_get_origin ( widget->window, &x, &y );
	 gdk_window_get_root_origin ( widget->window, &x, &y );
	 ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDIT_FILES];
	 ptp->x = x; ptp->y = y;
	 gtk_widget_hide (widget);
      }
      break;
     
    case EDIT_CMD_BND_FILES:
    case EDIT_BND_FILES:
    case EDIT_IMPORT_LLB_FILES:
      show_edit_files_widget (frame_num);
      break;

    case EDIT_EXTENDED:
      break;

    case EDIT_ADD_NEXT_BND:
      se_cycle_bnd();
      sebs->last_operation = MOVE_TO_NEXT_BOUNDARY;
      break;

    case EDIT_REDRAW_BND:
      if(sebs->last_operation == REDRAW_BOUNDARIES) {
	 sebs->last_operation = BND_OTHER;
	 se_erase_all_bnds();
	 ob = sebs->current_boundary;
	 if(ob->num_points > 1) {
	    se_draw_bnd(ob->top_bpm->last, ob->num_points, NO);
	 }
      }
      else {
	 sebs->last_operation = REDRAW_BOUNDARIES;
	 se_redraw_all_bnds();
      }
      break;

    case EDIT_CLEAR_BND:
      if(sebs->last_operation == BND_CLEARED) {
	 sebs->last_operation = BND_OTHER;
	 se_clr_all_bnds();
      }
      else {
	 sebs->last_operation = BND_CLEARED;
	 se_clr_current_bnd();
      }
      break;

    case EDIT_PREV_BND_SET:
      se_prev_bnd_set();
      sebs->last_operation = PREV_BND_SET;
      break;

    case EDIT_DO_IT:
      seds->fer_lines = gtk_editable_get_chars
	(GTK_EDITABLE (edd->data_widget[EDIT_FER_TEXT]), 0, -1);
      seds->oto_lines = gtk_editable_get_chars
	(GTK_EDITABLE (edd->data_widget[EDIT_OTO_TEXT]), 0, -1);
						
      sebs->last_operation = BND_OTHER;
      seds->pbs = NULL;
      wwptr = solo_return_wwptr(frame_num);
      time_series = wwptr->view->type_of_plot & SOLO_TIME_SERIES;
      automatic = wwptr->view->type_of_plot & TS_AUTOMATIC;
      down = wwptr->view->type_of_plot & TS_PLOT_DOWN;
      d_ctr = wwptr->view->ts_ctr_km;

      se_process_data(arg, cmds, time_series, automatic, down, d_ctr
		      , frame_num);
      sii_edit_reset_times (frame_num);
      if (edd->toggle[EDIT_AUTO_REPLOT])
	{ sii_plot_data (frame_num, REPLOT_LOCK_STEP); }
      break;

      /* Radio menu items */
    case EDIT_BND_IN:
    case EDIT_BND_OUT:
      g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

      if (active = GTK_CHECK_MENU_ITEM (check_item)->active)
	{ edd->bnd_in_out_state = task; }
      edd->toggle[task] = active;
      sebs->operate_outside_bnd = (task == EDIT_BND_OUT && active) ? YES : NO;
      break;

    case EDIT_BND_DRAW:
      edd->toggle[task] = GTK_CHECK_MENU_ITEM (rmi)->active;
      break;

    case EDIT_BND_PREV:
      break;

    case EDIT_BND_LIST:
      break;

      /* Check menu item */
    case EDIT_AUTO_REPLOT:
      edd->toggle[task] = GTK_CHECK_MENU_ITEM (rmi)->active;
      break;

    case EDIT_CLEAR_FER:
    case EDIT_CLEAR_OTO:
      widget = (task == EDIT_CLEAR_FER) ? edd->data_widget[EDIT_FER_TEXT]
	: edd->data_widget[EDIT_OTO_TEXT];
      gtk_editable_delete_text (GTK_EDITABLE (widget), 0, -1);
      break;

    case EDIT_CMD_DIR:
      cc = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_CMD_DIR]));
      if (!strlen (cc)) {
	cc = sii_return_swpfi_dir (frame_num);
      }
      g_string_assign (edd->orig_txt[EDIT_CMD_DIR], cc);
      cgs = sii_return_edit_cmd_files (frame_num, cc);
      text = edd->data_widget[EDIT_CMD_FILES];
      gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      gtk_text_insert (GTK_TEXT (text), edit_font, edit_fore, edit_back
		       , cgs->str, -1);
      g_string_assign (edd->orig_txt[EDIT_CMD_FILES], cgs->str);
      break;
     
    case EDIT_SAVE_CMDS:
    case EDIT_CMD_COMM:
      if (edd->data_widget[EDIT_CMD_DIR]) {
	 cc = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_CMD_DIR]));
	 bb = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_CMD_COMM]));
      }
      else {
	 cc = sii_return_swpfi_dir (frame_num);
	 bb = "_no_comment";
      }
      sii_save_commands (frame_num, cc, bb);

      g_string_assign (edd->orig_txt[EDIT_CMD_DIR], cc);
      g_string_assign (edd->orig_txt[EDIT_CMD_COMM], bb);
      cgs = sii_return_edit_cmd_files (frame_num, cc);

      text = edd->data_widget[EDIT_CMD_FILES];
      gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      gtk_text_insert (GTK_TEXT (text) , edit_font, edit_fore, edit_back
		       , cgs->str, -1);
      g_string_assign (edd->orig_txt[EDIT_CMD_FILES], cgs->str);
      break;

    case EDIT_LLB_DIR:		/* lat/lon boundary (LLB) */
      cc = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_LLB_DIR]));
      if (!strlen (cc)) {
	cc = sii_return_swpfi_dir (frame_num);
      }
      g_string_assign (edd->orig_txt[EDIT_LLB_DIR], cc);
      cgs = sii_return_llb_bnd_files (frame_num, cc);
      text = edd->data_widget[EDIT_LLB_FILES];
      gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      gtk_text_insert (GTK_TEXT (text) , edit_font, edit_fore, edit_back
		       , cgs->str, -1);
      g_string_assign (edd->orig_txt[EDIT_LLB_FILES], cgs->str);
      break;

    case EDIT_BND_DIR:
      cc = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_BND_DIR]));
      if (!strlen (cc)) {
	cc = sii_return_swpfi_dir (frame_num);
      }
      g_string_assign (edd->orig_txt[EDIT_BND_DIR], cc);
      cgs = sii_return_edit_bnd_files (frame_num, cc);
      text = edd->data_widget[EDIT_BND_FILES];
      gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      gtk_text_insert (GTK_TEXT (text) , edit_font, edit_fore, edit_back
		       , cgs->str, -1);
      g_string_assign (edd->orig_txt[EDIT_BND_FILES], cgs->str);
      break;

    case EDIT_SAVE_BND:
    case EDIT_BND_COMM:
      if (edd->data_widget[EDIT_BND_DIR]) {
	 cc = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_BND_DIR]));
	 bb = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_BND_COMM]));
      }
      else {
	 cc = sii_return_swpfi_dir (frame_num);
	 bb = "_no_comment";
      }
      sii_save_boundary (frame_num, cc, bb);

      g_string_assign (edd->orig_txt[EDIT_BND_DIR], cc);
      g_string_assign (edd->orig_txt[EDIT_BND_COMM], bb);
      cgs = sii_return_edit_bnd_files (frame_num, cc);

      text = edd->data_widget[EDIT_BND_FILES];
      gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      gtk_text_insert (GTK_TEXT (text) , edit_font, edit_fore, edit_back
		       , cgs->str, -1);
      g_string_assign (edd->orig_txt[EDIT_BND_FILES], cgs->str);
      break;

    case EDIT_RESET_TIMES:
      sii_edit_reset_times (frame_num);
      break;

    case EDIT_FIRST_SWP:
    case EDIT_LAST_SWP:
      if (task == EDIT_FIRST_SWP) {
	 wid = EDIT_START_TIME;
	 edd->start_time = DAY_ZERO;
	 cc = "0";
      }
      else {
	 edd->stop_time = ETERNITY;
	 wid = EDIT_STOP_TIME;
	 cc = "-1";
      }
      gtk_entry_set_text (GTK_ENTRY (edd->data_widget[wid]), cc );
      break;

    case EDIT_START_TIME:
    case EDIT_STOP_TIME:
      aa = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[wid]));
      strcpy (str, aa);
      g_strstrip (str);
      if (strcmp (str, "0") == 0) {
	 dtime = DAY_ZERO; cc = "0";
      }
      else if (strcmp (str, "-1") == 0) {
	 dtime = ETERNITY; cc = "-1";
      }
      else {
	 dtime = sii_dtime_from_string (str);
	 if (dtime == 0)
	   { break; }
	 cc = sii_string_from_dtime (dtime);
      }
      if (wid == EDIT_START_TIME)
	{ edd->start_time = dtime; }
      else
	{ edd->stop_time = dtime; }

      gtk_entry_set_text (GTK_ENTRY (edd->data_widget[wid]), cc );
      break;

    case EDIT_REPLOT_THIS:
    case EDIT_REPLOT_LINKS:
    case EDIT_REPLOT_ALL:
      sii_plot_data (frame_num, REPLOT_LOCK_STEP);
      break;
     
    case EDIT_EG_BB_UNF:
    case EDIT_EG_FLG_FRK:
    case EDIT_EG_FLG_GLTCH:
    case EDIT_EG_REG_HST:
    case EDIT_EG_IRREG_HST:
    case EDIT_EG_RSHEAR:
    case EDIT_EG_THRSH:
      switch (task) {
       case EDIT_EG_BB_UNF:
	 nn = sizeof (Bargen_Brown_ac_unfoldingz_cc)/sizeof (char *);
	 sii_edit_eg_split (frame_num, Bargen_Brown_ac_unfoldingz_cc, nn);
	 break;
       case EDIT_EG_FLG_FRK:
	 nn = sizeof (flag_freckles_cc)/sizeof (char *);
	 sii_edit_eg_split (frame_num, flag_freckles_cc, nn);
	 break;
       case EDIT_EG_FLG_GLTCH:
	 nn = sizeof (flag_glitches_cc)/sizeof (char *);
	 sii_edit_eg_split (frame_num, flag_glitches_cc, nn);
	 break;
       case EDIT_EG_REG_HST:
	 nn = sizeof (regular_histogram_cc)/sizeof (char *);
	 sii_edit_eg_split (frame_num, regular_histogram_cc, nn);
	 break;
       case EDIT_EG_IRREG_HST:
	 nn = sizeof (irregular_histogram_cc)/sizeof (char *);
	 sii_edit_eg_split (frame_num, irregular_histogram_cc, nn);
	 break;
       case EDIT_EG_RSHEAR:
	 nn = sizeof (radial_shear_cc)/sizeof (char *);
	 sii_edit_eg_split (frame_num, radial_shear_cc, nn);
	 break;
       case EDIT_EG_THRSH:
	 nn = sizeof (thresholding_cc)/sizeof (char *);
	 sii_edit_eg_split (frame_num, thresholding_cc, nn);
	 break;
      };
      break;

    case EDIT_HLP_CANCEL:
      widget = sii_get_widget_ptr (frame_num, FRAME_EDIT_CMDS_HELP);

      if( widget ) {
	 gdk_window_get_origin ( widget->window, &x, &y );
	 gdk_window_get_root_origin ( widget->window, &x, &y );
	 ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDIT_CMDS_HELP];
	 ptp->x = x; ptp->y = y;
	 gtk_widget_hide (widget);
      }
      break;

    case EDIT_HLP_OVERVIEW:
     nn = sizeof (edit_overview)/sizeof (char *);
     sii_glom_strings (edit_overview, nn, gen_gs);
     sii_message (gen_gs->str);
      break;

    case EDIT_HLP_FILE:
     nn = sizeof (hlp_edit_file)/sizeof (char *);
     sii_glom_strings (hlp_edit_file, nn, gen_gs);
     sii_message (gen_gs->str);
      break;

    case EDIT_HLP_SHORTCUTS:
     nn = sizeof (edit_cmd_editing)/sizeof (char *);
     sii_glom_strings (edit_cmd_editing, nn, gen_gs);
     sii_message (gen_gs->str);
      break;

    case EDIT_HLP_BNDS:
     nn = sizeof (edit_bnds)/sizeof (char *);
     sii_glom_strings (edit_bnds, nn, gen_gs);
     sii_message (gen_gs->str);
      break;

    case EDIT_HLP_EXTEND:
     nn = sizeof (edit_start_stop)/sizeof (char *);
     sii_glom_strings (edit_start_stop, nn, gen_gs);
     sii_message (gen_gs->str);
      break;

    case EDIT_HLP_WITH:
      show_edit_cmd_help_widget (frame_num);
      break;
   };
}

 /* c---------------------------------------------------------------------- */

void show_edit_cmd_help_widget (guint frame_num)
{
   GtkWidget *widget, *entry, *text;
   GdkPoint *ptp;
   gint x, y, wid;
   EditData *edd;
   const GString *cgs;
   const char *cc, *dir;
   
   widget = sii_get_widget_ptr (frame_num, FRAME_EDIT_CMDS_HELP);
   
   if( !widget )
     { edit_cmds_help_widget( frame_num ); }
   else {
      edd = (EditData *)frame_configs[frame_num]->edit_data;
      ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDIT_CMDS_HELP];
      x = ptp->x; y = ptp->y;
      gtk_widget_set_uposition (widget, ptp->x, ptp->y);
      gtk_widget_show (widget);
   }
}

/* c---------------------------------------------------------------------- */

int se_edit_start_stop (int frame_num, struct swp_file_input_control *sfic)
{
  gdouble dtime, tstart, tstop;
  EditData *edd = (EditData *)frame_configs[frame_num]->edit_data;
  gchar *aa, *bb, str[64], mess[256];
  const gchar *cc;
  gboolean ok = TRUE;
  gint jj, kk;

  aa = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_START_TIME]));
  strcpy (str, aa);
  g_strstrip (str);
  if (strcmp (str, "0") == 0) {
    dtime = DAY_ZERO;
    strcpy (sfic->first_sweep_text, "first");
  }
  else {
    dtime = sii_dtime_from_string (str);
    if (dtime == 0) {
      sprintf (mess, "%s is an unusable start time string\n", aa);
      ok = FALSE;
    }
  }
  edd->start_time = dtime;
  sfic->start_time = dtime;

  bb = gtk_entry_get_text (GTK_ENTRY (edd->data_widget[EDIT_STOP_TIME]));
  strcpy (str, bb);
  g_strstrip (str);
  if (strcmp (str, "-1") == 0) {
    dtime = ETERNITY;
    strcpy (sfic->last_sweep_text, "last");
  }
  else {
    dtime = sii_dtime_from_string (str);
    if (dtime == 0) {
      sprintf (mess, "%s is an unusable stop time string\n", bb);
      ok = FALSE;
    }
  }
  if (!ok) {
    sii_message (mess);
    return 0;
  }
  edd->stop_time = dtime;
  sfic->stop_time = dtime;
  return 1;
  
}

/* c---------------------------------------------------------------------- */

void show_edit_widget (GtkWidget *text, gpointer data )
{
  GtkWidget *widget, *entry;
  guint frame_num = GPOINTER_TO_UINT (data);
  GdkPoint *ptp;
  gint x, y;
  gdouble dd;
  const gchar *cc;
  EditData *edd;

  widget = sii_get_widget_ptr (frame_num, FRAME_EDITOR);

  if( !widget )
    { main_edit_widget( frame_num );}

  else {
    edd = (EditData *)frame_configs[frame_num]->edit_data;
    sii_edit_reset_times (frame_num);

    ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDITOR];
    x = ptp->x; y = ptp->y;
    gtk_widget_set_uposition (widget, ptp->x, ptp->y);
    gtk_widget_show (widget);
    edd->frame_active = TRUE;
  }
}

 /* c---------------------------------------------------------------------- */

void show_edit_files_widget (guint frame_num)
{
  GtkWidget *widget, *entry, *text;
  GdkPoint *ptp;
  gint x, y, wid;
  EditData *edd;
  const GString *cgs;
  const char *cc, *dir;

  widget = sii_get_widget_ptr (frame_num, FRAME_EDIT_FILES);

  if( !widget )
    { edit_files_widget( frame_num ); }
  else {
    edd = (EditData *)frame_configs[frame_num]->edit_data;

    ptp = &frame_configs[frame_num]->widget_origin[FRAME_EDIT_FILES];
    x = ptp->x; y = ptp->y;
    gtk_widget_set_uposition (widget, ptp->x, ptp->y);
    gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */

void sii_edit_click_in_data (gint frame_num)
{
  struct boundary_stuff *sebs, *return_se_bnd_struct();
  GtkWidget *widget;
  struct solo_click_info sci;
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  EditData *edd = (EditData *)frame_configs[frame_num]->edit_data;
  double xx, yy, theta, rr;

  if (!sii_get_widget_ptr (frame_num, FRAME_EDITOR) ||
      !edd->frame_active || !edd->toggle[EDIT_BND_DRAW])
    { return; }

  theta = RADIANS (CART_ANGLE (wwptr->clicked_angle));
  rr = KM_TO_M (wwptr->clicked_range);

  xx = rr * cos (theta);
  xx += (xx < 0) ? -.5 : .5;
  yy = rr * sin (theta);
  yy += (yy < 0) ? -.5 : .5;

  /* (x,y) should be meters relative to the local radar */

  sci.x = (gint)xx;
  sci.y = (gint)yy;

  if (wwptr->view->type_of_plot & SOLO_TIME_SERIES) {
     sci.x = wwptr->clicked_x_pxl;
     sci.y = wwptr->clicked_y_pxl;
  }

  sci.frame = frame_num;
  sebs = return_se_bnd_struct();
  xse_add_bnd_pt(&sci, sebs->current_boundary);
  sebs->last_operation = BND_POINT_APPENDED;
}

/* c---------------------------------------------------------------------- */

void boundary_buttons_cb (GtkWidget *text, gpointer data )
{
  GtkWidget *widget;
  guint num = GPOINTER_TO_UINT (data);
  guint frame_num, task;

  frame_num = num/TASK_MODULO;
  task = num % TASK_MODULO;

  switch (task) {
  case EDIT_CANCEL:
     widget = sii_get_widget_ptr (frame_num, FRAME_EDITOR);
     if( widget )
       { gtk_widget_hide (widget); }
     break;
  };
}

/* c---------------------------------------------------------------------- */

guint edit_text_event_cb(GtkWidget *text, GdkEvent *event
			      , gpointer data )
{
# ifdef notuseful
  guint position = gtk_text_get_point (GTK_TEXT (text));
# endif
  guint frame_num;
  gint mm, nn, cndx;		/* GPOINTER_TO_UINT (data) */
  gchar str[256], *aa, *bb, *cc, *line;
  GtkWidget *cmds;

  cmds = (GtkWidget *)gtk_object_get_data (GTK_OBJECT(text), "cmds_dest" );
  frame_num = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(text), "frame_num" ));

  aa = (gchar *)data;
  nn = GTK_TEXT (text)->cursor_mark.index;
  line = sii_nab_line_from_text (aa, nn);
  strcat( line, "\n" );

  gtk_text_insert (GTK_TEXT (cmds)
		   , edit_font
		   , edit_fore
		   , edit_back
		   , line
		   , strlen (line )
		   );
  return FALSE;
}
 
/* c---------------------------------------------------------------------- */

guint sii_edit_help_event_cb(GtkWidget *text, GdkEvent *event
			     , gpointer data )
{
  guint frame_num;
  gint mm, nn, nt, start, end;
  gchar str[256], *sptrs[32], *aa, *bb, *cc, *line;
  gboolean ok;

  aa = (gchar *)data;
  nn = GTK_TEXT (text)->cursor_mark.index;

  ok = sii_nab_region_from_text (aa, nn, &start, &end);
  if (ok) {
     gtk_editable_select_region (GTK_EDITABLE (text), start, end+1);
     gtk_editable_copy_clipboard (GTK_EDITABLE (text));
  }

  return FALSE;
}
 
/* c---------------------------------------------------------------------- */

guint edit_file_text_event_cb (GtkWidget *text, GdkEvent *event
			       , gpointer data )
{
  guint num = GPOINTER_TO_UINT (data);
  guint frame_num, task, wid, wid2, tid;
  gint mm, nn, cndx, jj;
  gchar str[256], *aa, *bb, *cc, *line;
  char *buf;
  int size;
  GtkWidget *entry, *text2;
  EditData *edd;
  gint start, end;
  gboolean ok;
  static GString *gs = NULL;
  struct sed_command_files *scf, *se_return_cmd_files_struct();
  struct solo_list_mgmt *slm, *se_update_list();

  if (!gs)
    { gs = g_string_new (""); }

  frame_num = num/TASK_MODULO;
  edd = (EditData *)frame_configs[frame_num]->edit_data;
  wid = task = num % TASK_MODULO;
	
  nn = GTK_TEXT (text)->cursor_mark.index;
  ok = sii_nab_region_from_text (edd->orig_txt[wid]->str, nn, &start, &end);

  if (ok) {
    gtk_editable_select_region (GTK_EDITABLE (text), start, end);
    line = sii_nab_line_from_text (edd->orig_txt[wid]->str, nn);
  }
  if (wid == EDIT_CMD_FILES) {
     scf = se_return_cmd_files_struct();
     strcpy (scf->directory_text, edd->orig_txt[EDIT_CMD_DIR]->str); 
     strcpy (scf->file_name_text, line);
     se_really_readin_cmds();
     slm = se_update_list(CURRENT_CMDS_LIST);
     g_string_truncate (gs, 0);

     for (jj=0; jj < slm->num_entries; jj++) {
	aa = solo_list_entry (slm, jj);
	if (strstr (aa, "for-each-ray"))
	  { break; }
	if (*aa == '!')
	  { continue; }
	
	strcpy (str, aa);
	g_strchomp (str);
	g_string_append (gs, str);
	g_string_append (gs, "\n");
     }
     if (gs->len) {
	gtk_text_insert (GTK_TEXT (edd->data_widget[EDIT_OTO_TEXT])
			 , edit_font, edit_fore, edit_back
			 , gs->str, -1);
     }	
     g_string_truncate (gs, 0);
     for (; jj < slm->num_entries; jj++) {
	aa = solo_list_entry (slm, jj);
	if (*aa == '!')
	  { continue; }
	
	strcpy (str, aa);
	g_strchomp (str);
	g_string_append (gs, str);
	g_string_append (gs, "\n");
     }
     if (gs->len) {
	gtk_text_insert (GTK_TEXT (edd->data_widget[EDIT_FER_TEXT])
			 , edit_font, edit_fore, edit_back
			 , gs->str, -1);
     }

# ifdef obsolete
     ok = sii_nab_command_set
       (frame_num, edd->orig_txt[EDIT_CMD_DIR]->str, line);
# endif
  }
  else if (wid == EDIT_LLB_FILES) {
    strcpy (str, edd->orig_txt[EDIT_BND_DIR]->str);
    strcat (str, "/");
    strcat (str, line);
    buf = absorb_zmap_bnd (str, 0, &size);
    if (buf) {
      se_unpack_bnd_buf( buf, size );
      free( buf );
    }
  }
  else {
    ok = sii_nab_boundary
      (frame_num, edd->orig_txt[EDIT_BND_DIR]->str, line);
  }
  return FALSE;
}
 
/* c---------------------------------------------------------------------- */

void edit_files_widget (guint frame_num )
{
  GtkWidget *label;

  GtkWidget *window;
  GtkWidget *vbox0;
  GtkWidget *hbox;
  GtkWidget *text;
  GtkWidget *table;
  GtkWidget *table2;
  GtkWidget *entry;
  GtkWidget *vscrollbar;
  GtkWidget *button;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 0;
  guint ypadding = 0;

  gint event_flags = GDK_BUTTON_PRESS_MASK;
  gchar str[128], *aa, *bb;
  gint mark, row, nn, wid;
  EditData *edd;
  const char *cc;
  const GString *cgs;
  gint x, y;
  GdkPoint *ptp;
				/* c...code */

  edd = (EditData *)frame_configs[frame_num]->edit_data;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  sii_set_widget_ptr ( frame_num, FRAME_EDIT_FILES, window );
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);
  
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num*TASK_MODULO+FRAME_EDIT_FILES));
  bb = g_strdup_printf ("Frame %d  Edit Command and Boundary Files"
			, frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 2);

  
  vbox0 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(window), vbox0);

  hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (vbox0), hbox);
  gtk_container_border_width (GTK_CONTAINER (hbox), 4);

  button = gtk_button_new_with_label ("Save Commands");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + EDIT_SAVE_CMDS;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("Save Boundary");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + EDIT_SAVE_BND;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("Cancel");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + EDIT_FILES_CANCEL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  label = gtk_label_new ( "Editor Command Files" );
  gtk_container_add (GTK_CONTAINER (vbox0), label);

  table = gtk_table_new (6, 4, FALSE);
  gtk_container_border_width (GTK_CONTAINER (table), 0);
  gtk_container_add (GTK_CONTAINER(vbox0), table);
  row = -1;
  ypadding = 2;

  row++;
  label = gtk_label_new ( " Directory " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  wid = EDIT_CMD_DIR;
  entry = gtk_entry_new ();
  edd->data_widget[wid] = entry;
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  gtk_entry_set_text (GTK_ENTRY (entry), edd->orig_txt[wid]->str );
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn);

  row++;
  label = gtk_label_new ( "Comment" );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  wid = EDIT_CMD_COMM;
  aa = "_no_comment";
  edd->orig_txt[wid] = g_string_new (aa);
  edd->txt[wid] = g_string_new (aa);
  entry = gtk_entry_new ();
  edd->data_widget[wid] = entry;
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  gtk_entry_set_text (GTK_ENTRY (entry), aa );
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn);

  
  cc = edd->orig_txt[EDIT_CMD_DIR]->str;
  cgs = sii_return_edit_cmd_files (frame_num, cc);

  row++;
  table2 = gtk_table_new (2, 1, FALSE);
  wid = EDIT_CMD_FILES;
  edd->orig_txt[wid] = g_string_new (cgs->str);
  text = gtk_text_new (NULL,NULL);
  gtk_widget_set_usize (text, 400, 72);
  gtk_widget_set_events (text, event_flags );
  edd->data_widget[wid] = text;
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect_after (GTK_OBJECT(text)
			    , "button_press_event"
			    , (GtkSignalFunc) edit_file_text_event_cb
			    , (gpointer)nn);
  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , edd->orig_txt[wid]->str, -1);

  gtk_table_attach (GTK_TABLE (table2), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table2), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), FALSE);

  gtk_table_attach (GTK_TABLE (table), table2, 0, 6, row++, row+1,
		    xoptions, yoptions, xpadding, ypadding );



  label = gtk_label_new ( "Editor Boundary Files" );
  gtk_container_add (GTK_CONTAINER (vbox0), label);

  table = gtk_table_new (6, 4, FALSE);
  gtk_container_border_width (GTK_CONTAINER (table), 0);
  gtk_container_add (GTK_CONTAINER(vbox0), table);
  row = -1;
  ypadding = 2;

  row++;
  label = gtk_label_new ( " Directory " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  wid = EDIT_BND_DIR;
  entry = gtk_entry_new ();
  edd->data_widget[wid] = entry;
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  gtk_entry_set_text (GTK_ENTRY (entry), edd->orig_txt[wid]->str );
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn);

  row++;
  label = gtk_label_new ( "Comment" );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  wid = EDIT_BND_COMM;
  entry = gtk_entry_new ();
  edd->data_widget[wid] = entry;
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  aa = "_no_comment";
  edd->orig_txt[wid] = g_string_new (aa);
  edd->txt[wid] = g_string_new (aa);
  gtk_entry_set_text (GTK_ENTRY (entry), aa );
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn);

  row++;

  cc = edd->orig_txt[EDIT_BND_DIR]->str;
  cgs = sii_return_edit_bnd_files (frame_num, cc);

  table2 = gtk_table_new (2, 1, FALSE);
  wid = EDIT_BND_FILES;
  edd->orig_txt[wid] = g_string_new (cgs->str);
  text = gtk_text_new (NULL,NULL);
  gtk_widget_set_usize (text, 400, 72);
  gtk_widget_set_events (text, event_flags );
  edd->data_widget[wid] = text;
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect_after (GTK_OBJECT(text)
			    , "button_press_event"
			    , (GtkSignalFunc) edit_file_text_event_cb
			    , (gpointer)nn);
  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , edd->orig_txt[wid]->str, -1);

  gtk_table_attach (GTK_TABLE (table2), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table2), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), FALSE);

  gtk_table_attach (GTK_TABLE (table), table2, 0, 6, row++, row+1,
		    xoptions, yoptions, xpadding, ypadding );


  label = gtk_label_new ( "Import ASCII Lat/Lon Boundary Files" );
  gtk_container_add (GTK_CONTAINER (vbox0), label);

  table = gtk_table_new (6, 3, FALSE);
  gtk_container_border_width (GTK_CONTAINER (table), 0);
  gtk_container_add (GTK_CONTAINER(vbox0), table);
  row = -1;
  ypadding = 2;

  row++;
  label = gtk_label_new ( " Directory " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  wid = EDIT_LLB_DIR;
  entry = gtk_entry_new ();
  edd->data_widget[wid] = entry;
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  gtk_entry_set_text (GTK_ENTRY (entry), edd->orig_txt[wid]->str );
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn);

  row++;
  cc = edd->orig_txt[EDIT_LLB_DIR]->str;
  cgs = sii_return_llb_bnd_files (frame_num, cc);

  table2 = gtk_table_new (2, 1, FALSE);
  wid = EDIT_LLB_FILES;
  edd->orig_txt[wid] = g_string_new (cgs->str);
  text = gtk_text_new (NULL,NULL);
  gtk_widget_set_usize (text, 400, 72);
  gtk_widget_set_events (text, event_flags );
  edd->data_widget[wid] = text;
  nn = frame_num * TASK_MODULO + wid;
  gtk_signal_connect_after (GTK_OBJECT(text)
			    , "button_press_event"
			    , (GtkSignalFunc) edit_file_text_event_cb
			    , (gpointer)nn);
  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , edd->orig_txt[wid]->str, -1);

  gtk_table_attach (GTK_TABLE (table2), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table2), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), FALSE);

  gtk_table_attach (GTK_TABLE (table), table2, 0, 6, row++, row+1,
		    xoptions, yoptions, xpadding, ypadding );




  /* --- Make everything visible --- */
  gtk_widget_show_all (window);

}

/* c---------------------------------------------------------------------- */

void main_edit_widget( guint frame_num )
{
  GtkWidget *label;

  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *edit_menubar;
  GtkWidget *hbox;
  GtkWidget *hbox1;
  GtkWidget *hbox2;

  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *vbox3;

  GtkWidget *text;
  GtkWidget *table;
  GtkWidget *entry;
  GtkWidget *hscrollbar;
  GtkWidget *vscrollbar;

  GtkWidget *menubar;
  GtkWidget *menu;
  GtkWidget *menuitem;

  GtkWidget *button;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 2;
  guint ypadding = 4;
  guint row = 0;

  char *aa;
  gdouble dd;
  gchar str[256], str2[256], *bb, *ee;
  gint event_flags = GDK_BUTTON_PRESS_MASK;
  guint wid;
  const gchar *cc;
  gint length, width, nn, jj;
  gfloat upper;
  GdkPoint *ptp;
  gint x, y;
  struct solo_edit_stuff *seds;
  struct solo_list_mgmt *slm;
  GString *gs;

				/* c...code */

  seds = return_sed_stuff();
  if( !frame_configs[frame_num]->edit_data ) {
     frame_configs[frame_num]->edit_data = g_malloc0 (sizeof( EditData ));
  }
  edata = (EditData *)frame_configs[frame_num]->edit_data;
  edata->frame_active = TRUE;

  cc = sii_return_swpfi_dir (frame_num);
  wid = EDIT_CMD_DIR;
  edata->orig_txt[wid] = g_string_new (cc);
  edata->txt[wid] = g_string_new (cc);

  wid = EDIT_BND_DIR;
  edata->orig_txt[wid] = g_string_new (cc);
  edata->txt[wid] = g_string_new (cc);

  wid = EDIT_LLB_DIR;
  edata->orig_txt[wid] = g_string_new (cc);
  edata->txt[wid] = g_string_new (cc);
  

  width = gdk_string_width( small_pro_font, title_tmplt );

  edit_font = med_pro_font;
  edit_fore = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"black");
  edit_back = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"white");


  /* find the max width of all the for-each-ray lines */

  if (!cmds_one_time_only) {
     slm = seds->all_other_cmds;
     cmds_one_time_only = g_string_new ("");

     for (jj = 0; jj < slm->num_entries; jj++) {
	aa = solo_list_entry (slm, jj);
	strcpy (str, aa);
	strcat (str, "\n");
	g_string_append(cmds_one_time_only, str);
     }
     slm = seds->all_fer_cmds;
     cmds_for_each_ray = g_string_new ("");

     for (jj = 0; jj < slm->num_entries; jj++) {
	aa = solo_list_entry (slm, jj);
	strcpy (str, aa);
	strcat (str, "\n");
	g_string_append(cmds_for_each_ray, str);
	width = gdk_string_width( edit_font, str );
	if( width > max_cmd_width )
	  { max_cmd_width = width; }
     }
     max_cmd_width += gdk_string_width( edit_font, "WWW" );
  }

  ee = str;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  sii_set_widget_ptr ( frame_num, FRAME_EDITOR, window );
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num * TASK_MODULO + FRAME_EDITOR));

  /* --- Title and border --- */
  bb = g_strdup_printf ("Frame %d  Edit Widget", frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  sii_edit_menubar2 (window, &edit_menubar, frame_num);
  gtk_container_add (GTK_CONTAINER(vbox), edit_menubar);

  /* --- Create a new horizontal box for storing 3 sets of widgets --- */
  hbox = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER(vbox), hbox);
    
  /* menu buttons */
  vbox1 = gtk_vbox_new (FALSE, 1);
  gtk_container_border_width (GTK_CONTAINER (vbox1), 2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox1, TRUE, TRUE, 0);

  table = gtk_table_new (6, 1, TRUE);
  gtk_box_pack_start (GTK_BOX (vbox1), table, FALSE, TRUE, 0 );
  row = -1;


  button = gtk_button_new_with_label ("Clear Bnd");
  row++;
  gtk_table_attach (GTK_TABLE (table), button, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + EDIT_CLEAR_BND;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("OK Do It!");
  row++;
  gtk_table_attach (GTK_TABLE (table), button, 0, 1, row, row+2,
		    xoptions, yoptions, xpadding, ypadding ); row++;
  nn = frame_num*TASK_MODULO + EDIT_DO_IT;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );
# ifdef obsolete
  button = gtk_button_new_with_label ("Redraw Bnd");
  row++;
  gtk_table_attach (GTK_TABLE (table), button, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + EDIT_REDRAW_BND;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );
# endif

  button = gtk_button_new_with_label (" Add Next Bnd ");
  gtk_widget_set_usize (button, 0, 32);
  row++;
  gtk_table_attach (GTK_TABLE (table), button, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + EDIT_ADD_NEXT_BND;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label (" Prev Bnd Set ");
  row++;
  gtk_table_attach (GTK_TABLE (table), button, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + EDIT_PREV_BND_SET;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  /* commands for each ray */

  vbox1 = gtk_vbox_new (FALSE, 1);
  gtk_container_border_width (GTK_CONTAINER (vbox1), 2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox1, TRUE, TRUE, 0);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER(vbox1), hbox1);

  label = gtk_label_new ( "Commands For Each Ray" );
  gtk_widget_set_usize (label, 0, 32);
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER );
  gtk_box_pack_start (GTK_BOX (hbox1), label, TRUE, TRUE, 0);

  button = gtk_button_new_with_label ("Clear");
  gtk_widget_set_usize (button, 0, 28);
  gtk_box_pack_start (GTK_BOX (hbox1), button, TRUE, TRUE, 0);
  nn = frame_num*TASK_MODULO + EDIT_CLEAR_FER;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  table = gtk_table_new (2, 1, FALSE);
  gtk_widget_set_usize (table, max_cmd_width, 112);
  gtk_container_add (GTK_CONTAINER (vbox1), table);

  edata->data_widget[EDIT_FER_TEXT] = text = gtk_text_new (NULL,NULL);

  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), TRUE);


  label = gtk_label_new ( "List of All For Each Ray Commands" );
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER );
  gtk_container_add (GTK_CONTAINER (vbox1), label);

  table = gtk_table_new (2, 1, FALSE);
  gtk_widget_set_usize (table, max_cmd_width, 168);
  gtk_container_add (GTK_CONTAINER (vbox1), table);

  text = gtk_text_new (NULL,NULL);
  gtk_widget_set_events (text, event_flags );


  gtk_object_set_data (GTK_OBJECT(text),
		       "frame_num",
		       (gpointer)frame_num);

  gtk_object_set_data (GTK_OBJECT(text),
		       "cmds_dest",
		       (gpointer)edata->data_widget[EDIT_FER_TEXT]);

  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), FALSE);

  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , cmds_for_each_ray->str, -1);

  gtk_signal_connect (GTK_OBJECT(text)
		      ,"button_release_event"
		      , (GtkSignalFunc) edit_text_event_cb
		      , (gpointer)cmds_for_each_ray->str);

 /* one time only commands */

  vbox3 = gtk_vbox_new (FALSE, 1);
  gtk_container_border_width (GTK_CONTAINER (vbox3), 2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox3, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER(vbox3), hbox2);

  label = gtk_label_new ( "One Time Only Commands" );
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER );
  gtk_box_pack_start (GTK_BOX (hbox2), label, TRUE, TRUE, 0);

  button = gtk_button_new_with_label ("Clear");
  gtk_widget_set_usize (button, 0, 28);
  gtk_box_pack_start (GTK_BOX (hbox2), button, TRUE, TRUE, 0);
  nn = frame_num*TASK_MODULO + EDIT_CLEAR_OTO;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );


  table = gtk_table_new (2, 1, FALSE);
  gtk_widget_set_usize (table, max_cmd_width, 112);
  gtk_container_add (GTK_CONTAINER (vbox3), table);

  edata->data_widget[EDIT_OTO_TEXT] = text = gtk_text_new (NULL,NULL);

  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), TRUE);


  label = gtk_label_new ( "List of All One Time Only Commands" );
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER );
  gtk_container_add (GTK_CONTAINER (vbox3), label);

  table = gtk_table_new (2, 1, FALSE);
  gtk_widget_set_usize (table, max_cmd_width, 168);
  gtk_container_add (GTK_CONTAINER (vbox3), table);

  text = gtk_text_new (NULL,NULL);
  gtk_widget_set_events (text, event_flags );

  gtk_object_set_data (GTK_OBJECT(text),
		       "frame_num",
		       (gpointer)frame_num);
  gtk_object_set_data (GTK_OBJECT(text),
		       "cmds_dest",
		       (gpointer)edata->data_widget[EDIT_OTO_TEXT]);

  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);


  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), FALSE);

  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , cmds_one_time_only->str, -1);
  gtk_signal_connect (GTK_OBJECT(text)
		      ,"button_release_event"
		      , (GtkSignalFunc) edit_text_event_cb
		      , (gpointer)cmds_one_time_only->str);


  table = gtk_table_new (2, 6, TRUE);
  gtk_container_add (GTK_CONTAINER (vbox), table);
  row = -1;

  dd = sii_return_swpfi_time_stamp (frame_num);
  cc = sii_string_from_dtime (dd);

  row++;
  label = gtk_label_new ( " Start Time " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  wid = EDIT_START_TIME;
  edata->orig_txt[wid] = g_string_new (cc);
  edata->txt[wid] = g_string_new (cc);
  entry = gtk_entry_new ();
  edata->data_widget[wid] = entry;
  gtk_entry_set_text (GTK_ENTRY (entry), cc );
  gtk_table_attach (GTK_TABLE (table), entry, 1, 3, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );
  wid = EDIT_FIRST_SWP;
  button = gtk_button_new_with_label ("First Sweep");
  gtk_table_attach (GTK_TABLE (table), button, 3, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  row++;
  label = gtk_label_new ( " Stop Time " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  wid = EDIT_STOP_TIME;
  edata->orig_txt[wid] = g_string_new (cc);
  edata->txt[wid] = g_string_new (cc);
  entry = gtk_entry_new ();
  edata->data_widget[wid] = entry;
  gtk_entry_set_text (GTK_ENTRY (entry), cc );
  gtk_table_attach (GTK_TABLE (table), entry, 1, 3, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );
  wid = EDIT_LAST_SWP;
  button = gtk_button_new_with_label ("Last Sweep");
  gtk_table_attach (GTK_TABLE (table), button, 3, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  nn = frame_num*TASK_MODULO + wid;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );

  sii_edit_reset_times (frame_num);
  

  gtk_container_foreach (GTK_CONTAINER (vbox),
			 sii_set_widget_frame_num,
			 (gpointer)frame_num );

  /* --- Make everything visible --- */
  gtk_widget_show_all (window);

}

/* c---------------------------------------------------------------------- */

void sii_edit_menubar2( GtkWidget  *window,
                    GtkWidget **menubar, guint frame_num )
{
   gint jj, nn, pdi, task, active, widget_id;
   guint radio_num = 0;
   GtkWidget *check_item, *mbar, *menuitem, *submenu, *next_item;

   radio_group = NULL;
   
   *menubar = mbar = gtk_menu_bar_new ();

   submenu = sii_submenu ( "File", mbar );
   
   
   widget_id = EDIT_CLOSE;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Close", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = EDIT_RESET_TIMES;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Reset Times", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

# ifdef notyet
   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = EDIT_PREV_SWP_SET;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Use Last Sweep Set", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
   
   widget_id = EDIT_SAVE_CMDS;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Save Commands", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
   
   widget_id = EDIT_SAVE_BND;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Save Boundary", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
# endif

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
   
   widget_id = EDIT_CMD_BND_FILES;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Save/List Cmds & Bnds ", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
   
   widget_id = EDIT_IMPORT_LLB_FILES;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Import Lat/Lon Bnds", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
   
   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = EDIT_AUTO_REPLOT; /* Check Item */
   edata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Auto Replot", submenu, widget_id
		       , sii_edit_menu_cb, frame_num, 0, &radio_group );


   submenu = sii_submenu ( "Boundary", mbar );
   radio_group = NULL;
   radio_num = 0;

   widget_id = EDIT_BND_IN;
   edata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Edit Inside", submenu, widget_id
		       , sii_edit_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   widget_id = EDIT_BND_OUT;
   edata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Edit Outside", submenu, widget_id
		       , sii_edit_menu_cb, frame_num, ++radio_num
			       , &radio_group );

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = EDIT_BND_DRAW;
   edata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Draw Boundary", submenu, widget_id
		       , sii_edit_menu_cb, frame_num, 0, &radio_group );


# ifdef notyet
   widget_id = EDIT_BND_PREV;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Get Previous Bnd Set", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );


   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = EDIT_BND_LIST;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "List Bnd Points", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
# endif

   widget_id = EDIT_REPLOT_LINKS;
   menuitem = gtk_menu_item_new_with_label ("Replot");
   gtk_menu_bar_append (GTK_MENU_BAR (mbar), menuitem );
   nn = frame_num * TASK_MODULO + widget_id;
   gtk_signal_connect (GTK_OBJECT(menuitem)
		       , "activate"
		       , (GtkSignalFunc)sii_edit_menu_cb 
		       , (gpointer)nn);

   gtk_object_set_data (GTK_OBJECT(menuitem)
			, "widget_id", (gpointer)widget_id);
   gtk_object_set_data (GTK_OBJECT(menuitem)
			, "frame_num", (gpointer)frame_num);


   submenu = sii_submenu ( "Examples", mbar );

   widget_id = EDIT_EG_BB_UNF;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Bargen-Brown-unfolding", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_EG_FLG_FRK;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "flag-freckles", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_EG_FLG_GLTCH;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "flag-glitches", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_EG_REG_HST;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "regular-histogram", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_EG_IRREG_HST;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "irregular-histogram", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_EG_RSHEAR;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "radial-shear", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_EG_THRSH;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "thresholding", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );



   submenu = sii_submenu ( "Help", mbar );

   widget_id = EDIT_HLP_OVERVIEW;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Overview Frame", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_HLP_FILE;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "With File Menu", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );

   widget_id = EDIT_HLP_SHORTCUTS;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "With Cmd Editing", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
  
   widget_id = EDIT_HLP_BNDS;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "With Boundaries", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
  
   widget_id = EDIT_HLP_EXTEND;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "With Star-Stop Times", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
# ifdef notyet
   widget_id = EDIT_HLP_EG;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Examples", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );
# endif
   widget_id = EDIT_HLP_WITH;
   edata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Cmds with Help", submenu, widget_id
		       , sii_edit_menu_cb, frame_num );


   widget_id = EDIT_CANCEL;
   menuitem = gtk_menu_item_new_with_label ("Cancel");
   gtk_menu_bar_append (GTK_MENU_BAR (mbar), menuitem );
   nn = frame_num * TASK_MODULO + widget_id;
   gtk_signal_connect (GTK_OBJECT(menuitem)
		       , "activate"
		       , (GtkSignalFunc)sii_edit_menu_cb 
		       , (gpointer)nn);

   gtk_object_set_data (GTK_OBJECT(menuitem)
			, "widget_id", (gpointer)widget_id);
   gtk_object_set_data (GTK_OBJECT(menuitem)
			, "frame_num", (gpointer)frame_num);

  check_item = edata->data_widget[EDIT_BND_DRAW];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  check_item = edata->data_widget[EDIT_AUTO_REPLOT];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

void edit_cmds_help_widget (guint frame_num )
{
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *event;

  gint event_flags = GDK_BUTTON_PRESS_MASK;
  gchar str[128], *aa, *bb;
  gint mark, row, nn, wid;
  EditData *edd;
  const char *cc;
  const GString *cgs;
  gint x, y;
  GdkPoint *ptp;
				/* c...code */

  edd = (EditData *)frame_configs[frame_num]->edit_data;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  sii_set_widget_ptr ( frame_num, FRAME_EDIT_CMDS_HELP, window );
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);
  
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num*TASK_MODULO+FRAME_EDIT_CMDS_HELP));

  bb = g_strdup_printf ("Frame %d  Edit Commands Help List"
			, frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 2);

  vbox_hlp_cmds = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(window), vbox_hlp_cmds);

  button = gtk_button_new_with_label ("Cancel");
  gtk_container_add (GTK_CONTAINER(vbox_hlp_cmds), button );

  nn = frame_num*TASK_MODULO + EDIT_HLP_CANCEL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_edit_menu_cb
		      , (gpointer)nn
		      );
  
  nn = sizeof (BB_ac_unfolding)/sizeof (char *);
  sii_glom_strings (BB_ac_unfolding, nn, gen_gs);
  put_edit_cmds_help ("BB-ac-unfolding", gen_gs);

  nn = sizeof (absolute_value)/sizeof (char *);
  sii_glom_strings (absolute_value, nn, gen_gs);
  put_edit_cmds_help ("absolute-value", gen_gs);

  nn = sizeof (and_bad_flags)/sizeof (char *);
  sii_glom_strings (and_bad_flags, nn, gen_gs);
  put_edit_cmds_help ("and-bad-flags", gen_gs);

  nn = sizeof (assert_bad_flags)/sizeof (char *);
  sii_glom_strings (assert_bad_flags, nn, gen_gs);
  put_edit_cmds_help ("assert-bad-flags", gen_gs);

  nn = sizeof (assign_value)/sizeof (char *);
  sii_glom_strings (assign_value, nn, gen_gs);
  put_edit_cmds_help ("assign-value", gen_gs);

  nn = sizeof (bad_flags_mask)/sizeof (char *);
  sii_glom_strings (bad_flags_mask, nn, gen_gs);
  put_edit_cmds_help ("bad-flags-mask", gen_gs);

  nn = sizeof (clear_bad_flags)/sizeof (char *);
  sii_glom_strings (clear_bad_flags, nn, gen_gs);
  put_edit_cmds_help ("clear-bad-flags", gen_gs);

  nn = sizeof (copy_bnd)/sizeof (char *);
  sii_glom_strings (copy_bnd, nn, gen_gs);
  put_edit_cmds_help ("copy", gen_gs);

  nn = sizeof (copy_bad_flags)/sizeof (char *);
  sii_glom_strings (copy_bad_flags, nn, gen_gs);
  put_edit_cmds_help ("copy-bad-flags", gen_gs);

  nn = sizeof (despeckle)/sizeof (char *);
  sii_glom_strings (despeckle, nn, gen_gs);
  put_edit_cmds_help ("despeckle", gen_gs);

  nn = sizeof (do_defreckling)/sizeof (char *);
  sii_glom_strings (do_defreckling, nn, gen_gs);
  put_edit_cmds_help ("do-defreckling", gen_gs);

  nn = sizeof (do_histogram)/sizeof (char *);
  sii_glom_strings (do_histogram, nn, gen_gs);
  put_edit_cmds_help ("do-histogram", gen_gs);

  nn = sizeof (duplicate)/sizeof (char *);
  sii_glom_strings (duplicate, nn, gen_gs);
  put_edit_cmds_help ("duplicate", gen_gs);

  nn = sizeof (establish_and_reset)/sizeof (char *);
  sii_glom_strings (establish_and_reset, nn, gen_gs);
  put_edit_cmds_help ("establish-and-reset", gen_gs);

  nn = sizeof (fix_vortex_velocities)/sizeof (char *);
  sii_glom_strings (fix_vortex_velocities, nn, gen_gs);
  put_edit_cmds_help ("fix-vortex-velocities", gen_gs);

  nn = sizeof (flag_freckles)/sizeof (char *);
  sii_glom_strings (flag_freckles, nn, gen_gs);
  put_edit_cmds_help ("flag-freckles", gen_gs);

  nn = sizeof (flag_glitches)/sizeof (char *);
  sii_glom_strings (flag_glitches, nn, gen_gs);
  put_edit_cmds_help ("flag-glitches", gen_gs);

  nn = sizeof (flagged_add)/sizeof (char *);
  sii_glom_strings (flagged_add, nn, gen_gs);
  put_edit_cmds_help ("flagged-add", gen_gs);

  nn = sizeof (flagged_multiply)/sizeof (char *);
  sii_glom_strings (flagged_multiply, nn, gen_gs);
  put_edit_cmds_help ("flagged-multiply", gen_gs);

  nn = sizeof (forced_unfolding)/sizeof (char *);
  sii_glom_strings (forced_unfolding, nn, gen_gs);
  put_edit_cmds_help ("forced-unfolding", gen_gs);

  nn = sizeof (header_value)/sizeof (char *);
  sii_glom_strings (header_value, nn, gen_gs);
  put_edit_cmds_help ("header-value", gen_gs);

  nn = sizeof (ignore_field)/sizeof (char *);
  sii_glom_strings (ignore_field, nn, gen_gs);
  put_edit_cmds_help ("ignore-field", gen_gs);

  nn = sizeof (merge_field)/sizeof (char *);
  sii_glom_strings (merge_field, nn, gen_gs);
  put_edit_cmds_help ("merge-field", gen_gs);

  nn = sizeof (one_time)/sizeof (char *);
  sii_glom_strings (one_time, nn, gen_gs);
  put_edit_cmds_help ("one-time", gen_gs);

  nn = sizeof (or_bad_flags)/sizeof (char *);
  sii_glom_strings (or_bad_flags, nn, gen_gs);
  put_edit_cmds_help ("or-bad-flags", gen_gs);

  nn = sizeof (radial_shear)/sizeof (char *);
  sii_glom_strings (radial_shear, nn, gen_gs);
  put_edit_cmds_help ("radial-shear", gen_gs);

  nn = sizeof (remove_aircraft_motion)/sizeof (char *);
  sii_glom_strings (remove_aircraft_motion, nn, gen_gs);
  put_edit_cmds_help ("remove-aircraft-motion", gen_gs);

  nn = sizeof (remove_ring)/sizeof (char *);
  sii_glom_strings (remove_ring, nn, gen_gs);
  put_edit_cmds_help ("remove-ring", gen_gs);

  nn = sizeof (remove_storm_motion)/sizeof (char *);
  sii_glom_strings (remove_storm_motion, nn, gen_gs);
  put_edit_cmds_help ("remove-storm-motion", gen_gs);

  nn = sizeof (rewrite)/sizeof (char *);
  sii_glom_strings (rewrite, nn, gen_gs);
  put_edit_cmds_help ("rewrite", gen_gs);

  nn = sizeof (remove_surface)/sizeof (char *);
  sii_glom_strings (remove_surface, nn, gen_gs);
  put_edit_cmds_help ("remove-surface", gen_gs);

  nn = sizeof (set_bad_flags)/sizeof (char *);
  sii_glom_strings (set_bad_flags, nn, gen_gs);
  put_edit_cmds_help ("set-bad-flags", gen_gs);

  nn = sizeof (threshold)/sizeof (char *);
  sii_glom_strings (threshold, nn, gen_gs);
  put_edit_cmds_help ("threshold", gen_gs);

  nn = sizeof (unconditional_delete)/sizeof (char *);
  sii_glom_strings (unconditional_delete, nn, gen_gs);
  put_edit_cmds_help ("unconditional-delete", gen_gs);

  nn = sizeof (xor_bad_flags)/sizeof (char *);
  sii_glom_strings (xor_bad_flags, nn, gen_gs);
  put_edit_cmds_help ("xor-bad-flags", gen_gs);

  nn = sizeof (xy_listing)/sizeof (char *);
  sii_glom_strings (xy_listing, nn, gen_gs);
  put_edit_cmds_help ("xy-listing", gen_gs);


# ifdef obsolete
  put_edit_cmds_help ("BB-ac-unfolding", (gpointer)BB_ac_unfolding);
  put_edit_cmds_help ("absolute-value", (gpointer)absolute_value);
  put_edit_cmds_help ("and-bad-flags", (gpointer)and_bad_flags);
  put_edit_cmds_help ("assert-bad-flags", (gpointer)assert_bad_flags);
  put_edit_cmds_help ("assign-value", (gpointer)assign_value);
  put_edit_cmds_help ("bad-flags-mask", (gpointer)bad_flags_mask);
  put_edit_cmds_help ("clear-bad-flags", (gpointer)clear_bad_flags);
  put_edit_cmds_help ("copy", (gpointer)copy_bnd);
  put_edit_cmds_help ("copy-bad-flags", (gpointer)copy_bad_flags);
  put_edit_cmds_help ("despeckle", (gpointer)despeckle);
  put_edit_cmds_help ("do-defreckling", (gpointer)do_defreckling);
  put_edit_cmds_help ("do-histogram", (gpointer)do_histogram);
  put_edit_cmds_help ("duplicate", (gpointer)duplicate);
  put_edit_cmds_help ("establish-and-reset", (gpointer)establish_and_reset);
  put_edit_cmds_help ("fix-vortex-velocities", (gpointer)fix_vortex_velocities);
  put_edit_cmds_help ("flag-freckles", (gpointer)flag_freckles);
  put_edit_cmds_help ("flag-glitches", (gpointer)flag_glitches);
  put_edit_cmds_help ("flagged-add", (gpointer)flagged_add);
  put_edit_cmds_help ("flagged-multiply", (gpointer)flagged_multiply);
  put_edit_cmds_help ("forced-unfolding", (gpointer)forced_unfolding);
  put_edit_cmds_help ("header-value", (gpointer)header_value);
  put_edit_cmds_help ("ignore-field", (gpointer)ignore_field);
  put_edit_cmds_help ("merge-field", (gpointer)merge_field);
  put_edit_cmds_help ("one-time", (gpointer)one_time);
  put_edit_cmds_help ("or-bad-flags", (gpointer)or_bad_flags);
  put_edit_cmds_help ("radial-shear", (gpointer)radial_shear);
  put_edit_cmds_help ("remove-aircraft-motion", (gpointer)remove_aircraft_motion);
  put_edit_cmds_help ("remove-ring", (gpointer)remove_ring);
  put_edit_cmds_help ("remove-surface", (gpointer)remove_surface);
  put_edit_cmds_help ("set-bad-flags", (gpointer)set_bad_flags);
  put_edit_cmds_help ("threshold", (gpointer)threshold);
  put_edit_cmds_help ("unconditional-delete", (gpointer)unconditional_delete);
  put_edit_cmds_help ("xor-bad-flags", (gpointer)xor_bad_flags);
  put_edit_cmds_help ("xy-listing", (gpointer)xy_listing);
# endif

  g_tree_traverse ( edit_cmds_help
		   , (gpointer)set_edit_help_items, G_IN_ORDER, 0 );
  gtk_widget_show_all (window);

}
 
/* c---------------------------------------------------------------------- */

gboolean set_edit_help_items( gpointer a_str, gpointer words )
{
  GtkWidget *label;
  GtkWidget *event;
  gint event_flags = GDK_BUTTON_PRESS_MASK;

  event = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (vbox_hlp_cmds), event);
  gtk_widget_set_events (event, event_flags );
  label = gtk_label_new ((gchar *)a_str);
  gtk_label_set_justify ((GtkLabel *)label, GTK_JUSTIFY_LEFT );
  gtk_container_add (GTK_CONTAINER (event), label);
  gtk_signal_connect (GTK_OBJECT(event),"button_press_event",
		      (GtkSignalFunc)sii_cmds_help_event 
		      , (gpointer)words);

  return( FALSE );		/* to continue a tree traverse */
}

/* c---------------------------------------------------------------------- */

void sii_cmds_help_event (GtkWidget *w, GdkEvent *event
			      , gpointer data )
{
   gchar *words = (gchar *)data;
   gchar str[128], *sptr[16], *aa;
   GdkModifierType state, statexy;

   edit_help_widget  ((gchar *)data);
}

/* c---------------------------------------------------------------------- */

void put_edit_cmds_help (gchar *name, GString *words)
{
   int nn;
   gchar *new_name;
   gchar *aa;

   aa = (gchar *)g_malloc0( words->len +1 );
   memcpy ((void *)aa, words->str, words->len );
   aa[words->len] = '\0';

   new_name = (gchar *)g_malloc0( strlen( name ) +1 );
   strcpy( new_name, name );
   if (!edit_cmds_help) {
      edit_cmds_help = g_tree_new ((GCompareFunc)strcmp);
   }
   g_tree_insert (edit_cmds_help, (gpointer)new_name, (gpointer)aa );
}

/* c---------------------------------------------------------------------- */

static void edit_help_widget (gchar *words)
{
  GtkWidget *label;

  GtkWidget *window;
  GtkWidget *vbox;

  GtkWidget *text;
  GtkWidget *table;
  GtkWidget *hscrollbar;
  GtkWidget *vscrollbar;

  GtkWidget *button;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 0;
  guint ypadding = 0;
  guint padding = 0, row = 0;

  gchar str[256], str2[256], *bb, *ee;
  gint event_flags = GDK_BUTTON_PRESS_MASK;
  const gchar *cc;
  gint length, width=0, height=0, max_width = 0, jj, nn;
  gfloat upper;
  GdkPoint *ptp;
  gint x, y;
  guint cancel_wid, text_wid = 0;
  char *aa = words;
				/* c...code starts here */

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);

  bb = g_strdup_printf ("Edit Command Help" );
  gtk_window_set_title (GTK_WINDOW (window), "Edit Command Help");
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  table = gtk_table_new (2, 1, FALSE);
  gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0 );

  text = gtk_text_new (NULL,NULL);
# ifdef notyet
# endif
  gtk_widget_set_usize (text, 480, 300);
  gtk_widget_set_events (text, event_flags );
  gtk_signal_connect_after (GTK_OBJECT(text)
		      ,"button_press_event"
		      , (GtkSignalFunc) sii_edit_help_event_cb
		      , (gpointer)words);


  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    xoptions, yoptions, 0, 0);
  
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    xoptions, yoptions, 0, 0);


  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , aa, -1 );

  button = gtk_button_new_with_label ("Dismiss");
  gtk_container_add (GTK_CONTAINER(vbox), button );

  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_zap_quick_message
		      , window
		      );

  /* --- Make everything visible --- */
  gtk_widget_show_all (window);

}

/* c---------------------------------------------------------------------- */

void sii_edit_eg_split (guint frame_num, const gchar **eg, int nlines)
{
   static gchar *buf = NULL;
   static gint bfsize = 0;
   gint jj, nn, nt;
   gchar *aa, *bptrs[64], str[256], *sptrs[64];
   gboolean fer = FALSE;
   GtkWidget *text;
   EditData *edd;

   edd = (EditData *)frame_configs[frame_num]->edit_data;
   text = edd->data_widget[EDIT_FER_TEXT];
   gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
   text = edd->data_widget[EDIT_OTO_TEXT];
   gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);

   for (jj=0; jj < nlines; jj++) {
      strcpy (str, eg[jj]);
      nt = dd_tokens (str, sptrs);

      if (nt < 1 || strstr(eg[jj], "only once"))
	 { continue; }
      if (strstr(eg[jj], "for-each-ray"))
	 { fer = TRUE; continue; }
      strcpy (str, eg[jj]);
      strcat (str, "\n");
      text = (fer) ? edd->data_widget[EDIT_FER_TEXT] :
	edd->data_widget[EDIT_OTO_TEXT];

      gtk_text_insert (GTK_TEXT (text), edit_font, edit_fore, edit_back
		       , str, -1);
   }
}

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

