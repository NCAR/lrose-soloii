/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "sii_utils.h"
# include "help_exam.h"
# include <gdk/gdkkeysyms.h>

# include <solo_window_structs.h>
# include <solo_editor_structs.h>
# include <sed_shared_structs.h>
# include <seds.h>

/* Examine Menu Actions  */

enum {
   EXAM_ZERO,
   EXAM_CANCEL,
   EXAM_CLOSE,

   EXAM_FIELDS_LIST,

   EXAM_REPLOT_THIS,
   EXAM_REPLOT_LINKS,
   EXAM_REPLOT_ALL,

   EXAM_DELETE,
   EXAM_NEG_FOLD,
   EXAM_POS_FOLD,
   EXAM_ZAP_GND_SPD,
   EXAM_DELETE_RAY,
   EXAM_NEG_FOLD_RAY,
   EXAM_POS_FOLD_RAY,
   EXAM_NEG_FOLD_SEG,
   EXAM_POS_FOLD_SEG,
   EXAM_NEG_FOLD_GT,
   EXAM_POS_FOLD_GT,
   EXAM_REPLACE,

   EXAM_DATA,
   EXAM_RAYS,
   EXAM_METADATA,
   EXAM_EDT_HIST,
   EXAM_REFRESH,

   EXAM_SCROLL_COUNT,
   EXAM_SCROLL_LEFT,
   EXAM_SCROLL_RIGHT,
   EXAM_SCROLL_UP,
   EXAM_SCROLL_DOWN,

   EXAM_UNDO,
   EXAM_CLEAR_CHANGES,
   EXAM_APPLY_CHANGES,

   EXAM_LST_RNGAZ,
   EXAM_LST_CELLRAY,

   EXAM_FORMAT,
   EXAM_RAY,
   EXAM_RAY_COUNT,
   EXAM_CELL,
   EXAM_RANGE,
   EXAM_CELL_COUNT,
   EXAM_CHANGE_COUNT,
   EXAM_ELECTRIC,

   EXAM_LAYOUT,
   EXAM_SCROLLED_WIN,
   EXAM_NYQVEL,
   EXAM_LOG_DIR,
   EXAM_LOG_ACTIVE,
   EXAM_LOG_FLUSH,
   EXAM_LOG_CLOSE,


   EXAM_OVERVIEW,
   EXAM_HLP_OPRS,
   EXAM_HLP_OPTIONS,
   EXAM_HLP_NYQVEL,
   EXAM_HLP_LOG_DIR,
   EXAM_HLP_,

   EXAM_LAST_ENUM,
};

# define EXAM_MAX_WIDGETS EXAM_LAST_ENUM


/* c---------------------------------------------------------------------- */

typedef struct {
   GtkWidget *data_widget[EXAM_MAX_WIDGETS];

   gboolean entry_flag[EXAM_MAX_WIDGETS];
   guint num_values[EXAM_MAX_WIDGETS];
   gdouble orig_values[EXAM_MAX_WIDGETS][2];
   gdouble values[EXAM_MAX_WIDGETS][2];
   guint precision[EXAM_MAX_WIDGETS];

   GString *orig_txt[EXAM_MAX_WIDGETS];
   GString *txt[EXAM_MAX_WIDGETS];

   gboolean toggle[EXAM_MAX_WIDGETS];

   gint equiv_solo_state[EXAM_MAX_WIDGETS];

   gboolean frame_active;
   guint display_state;
   guint operation_state;
   guint label_state;

   guint label_height;
   guint label_char_width;

   guint last_list_index;
   guint last_char_index;

   guint max_cells;
   guint max_possible_cells;
   guint max_chars_per_line;
   guint prior_display;
   guint prior_num_entries;

   gfloat r0_km;
   gfloat gs_km;
   gfloat clicked_range_km;
   gfloat nyq_vel;

   GString *log_dir;

   GtkWidget *label_bar0;
   GtkWidget *label_bar1;
   GtkWidget **label_items;

} ExamData;

static ExamData *xmdata;
static GSList *radio_group = NULL;

/* c---------------------------------------------------------------------- */

void sii_exam_return_click_indices (guint frame_num, gint *list_index
				    , gint *char_index);

void sii_exam_menubar( GtkWidget  *window, GtkWidget **menubar
		       , guint frame_num);
void sii_exam_menubar2( GtkWidget  *window, GtkWidget **menubar
		       , guint frame_num);
void sii_exam_widget( guint frame_num );

void show_exam_widget (GtkWidget *text, gpointer data );

void sp_cell_range_info (int frame_num, gfloat *r0, gfloat *gs, gint *ncells);

char *solo_list_entry(struct solo_list_mgmt *which, int entry_num);

struct solo_click_info *clear_click_info();

void sxm_process_click(struct solo_click_info *sci);

gfloat sp_nyquist_vel (int frame_num);

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

void sii_exam_return_click_indices (guint frame_num, gint *list_index
				    , gint *char_index)
{
   ExamData *xmd;
   xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   *list_index = xmd->last_list_index;
   *char_index = xmd->last_char_index;
}

/* c------------------------------------------------------------------------ */

void se_dump_examine_widget(int frame_num, struct examine_widget_info *ewi)
{
   gchar str[128];
   const gchar *aa;
   ExamData *xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   gint count, nn;

   aa = gtk_entry_get_text( GTK_ENTRY (xmd->data_widget[EXAM_RAY]));
   count = sscanf (aa, "%d", &ewi->ray_num);

   aa = gtk_entry_get_text( GTK_ENTRY (xmd->data_widget[EXAM_RAY_COUNT]));
   count = sscanf (aa, "%d", &ewi->ray_count);

   aa = gtk_entry_get_text( GTK_ENTRY (xmd->data_widget[EXAM_FORMAT]));
   strcpy (ewi->display_format, aa);

   aa = gtk_entry_get_text( GTK_ENTRY (xmd->data_widget[EXAM_FIELDS_LIST]));
   strcpy (ewi->fields_list, aa);

   ewi->whats_selected = xmd->equiv_solo_state[xmd->display_state];
   ewi->typeof_change = xmd->equiv_solo_state[xmd->operation_state];
   if (xmd->toggle[EXAM_LST_CELLRAY]) {
      ewi->row_annotation = EX_VIEW_CELL_NUMS;
      ewi->col_annotation = EX_VIEW_RAY_NUMS; 
   }
   else {
      ewi->row_annotation = EX_VIEW_RANGES;
      ewi->col_annotation = EX_VIEW_ROT_ANGS; 
   }
   ewi->at_cell = 0;
   ewi->cell_count = -1;
}

/* c------------------------------------------------------------------------ */

void se_refresh_examine_list(int frame_num, struct solo_list_mgmt  *slm)
{
   ExamData *xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   WW_PTR wwptr = solo_return_wwptr (frame_num);
   gint jj, nn = 64, gate, lim;
   gchar *aa, *bb, str[256];
   GtkWidget *label, *widget;
   GtkAdjustment *adj;
   GtkLabel *lbl;
   gint value;
   guint height;
   float f;

   label = xmd->label_items[0];
   if (!label)
     { return; }

   if (!slm) {
      slm = wwptr->examine_list;
   }
   if (!slm || !slm->num_entries)
     { return; }

   if (slm->num_entries <= xmd->max_possible_cells)
     { lim = slm->num_entries; }
   else {
      lim = xmd->max_possible_cells-8;
      sprintf (str, "Max lines:%d Lines needed:%d"
	       , xmd->max_possible_cells, slm->num_entries);
      sii_message (str);
   }

   for (jj=0; jj < lim; jj++) {
      aa = bb = solo_list_entry (slm, jj);
      if (!aa)
	{ break; }
# ifndef notyet
      if (jj < 2) {
	label = (jj == 0) ? xmd->label_bar0 : xmd->label_bar1;
	if (xmd->display_state == EXAM_DATA) 
	  { bb = ""; }
	else 
	  { aa = ""; }
	gtk_label_set_text (GTK_LABEL (label), aa );
	gtk_label_set_text (GTK_LABEL (xmd->label_items[jj]), bb ); 
      }
      else {
	gtk_label_set_text (GTK_LABEL (xmd->label_items[jj]), aa );
      }
# else
      gtk_label_set_text (GTK_LABEL (xmd->label_items[jj]), aa );
# endif
   }
   if (lim < xmd->prior_num_entries) {
     aa = "";
     for (; jj < xmd->prior_num_entries; jj++) {
       gtk_label_set_text (GTK_LABEL (xmd->label_items[jj]), aa );
     }
   }
   xmd->prior_num_entries = lim;

   if (xmd->clicked_range_km > 0 && xmd->display_state == EXAM_DATA) {
     /* shift data display to clicked range */

     widget = xmd->data_widget[EXAM_LAYOUT];
     adj = GTK_ADJUSTMENT
       (GTK_LAYOUT (widget)->vadjustment);
     height = widget->allocation.height;
     nn = height/xmd->label_height; /* lines visible */
     jj = (gint)((xmd->clicked_range_km -xmd->r0_km)/xmd->gs_km);
     jj -= nn/2;
     value = ( jj +2) * xmd->label_height;
     if (value < 1 )
       { value = 1; }
     if (value + nn * xmd->label_height > adj->upper)
       { value = adj->upper - nn *xmd->label_height; }
     
     gtk_adjustment_set_value (adj, value);
   }
   xmd->clicked_range_km = 0;
}

/* c------------------------------------------------------------------------ */

void se_update_exam_widgets (gint frame_num)
{
   ExamData *   xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   struct solo_edit_stuff *seds, *return_sed_stuff();
   gfloat ff;
   guint wid;

   wid = EXAM_FORMAT;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

   wid = EXAM_RAY;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

   wid = EXAM_RAY_COUNT;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

   wid = EXAM_FIELDS_LIST;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

   wid = EXAM_CELL;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

   wid = EXAM_RANGE;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

   wid = EXAM_CHANGE_COUNT;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

   wid = EXAM_NYQVEL;
   seds = return_sed_stuff();
   ff = seds->nyquist_velocity;
   if (!ff) {
     g_string_assign (xmd->orig_txt[wid], "0");
   }
   else {
     g_string_sprintf (xmd->orig_txt[wid], "%.3f"
		       , seds->nyquist_velocity);
   }
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);

# ifdef obsolete
   wid = EXAM_LOG_DIR;
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid])
		       , xmd->orig_txt[wid]->str);
# endif
}

/* c------------------------------------------------------------------------ */

void se_refresh_examine_widgets (int frame_num, struct solo_list_mgmt  *slm)
{

   ExamData *xmd;
   gchar str[128], *aa;
   gint jj, nn, ncells;
   WW_PTR wwptr = solo_return_wwptr (frame_num);
   GtkWidget *widget;
   gfloat ff, r0, gs, nyq_vel;
   struct solo_edit_stuff *seds, *return_sed_stuff();

   sp_cell_range_info (frame_num, &r0, &gs, &ncells);
   nyq_vel = sp_nyquist_vel (frame_num);

   if( !frame_configs[frame_num]->exam_data ) {
      xmd =
	frame_configs[frame_num]->exam_data = g_malloc0 (sizeof( ExamData ));
     xmd->max_possible_cells = xmd->max_cells = 2064;
     xmd->max_chars_per_line = 256;
     xmd->label_items = (GtkWidget **)g_malloc0
       (xmd->max_possible_cells * sizeof (GtkWidget *));

      xmd->entry_flag[EXAM_RAY] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_RAY_COUNT] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_CHANGE_COUNT] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_CELL] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_RANGE] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_FIELDS_LIST] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_CELL_COUNT] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_FORMAT] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_LOG_DIR] = ENTRY_TXT_ONLY;
      xmd->entry_flag[EXAM_NYQVEL] = ENTRY_TXT_ONLY;

      for (jj=0; jj < EXAM_MAX_WIDGETS; jj++) {
	 if (xmd->entry_flag[jj])
	   { xmd->orig_txt[jj] = g_string_new (""); }
      }
   }
   xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   xmd->max_cells = ncells;
   xmd->r0_km = r0;
   xmd->gs_km = gs;

   se_refresh_examine_list(frame_num, slm);

   for (jj=0; jj < EXAM_MAX_WIDGETS; jj++) {
      if (xmd->entry_flag[jj]) {
	 if (xmd->orig_txt[jj])
	   { g_string_truncate (xmd->orig_txt[jj], 0); }
	 else
	   { xmd->orig_txt[jj] = g_string_new (""); }
      }
   }

   g_string_sprintfa (xmd->orig_txt[EXAM_RAY]
		      ,"%d", wwptr->examine_info->ray_num);
   g_string_sprintfa (xmd->orig_txt[EXAM_RAY_COUNT]
		     ,"%d", wwptr->examine_info->ray_count);
   g_string_sprintfa (xmd->orig_txt[EXAM_CHANGE_COUNT]
		      ,"%d", wwptr->examine_info->change_count);

   nn = wwptr->examine_info->at_cell;
   g_string_sprintfa(xmd->orig_txt[EXAM_CELL],"%d", nn);

   ff = xmd->r0_km + nn * xmd->gs_km;
   g_string_sprintfa (xmd->orig_txt[EXAM_RANGE], "%.2f", ff);
   
   g_string_assign
     (xmd->orig_txt[EXAM_FIELDS_LIST], wwptr->examine_info->fields_list);

   g_string_sprintfa(xmd->orig_txt[EXAM_CELL_COUNT]
		     ,"%d",wwptr->examine_info->cell_count);
   g_string_assign
     (xmd->orig_txt[EXAM_FORMAT],wwptr->examine_info->display_format);

   seds = return_sed_stuff();
   ff = seds->nyquist_velocity;
   if (!ff) {
     g_string_assign (xmd->orig_txt[EXAM_NYQVEL], "0");
   }
   else {
     g_string_sprintf (xmd->orig_txt[EXAM_NYQVEL], "%.3f"
		       , seds->nyquist_velocity);
   }
   if (xmd->data_widget[EXAM_FIELDS_LIST]) {
      se_update_exam_widgets (frame_num);
   }
}

/* c---------------------------------------------------------------------- */

void show_exam_widget (GtkWidget *text, gpointer data )
{
  GtkWidget *widget;
  guint frame_num = GPOINTER_TO_UINT (data);
  ExamData *xmd;
  GdkPoint *ptp;
  gint x, y, task;
  struct solo_click_info *sci = clear_click_info();

  widget = sii_get_widget_ptr (frame_num, FRAME_EXAM_MENU);

  if( !widget )
    { sii_exam_widget( frame_num ); }
  else {
     xmd = frame_configs[frame_num]->exam_data;
     xmd->frame_active = TRUE;

# ifdef obsolete
     if (xmd->toggle[EXAM_DATA])
       { task = EXAM_DATA; }
     else if (xmd->toggle[EXAM_RAYS])
       { task = EXAM_RAYS; }
     else if (xmd->toggle[EXAM_METADATA])
       { task = EXAM_METADATA; }
     else if (xmd->toggle[EXAM_EDT_HIST])
       { task = EXAM_EDT_HIST; }
# endif
     sci->frame = frame_num;
     sxm_refresh_list (sci);
     se_update_exam_widgets (frame_num);

     ptp = &frame_configs[frame_num]->widget_origin[FRAME_EXAM_MENU];
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     gtk_widget_show (widget);

     widget = sii_get_widget_ptr (frame_num, FRAME_EXAM_DISPLAY );
     ptp = &frame_configs[frame_num]->widget_origin[FRAME_EXAM_DISPLAY];
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */

void sii_exam_list_event (GtkWidget *w, GdkEvent *event
			      , gpointer data )
{
   GtkWidget *label = GTK_WIDGET (data);
   guint num = GPOINTER_TO_UINT (data);
   guint frame_num, task, wid, exam_index;
   ExamData *xmd;
   gint xx, yy, cndx;
   GdkModifierType state;
   struct solo_click_info *sci = clear_click_info();
   
   sci->frame = 
     frame_num =
       GPOINTER_TO_UINT (gtk_object_get_data
			 (GTK_OBJECT(label), "frame_num" ));

   xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   if (xmd->display_state != EXAM_DATA)
     { return; }

   sci->which_list_entry =
     exam_index =
       GPOINTER_TO_UINT (gtk_object_get_data
			 (GTK_OBJECT(label), "exam_index" ));

   xmd->last_list_index = exam_index;
   gdk_window_get_pointer (w->window, &xx, &yy, &state);
   sci->which_character = 
     xmd->last_char_index =
       cndx = xx/xmd->label_char_width;
   
   sci->which_widget_button = EX_CLICK_IN_LIST;
   sxm_process_click (sci);

   g_message ("Exam_list: f:%d i:%d c:%d x:%d y:%d s:%d"
	      , frame_num, exam_index, cndx, xx, yy, state);
}

/* c---------------------------------------------------------------------- */

void sii_exam_keyboard_event (GtkWidget *w, GdkEvent *event
			      , gpointer data )
{
   guint num = GPOINTER_TO_UINT (data);
   guint frame_num, task, wid, height;
   gint value;
   ExamData *xmd;
   GdkEventKey *kbev = (GdkEventKey *)event;
   GtkWidget *widget;
   GtkAdjustment *adj;
   gchar *aa;
   struct solo_click_info *sci = clear_click_info();
   
   sci->frame = 
     frame_num =
       num/TASK_MODULO;
   wid =
     task =
       num % TASK_MODULO;

   xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   aa = gdk_keyval_name( kbev->keyval );
   
   g_message ("Got keyboard event f:%d w:%d k:%d s:%a"
	      , frame_num, wid, kbev->keyval, aa );

   switch( kbev->keyval ) {

    case GDK_Right:
      /* move one ray to the right */
    case GDK_Left:
      /* move one ray to the left */
      sci->which_widget_button = (kbev->keyval == GDK_Left)
	? EX_SCROLL_LEFT : EX_SCROLL_RIGHT;
      sxm_process_click (sci);
      break;

    case GDK_Up:
      /* shift up one screen */
    case GDK_Down:
      /* shift down one screen */

      widget = xmd->data_widget[EXAM_LAYOUT];
      height = widget->allocation.height -xmd->label_height;

      adj = GTK_ADJUSTMENT (GTK_LAYOUT (widget)->vadjustment);
      value = adj->value;
      value += (kbev->keyval == GDK_Up) ? -height : height;

      if (value < 1)
	{ value = 1; }
      if (value +height >= adj->upper)
	{ value = adj->upper -height; }

      gtk_adjustment_set_value (adj, value);

      break;

   };
}

/* c---------------------------------------------------------------------- */

void sii_exam_click_in_data (guint frame_num)
{
   struct solo_click_info *sci = clear_click_info();
   ExamData *xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   WW_PTR wwptr = solo_return_wwptr (frame_num);

   if (!xmd || !xmd->frame_active)
     { return; }
   sci->frame = frame_num;
   xmd->clicked_range_km = wwptr->clicked_range;
   sxm_click_in_data(sci);
}

/* c---------------------------------------------------------------------- */

void sii_exam_menu_cb ( GtkWidget *w, gpointer   data )
{
   guint num = GPOINTER_TO_UINT (data);
   guint frame_num, task, wid, frame_wid, cancel_wid, active, height;
   gint nn, jj, value, cells;
   gchar str[128], *sptrs[16];
   const gchar *aa, *bb, *line;
   ExamData *xmd;
   GtkWidget *widget, *check_item, *rmi, *layout;
   GtkAdjustment *adj;
   gfloat ff, gg;
   struct solo_click_info *sci = clear_click_info();
   WW_PTR wwptr;
   gint x, y;
   GdkPoint *ptp;
   struct solo_edit_stuff *seds, *return_sed_stuff();



   frame_num = num/TASK_MODULO;
   xmd = (ExamData *)frame_configs[frame_num]->exam_data;
   wwptr = solo_return_wwptr (frame_num);
   wid = task = num % TASK_MODULO;

   widget =
     check_item =
       rmi = xmd->data_widget[task];

   sci->frame = frame_num;
   sci->which_widget_button = xmd->equiv_solo_state[task];

				/* c...code */

   switch (task) {

    case EXAM_CANCEL:
    case EXAM_CLOSE:
      widget = sii_get_widget_ptr (frame_num, FRAME_EXAM_MENU);
      if( widget ) {
	gdk_window_get_origin ( widget->window, &x, &y );
	ptp = &frame_configs[frame_num]->widget_origin[FRAME_EXAM_MENU];
	ptp->x = x; ptp->y = y;
	gtk_widget_hide (widget);
      }
      widget = sii_get_widget_ptr (frame_num, FRAME_EXAM_DISPLAY);
      if( widget ) {
	gdk_window_get_origin ( widget->window, &x, &y );
	ptp = &frame_configs[frame_num]->widget_origin[FRAME_EXAM_DISPLAY];
	ptp->x = x; ptp->y = y;
	gtk_widget_hide (widget);
      }
      xmd->frame_active = FALSE;
      break;

    case EXAM_DATA:
    case EXAM_RAYS:
    case EXAM_METADATA:
    case EXAM_EDT_HIST:
      g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

      active = GTK_CHECK_MENU_ITEM (check_item)->active;
# ifdef obsolete
      g_message ( "Exam Task: %d  State: %d  Active: %d"
		 , task, xmd->toggle[task], active );
# endif
      xmd->toggle[task] = active;

      if(active) {
	 xmd->display_state = task;
	 sxm_process_click(sci);
      }
      break;

    case EXAM_REFRESH:
     sxm_refresh_list (sci);
     se_update_exam_widgets (frame_num);
      break;

    case EXAM_DELETE:
    case EXAM_NEG_FOLD:
    case EXAM_POS_FOLD:
    case EXAM_ZAP_GND_SPD:
    case EXAM_DELETE_RAY:
    case EXAM_NEG_FOLD_RAY:
    case EXAM_POS_FOLD_RAY:
    case EXAM_NEG_FOLD_GT:
    case EXAM_POS_FOLD_GT:
    case EXAM_REPLACE:
      g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

      active = GTK_CHECK_MENU_ITEM (check_item)->active;
# ifdef obsolete
      g_message ( "Exam Task: %d  State: %d  Active: %d"
		 , task, xmd->toggle[task], active );
# endif
      xmd->toggle[task] = active;

      if(active) {
	 xmd->operation_state = task;
	 sxm_process_click(sci);
      }
      break;

    case EXAM_LST_RNGAZ:
    case EXAM_LST_CELLRAY:
      g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));

      active = GTK_CHECK_MENU_ITEM (check_item)->active;
# ifdef obsolete
      g_message ( "Exam Task: %d  State: %d  Active: %d"
		 , task, xmd->toggle[task], active );
# endif
      xmd->toggle[task] = active;

      if( active) {
	 xmd->label_state = task;
	 sxm_refresh_list (sci);
      }

      break;

    case EXAM_REPLOT_THIS:
    case EXAM_REPLOT_LINKS:
    case EXAM_REPLOT_ALL:
      sii_plot_data (frame_num, xmd->equiv_solo_state[task]);
      break;

    case EXAM_ELECTRIC:
      xmd->toggle[task] = GTK_CHECK_MENU_ITEM (rmi)->active;
      break;

    case EXAM_RAY:
    case EXAM_RAY_COUNT:
    case EXAM_FORMAT:
    case EXAM_FIELDS_LIST:
      sxm_refresh_list (sci);
      break;

    case EXAM_RANGE:
    case EXAM_CELL:

      layout = xmd->data_widget[EXAM_LAYOUT];
      height = layout->allocation.height;
      adj = GTK_ADJUSTMENT (GTK_LAYOUT (layout)->vadjustment);

      widget = xmd->data_widget[wid]; 
      aa = gtk_entry_get_text( GTK_ENTRY (widget));
      if (wid == EXAM_RANGE) {
	 ff = -32768;
	 jj = sscanf (aa, "%f", &ff);
	 nn = (gint)((ff - xmd->r0_km)/xmd->gs_km +.5);
      }
      else {
	 nn = -1;
	 jj = sscanf (aa, "%d", &nn);
      }
      if (nn < 0)
	{ nn = 0; }
      cells = height/xmd->label_height;
      if (nn +cells >= xmd->max_cells)
	{ nn = xmd->max_cells -cells; }

      value = (nn +2) * xmd->label_height +1;

      sprintf (str, "%d", nn);
      gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[EXAM_CELL]), str);

      ff = xmd->r0_km + nn * xmd->gs_km;
      sprintf (str, "%.2f", ff);
      gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[EXAM_RANGE]), str);

      gtk_adjustment_set_value (adj, value);

      break;

    case EXAM_UNDO:
    case EXAM_CLEAR_CHANGES:
      sxm_process_click(sci);
      break;

    case EXAM_APPLY_CHANGES:
      sxm_process_click(sci);

      if (task == EXAM_APPLY_CHANGES && xmd->toggle[EXAM_ELECTRIC]) {
	 sii_plot_data (frame_num, xmd->equiv_solo_state[EXAM_REPLOT_LINKS]);
      }
      break;

   case EXAM_NYQVEL:
     aa = gtk_entry_get_text (GTK_ENTRY (xmd->data_widget[task]));
     if (!strlen (aa))
       { ff = 0; }
     else {
       sii_str_values (aa, 1, &ff, &gg);
       if (ff < 0)
	 { ff = 0; }
     }
     if (!ff) {
       g_string_assign (xmd->orig_txt[task], "0");
     }
     else {
       g_string_sprintf (xmd->orig_txt[task], "%.3f", ff);
     }
     seds = return_sed_stuff();
     seds->nyquist_velocity = ff;
     gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[task])
			      , xmd->orig_txt[task]->str);
     break;

   case EXAM_LOG_DIR:
     aa = gtk_entry_get_text (GTK_ENTRY (xmd->data_widget[task]));
     g_string_assign (xmd->orig_txt[task], aa);
     sxm_set_log_dir (aa);
     check_item = xmdata->data_widget[EXAM_LOG_ACTIVE];
     gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
     break;

   case EXAM_LOG_ACTIVE:
      xmd->toggle[task] = GTK_CHECK_MENU_ITEM (rmi)->active;
      sxm_toggle_log_dir ((xmd->toggle[task]) ? 1 : 0);
     break;

   case EXAM_LOG_CLOSE:
     sxm_close_log_stream();
     break;

   case EXAM_LOG_FLUSH:
     sxm_flush_log_stream();
     break;

   case EXAM_OVERVIEW:
     nn = sizeof (hlp_exam_overview)/sizeof (char *);
     sii_glom_strings (hlp_exam_overview, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case EXAM_HLP_OPRS:
     nn = sizeof (hlp_exam_operations)/sizeof (char *);
     sii_glom_strings (hlp_exam_operations, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case EXAM_HLP_OPTIONS:
     nn = sizeof (hlp_exam_options)/sizeof (char *);
     sii_glom_strings (hlp_exam_options, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case EXAM_HLP_NYQVEL:
     nn = sizeof (hlp_exam_nyqvel)/sizeof (char *);
     sii_glom_strings (hlp_exam_nyqvel, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case EXAM_HLP_LOG_DIR:
     nn = sizeof (hlp_exam_logdir)/sizeof (char *);
     sii_glom_strings (hlp_exam_logdir, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   };
}

/* c---------------------------------------------------------------------- */

void sii_exam_entry_paste_cb ( GtkWidget *w, gpointer   data )
{
   GtkWidget *widget;
   guint num = GPOINTER_TO_UINT (data);
   guint frame_num, wid;
   ExamData *xmd;
   
   frame_num = num/TASK_MODULO;
   wid = num % TASK_MODULO;
   xmd = (ExamData *)frame_configs[frame_num]->exam_data;

				/* clear out the entry befor pasting */
   gtk_entry_set_text (GTK_ENTRY (xmd->data_widget[wid]), "");
}

/* c---------------------------------------------------------------------- */

guint exam_list_cb(GtkWidget *label, gpointer data )
{
   GtkWidget *button = GTK_WIDGET (data);
   guint frame_num, exam_index;

   frame_num = GPOINTER_TO_UINT (gtk_object_get_data (GTK_OBJECT(button),
			  "frame_num" ));
   exam_index = GPOINTER_TO_UINT (gtk_object_get_data (GTK_OBJECT(button),
			  "exam_index" ));
   
   g_message( "layout widget button fn: %d  exam_index: %d"
	     , frame_num, exam_index );

   return FALSE;
}

/* c---------------------------------------------------------------------- */

guint exam_text_event_cb(GtkWidget *text, GdkEvent *event
			      , gpointer data )
{
  guint position = gtk_text_get_point (GTK_TEXT (text));
  guint frame_num, wid;
  gint nn, mark, start, end;
  gchar *aa, *bb, *line;
  ExamData *xmd;
  gboolean ok;

  frame_num = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(text), "frame_num" ));

  wid  = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(text), "widget_id" ));

  xmd = (ExamData *)frame_configs[frame_num]->exam_data;

  aa = (gchar *)data;
  nn = GTK_TEXT (text)->cursor_mark.index;

  mark = 0;
}

/* c---------------------------------------------------------------------- */

void sii_exam_entry_info
( GtkWidget *w, guint wid, guint frame_num, guint num_values )
{
   xmdata->data_widget[wid] = w;
   xmdata->entry_flag[wid] = TRUE;
   xmdata->num_values[wid] = num_values;
}

/* c---------------------------------------------------------------------- */

void sii_exam_widget( guint frame_num )
{
  GtkWidget *label;

  GtkWidget *window;
  GtkWidget *scrolledwindow;
  GtkWidget *edit_menubar;
  GtkWidget *hbox0;
  GtkWidget *hbox1;
  GtkWidget *hbox2;
  GtkWidget *hbox3;
  GtkWidget *hbbox;

  GtkWidget *vbox00;
  GtkWidget *vbox0;
  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *vbox3;

  GtkWidget *text;
  GtkWidget *entry;
  GtkWidget *event;
  GtkWidget *table;
  GtkWidget *hscrollbar;
  GtkWidget *vscrollbar;

  GtkWidget *menubar;
  GtkWidget *menu;
  GtkWidget *menuitem;
  GtkWidget *layout;

  GtkStyle *defstyle;
  static GtkStyle *style = NULL;
  GdkFont *font;

  GtkWidget *button;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 0;
  guint ypadding = 0;

  gint event_flags = GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK;
  gchar str[256], *aa, *bb, *ee, ch, A = 'A';
  const gchar *cc, *tt = "00/00/2000 00:00:00 HHHHHHHH 000.0 HHH HHHHHHHH";
  gint length, width, height, mm, nn, mark, jj, kk, len;
  GdkPoint *ptp;
  gint x, y, row;
  GString *gstr = g_string_new ("");
  guint widget_id = FRAME_EXAM_MENU, arg_id;

				/* c...code */

  sxm_update_examine_data (frame_num, 0);

  if( !frame_configs[frame_num]->exam_data ) {
     xmdata =
       frame_configs[frame_num]->exam_data = g_malloc0 (sizeof( ExamData ));
     xmdata->max_possible_cells = xmdata->max_cells = 2064;
     xmdata->max_chars_per_line = 256;
     xmdata->label_items = (GtkWidget **)g_malloc0
       (xmdata->max_possible_cells * sizeof (GtkWidget *));
  }
  xmdata = (ExamData *)frame_configs[frame_num]->exam_data;
  xmdata->frame_active = TRUE;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  sii_set_widget_ptr ( frame_num, widget_id, window );
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num*TASK_MODULO+widget_id));

  bb = g_strdup_printf ("Frame %d  Examine Menu", frame_num+1 );

  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);


  /* --- Create a new vertical box for storing widgets --- */
  vbox0 = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox0);

  sii_exam_menubar2 (window, &menubar, frame_num);
  gtk_container_add (GTK_CONTAINER(vbox0), menubar);



  table = gtk_table_new (4, 6, FALSE);
  gtk_container_add (GTK_CONTAINER(vbox0), table);
  row = -1;

  row++;
  label = gtk_label_new ( " Fields " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  jj = 7; kk = jj+1;
  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "DZ VE RHOHV PHIDP");
  sii_exam_entry_info( entry, EXAM_FIELDS_LIST, frame_num, 2 );
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  row++;
  label = gtk_label_new ( " Format " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "6.1f");
  sii_exam_entry_info( entry, EXAM_FORMAT, frame_num, 2 );
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  label = gtk_label_new ( " Ray " );
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "0");
  sii_exam_entry_info( entry, EXAM_RAY, frame_num, 2 );
  gtk_table_attach (GTK_TABLE (table), entry, 3, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  row++;
  label = gtk_label_new ( " Changes " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "0");
  gtk_entry_set_editable (GTK_ENTRY (entry), FALSE ); 
  sii_exam_entry_info( entry, EXAM_CHANGE_COUNT, frame_num, 2 );
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  label = gtk_label_new ( " Rays " );
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "5");
  sii_exam_entry_info( entry, EXAM_RAY_COUNT, frame_num, 2 );
  gtk_table_attach (GTK_TABLE (table), entry, 3, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  row++;
  sprintf (str, "%.3f", xmdata->r0_km );
  label = gtk_label_new ( " Range " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "0");
  sii_exam_entry_info( entry, EXAM_RANGE, frame_num, 2 );
  xmdata->precision[EXAM_RANGE] = 3;
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  label = gtk_label_new ( " Cell " );
  gtk_table_attach (GTK_TABLE (table), label, 2, 3, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "0");
  sii_exam_entry_info( entry, EXAM_CELL, frame_num, 2 );
  gtk_table_attach (GTK_TABLE (table), entry, 3, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  row++;
  label = gtk_label_new ( " Nyq Vel " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  widget_id = EXAM_NYQVEL;
  entry = gtk_entry_new ();
  sii_exam_entry_info( entry, widget_id, frame_num, 1 );
  xmdata->precision[widget_id] = 2;
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  label = gtk_label_new ( " 0 Implies the default Nyq Vel " );
  gtk_table_attach (GTK_TABLE (table), label, 2, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

+  row++;
  label = gtk_label_new ( " Log Dir " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  widget_id = EXAM_LOG_DIR;
  entry = gtk_entry_new ();
  xmdata->log_dir = g_string_new ("./");
  gtk_entry_set_text (GTK_ENTRY (entry), xmdata->log_dir->str);
  sii_exam_entry_info( entry, widget_id, frame_num, 2 );
  gtk_table_attach (GTK_TABLE (table), entry, 1, 4, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );




  hbox0 = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER(vbox0), hbox0);

  widget_id = EXAM_CLEAR_CHANGES;
  xmdata->equiv_solo_state[widget_id] = EX_CLEAR_CHANGES;
  button = gtk_button_new_with_label ("Clear Edits");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + widget_id;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_exam_menu_cb
		      , (gpointer)nn
		      );

  widget_id = EXAM_UNDO;
  xmdata->equiv_solo_state[widget_id] = EX_UNDO;
  button = gtk_button_new_with_label ("Undo");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + widget_id;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_exam_menu_cb
		      , (gpointer)nn
		      );

  widget_id = EXAM_APPLY_CHANGES;
  xmdata->equiv_solo_state[widget_id] = EX_COMMIT;
  button = gtk_button_new_with_label ("Apply Edits");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + widget_id;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_exam_menu_cb
		      , (gpointer)nn
		      );

  widget_id = EXAM_REFRESH;
  button = gtk_button_new_with_label ("Refresh");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num * TASK_MODULO + widget_id;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_exam_menu_cb
		      , (gpointer)nn
		      );



  for (jj=0; jj < EXAM_MAX_WIDGETS; jj++ ) {

     if (xmdata->entry_flag[jj] && xmdata->data_widget[jj]) {
	nn = frame_num * TASK_MODULO + jj;
	gtk_signal_connect (GTK_OBJECT(xmdata->data_widget[jj])
			    ,"activate"
			    , (GtkSignalFunc) sii_exam_menu_cb
			    , (gpointer)nn );
	gtk_signal_connect (GTK_OBJECT(xmdata->data_widget[jj])
			    ,"paste_clipboard"
			    , (GtkSignalFunc) sii_exam_entry_paste_cb
			    , (gpointer)nn );
     }
  }
  se_update_exam_widgets (frame_num);


  /* --- Make everything visible --- */
  gtk_widget_show_all (window);
  mark = 0;


  widget_id = FRAME_EXAM_DISPLAY;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_widget_set_events (window, event_flags );
  sii_set_widget_ptr ( frame_num, widget_id, window );
  gtk_widget_set_uposition (window, ptp->x +50, ptp->y +100);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num*TASK_MODULO+widget_id));

  nn = frame_num * TASK_MODULO + widget_id;
  gtk_signal_connect (GTK_OBJECT(window),"key_press_event",
		      (GtkSignalFunc) sii_exam_keyboard_event
		      , (gpointer)nn);

  bb = g_strdup_printf ("Frame %d  Examine Display Widget", frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  vbox00 = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox00);

  font = med_fxd_font;
  if (!style) {
     defstyle = gtk_widget_get_default_style ();
     style = gtk_style_copy (defstyle);
     style->font = font;
  }
  /* labels for data only
   */
  xmdata->label_bar0 = label = gtk_label_new ( "" );
  gtk_container_add (GTK_CONTAINER(vbox00), label);
  gtk_widget_set_style (label, style);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

  xmdata->label_bar1 = label = gtk_label_new ( "" );
  gtk_container_add (GTK_CONTAINER(vbox00), label);
  gtk_widget_set_style (label, style);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

  /* set keyboard events for this window to signal
   * arrows used for scrolling
   */
  vbox0 = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(vbox00), vbox0);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  xmdata->data_widget[EXAM_SCROLLED_WIN] = scrolledwindow;
  gtk_widget_set_usize (scrolledwindow, 800, 440);
  gtk_container_add (GTK_CONTAINER (vbox0), scrolledwindow);

  nn = xmdata->max_possible_cells;
  xmdata->label_height = 
    height = font->ascent + font->descent+1;
  xmdata->label_char_width =
    width = gdk_string_width (font, "0123456789")/10;
  len = xmdata->max_chars_per_line -1;
  str[len] = '\0';
  gtk_widget_set_usize (scrolledwindow, 120*width, 30*height);

  layout = gtk_layout_new (NULL, NULL);
  xmdata->data_widget[EXAM_LAYOUT] = layout;
  gtk_container_add (GTK_CONTAINER(scrolledwindow), layout);
  gtk_layout_set_size (GTK_LAYOUT (layout), len*width, nn*height);

  /* We set step sizes here since GtkLayout does not set
   * them itself. (Doesn't appear to work here!)
   */
  GTK_LAYOUT (layout)->hadjustment->step_increment = height;
  GTK_LAYOUT (layout)->vadjustment->step_increment = width;

  for (mm=0; mm < nn; mm++ ) {
     ch = A + (mm % 26);
     sprintf (str, "%3d.", mm);
     memset (str+strlen(str), ch, len-strlen(str));

     event = gtk_event_box_new ();
     gtk_widget_set_events (event, event_flags );

     label = gtk_label_new ( "" );
     gtk_container_add (GTK_CONTAINER (event), label);
     xmdata->label_items[mm] = label;
     gtk_widget_set_style (label, style);
     gtk_label_set_justify ((GtkLabel *)label, GTK_JUSTIFY_LEFT );
     gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

     gtk_signal_connect (GTK_OBJECT(event),"button_press_event",
			 (GtkSignalFunc) sii_exam_list_event
			 , (gpointer)label);

     gtk_layout_put (GTK_LAYOUT (layout), event,
		     0, mm*height);

     gtk_object_set_data (GTK_OBJECT(label),
			  "frame_num",
			  (gpointer)frame_num);
     gtk_object_set_data (GTK_OBJECT(label),
			  "exam_index",
			  (gpointer)mm);
  }
  se_refresh_examine_list(frame_num, NULL);

  gtk_widget_show_all (window);
  mark = 0;
}

/* c---------------------------------------------------------------------- */

static GtkItemFactoryEntry exam_menu_items[] = {
  { "/Display",               NULL,             NULL,                 0,  "<Branch>" },
  { "/Display/Cell Values",   NULL, sii_exam_menu_cb,         EXAM_DATA,  "<RadioItem>" },
  { "/Display/Ray Info",      NULL, sii_exam_menu_cb,         EXAM_RAYS,  "/Display/Cell Values" },
  { "/Display/Metadata",      NULL, sii_exam_menu_cb,     EXAM_METADATA,  "/Display/Cell Values" },
  { "/Display/Edit Hist",     NULL, sii_exam_menu_cb,     EXAM_EDT_HIST,  "/Display/Cell Values" },

  { "/Edit",                  NULL,             NULL,                 0,  "<Branch>" },
  { "/Edit/Options",          NULL,             NULL,                 0,  "<Title>" },
  { "/Edit/Delete",           NULL, sii_exam_menu_cb,       EXAM_DELETE,  "<RadioItem>" },
  { "/Edit/- Fold",           NULL, sii_exam_menu_cb,     EXAM_NEG_FOLD,  "/Edit/Delete" },
  { "/Edit/+ Fold",           NULL, sii_exam_menu_cb,     EXAM_POS_FOLD,  "/Edit/Delete" },
  { "/Edit/Delete Ray",       NULL, sii_exam_menu_cb,   EXAM_DELETE_RAY,  "/Edit/Delete" },
  { "/Edit/- Fold Ray",       NULL, sii_exam_menu_cb, EXAM_NEG_FOLD_RAY,  "/Edit/Delete" },
  { "/Edit/+ Fold Ray",       NULL, sii_exam_menu_cb, EXAM_POS_FOLD_RAY,  "/Edit/Delete" },
  { "/Edit/Zap Gnd Spd",      NULL, sii_exam_menu_cb,  EXAM_ZAP_GND_SPD,  "/Edit/Delete" },

  { "/Options",               NULL,             NULL,                 0,  "<Branch>" },
  { "/Options/List By",       NULL,             NULL,                 0,  "<Title>" },
  { "/Options/Rng,Az",        NULL, sii_exam_menu_cb,    EXAM_LST_RNGAZ,  "<RadioItem>" },
  { "/Options/Cell,Ray",      NULL, sii_exam_menu_cb,  EXAM_LST_CELLRAY,  "/Options/Rng,Az" },
  { "/Options/sep3",          NULL,             NULL,                 0,  "<Separator>" },
  
  { "/Replot",                NULL,             NULL,                 0,  "<Branch>" },
  { "/Replot/rePlot This",    NULL, sii_exam_menu_cb,  EXAM_REPLOT_THIS,  NULL },
  { "/Replot/rePlot Links",   NULL, sii_exam_menu_cb, EXAM_REPLOT_LINKS,  NULL },
  { "/Replot/rePlot All",     NULL, sii_exam_menu_cb,   EXAM_REPLOT_ALL,  NULL },
  
  { "/Help",                  NULL,             NULL,                 0,  "<Branch>" },
  { "/Help/Overview",         NULL, sii_exam_menu_cb,     EXAM_OVERVIEW,  NULL },
  { "/Help/With ",            NULL, sii_exam_menu_cb,         EXAM_HLP_,  NULL },
  
  { "/ ",                     NULL,             NULL,                  0,  "<Title>" },
  { "/Cancel",                NULL, sii_exam_menu_cb,        EXAM_CANCEL,  "<Item>" },

};

/* c---------------------------------------------------------------------- */

void sii_exam_menubar2( GtkWidget  *window, GtkWidget **menubar
		       , guint frame_num)
{
   gint jj, nn;
   guint radio_num = 0, widget_id;
   GtkWidget *check_item, *mbar, *menuitem, *submenu, *next_item;

   radio_group = NULL;
   
   *menubar = mbar = gtk_menu_bar_new ();

   submenu = sii_submenu ( "Display", mbar );
   
   widget_id = EXAM_CLOSE;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Close", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   radio_group = NULL;
   radio_num = 0;
   
   widget_id = EXAM_DATA;
   xmdata->equiv_solo_state[widget_id] = EX_RADAR_DATA;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Cell Values", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem), TRUE );
   
   widget_id = EXAM_RAYS;
   xmdata->equiv_solo_state[widget_id] = EX_BEAM_INVENTORY;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Ray Info", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem), FALSE );
   
   widget_id = EXAM_METADATA;
   xmdata->equiv_solo_state[widget_id] = EX_DESCRIPTORS;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Metadata", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem), FALSE );
   
   widget_id = EXAM_EDT_HIST;
   xmdata->equiv_solo_state[widget_id] = EX_EDIT_HIST;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Edit Hist", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem), FALSE );
   

   submenu = sii_submenu ( "Edit", mbar );
   radio_group = NULL;
   radio_num = 0;
   
   widget_id = EXAM_DELETE;
   xmdata->equiv_solo_state[widget_id] = EX_DELETE;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Delete", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_NEG_FOLD;
   xmdata->equiv_solo_state[widget_id] = EX_MINUS_FOLD;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "- Fold", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_POS_FOLD;
   xmdata->equiv_solo_state[widget_id] = EX_PLUS_FOLD;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "+ Fold", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_DELETE_RAY;
   xmdata->equiv_solo_state[widget_id] = EX_RAY_IGNORE;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Delete Ray", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_NEG_FOLD_RAY;
   xmdata->equiv_solo_state[widget_id] = EX_RAY_MINUS_FOLD;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "- Fold Ray", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_POS_FOLD_RAY;
   xmdata->equiv_solo_state[widget_id] = EX_RAY_PLUS_FOLD;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "+ Fold Ray", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_NEG_FOLD_GT;
   xmdata->equiv_solo_state[widget_id] = EX_GT_MINUS_FOLD;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "- Fold Ray >", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_POS_FOLD_GT;
   xmdata->equiv_solo_state[widget_id] = EX_GT_PLUS_FOLD;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "+ Fold Ray >", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_ZAP_GND_SPD;
   xmdata->equiv_solo_state[widget_id] = EX_REMOVE_AIR_MOTION;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Zap Gnd Spd", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   submenu = sii_submenu ( "Options", mbar );
   radio_num = 0;

   widget_id = EXAM_LST_RNGAZ;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Az,Rng Labels", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);
   
   widget_id = EXAM_LST_CELLRAY;
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Cell,Ray Labels", submenu, widget_id
			      , (GtkSignalFunc)sii_exam_menu_cb, frame_num, ++radio_num
			       , &radio_group);


   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
     
   widget_id = EXAM_LOG_ACTIVE;
   radio_num = 0;		/* implies a check menu item */
   xmdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Logging Active", submenu, widget_id
			       , (GtkSignalFunc)sii_exam_menu_cb, frame_num, radio_num
			       , &radio_group);

   widget_id = EXAM_LOG_CLOSE;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Close log file", submenu, widget_id
			, (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   widget_id = EXAM_LOG_FLUSH;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Flush to log file", submenu, widget_id
			, (GtkSignalFunc)sii_exam_menu_cb, frame_num );



   submenu = sii_submenu ( "Replot", mbar );

   widget_id = EXAM_REPLOT_THIS;
   xmdata->equiv_solo_state[widget_id] = REPLOT_THIS_FRAME;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Replot This", submenu, widget_id
			, (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   widget_id = EXAM_REPLOT_LINKS;
   xmdata->equiv_solo_state[widget_id] = REPLOT_LOCK_STEP;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Replot Links", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   widget_id = EXAM_REPLOT_ALL;
   xmdata->equiv_solo_state[widget_id] = REPLOT_ALL;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Replot All", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   submenu = sii_submenu ( "Help", mbar );

   widget_id = EXAM_OVERVIEW;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Overview", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   widget_id = EXAM_HLP_OPRS;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Edit", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   widget_id = EXAM_HLP_OPTIONS;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Options", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   widget_id = EXAM_HLP_NYQVEL;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Nyq Vel", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );

   widget_id = EXAM_HLP_LOG_DIR;
   xmdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Log Dir", submenu, widget_id
		       , (GtkSignalFunc)sii_exam_menu_cb, frame_num );


   widget_id = EXAM_CANCEL;
   menuitem = gtk_menu_item_new_with_label ("Cancel");
   gtk_menu_bar_append (GTK_MENU_BAR (mbar), menuitem );
   nn = frame_num * TASK_MODULO + widget_id;
   gtk_signal_connect (GTK_OBJECT(menuitem)
		       ,"activate"
		       , (GtkSignalFunc)sii_exam_menu_cb 
		       , (gpointer)nn);
   gtk_object_set_data (GTK_OBJECT(menuitem)
			, "widget_id", (gpointer)widget_id);
   gtk_object_set_data (GTK_OBJECT(menuitem)
			, "frame_num", (gpointer)frame_num);


  check_item = xmdata->data_widget[EXAM_LOG_ACTIVE];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), FALSE );

  check_item = xmdata->data_widget[EXAM_DATA];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  xmdata->display_state = EXAM_DATA;

  check_item = xmdata->data_widget[EXAM_DELETE];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  xmdata->operation_state = EXAM_DELETE;

  check_item = xmdata->data_widget[EXAM_LST_RNGAZ];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  xmdata->label_state = EXAM_LST_RNGAZ;

}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */












