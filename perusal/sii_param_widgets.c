/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "color_tables.h"
# include "sii_utils.h"
# include <solo_window_structs.h>
# include "help_param.h"
# include <math.h>
# include <stdio.h>


static gchar *param_names = NULL;
static gchar *param_palette_names = NULL;
static gchar *param_color_table_names = NULL;
static gchar *param_color_names = NULL;

static GString *gs_color_table_names = NULL;
static GString *gs_color_names = NULL;

static SiiLinkedList *palette_stack=NULL;
static SiiLinkedList *solo_palette_stack=NULL;

static GdkFont *edit_font;
static GdkColor *edit_fore;
static GdkColor *edit_back;


# define CB_ROUND .000001
# define COMMA ","

 /* feature colors */

enum {
  FEATURE_ZERO,
  FEATURE_RNG_AZ_GRID,
  FEATURE_TIC_MARKS,
  FEATURE_BACKGROUND,
  FEATURE_ANNOTATION,
  FEATURE_EXCEEDED,
  FEATURE_MISSING,
  FEATURE_EMPHASIS,
  FEATURE_BND,
  FEATURE_OVERLAY1,
  FEATURE_OVERLAY2,
  FEATURE_OVERLAY3,
  FEATURE_OVERLAY4,
  FEATURE_OVERLAY5,
  FEATURE_LAST_ENUM,
};

# define MAX_FEATURE_COLORS FEATURE_LAST_ENUM

 /* Parameter Menu Actions */

enum {
   PARAM_ZERO,
   PARAM_PALETTE_LIST,
   PARAM_CLR_TBL_LIST,
   PARAM_MPORT_CLR_TBL,
   PARAM_COLORS_LIST,
   PARAM_ELECTRIC,
   PARAM_HILIGHT,
   PARAM_LABEL,
   PARAM_COLOR_TEST,
   PARAM_NAMES_TEXT,
   PARAM_PALETTES_TEXT,
   PARAM_CLR_TBL_TEXT,
   PARAM_CLR_TBL_PATH,
   PARAM_COLORS_TEXT,
   PARAM_COLORS_FILESEL,

   PARAM_GNRK_ENTRY,
   PARAM_GNRK_ENTRY_CANCEL,

   PARAM_REPLOT_THIS,
   PARAM_REPLOT_LINKS,
   PARAM_REPLOT_ALL,

   PARAM_OK,
   PARAM_CANCEL,
   PARAM_CLOSE,
   PARAM_LINKS,

   PARAM_PALETTE_CANCEL,
   PARAM_CLR_TBL_CANCEL,
   PARAM_COLORS_CANCEL,

   PARAM_NAME,
   PARAM_MINMAX,
   PARAM_CTRINC,
   PARAM_GRID_CLR,
   PARAM_PALETTE,

   PARAM_BND_CLR,
   PARAM_XCEED_CLR,
   PARAM_MSSNG_CLR,
   PARAM_ANNOT_CLR,
   PARAM_BACKG_CLR,
   PARAM_EMPH_MINMAX,
   PARAM_EMPH_CLR,
   PARAM_CLR_TBL_NAME,

   PARAM_CB_BOTTOM,		/* color bar */
   PARAM_CB_LEFT,
   PARAM_CB_RIGHT,
   PARAM_CB_TOP,
   PARAM_CB_SYMBOLS,
   PARAM_CB_ALL_SYMBOLS,

   PARAM_BC_GRID,		/* broadcast */
   PARAM_BC_BND,
   PARAM_BC_CB,
   PARAM_BC_BACKG,
   PARAM_BC_XCEED,
   PARAM_BC_ANNOT,
   PARAM_BC_MSSNG,
   PARAM_BC_EMPH,

   PARAM_OVERVIEW,
   PARAM_HLP_FILE,
   PARAM_HLP_OPTIONS,
   PARAM_HLP_LINKS,
   PARAM_HLP_PALETTES,
   PARAM_HLP_MINMAX,
   PARAM_HLP_EMPHASIS,
   PARAM_HLP_,

   PARAM_LAST_ENUM,

}ParamWidgetIds;

# define PARAM_MAX_WIDGETS PARAM_LAST_ENUM
# define MAX_COLOR_TABLE_SIZE 128
# define MAX_BAR_CLRS 64

enum color_bar_orientation{
  CB_BOTTOM      = 1 << 0,
  CB_LEFT        = 1 << 1,
  CB_RIGHT       = 1 << 2,
  CB_TOP         = 1 << 3,
  CB_SYMBOLS     = 1 << 4,
  CB_ALL_SYMBOLS = 1 << 5,
};

/* c---------------------------------------------------------------------- */

typedef struct {

   GString *p_name;
   GString *usual_parms;

   gfloat minmax[2];
   gfloat ctrinc[2];
   gfloat emphasis_zone[2];

   GString *grid_color;
   GString *missing_data_color;
   GString *exceeded_color;
   GString *annotation_color;
   GString *background_color;
   GString *emphasis_color;
   GString *boundary_color;
 
   GString *units;
   guint num_colors;
   GString *color_table_name;

   GdkColor *feature_color[MAX_FEATURE_COLORS];
   GdkColor *data_color_table[MAX_COLOR_TABLE_SIZE];

   guchar feature_rgbs[MAX_FEATURE_COLORS][3];
   guchar color_table_rgbs[MAX_COLOR_TABLE_SIZE][3];

} SiiPalette;

/* c---------------------------------------------------------------------- */

typedef struct {
  gchar name[16];
  SiiPalette *pal;
} ParamFieldInfo;

/* c---------------------------------------------------------------------- */

typedef struct {
   gint change_count;

   GtkWidget *data_widget[PARAM_MAX_WIDGETS];

   gboolean change_flag[PARAM_MAX_WIDGETS];
   gboolean entry_flag[PARAM_MAX_WIDGETS];

   guint num_values[PARAM_MAX_WIDGETS];
   gdouble orig_values[PARAM_MAX_WIDGETS][2];
   gdouble values[PARAM_MAX_WIDGETS][2];
   guint precision[PARAM_MAX_WIDGETS];

   GString *orig_txt[PARAM_MAX_WIDGETS];
   GString *txt[PARAM_MAX_WIDGETS];

   gboolean toggle[PARAM_MAX_WIDGETS];
   gboolean  electric_params;
   guint cb_loc;
   guint cb_labels_state;
   guint num_colors;

   SiiPalette *pal;
   SiiPalette *orig_pal;
   LinksInfo *param_links;
   GString *param_names_list;

   int orientation;
   GString *cb_labels;
   GString *cb_symbols;

   guint field_toggle_count;
   SiiLinkedList *fields_list;
   SiiLinkedList *toggle_field;

} ParamData;


static ParamData *pdata;
static GSList *radio_group = NULL;
static gboolean its_changed = FALSE;


/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

GtkWidget *
sii_filesel (gint which_but, gchar * dirroot);

void 
param_entry_widget( guint frame_num, gchar *prompt, gchar * dirroot);

static void
param_list_widget( guint frame_num, guint widget_id
		  , guint menu_wid);
guint
param_text_event_cb(GtkWidget *text, GdkEvent *event, gpointer data );

const gchar *
set_cb_labeling_info (guint frame_num, gdouble *relative_locs);

void
set_color_bar (guint frame_num, guint font_height);

static void
show_param_generic_list_widget (guint frame_num
				      , guint menu_wid );
void
show_param_widget (GtkWidget *text, gpointer data );

GdkColor *
sii_annotation_color (guint frame_num, gint exposed);

GdkColor *
sii_background_color (guint frame_num, gint exposed);

GdkColor *
sii_boundary_color (guint frame_num, gint exposed);

gchar *
sii_color_tables_lister ( gpointer name, gpointer data );

void
sii_colorize_image (guint frame_num);

gchar *
sii_colors_lister ( gpointer name, gpointer data );

void
sii_ctr_inc_from_min_max (guint ncolors, gfloat *ctr, gfloat *inc,
			  gfloat *min, gfloat *max );
gchar *
sii_default_palettes_list();

void
sii_do_annotation (guint frame_num, gint exposed, gboolean blow_up);

void
sii_double_colorize_image (guint frame_num);

void
sii_get_clip_rectangle (guint frame_num, Rectangle *clip
			, gboolean blow_up);
gint
sii_get_ctr_and_inc (guint frame_num, float *ctr, float *inc);

gint
sii_get_emph_min_max (guint frame_num, float *zmin, float *zmax);

GdkColor *
sii_grid_color (guint frame_num, gint exposed);

void
sii_initialize_parameter (guint frame_num, gchar *name);

SiiPalette *
sii_malloc_palette();

void
sii_min_max_from_ctr_inc (guint ncolors, gfloat *ctr, gfloat *inc,
			  gfloat *min, gfloat *max );
SiiLinkedList *
sii_new_palette_for_param (gchar *p_name, gchar *name);

gchar *
sii_new_palettes_list ();

gchar *
sii_new_color_tables_list ();

gchar *
sii_new_colors_list ();

SiiLinkedList *
sii_palette_for_param (gchar *name);

void
sii_palette_prepend_usual_param ( SiiPalette *pal, gchar *name);

void 
sii_palette_remove_usual_param ( SiiPalette *palx, gchar *name);

SiiLinkedList *
sii_palette_seek (gchar *p_name);

void
sii_param_dup_entries (guint frame_num);

void
sii_param_dup_opal (struct solo_palette *opal);

void
sii_param_dup_orig (guint frame_num);

void
sii_param_dup_pal (gpointer sii_pal, gpointer old_pal);

void
sii_param_check_changes (guint frame_num);

void
sii_param_entry_info( GtkWidget *w, guint wid, guint frame_num
		     , guint num_values );
void
sii_param_entry_paste_cb ( GtkWidget *w, gpointer   data );

gchar *
sii_param_fix_color (gchar *old_color);

void
sii_param_menu_cb ( GtkWidget *w, gpointer   data );

void
sii_param_menubar2( GtkWidget  *window, GtkWidget **menubar
		       , guint frame_num);

const gchar *
sii_param_palette_name (guint frame_num);

void 
sii_param_process_changes (guint frame_num);

void
sii_param_reset_changes (guint frame_num);

void
sii_param_set_plot_field (int frame_num, char *fname);

void
sii_param_toggle_field (guint frame_num);

void
sii_param_widget( guint frame_num );

void
sii_redisplay_list (guint frame_num, guint widget_id);

void
sii_reset_image (guint frame_num);

void
sii_set_entries_from_palette (guint frame_num, gchar *name, gchar *p_name);

SiiPalette *
sii_set_palette (gchar *name);

gboolean
sii_set_param_info (guint frame_num);

gchar *
sii_set_param_names_list(guint frame_num, GString *gs_list, guint count);

void
solo_get_palette(char *name, int frame_num);

int
solo_hardware_color_table(gint frame_num);

int
solo_palette_color_numbers(int frame_num);

SiiLinkedList *
sii_solo_palette_stack ();

void
sii_update_param_widget (guint frame_num);


/* external routines */

struct solo_perusal_info *
solo_return_winfo_ptr();
void solo_sort_slm_entries ();

/* c...mark */

/* c---------------------------------------------------------------------- */

void param_set_cb_loc (int frame_num, int loc)
{
  ParamData *pd = frame_configs[frame_num]->param_data;
  GtkWidget *check_item, *widget;

  switch (loc) {
  case -1:
    pd->cb_loc = PARAM_CB_LEFT;
    break;
  case 1:
    pd->cb_loc = PARAM_CB_RIGHT;
    break;
  default:
    pd->cb_loc = PARAM_CB_BOTTOM;
    break;
  };
  widget = sii_get_widget_ptr (frame_num, FRAME_PARAMETERS);
  if (widget) {
    check_item = pd->data_widget[pd->cb_loc];
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );
  }
}

/* c---------------------------------------------------------------------- */

void 
sii_param_colors_filesel (const gchar *str, GtkWidget *fs);
int sii_initialize_cb (GtkWidget *w, gpointer data);

void 
sii_param_colors_filesel (const gchar *str, GtkWidget *fs )
{
   gint frame_num =  GPOINTER_TO_UINT (gtk_object_get_data (GTK_OBJECT(fs)
							    , "frame_num"));
   sii_param_absorb_ctbl (frame_num, str);
}
/* c---------------------------------------------------------------------- */

void param_entry_widget( guint frame_num, gchar *prompt, gchar * dirroot)
{
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *button;
  GtkWidget *entry;
  guint wid, nn;
  GdkPoint *ptp;
  gint x, y;
  gchar *bb;
  static gchar str[256];

  wid = FRAME_PARAM_ENTRY;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  sii_set_widget_ptr (frame_num, wid, window);
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);


  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num*TASK_MODULO+wid));
  bb = g_strdup_printf ("Frame %d  %s", frame_num+1, "Entry Widget" );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );

  vbox = gtk_vbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  label = gtk_label_new ( prompt );
  gtk_container_add (GTK_CONTAINER(vbox), label);

  entry = gtk_entry_new ();
  pd->data_widget[PARAM_GNRK_ENTRY] = entry;
  gtk_container_add (GTK_CONTAINER(vbox), entry);
  nn = frame_num * TASK_MODULO + PARAM_CLR_TBL_PATH;
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn );

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_container_add (GTK_CONTAINER(vbox), hbox);

  button = gtk_button_new_with_label ("OK");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + PARAM_CLR_TBL_PATH;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn
		      );

  if (dirroot) {
     button = gtk_button_new_with_label ("Colors FileSelect");
     gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
     nn = frame_num*TASK_MODULO + PARAM_COLORS_FILESEL;
     gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn
		      );
     slash_path (str, dirroot);
     gtk_object_set_data (GTK_OBJECT(button)
			  , "dirroot", (gpointer)str);
   }				      

  button = gtk_button_new_with_label ("Cancel");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + PARAM_GNRK_ENTRY_CANCEL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn
		      );

  gtk_widget_show_all (window);
}

/* c---------------------------------------------------------------------- */

void sii_param_add_ctbl (const char **at, int nn)
{
  char *buf, str[256], *sptrs[32], *aa, *bb;
  int nt;

  strcpy (str, at[0]);
  nt = dd_tokens (str, sptrs);	/* the name should be the second token */
  sii_glom_strings (at, nn, gen_gs);

  buf = (char *)g_malloc0 (gen_gs->len+ 1);
  strncpy (buf, gen_gs->str, gen_gs->len);
  buf[gen_gs->len] = '\0';
  put_ascii_color_table (sptrs[1], buf);
  sii_new_color_tables_list ();
}

/* c---------------------------------------------------------------------- */

gboolean sii_param_absorb_ctbl (guint frame_num, gchar *filename)
{
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  gchar str[256], *sptrs[32], mess[256], *buf, *aa, *bb;
  gchar *name, *bptrs[128];
  FILE *stream;
  size_t lenx, len0, len2;
  gint ii, jj, nn, nt;

  if(!(stream = fopen(filename, "r"))) {
    sprintf(mess, "Unable to open color table file %s\n", filename);
    sii_message(mess);
    return FALSE;
  }

  lenx = fseek(stream, 0L, (int)2); /* at end of file */
  len0 = ftell(stream);	/* how big is the file */
  rewind(stream);

  if (name = strrchr (filename, '/'))
    { name++; }
  else
    { name = filename; }
  strcpy (str, "colortable ");
  strcat (str, name);
  strcat (str, "\n");

  buf = (char *)g_malloc0 (len0+1);
  lenx = fread (buf, sizeof(char), len0, stream);
  nt = dd_tokenz (buf, bptrs, "\n");
        
  g_string_sprintf (gen_gs, "colortable %s\n", name);

  for (jj=0; jj < nt; jj++) {
     if (aa = strstr (bptrs[jj], "!"))
       { *aa = '\0'; }

     if (strlen (bptrs[jj]) < 2)
       { continue; }
     if (strstr( bptrs[jj], "colortable"))
       { continue; }

     g_string_sprintfa (gen_gs, "%s\n", bptrs[jj]);
  }
  g_free (buf);
  buf = (char *)g_malloc0 (gen_gs->len +1);
  memcpy (buf, gen_gs->str, gen_gs->len); 
  buf[gen_gs->len] = '\0';
  put_ascii_color_table (name, buf);
  sii_new_color_tables_list ();

  gtk_entry_set_text
    (GTK_ENTRY (pd->data_widget[PARAM_CLR_TBL_NAME]), name);
  sii_redisplay_list (frame_num, PARAM_CLR_TBL_TEXT);
}

/* c---------------------------------------------------------------------- */

int sii_param_dump_ctbls (FILE *stream)
{
  GSList *gsl = color_table_names;
  static GString *gs = NULL;
  gchar mess[256], *aa, *bb;
  struct gen_dsc {
    char name_struct[4];
    long sizeof_struct;
  };
  struct gen_dsc gd;
  size_t nn, len;
  
  strncpy (gd.name_struct, "SCTB", 4);

  for(; gsl; gsl = gsl->next) {
    aa = (gchar *)gsl->data;
    bb = (gchar *)g_tree_lookup (ascii_color_tables, (gpointer)aa);
    len = strlen (bb);
    gd.sizeof_struct = 8 + len;
    if((nn = fwrite(&gd, sizeof(char), sizeof(gd), stream))
	   < sizeof(gd))
      {
	sprintf(mess, "Problem writing color table: %s\n", aa);
	solo_message(mess);
	return 0;
      }
    if((nn = fwrite((void *)bb, sizeof(char), len, stream))
	   < len)
      {
	sprintf(mess, "Problem writing color table: %s\n", aa);
	solo_message(mess);
	return 0;
      }
    
  }
  return 1;
}

/* c---------------------------------------------------------------------- */

static void param_list_widget( guint frame_num, guint widget_id
			     , guint menu_wid)
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

  char *aa;
  gchar str[256], str2[256], *bb, *ee, *name;
  gint event_flags = GDK_BUTTON_PRESS_MASK;
  const gchar *cc;
  gint length, width=0, height=0, max_width = 0, jj, nn;
  gfloat upper;
  GdkPoint *ptp;
  gint x, y;
  guint cancel_wid, text_wid = 0;
				/* c...mark code starts here */

  bb = str;
  strcpy (bb, "  2 and 5     " );
  ee = g_strstrip (bb);

  switch (menu_wid) {

   case PARAM_PALETTE_LIST:
     if (!param_palette_names)
       { sii_new_palettes_list (); }
     aa = param_palette_names;
     width = height = 360;
     name = "Color Palettes Widget";
     cancel_wid = PARAM_PALETTE_CANCEL;
     text_wid = PARAM_PALETTES_TEXT;
     break;

   case PARAM_CLR_TBL_LIST:
     if (!param_color_table_names)
       { sii_new_color_tables_list (); }
     aa = param_color_table_names;
     width = height = 220;
     name = "Color Tables Widget";
     text_wid = PARAM_CLR_TBL_TEXT;
     cancel_wid = PARAM_CLR_TBL_CANCEL;
     break;

   case PARAM_COLORS_LIST:
     if (!param_color_names)
       { sii_new_colors_list (); }
     aa = param_color_names;
     height = 512;
     name = "Color Names Widget";
     text_wid = PARAM_COLORS_TEXT;
     cancel_wid = PARAM_COLORS_CANCEL;
     break;
  }
  length = strlen( aa );

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

  /* --- Title and border --- */
  bb = g_strdup_printf ("Frame %d  %s", frame_num+1, name );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  button = gtk_button_new_with_label ("Cancel");
  gtk_container_add (GTK_CONTAINER(vbox), button );

  nn = frame_num*TASK_MODULO + cancel_wid;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn
		      );

  table = gtk_table_new (2, 1, FALSE);
  gtk_box_pack_start (GTK_BOX (vbox), table, TRUE, TRUE, 0 );

  text = gtk_text_new (NULL,NULL);
  if (text_wid)
    { pdata->data_widget[text_wid] = text; }

  gtk_widget_set_usize (text, width, height);
  gtk_widget_set_events (text, event_flags );

  gtk_signal_connect_after (GTK_OBJECT(text)
		      ,"button_press_event"
		      , (GtkSignalFunc) param_text_event_cb
		      , (gpointer)aa);

  gtk_object_set_data (GTK_OBJECT(text),
		       "frame_num", (gpointer)frame_num);

  gtk_object_set_data (GTK_OBJECT(text),
		       "widget_id", (gpointer)text_wid);

  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    xoptions, yoptions, 0, 0);
  
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    xoptions, yoptions, 0, 0);


  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , aa, length );


  gtk_container_foreach (GTK_CONTAINER (vbox),
			 sii_set_widget_frame_num,
			 (gpointer)frame_num );

  /* --- Make everything visible --- */
  gtk_widget_show_all (window);

}

/* c---------------------------------------------------------------------- */

guint param_text_event_cb(GtkWidget *text, GdkEvent *event
			      , gpointer data )
{
  GdkDrawable *drawable;
  GtkWidget* draw_widget;
  static GdkGC *gc = NULL;
  GtkAllocation alloc;
  guint position = gtk_text_get_point (GTK_TEXT (text));
  guint frame_num, wid;
  gint jj, kk, nn, nt, mark, start, end;
  gchar *aa, *bb = "", *line, *p_name, *name;
  ParamData *pd;
  gboolean ok;
  gchar str[256], *sptrs[32];

  static GtkStyle *style = NULL;
  GdkColor *test_color;

  nn = PARAM_CLR_TBL_TEXT;

  frame_num = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(text), "frame_num" ));

  wid  = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(text), "widget_id" ));

  pd = (ParamData *)frame_configs[frame_num]->param_data;

  nn = GTK_TEXT (text)->cursor_mark.index;

  switch (wid) {
  case PARAM_NAMES_TEXT:
    aa = pd->param_names_list->str;
    bb = "PARAM_NAMES_TEXT";
    break;
  case PARAM_PALETTES_TEXT:
    aa = param_palette_names;
    bb = "PARAM_PALETTES_TEXT";
    break;
  case PARAM_COLORS_TEXT:
    aa = param_color_names;
    bb = "PARAM_COLORS_TEXT";
    break;
  case PARAM_CLR_TBL_TEXT:
    aa = param_color_table_names;
    bb = "PARAM_CLR_TBL_TEXT";
    break;
  };

  line = sii_nab_line_from_text (aa, nn);

  if( line ) {
# ifdef obsolete
     g_message ("Frame:%d Parameter_text: %s", frame_num, line);
# endif

     if( wid == PARAM_NAMES_TEXT ) {
	g_strstrip (line);
	gtk_entry_set_text (GTK_ENTRY (pd->data_widget[PARAM_NAME]), line);
	if (pd->toggle[PARAM_ELECTRIC]) {
	  if (sii_set_param_info (frame_num))
	    { sii_plot_data (frame_num, REPLOT_THIS_FRAME); }
	}
	else {
	  sii_param_process_changes (frame_num);
	}
	sii_redisplay_list (frame_num, PARAM_PALETTES_TEXT);
     }
     else if ( wid == PARAM_PALETTES_TEXT ) {
	strcpy (str, line);
	nt = dd_tokens (str, sptrs);
	p_name = sptrs [0];
	gtk_entry_set_text
	    (GTK_ENTRY (pd->data_widget[PARAM_PALETTE]), p_name);
	name = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[PARAM_NAME]));
	sii_new_palette_for_param (p_name, name);
	sii_param_process_changes (frame_num);
	sii_redisplay_list (frame_num, PARAM_PALETTES_TEXT);
     }
     else if ( wid == PARAM_CLR_TBL_TEXT ) {
	gtk_entry_set_text
	    (GTK_ENTRY (pd->data_widget[PARAM_CLR_TBL_NAME]), line);
     }
     else if ( wid == PARAM_COLORS_TEXT &&
	      (ok = sii_nab_region_from_text (aa, nn, &start, &end )))
       {
	  gtk_editable_select_region (GTK_EDITABLE (text), start, end);
	  gtk_editable_copy_clipboard (GTK_EDITABLE (text));

# ifndef obsolete
	  for (kk=0,jj=start; jj < end; str[kk++] = aa[jj++]); str[kk] = '\0';
	  test_color = (GdkColor *)g_hash_table_lookup
	    (colors_hash, (gpointer)str);

	  draw_widget = pd->data_widget[PARAM_COLOR_TEST];
	  drawable = drawable = draw_widget->window;
	  if (!gc)
	    { gc = gdk_gc_new (drawable); }
	  gdk_gc_set_foreground (gc, test_color );
	  alloc = draw_widget->allocation;
	  gdk_draw_rectangle (drawable, gc, TRUE, 0,0,
			      alloc.width, alloc.height );
# endif
       }
     else {
     }
  }
  else {
     g_message ("Frame:%d Parameter_text: Bad Pointer from %s"
		, frame_num, bb );
  }
  mark = 0;
}

/* c---------------------------------------------------------------------- */

const gchar *set_cb_labeling_info (guint frame_num, gdouble *relative_locs)
{
  gdouble min, max, ctr, inc, log10diff, dval, dloc, dinc, d;
  gint jj, kk, nc, nn, nlabs, nnlabs;
  ParamData *pd = frame_configs[frame_num]->param_data;
  gchar *fmt, *name, str[512], *sptrs[64];
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  const gchar **syms=NULL;

  /* Take the log10 of the increment to get decimal places in the label
   * n = (gint)((f < 0) ? f-.5 : f+.5);
   */
  nc = pd->pal->num_colors;
  ctr = pd->pal->ctrinc[0];
  inc = pd->pal->ctrinc[1];
  name = pd->orig_txt[PARAM_NAME]->str;
  name = wwptr->parameter->parameter_name;

  if (pd->toggle[PARAM_CB_SYMBOLS] || wwptr->color_bar_symbols) {
    if (strstr ("RR_DSD,RNX,RZD,RKD,", name )) {
      syms = log_rr_symbols;
      nnlabs = sizeof (log_rr_symbols)/sizeof (char *);
    }
    else if (strstr ("PD,WPD,HYDID,", name )) {
      syms = pid_labelz;
      nnlabs = sizeof (pid_labelz)/sizeof (char *);
      syms = pid_symbols;
      nnlabs = sizeof (pid_symbols)/sizeof (char *);
    }
  }

  nlabs = (nc < 7) ? nc : 7;
  switch (nc) {
  case 13:
  case 12:
  case 11:
  case 10:
  case 9:
    nlabs = 9;
    break;
  };

  d = nc*inc;
  log10diff = log10 (d);

  if (log10diff > 1.3)		/* 20 */
    { fmt = "%.0f "; }
  else if (log10diff > -.15)	/* .7 */
    { fmt = "%.1f "; }
  else 
    { fmt = "%.2f "; }

  nn = (gint)((double)nc/(nlabs +1) + .4);
  dinc = (double)nn/nc;
  dval = ctr - (nlabs/2) * nn * inc;
  dloc = .5 - (nlabs/2) * dinc;

  if (syms) {
    nlabs = nnlabs;
    nn = 1;
    dinc = 1.0/nc;
    dloc = .5 * dinc;
  }

  if (!pd->cb_labels)
    { pd->cb_labels = g_string_new (""); }
  else
    { g_string_truncate (pd->cb_labels, 0); }

  for (jj = 0; jj < nlabs; jj++, dval += nn*inc, dloc += dinc) {
    d = (dval < 0) ? dval-.0001 : dval+.0001;
    if (syms) {
      g_string_sprintfa (pd->cb_labels, "%s ", syms[jj]);
    }
    else {
      g_string_sprintfa (pd->cb_labels, fmt, d);
    }
    relative_locs[jj] = dloc;
  }
  return (const gchar *)pd->cb_labels->str;
}

/* c---------------------------------------------------------------------- */

void set_color_bar (guint frame_num, guint font_height)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  double f_inc, f_min, f_max, f_val, ctr, inc, half;
  guchar *aa, *bb, *cc, *ee;
  glong *lp;
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  gint x, y, jj, kk, ll, nn, nc, len, width, offs, stride;
  
  len = (pd->cb_loc == PARAM_CB_LEFT || pd->cb_loc == PARAM_CB_RIGHT)
    ? sfc->height : sfc->width;

  if (len > sfc->cb_pattern_len) {
    if (sfc->cb_pattern)
      { g_free (sfc->cb_pattern); }
    sfc->cb_pattern = (guchar *)g_malloc0 (len);
    sfc->cb_pattern_len = len;
  }
  ctr = pd->pal->ctrinc[0];
  half = .5 * pd->pal->num_colors * pd->pal->ctrinc[1];
  len -= 4;

  f_val = f_min = ctr - half;
  f_max = ctr + half;
  f_inc = 2 * half/len;
  aa = sfc->cb_pattern;

  for(jj=0; jj++ < len; f_val += f_inc) {
    kk = DD_SCALE(f_val, wwptr->parameter_scale, wwptr->parameter_bias);
    if(kk < -K32) kk = -K32;
    if(kk >= K32) kk = K32-1;
    *aa++ = *(wwptr->data_color_lut+kk);
  }

  aa = sfc->cb_pattern;
  bb = (guchar *)frame_configs[frame_num]->image->data;

  font_height = sfc->font_height;

  if (pd->cb_loc == PARAM_CB_LEFT || pd->cb_loc == PARAM_CB_RIGHT) {
    /* color bar increases from bottom to top */
    cc = bb + (sfc->height -2) * sfc->width;

    cc += (pd->cb_loc == PARAM_CB_LEFT) ? 1 : sfc->width -font_height -1;
# ifdef obsolete
    cc -= 2 * sfc->width;
# endif
    stride = sfc->width +font_height;

    for (jj=0; jj < len; jj++, y--, aa++, cc -= stride) {
      ee = cc + font_height;
      for (; cc < ee ; *cc++ = *aa);
    }
  }
  else {
    width = sfc->width;
    y = sfc->height -2;
    bb += y * width +2;
    x = 0;
    for (jj = 0; jj < font_height; jj++, bb -= width) {
      memcpy (bb, aa, len);
    }
  }
 }

/* c---------------------------------------------------------------------- */

static void show_param_generic_list_widget (guint frame_num, guint menu_wid )
{
  GdkPoint *ptp;
  gint x, y;
  guint widget_id;
  GtkWidget *widget;

  switch (menu_wid) {
    case PARAM_PALETTE_LIST:
     widget_id = FRAME_PALETTE_LIST;
     break;

    case PARAM_CLR_TBL_LIST:
     widget_id = FRAME_COLR_TBL_LIST;
     break;

    case PARAM_COLORS_LIST:
     widget_id = FRAME_COLORS_LIST;
     break;
  };

  widget = sii_get_widget_ptr (frame_num, widget_id);

  if( !widget )
    { param_list_widget ( frame_num, widget_id, menu_wid); }
  else {
    if (menu_wid == PARAM_CLR_TBL_LIST) {
      sii_redisplay_list (frame_num, PARAM_CLR_TBL_TEXT);
    }
     ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
     x = ptp->x; y = ptp->y;
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */

void show_param_widget (GtkWidget *text, gpointer data )
{
  GtkWidget *widget;
  guint frame_num = GPOINTER_TO_UINT (data);
  GdkPoint *ptp;
  gint x, y;

  widget = sii_get_widget_ptr (frame_num, FRAME_PARAMETERS);

  if( !widget )
    { sii_param_widget( frame_num ); }
  else {
     ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
     x = ptp->x; y = ptp->y;
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     sii_update_param_widget (frame_num);
     gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */

GdkColor *sii_annotation_color (guint frame_num, gint exposed)
{
  GdkColor *gcolor;
  ParamData *pd = frame_configs[frame_num]->param_data;
  SiiPalette *pal;

  pal = (exposed) ? pd->orig_pal : pd->pal;
  gcolor = pal->feature_color[FEATURE_ANNOTATION];
  return gcolor;
}

/* c---------------------------------------------------------------------- */

GdkColor *sii_background_color (guint frame_num, gint exposed)
{
  GdkColor *gcolor;
  ParamData *pd = frame_configs[frame_num]->param_data;
  SiiPalette *pal;

  pal = (exposed) ? pd->orig_pal : pd->pal;
  gcolor = pal->feature_color[FEATURE_BACKGROUND];
  return gcolor;
}

/* c---------------------------------------------------------------------- */

GdkColor *sii_boundary_color (guint frame_num, gint exposed)
{
  GdkColor *gcolor;
  ParamData *pd = frame_configs[frame_num]->param_data;
  SiiPalette *pal;

  pal = (exposed) ? pd->orig_pal : pd->pal;
# ifdef notyet
  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)"purple1");
# else
  gcolor = pal->feature_color[FEATURE_BND];
# endif
  return gcolor;
}

/* c---------------------------------------------------------------------- */

gchar *sii_color_tables_lister ( gpointer name, gpointer data )
{
   g_string_sprintfa (gs_color_table_names, "%s\n", (gchar*)name );

   return( FALSE );		/* to continue a tree traverse */
}

/* c---------------------------------------------------------------------- */

void sii_colorize_image (guint frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  guchar *aa, *bb, *cc, **ct_rgbs, *rgbs;
  gint jj, nn, ndx;

  sfc->colorize_count++;
  nn = sfc->width * sfc->height;
  aa = bb = (guchar *)sfc->image->data;
  bb += sfc->height * sfc->width; /* 3 byte color data space follows the image */
  rgbs = pd->pal->color_table_rgbs[0]; /* table of 3 byte colors */


  for (jj=0; jj < nn; jj++, aa++) {
    cc = rgbs + (*aa) * 3;
    *bb++ = *cc++;
    *bb++ = *cc++;
    *bb++ = *cc++;
  }
}

/* c---------------------------------------------------------------------- */

gchar *sii_colors_lister ( gpointer name, gpointer data )
{
   g_string_sprintfa (gs_color_names, "%s\n", (gchar*)name );

   return( FALSE );		/* to continue a tree traverse */
}

/* c---------------------------------------------------------------------- */

void sii_ctr_inc_from_min_max (guint ncolors, gfloat *ctr, gfloat *inc,
			       gfloat *min, gfloat *max )
{
   gfloat tmp;

   if ( *min > *max ) {
      tmp = *min; *min = *max; *max = tmp;
   }
   *inc = (*max - *min)/ncolors + CB_ROUND;
   *ctr = *min + *inc * ncolors * .5;
   *ctr += (*ctr < 0) ? -CB_ROUND : CB_ROUND;
}

/* c---------------------------------------------------------------------- */

gchar *sii_default_palettes_list()
{
   guint size=0, jj, nn;
   gchar *aa, *bb, *name;
   SiiPalette * pal;
   SiiLinkedList *item;


# ifdef notyet

   pal = sii_malloc_palette();
   pal->pname = g_string_new ("");
   pal->usual_parms = g_string_new ();
   pal->ctrinc[0] = ; pal->ctrinc[1] = ;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);
# endif
   
   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_ahav");
   pal->usual_parms = g_string_new ("AH,AV,");
   pal->ctrinc[0] = 0; pal->ctrinc[1] = 22;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_chcv");
   pal->usual_parms = g_string_new ("CH,CV,");
   pal->ctrinc[0] = .5; pal->ctrinc[1] = .1;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_pdesc");
   pal->usual_parms = g_string_new ("PD,WPD,HYDID,");
   pal->ctrinc[0] = 9.; pal->ctrinc[1] = 1.;
   g_string_assign (pal->color_table_name, "pd17");
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_raccum");
   pal->usual_parms = g_string_new ("KAC,ZAC,DAC,HAC,NAC,GAC,");
   pal->num_colors = 10;
   pal->ctrinc[0] = 50.; pal->ctrinc[1] = 10.;
   g_string_truncate (pal->color_table_name, 0);
   g_string_append (pal->color_table_name, "bluebrown10");
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_rho");
   pal->usual_parms = g_string_new ("RHOHV,RHO,RH,RX,");
   pal->ctrinc[0] = .5; pal->ctrinc[1] = .1;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_kdp");
   pal->usual_parms = g_string_new ("KDP,CKDP,NKDP,MKDP,DKD_DSD,");
   pal->ctrinc[0] = .7; pal->ctrinc[1] = .12;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_nt");
   pal->usual_parms = g_string_new ("NT_DSD,");
   pal->ctrinc[0] = 2.; pal->ctrinc[1] = .25;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_res");
   pal->usual_parms = g_string_new ("RES_DSD,");
   pal->ctrinc[0] = 5.; pal->ctrinc[1] = .6;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_d0");
   pal->usual_parms = g_string_new ("D0_DSD,");
   pal->ctrinc[0] = 2.; pal->ctrinc[1] = .25;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_lam");
   pal->usual_parms = g_string_new ("LAM_DSD,");
   pal->ctrinc[0] = 5.; pal->ctrinc[1] = .6;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_lwd");
   pal->usual_parms = g_string_new ("LWC_DSD,");
   pal->ctrinc[0] = .8; pal->ctrinc[1] = .1;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_mu");
   pal->usual_parms = g_string_new ("MU_DSD,");
   pal->ctrinc[0] = 5.; pal->ctrinc[1] = .6;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_n0");
   pal->usual_parms = g_string_new ("N0_DSD,");
   pal->ctrinc[0] = 4.; pal->ctrinc[1] = .5;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_phi");
   pal->usual_parms = g_string_new ("PHIDP,PHI,PH,DP,NPHI,CPHI");
   pal->ctrinc[0] = 70; pal->ctrinc[1] = 10;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_zdr");
   pal->usual_parms = g_string_new ("ZDR,ZD,DR,UZDR,");
   pal->ctrinc[0] = 4; pal->ctrinc[1] = .7;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_ldr");
   pal->usual_parms = g_string_new ("LDR,TLDR,ULDR,LVDR,LH,LV");
   pal->ctrinc[0] = -6; pal->ctrinc[1] = 4;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_dBm");
# ifdef notyet
   pal->usual_parms = g_string_new ("DM,LM,XM,XL,DL,DX,XH,XV,CH,CV");
# else
   pal->usual_parms = g_string_new ("DM,LM,XM,XL,DL,DX");
# endif
   pal->ctrinc[0] = -80.; pal->ctrinc[1] = 5;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_dBz");
   pal->usual_parms = g_string_new ("DBZ,DZ,XZ,DB,Z,UDBZ,CDZ,DCZ,");
   pal->ctrinc[0] = 15; pal->ctrinc[1] = 5;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_spectral");
   pal->usual_parms = g_string_new ("SR,SW,S1,S2");
   pal->ctrinc[0] = 8; pal->ctrinc[1] = 1;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_ncp");
   pal->usual_parms = g_string_new ("NCP,NC,");
   pal->ctrinc[0] = .5; pal->ctrinc[1] = .1;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

# ifdef notyet
   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_fore");
   pal->usual_parms = g_string_new ("VR,");
   pal->ctrinc[0] = -40.; pal->ctrinc[1] = 7;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_aft");
   pal->usual_parms = g_string_new ("VR,");
   pal->ctrinc[0] = 40.; pal->ctrinc[1] = 7;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);
# endif

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_vel");
   pal->usual_parms = g_string_new ("VR,VF,VG,VH,VN,VE,VU,VT,V1,V2,VELOCITY,");
   pal->ctrinc[0] = 0; pal->ctrinc[1] = 3;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_rrate");
   pal->usual_parms = g_string_new ("RR_DSD,RNX,RZD,RKD,");
   pal->ctrinc[0] = 0; pal->ctrinc[1] = .4;
   pal->color_table_name = g_string_new ("rrate11");
   pal->num_colors = 11;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_niq");
   pal->usual_parms = g_string_new ("NIQ,");
   pal->ctrinc[0] = -60; pal->ctrinc[1] = 7;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   pal = sii_malloc_palette();
   pal->p_name = g_string_new ("p_aiq");
   pal->usual_parms = g_string_new ("AIQ,");
   pal->ctrinc[0] = 0; pal->ctrinc[1] = 22;
   item = sii_ll_malloc_item ();
   item->data = (gpointer)pal;
   sii_ll_push (&palette_stack, item);

   return NULL;
}

/* c---------------------------------------------------------------------- */

void sii_do_annotation (guint frame_num, gint exposed, gboolean blow_up)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  SiiFrameConfig *sfc0 = frame_configs[0];
  GdkFont *font;
  GdkColor *gcolor;
  GtkWidget *frame;
  ParamData *pd = frame_configs[frame_num]->param_data;
  SiiPalette *pal;
  gchar *aa, annot[16], str[256], *sptrs[32];
  const gchar *cc;
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  gint x, y, lbearing, rbearing, width, height, ascent, descent, font_height;
  gint frame_width, frame_height, label_height;
  gint b_width, mx_width = 0, jj, kk, nl, widths[64], cb_height, xb, yb;
  gint cb_thickness;
  gdouble cb_locs[64];
  GdkGC *gc;

  
  if (blow_up) {
    font = sfc0->big_font;
    gc = sfc0->big_gc;
    cb_thickness = sfc->font_height *2;
    font_height = sfc0->big_font_height;
    frame  = sfc0->blow_up_frame;
    frame_width = sfc0->big_width;
    frame_height = sfc0->big_height;
  }
  else {
    font = sfc->font;
    gc =  sfc->gc;
    cb_thickness = 
      font_height = sfc->font_height;
    frame  = sfc->frame;
    frame_width = sfc->width;
    frame_height = sfc->height;
  }

  pal = (exposed) ? pd->orig_pal : pd->pal;
  gdk_text_extents (font, " ", strlen (" ")
		    , &lbearing, &rbearing, &b_width, &ascent, &descent);
  

  /* color bar labels */
  cc = set_cb_labeling_info (frame_num, cb_locs);
  strcpy (str, cc);
  nl = dd_tokens (str, sptrs);
  label_height = font->ascent + font->descent;
  
  for (mx_width=0,jj=0; jj < nl; jj++) {	/* set width of longest annotation */
    aa = sptrs[jj];
    gdk_text_extents (font, aa, strlen (aa), &lbearing, &rbearing
		      , widths+jj, &ascent, &descent);
    if (widths[jj] > mx_width)
      { mx_width = widths[jj]; }
  }

  if (pd->cb_loc == PARAM_CB_LEFT || pd->cb_loc == PARAM_CB_RIGHT) {
    if (blow_up) {
      sfc0->big_max_lbl_wdt = mx_width + 2*b_width;
    }
    else {
      sfc->max_lbl_wdt = mx_width + 2*b_width;
    }
    cb_height = frame_height;

    for (kk=nl-1,jj=0; jj < nl; jj++,kk--) {
      x = (pd->cb_loc == PARAM_CB_LEFT)
	? cb_thickness + b_width + mx_width -widths[jj]
	: frame_width -cb_thickness -b_width -widths[jj];

      y = frame_height -cb_height * cb_locs[jj]
	- font_height/2 -2;
      gcolor = pal->feature_color[FEATURE_BACKGROUND];
      gdk_gc_set_foreground (gc, gcolor );
      if (pd->toggle[PARAM_HILIGHT]) {
	gdk_draw_rectangle (frame->window
			    , gc
			    , TRUE
			    , x-1, y+3
			    , widths[jj]+1, font_height-3 );
      }
      y += label_height +1;
      gcolor = pal->feature_color[FEATURE_ANNOTATION];
      gdk_gc_set_foreground (gc, gcolor );
      aa = sptrs[jj];
      gdk_draw_text (frame->window
		     , font, gc
		     , x, y
		     , aa, strlen (aa));
    }
  }
  else {
    
    /* clear space for color bar labels */
    gcolor = pal->feature_color[FEATURE_BACKGROUND];
    gdk_gc_set_foreground (gc, gcolor );

    y = frame_height -cb_thickness -4;
    yb = frame_height -cb_thickness -font_height +1;
    
    for (jj=0; jj < nl; jj++) {
      x = (gint)(frame_width * cb_locs[jj]);
      aa = sptrs[jj];
      x -= widths[jj]/2;

      if (pd->toggle[PARAM_HILIGHT]) {
	gcolor = pal->feature_color[FEATURE_BACKGROUND];
	gdk_gc_set_foreground (gc, gcolor );
	gdk_draw_rectangle (frame->window
			    , gc
			    , TRUE
			    , x-1, yb
			    , widths[jj]+2, font_height-2 );
      }
      gcolor = pal->feature_color[FEATURE_ANNOTATION];
      gdk_gc_set_foreground (gc, gcolor );
      gdk_draw_text (frame->window
		     , font, gc
		     , x, y
		     , aa, strlen (aa));
    }
  }

  gcolor = pal->feature_color[FEATURE_BACKGROUND];
  gdk_gc_set_foreground (gc, gcolor );
  
  aa = wwptr->top_line;
  gdk_text_extents (font, aa, strlen (aa)
		    , &lbearing, &rbearing, &width, &ascent, &descent);
  x = (frame_width -width)/2;
  
  /* clear space for plot title */
  if (pd->toggle[PARAM_HILIGHT]) {
    gdk_draw_rectangle (frame->window
			, gc
			, TRUE
			, x, 0
			, width+1, font_height );
  }
  y = font_height -2;
  gcolor = pal->feature_color[FEATURE_ANNOTATION];
  gdk_gc_set_foreground (gc, gcolor );
  
  gdk_draw_text (frame->window
		 , font, gc
		 , x, y
		 , aa, strlen (aa));

  gdk_text_extents (font, " ", strlen (" ")
		    , &lbearing, &rbearing, &b_width, &ascent, &descent);
}

/* c---------------------------------------------------------------------- */

void sii_double_colorize_image (guint frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  SiiFrameConfig *sfc0 = frame_configs[0];
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  guchar *aa, *bb, *cc, *ee, **ct_rgbs, *rgbs;
  gint jj, kk, mm, nn, stride, ndx;

  mm = sfc->width;
  nn = sfc->height;
  stride = sfc->width * 3 * 2;

  aa = (guchar *)sfc->image->data;
  ee = bb = frame_configs[0]->big_image->data;
  rgbs = pd->pal->color_table_rgbs[0]; /* table of 3 byte colors */


  for (kk=0; kk < nn; kk++, bb += stride) {
     ee = bb;
     for (jj=0; jj < mm; jj++, aa++) {
	cc = rgbs + (*aa) * 3;
	*bb++ = *cc;
	*bb++ = *(cc+1);
	*bb++ = *(cc+2);
	*bb++ = *cc;
	*bb++ = *(cc+1);
	*bb++ = *(cc+2);
     }
     memcpy (bb, ee, stride);
  }

}

/* c---------------------------------------------------------------------- */

void sii_get_clip_rectangle (guint frame_num, Rectangle *clip
			     , gboolean blow_up)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  SiiFrameConfig *sfc0 = frame_configs[0];
  ParamData *pd = frame_configs[frame_num]->param_data;
  guint mlw;
  gint font_height, max_lbl_wdt, frame_width, frame_height, cb_thickness;

  if (blow_up) {
    max_lbl_wdt = sfc0->big_max_lbl_wdt;
    font_height = sfc0->big_font_height;  
    frame_width = sfc0->big_width;
    frame_height = sfc0->big_height;
    cb_thickness = 2 * sfc->font_height;
  }
  else {
    max_lbl_wdt = sfc->max_lbl_wdt;
    font_height = sfc->font_height;  
    frame_width = sfc->width;
    frame_height = sfc->height;
    cb_thickness = sfc->font_height;
  }

  if (pd->cb_loc == PARAM_CB_LEFT || pd->cb_loc == PARAM_CB_RIGHT) {
    mlw = max_lbl_wdt;

    clip->x = (pd->cb_loc == PARAM_CB_LEFT)
      ? cb_thickness + mlw +1
      : 0;
    clip->y = font_height +2;
    clip->width = frame_width -cb_thickness - mlw -2;
    clip->height = frame_height -font_height -2;
  }
  else {
    clip->x = 0;
    clip->width = frame_width;
    clip->y = font_height +2;
    clip->height = frame_height - 2*font_height -cb_thickness -4;
  }
}

/* c---------------------------------------------------------------------- */

gint sii_get_ctr_and_inc (guint frame_num, float *ctr, float *inc)
{
  WW_PTR wwptr;
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;

  *ctr = pd->pal->ctrinc[0];
  *inc = pd->pal->ctrinc[1];
  return pd->pal->num_colors;
}

/* c---------------------------------------------------------------------- */

gint sii_get_emph_min_max (guint frame_num, float *zmin, float *zmax)
{
  WW_PTR wwptr;
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;

  *zmin = pd->pal->emphasis_zone[0];
  *zmax = pd->pal->emphasis_zone[1];
  return pd->pal->num_colors;
}

/* c---------------------------------------------------------------------- */

GdkColor *sii_grid_color (guint frame_num, gint exposed)
{
  GdkColor *gcolor;
  ParamData *pd = frame_configs[frame_num]->param_data;
  SiiPalette *pal;

  pal = (exposed) ? pd->orig_pal : pd->pal;
  gcolor = pal->feature_color[FEATURE_RNG_AZ_GRID];
  return gcolor;
}

/* c---------------------------------------------------------------------- */

void sii_initialize_parameter (guint frame_num, gchar *name)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   SiiLinkedList *item;
   GString gs;
   SiiPalette *pal;
   guint size, jj;
   gchar *aa;

   if (!pd) {
      pd = (ParamData *)g_malloc0 (sizeof(ParamData));
      frame_configs[frame_num]->param_data = (gpointer)pd;

      frame_configs[frame_num]->link_set[LI_PARAM] = 
	pd->param_links = sii_new_links_info
	  ( "Parameter Links", frame_num, FRAME_PARAM_LINKS, TRUE );
      pd->cb_loc = PARAM_CB_BOTTOM;
      pd->toggle[PARAM_HILIGHT] = TRUE;
   }
   if (!param_palette_names)
     { sii_new_palettes_list (); }

   pd->pal = pal = sii_set_palette (name);

   for (jj=0; jj < PARAM_MAX_WIDGETS; jj++) {
      pd->precision[jj] = 3;
      pd->orig_txt[jj] = g_string_new ("");
      pd->txt[jj] = g_string_new ("");
   }
   /*
    * Set all the original values and copies from the palette
    *
    */
   g_string_assign (pd->orig_txt[PARAM_NAME], name);

   g_string_assign (pd->orig_txt[PARAM_GRID_CLR], pal->grid_color->str);

   g_string_assign (pd->orig_txt[PARAM_CLR_TBL_NAME], pal->color_table_name->str);

   g_string_assign (pd->orig_txt[PARAM_PALETTE], pal->p_name->str);

   g_string_assign (pd->orig_txt[PARAM_XCEED_CLR], pal->exceeded_color->str);

   g_string_assign (pd->orig_txt[PARAM_MSSNG_CLR], pal->missing_data_color->str);

   g_string_assign (pd->orig_txt[PARAM_BND_CLR], pal->boundary_color->str);

   g_string_assign (pd->orig_txt[PARAM_ANNOT_CLR], pal->annotation_color->str);

   g_string_assign (pd->orig_txt[PARAM_BACKG_CLR], pal->background_color->str);

   g_string_assign (pd->orig_txt[PARAM_EMPH_CLR], pal->emphasis_color->str);


   size = 2 * sizeof (gfloat);

   g_memmove (pd->orig_values[PARAM_CTRINC], pal->ctrinc, size);
   aa = sii_set_string_from_vals (pd->orig_txt[PARAM_CTRINC], 2
				  , pal->ctrinc[0], pal->ctrinc[1], 3);

   sii_min_max_from_ctr_inc (pd->pal->num_colors, &pal->ctrinc[0], &pal->ctrinc[1],
			     &pal->minmax[0], &pal->minmax[1]);
   
   g_memmove (pd->orig_values[PARAM_MINMAX], pal->minmax, size);
   aa = sii_set_string_from_vals (pd->orig_txt[PARAM_MINMAX], 2
				  , pal->minmax[0], pal->minmax[1], 3);

   g_memmove (pd->orig_values[PARAM_EMPH_MINMAX], pal->emphasis_zone, size);
   aa = sii_set_string_from_vals (pd->orig_txt[PARAM_EMPH_MINMAX], 2
				  , pal->emphasis_zone[0], pal->emphasis_zone[1], 3);


   sii_param_dup_orig (frame_num);

   /* set up circular que of toggle info */

   pd->fields_list = sii_init_circ_que (maxFrames);
   pd->field_toggle_count = 2;

}
/* c---------------------------------------------------------------------- */

SiiPalette *sii_malloc_palette()
{
   SiiPalette * pal;

   pal = (SiiPalette*)g_malloc0 (sizeof (SiiPalette));
   pal->ctrinc[0] = 0; pal->ctrinc[1] = 5;
   pal->minmax[0] = pal->ctrinc[0] - .5 * pal->num_colors * pal->ctrinc[1];
   pal->minmax[1] = pal->ctrinc[0] + pal->num_colors * pal->ctrinc[1];
   pal->num_colors = 17;
   pal->color_table_name = g_string_new ("carbone17");
   pal->grid_color = g_string_new ("gray90");
   pal->missing_data_color = g_string_new ("darkslateblue");
   pal->exceeded_color = g_string_new ("gray70");
   pal->annotation_color = g_string_new ("gray90");
   pal->background_color = g_string_new ("midnightblue");
   pal->emphasis_color = g_string_new ("hotpink");
   pal->boundary_color = g_string_new ("orangered");
   return pal;
}

/* c---------------------------------------------------------------------- */

void sii_min_max_from_ctr_inc (guint ncolors, gfloat *ctr, gfloat *inc,
			       gfloat *min, gfloat *max )
{
   gfloat tmp;

   *min = *ctr - *inc * ncolors * .5;
   *max = *min + *inc * ncolors;

   *min += (*min < 0) ? -CB_ROUND : CB_ROUND;
   *max += (*max < 0) ? -CB_ROUND : CB_ROUND;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_new_palette_for_param (gchar *p_name, gchar *name)
{
   SiiPalette *pal = NULL;
   SiiLinkedList *item;
   gchar str[512], *sptrs[64];
   int nt, nn;
   
   /* see if this palette already exists
    * and if so return it with the name added
    * to the usualparams
    */

   if (item = sii_palette_seek (p_name)) {			/* found */
     sii_ll_remove (&palette_stack, item);
   }
   else {
      pal = sii_malloc_palette();
      pal->p_name = g_string_new (p_name);
      pal->usual_parms = g_string_new ("");
      item = sii_ll_malloc_item ();
      item->data = (gpointer)pal;
   }
   pal = (SiiPalette *)item->data;
   sii_palette_prepend_usual_param (pal, name);
   sii_palette_remove_usual_param (pal, name);
   sii_ll_push (&palette_stack, item);
   sii_new_palettes_list ();
   return item;
}

/* c---------------------------------------------------------------------- */

gchar *sii_new_color_tables_list ()
{ 
   SiiPalette * pal;
   GSList *list;
   guint size=0, jj, nn;
   gchar *aa, *bb, *name, *usual_names;;

   if (!gs_color_table_names) {
      gs_color_table_names = g_string_new ("");
   }
   g_string_truncate (gs_color_table_names, 0);

   g_tree_traverse ( ascii_color_tables
		   , (gpointer)sii_color_tables_lister, G_IN_ORDER, 0 );

   param_color_table_names = gs_color_table_names->str;
   return param_color_table_names;
}

/* c---------------------------------------------------------------------- */

gchar *sii_new_colors_list ()
{ 
   SiiPalette * pal;
   GSList *list;
   guint size=0, jj, nn;
   gchar *aa, *bb, *name, *usual_names;

   if (!gs_color_names) {
      gs_color_names = g_string_new ("");
   }
   g_string_truncate (gs_color_names, 0);

   g_tree_traverse ( colors_tree
		   , (gpointer)sii_colors_lister, G_IN_ORDER, 0 );

   param_color_names = gs_color_names->str;
   return param_color_names;
}

/* c---------------------------------------------------------------------- */

gchar *sii_new_palettes_list ()
{ 
   static GString *gs_list = NULL;
   SiiPalette * pal;
   GSList *list;
   guint size=0, jj, nn;
   gchar *aa, *bb, *name, *usual_names;
   SiiLinkedList *item = palette_stack;

   if (!gs_list) {
      gs_list = g_string_new ("");
      sii_default_palettes_list();
   }
   g_string_truncate (gs_list, 0);

   for (item = palette_stack; item;  item = item->next) {
      pal = (SiiPalette *)item->data;
      g_string_sprintfa (gs_list, "%s  (%s)\n", pal->p_name->str
			 , pal->usual_parms->str );
   }
   param_palette_names = gs_list->str;

   return param_palette_names;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_palette_for_param (gchar *name)
{
   SiiPalette *pal = NULL;
   SiiLinkedList *item = palette_stack;
   gchar str[512], *sptrs[64];
   int nt, nn;
   
   for(; item; item = item->next) {
      pal = (SiiPalette *)item->data;
      strcpy (str, pal->usual_parms->str);
      nt = dd_tokenz (str, sptrs, " ,");
      nn = sii_str_seek (sptrs, nt, name);
      if (nn >= 0)
	{ return item; }
   }
   return NULL;
}

/* c---------------------------------------------------------------------- */

void sii_palette_prepend_usual_param (SiiPalette *pal, gchar *name)
{
   /*
    * Or shift the name to the front
    */
   gchar str[512], *sptrs[64];
   int jj, nt, nn;
   
   if (!pal)
     { return; }

   strcpy (str, pal->usual_parms->str);
   nt = dd_tokenz (str, sptrs, COMMA);
   nn = sii_str_seek (sptrs, nt, name);

   g_string_truncate (pal->usual_parms, 0);
   g_string_append (pal->usual_parms, name);
   g_string_append (pal->usual_parms, COMMA);

   for (jj=0; jj < nt; jj++) {
      if( nn >= 0 && jj == nn)
	{ continue; }		/* ignore if already there */
      g_string_append (pal->usual_parms, sptrs[jj]);
      g_string_append (pal->usual_parms, COMMA);
   }
}

/* c---------------------------------------------------------------------- */

void sii_palette_remove_usual_param ( SiiPalette *palx, gchar *name)
{
   /*
    * remove the name from all other palettes except this one
    */
   SiiPalette *pal = NULL;
   SiiLinkedList *item = palette_stack;
   gchar str[512], *sptrs[64];
   int jj, nt, nn;
   
   if (!palx)
     { return; }

   for(; item; item = item->next) {

      pal = (SiiPalette *)item->data;
      if (pal == palx)
	{ continue;}
      strcpy (str, pal->usual_parms->str);
      nt = dd_tokenz (str, sptrs, COMMA);
      nn = sii_str_seek (sptrs, nt, name);

      if (nn < 0 || nt < 1)
	{ continue; }

      g_string_truncate (pal->usual_parms, 0);
      if (nt == 1) {
	 g_string_append (pal->usual_parms, COMMA);
	 continue;
      }
      for (jj=0; jj < nt; jj++) {
	 if( nn >= 0 && jj == nn)
	   { continue; }	/* ignore if already there */
	 g_string_append (pal->usual_parms, sptrs[jj]);
	 g_string_append (pal->usual_parms, COMMA);
      }
   }
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_palette_seek (gchar *p_name)
{
   SiiPalette *pal = NULL;
   SiiLinkedList *item = palette_stack;
   gchar str[512], *sptrs[64];
   int nt, nn;
   
   for(; item; item = item->next) {
      pal = (SiiPalette *)item->data;
      if (strcmp (pal->p_name->str, p_name) == 0 )
	{ return item; }
   }
   return NULL;
}

/* c---------------------------------------------------------------------- */

void sii_param_dup_entries (guint frame_num)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   guint size, jj, entry_flag;
   gchar *aa;
   size = sizeof (*pd->values[jj]);

   for (jj=0; jj < PARAM_MAX_WIDGETS; jj++) {
      if (!pd->entry_flag[jj])
	{ continue; }

      aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[jj]));
      g_string_assign (pd->txt[jj], aa);
   }
}


/* c---------------------------------------------------------------------- */

void sii_param_dup_opal (struct solo_palette *opal)
{
  SiiPalette *pal;
  SiiLinkedList *item;
  gchar *aa, *bb, str[256];

  item = sii_palette_seek (opal->palette_name);
  if (item) {
    sii_ll_remove (&palette_stack, item);
  }
  else {
    pal = sii_malloc_palette();
    pal->p_name = g_string_new (opal->palette_name);
    pal->usual_parms = g_string_new ("");
    item = sii_ll_malloc_item ();
    item->data = (gpointer)pal;
  }
  pal = (SiiPalette *)item->data;
  sii_ll_push (&palette_stack, item);
  sii_new_palettes_list ();


  g_string_assign (pal->usual_parms, opal->usual_parms);
  pal->ctrinc[0] = opal->center;
  pal->ctrinc[1] = opal->increment;
  pal->num_colors = opal->num_colors;
  pal->emphasis_zone[0] = opal->emphasis_zone_lower;
  pal->emphasis_zone[1] = opal->emphasis_zone_upper;

  aa = opal->color_table_name;
  bb = (gchar *)g_tree_lookup( ascii_color_tables, (gpointer)aa);

  if (!bb) {
    strcpy (str, aa);
    if (bb = strstr (str, ".colors")) {
      *bb = '\0';
      aa = str;
      bb = (gchar *)g_tree_lookup( ascii_color_tables, (gpointer)aa);
      if (!bb)
      { aa = "carbone17"; pal->num_colors = 17; }
    }
    else
      { aa = "carbone17"; pal->num_colors = 17; }
  }

  g_string_assign (pal->color_table_name, aa);

  g_string_assign (pal->grid_color
		   , sii_param_fix_color (opal->grid_color));
  g_string_assign (pal->missing_data_color
		   , sii_param_fix_color (opal->missing_data_color));
  g_string_assign (pal->exceeded_color
		   , sii_param_fix_color (opal->exceeded_color));
  g_string_assign (pal->annotation_color
		   , sii_param_fix_color (opal->annotation_color));
  g_string_assign (pal->background_color
		   , sii_param_fix_color (opal->background_color));
  g_string_assign (pal->emphasis_color
		   , sii_param_fix_color (opal->emphasis_color));
  g_string_assign (pal->boundary_color
		   , sii_param_fix_color (opal->bnd_color));
}

/* c---------------------------------------------------------------------- */

void sii_param_dup_orig (guint frame_num)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   guint size, jj, entry_flag;
   size = sizeof (*pd->values[jj]);

   for (jj=0; jj < PARAM_MAX_WIDGETS; jj++) {

      entry_flag = pd->entry_flag[jj];
      if (!entry_flag)
	{ continue; }
				/*
      g_message ("jj:%d  ef:%d", jj, entry_flag);
				 */
      switch (entry_flag) {

       case ENTRY_TXT_ONLY:
	 break;

       default:
	 g_string_assign (pd->txt[jj], pd->orig_txt[jj]->str);
	 g_memmove (pd->values[jj], pd->orig_values[jj], sizeof (*pd->values[jj]));
	 break;
      };
   }
}

/* c---------------------------------------------------------------------- */

void sii_param_dup_pal (gpointer sii_pal, gpointer old_pal)
{
  struct solo_palette *opal;
  SiiPalette *pal;

  opal = (struct solo_palette *)old_pal;
  pal = (SiiPalette *)sii_pal;

  strcpy (opal->palette_name, pal->p_name->str);
  strcpy (opal->usual_parms, pal->usual_parms->str);
  opal->center = pal->ctrinc[0];
  opal->increment = pal->ctrinc[1];
  opal->num_colors = pal->num_colors;
  opal->emphasis_zone_lower = pal->emphasis_zone[0];
  opal->emphasis_zone_upper = pal->emphasis_zone[1];  
  strcpy (opal->color_table_name, pal->color_table_name->str);
  strcpy (opal->grid_color, pal->grid_color->str);
  strcpy (opal->missing_data_color, pal->missing_data_color->str);
  strcpy (opal->exceeded_color, pal->exceeded_color->str);
  strcpy (opal->annotation_color, pal->annotation_color->str);
  strcpy (opal->background_color, pal->background_color->str);
  strcpy (opal->emphasis_color, pal->emphasis_color->str);
  strcpy (opal->bnd_color, pal->boundary_color->str);

}

/* c---------------------------------------------------------------------- */

void sii_param_check_changes (guint frame_num)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   gchar *aa;
   guint jj;

   for (jj=0; jj < PARAM_MAX_WIDGETS; pd->change_flag[jj++] = FALSE);
   pd->change_count = 0;
   
   for (jj=0; jj < PARAM_MAX_WIDGETS; jj++) {
     if (!pd->entry_flag[jj])
       { continue; }
     aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[jj]));
     aa = pd->txt[jj]->str;

     if (strcmp (pd->orig_txt[jj]->str, aa) != 0) {
       pd->change_flag[jj] = TRUE;
       pd->change_count++;
     }
   }
}

/* c---------------------------------------------------------------------- */

void sii_param_entry_info
(GtkWidget *w, guint wid, guint frame_num, guint entry_flag)
{
   pdata->data_widget[wid] = w;
   pdata->entry_flag[wid] = entry_flag;
}

/* c---------------------------------------------------------------------- */

void sii_param_entry_paste_cb ( GtkWidget *w, gpointer   data )
{
   GtkWidget *widget;
   guint num = GPOINTER_TO_UINT (data);
   guint frame_num, wid;
   ParamData *pdata;
   
   frame_num = num/TASK_MODULO;
   wid = num % TASK_MODULO;
   pdata = (ParamData *)frame_configs[frame_num]->param_data;

				/* clear out the entry befor pasting */
   gtk_entry_set_text (GTK_ENTRY (pdata->data_widget[wid]), "");

}

/* c---------------------------------------------------------------------- */

gchar *sii_param_fix_color (gchar *old_color)
{
  static gchar str[32], *aa;

  if(!(aa = strstr (old_color, "grey")))
    { return old_color; }
  strcpy (str, old_color);
  strncpy (str, "gray", 4);
  return str;
}
/* c---------------------------------------------------------------------- */

void sii_param_menu_cb ( GtkWidget *w, gpointer   data )
{
   guint num = GPOINTER_TO_UINT (data);
   guint nn, frame_num, task, wid, frame_wid, cancel_wid, active, taskx;
   gchar *aa, *bb, *line, str[128], *sptrs[16], *p_name, *name;
   gfloat f1, f2, ftmp, ctr, inc, rnd = .000001;
   gint nt, ncolors = 17, jj, flag;
   ParamData *pd, *pdx;
   GtkWidget *widget, *check_item, *rmi;
   SiiLinkedList *item;   
   SiiPalette *pal;
   WW_PTR wwptr;
   GtkWidget *fs;
   gchar * dirroot;

				/* c...menu_cb */
   frame_num = num/TASK_MODULO;
   wid = task = num % TASK_MODULO;

   pd = (ParamData *)frame_configs[frame_num]->param_data;
   widget =
     check_item =
       rmi = pd->data_widget[task];



   switch (task) {
   case PARAM_REPLOT_THIS:
   case PARAM_REPLOT_LINKS:
   case PARAM_REPLOT_ALL:
     flag = REPLOT_THIS_FRAME;

     if (task == PARAM_REPLOT_LINKS)
       { flag = REPLOT_LOCK_STEP; }
     else if (task == PARAM_REPLOT_ALL)
       { flag = REPLOT_ALL; }

     sii_param_dup_entries (frame_num);
     if (sii_set_param_info (frame_num))
       { sii_plot_data (frame_num, flag); }
     break;
     
   case PARAM_PALETTE_LIST:
   case PARAM_COLORS_LIST:
     show_param_generic_list_widget (frame_num, task);
     break;

   case PARAM_CLR_TBL_LIST:
     show_param_generic_list_widget (frame_num, task);
     break;

   case PARAM_COLORS_FILESEL:
     wid = sii_return_colors_filesel_wid();
     dirroot = (gchar *)gtk_object_get_data (GTK_OBJECT(w), "dirroot");
     fs = sii_filesel (wid, dirroot);
     gtk_object_set_data (GTK_OBJECT(fs)
			  , "frame_num", (gpointer)frame_num);
     widget = sii_get_widget_ptr (frame_num, FRAME_PARAM_ENTRY);
     gtk_widget_hide (widget);
     break;

   case PARAM_MPORT_CLR_TBL:
     widget = sii_get_widget_ptr (frame_num, FRAME_PARAM_ENTRY);
     if (!widget)
       {
	  if (!(aa = getenv ("COLORS_FILESEL")))
	    { aa = "/"; }
	 param_entry_widget(frame_num
 , "\n Type the full path name of the color table file and press return \n"
       , aa); }
     else
       { gtk_widget_show (widget); }
     break;

   case PARAM_CLR_TBL_PATH:
     aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[PARAM_GNRK_ENTRY]));
     sii_param_absorb_ctbl (frame_num, aa);

   case PARAM_GNRK_ENTRY_CANCEL:
     widget = sii_get_widget_ptr (frame_num, FRAME_PARAM_ENTRY);
     if (widget)
       { gtk_widget_hide (widget); }
     break;

   case PARAM_PALETTE:
      p_name = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[wid]));
      name = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[PARAM_NAME]));
      sii_new_palette_for_param (p_name, name);
      sii_param_process_changes (frame_num);
      sii_redisplay_list (frame_num, PARAM_PALETTES_TEXT);
      break;

    case PARAM_NAME:
      nn = PARAM_NAME;
      aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[wid]));
      if( aa && strlen (aa))
	{ sii_param_dup_entries (frame_num); }
      else
	{ break; }

      sii_param_process_changes (frame_num);
      sii_redisplay_list (frame_num, PARAM_PALETTES_TEXT);
      break;

    case PARAM_MINMAX:
      aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[wid]));
      if( !sii_str_values ( aa, 2, &f1, &f2 )) {
	 gtk_entry_set_text (GTK_ENTRY (pd->data_widget[PARAM_MINMAX])
			     , pd->txt[PARAM_MINMAX]->str);
	 break;
      }
      sii_ctr_inc_from_min_max (pd->pal->num_colors, &ctr, &inc, &f1, &f2);

      aa = sii_set_string_from_vals (pd->txt[PARAM_MINMAX], 2, f1, f2, 3);
      gtk_entry_set_text (GTK_ENTRY (pd->data_widget[PARAM_MINMAX]), aa);
			  
      aa = sii_set_string_from_vals (pd->txt[PARAM_CTRINC], 2, ctr, inc, 3);
      gtk_entry_set_text (GTK_ENTRY (pd->data_widget[PARAM_CTRINC]), aa);

      break;

    case PARAM_CTRINC:
      aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[wid]));
      if( !sii_str_values ( aa, 2, &ctr, &inc ))
	{ break; }

      sii_min_max_from_ctr_inc (pd->pal->num_colors, &ctr, &inc, &f1, &f2);

      aa = sii_set_string_from_vals (pd->txt[PARAM_MINMAX], 2, f1, f2, 3);
      gtk_entry_set_text (GTK_ENTRY (pd->data_widget[PARAM_MINMAX]), aa);
			  
      aa = sii_set_string_from_vals (pd->txt[PARAM_CTRINC], 2, ctr, inc, 3);
      gtk_entry_set_text (GTK_ENTRY (pd->data_widget[PARAM_CTRINC]), aa);

      break;

    case PARAM_EMPH_MINMAX:
      aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[wid]));
      if( !sii_str_values ( aa, 2, &f1, &f2 ))
	{ break; }
      
      pd->values[task][0] = f1;
      pd->values[task][1] = f2;
      break;
      
    case PARAM_OK:
      sii_param_dup_entries (frame_num);
      sii_set_param_info (frame_num);
    case PARAM_CANCEL:
    case PARAM_CLOSE:
      widget = sii_get_widget_ptr (frame_num, FRAME_PARAMETERS);
      if( widget )
	{ gtk_widget_hide (widget); }
      widget = sii_get_widget_ptr (frame_num, FRAME_PALETTE_LIST);
      if( widget )
	{ gtk_widget_hide (widget); }
      widget = sii_get_widget_ptr (frame_num, FRAME_COLR_TBL_LIST);
      if( widget )
	{ gtk_widget_hide (widget); }
      widget = sii_get_widget_ptr (frame_num, FRAME_COLORS_LIST);
      if( widget )
	{ gtk_widget_hide (widget); }
      widget = sii_get_widget_ptr (frame_num, FRAME_PARAM_LINKS);
      if( widget )
	{ gtk_widget_hide (widget); }
      break;
      
    case PARAM_LINKS:
      show_links_widget (pd->param_links); 
      break;

    case PARAM_PALETTE_CANCEL:
      widget = sii_get_widget_ptr (frame_num, FRAME_PALETTE_LIST);
      if( widget )
	{ gtk_widget_hide (widget); }
      break;
      
    case PARAM_CLR_TBL_CANCEL:
      widget = sii_get_widget_ptr (frame_num, FRAME_COLR_TBL_LIST);
      if( widget )
	{ gtk_widget_hide (widget); }
      break;
      
    case PARAM_COLORS_CANCEL:
      widget = sii_get_widget_ptr (frame_num, FRAME_COLORS_LIST);
      if( widget )
	{ gtk_widget_hide (widget); }
      break;
      
    case PARAM_GRID_CLR:
    case PARAM_CLR_TBL_NAME:
    case PARAM_XCEED_CLR:
    case PARAM_MSSNG_CLR:
    case PARAM_ANNOT_CLR:
    case PARAM_BACKG_CLR:
    case PARAM_EMPH_CLR:
    case PARAM_BND_CLR:
      aa = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[wid]));
      if( aa && strlen (aa))
	{ g_string_assign (pd->txt[task], aa ); }
      break;

      /* Check menu item */
   case PARAM_ELECTRIC:
   case PARAM_HILIGHT:
   case PARAM_CB_SYMBOLS:
      pd->toggle[task] = GTK_CHECK_MENU_ITEM (rmi)->active;
      break;

   case PARAM_CB_BOTTOM:
   case PARAM_CB_LEFT:
   case PARAM_CB_RIGHT:
     g_return_if_fail (GTK_IS_RADIO_MENU_ITEM (rmi));
     if( active = GTK_CHECK_MENU_ITEM (check_item)->active)
       { pd->cb_loc = task; }
     pd->toggle[task] = active;
     break;

   case PARAM_BC_GRID:
   case PARAM_BC_BND:
   case PARAM_BC_BACKG:
   case PARAM_BC_XCEED:
   case PARAM_BC_MSSNG:
   case PARAM_BC_EMPH:
     for(item = palette_stack; item; item = item->next) {
	pal = (SiiPalette *)item->data;
	if (pal == pd->pal)
	  { continue; }
	if (task == PARAM_BC_GRID)
	  {g_string_assign (pal->grid_color, pd->pal->grid_color->str);}
	else if (task == PARAM_BC_BND)
	  {g_string_assign (pal->boundary_color, pd->pal->boundary_color->str);}
	else if (task == PARAM_BC_BACKG)
	  {g_string_assign (pal->background_color, pd->pal->background_color->str);}
	else if (task == PARAM_BC_XCEED)
	  {g_string_assign (pal->exceeded_color, pd->pal->exceeded_color->str);}
	else if (task == PARAM_BC_MSSNG)
	  {g_string_assign (pal->missing_data_color, pd->pal->missing_data_color->str);}
	else if (task == PARAM_BC_EMPH)
	  {g_string_assign (pal->emphasis_color, pd->pal->emphasis_color->str);}


	g_string_assign (pal->grid_color, pd->pal->grid_color->str);
     }
     for (jj=0; jj < maxFrames; jj++) {
	solo_return_wwptr(jj)->parameter->changed = YES;
     }
     break;

   case PARAM_BC_ANNOT:
     for(item = palette_stack; item; item = item->next) {
	pal = (SiiPalette *)item->data;
	if (pal == pd->pal)
	  { continue; }
	g_string_assign (pal->annotation_color
			 , pd->pal->annotation_color->str);
     }
     for (jj=0; jj < maxFrames; jj++) {
	if (jj == frame_num)
	  { continue; }
	widget = sii_get_widget_ptr (jj, FRAME_PARAMETERS);
	pdx = (ParamData *)frame_configs[jj]->param_data;
	pdx->toggle[PARAM_HILIGHT] = active = 
	  pd->toggle[PARAM_HILIGHT];
	if( widget ) {
	   check_item = pd->data_widget[PARAM_HILIGHT];
	   gtk_check_menu_item_set_active
	     (GTK_CHECK_MENU_ITEM (check_item), active );
	}
	solo_return_wwptr(jj)->parameter->changed = YES;
     }
     break;

   case PARAM_BC_CB:
     for(item = palette_stack; item; item = item->next) {
	pal = (SiiPalette *)item->data;
	if (pal == pd->pal)
	  { continue; }
     }
     for (jj=0; jj < maxFrames; jj++) {
	if (jj == frame_num)
	  { continue; }
	widget = sii_get_widget_ptr (jj, FRAME_PARAMETERS);
	pdx = (ParamData *)frame_configs[jj]->param_data;
	pdx->cb_loc = pd->cb_loc;

	if( widget ) {
	   check_item = pd->data_widget[pdx->cb_loc];
	   gtk_check_menu_item_set_active
	     (GTK_CHECK_MENU_ITEM (check_item), TRUE );
	}
	pdx->toggle[PARAM_CB_SYMBOLS] = active = 
	  pd->toggle[PARAM_CB_SYMBOLS];
	if( widget ) {
	   check_item = pd->data_widget[PARAM_CB_SYMBOLS];
	   gtk_check_menu_item_set_active
	     (GTK_CHECK_MENU_ITEM (check_item), active );
	}
	pdx->toggle[PARAM_HILIGHT] = active = 
	  pd->toggle[PARAM_HILIGHT];
	if( widget ) {
	   check_item = pd->data_widget[PARAM_HILIGHT];
	   gtk_check_menu_item_set_active
	     (GTK_CHECK_MENU_ITEM (check_item), active );
	}
	solo_return_wwptr(jj)->parameter->changed = YES;
     }
     break;

   case PARAM_OVERVIEW:
     nn = sizeof (hlp_param_overview)/sizeof (char *);
     sii_glom_strings (hlp_param_overview, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case PARAM_HLP_FILE:
     nn = sizeof (hlp_param_file)/sizeof (char *);
     sii_glom_strings (hlp_param_file, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case PARAM_HLP_OPTIONS:
     nn = sizeof (hlp_param_options)/sizeof (char *);
     sii_glom_strings (hlp_param_options, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case PARAM_HLP_LINKS:
     nn = sizeof (hlp_param_links)/sizeof (char *);
     sii_glom_strings (hlp_param_links, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case PARAM_HLP_PALETTES:
     nn = sizeof (hlp_param_palettes)/sizeof (char *);
     sii_glom_strings (hlp_param_palettes, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   case PARAM_HLP_MINMAX:
     nn = sizeof (hlp_param_minmax)/sizeof (char *);
     sii_glom_strings (hlp_param_minmax, nn, gen_gs);
     sii_message (gen_gs->str);
     break;

   };
}

/* c---------------------------------------------------------------------- */

void sii_param_menubar2( GtkWidget  *window, GtkWidget **menubar
		       , guint frame_num)
{
   gint jj, nn, pdi, task, active, widget_id;
   guint radio_num = 0;
   GtkWidget *check_item, *mbar, *menuitem, *submenu, *next_item;

   radio_group = NULL;
   
   *menubar = mbar = gtk_menu_bar_new ();

   submenu = sii_submenu ( "File", mbar );
   
   widget_id = PARAM_CLOSE;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Close", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = PARAM_PALETTE_LIST;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "List Palettes", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_CLR_TBL_LIST;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "List Color Tables", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_COLORS_LIST;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "List Colors", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = PARAM_MPORT_CLR_TBL;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Import a Color Table", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = PARAM_ZERO;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Broadcast...", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_GRID;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Grid Color", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_BND;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Boundary Color", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_XCEED;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Exceeded Color", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_MSSNG;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Missing Data Color", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_ANNOT;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Annotation Color", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_BACKG;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Background Color", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_EMPH;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Emphasis Color", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_BC_CB;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Color Bar Settings", submenu, widget_id
		       , sii_param_menu_cb, frame_num );



   submenu = sii_submenu ( "Options", mbar );

   widget_id = PARAM_ZERO;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Color Bar", submenu, widget_id
			, sii_param_menu_cb, frame_num );

   radio_group = NULL;
   radio_num = 0;

   widget_id = PARAM_CB_BOTTOM;
   pdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "At Bottom", submenu, widget_id
			       , sii_param_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   widget_id = PARAM_CB_LEFT;
   pdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "At Left", submenu, widget_id
			       , sii_param_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   widget_id = PARAM_CB_RIGHT;
   pdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "At Right", submenu, widget_id
			       , sii_param_menu_cb, frame_num, ++radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );

   widget_id = PARAM_CB_SYMBOLS;
   radio_num = 0;		/* implies a check menu item */
   pdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Use Symbols", submenu, widget_id
			       , sii_param_menu_cb, frame_num, radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
     
   widget_id = PARAM_HILIGHT;
   radio_num = 0;		/* implies a check menu item */
   pdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "HiLight Labels", submenu, widget_id
			       , sii_param_menu_cb, frame_num, radio_num
			       , &radio_group);

   /* add a seperator */
   menuitem = sii_submenu_item ( NULL, submenu, 0, NULL, frame_num );
     
   widget_id = PARAM_ELECTRIC;
   radio_num = 0;		/* implies a check menu item */
   pdata->data_widget[widget_id] = menuitem =
     sii_toggle_submenu_item ( "Electric Params", submenu, widget_id
			       , sii_param_menu_cb, frame_num, radio_num
			       , &radio_group);




   submenu = sii_submenu ( "Replot", mbar );

   widget_id = PARAM_REPLOT_LINKS;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Replot Links", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_REPLOT_ALL;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Replot All", submenu, widget_id
		       , sii_param_menu_cb, frame_num );



   submenu = sii_submenu ( "ParamLinks", mbar );

   widget_id = PARAM_LINKS;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Set Links", submenu, widget_id
		       , sii_param_menu_cb, frame_num );
     


   submenu = sii_submenu ( "Help", mbar );

   widget_id = PARAM_OVERVIEW;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Overview", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_HLP_FILE;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "File", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_HLP_OPTIONS;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Options", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_HLP_LINKS;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "ParamLinks", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_HLP_PALETTES;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Palettes", submenu, widget_id
		       , sii_param_menu_cb, frame_num );

   widget_id = PARAM_HLP_MINMAX;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Min & Max", submenu, widget_id
		       , sii_param_menu_cb, frame_num );
# ifdef obsolete
   widget_id = PARAM_HLP_EMPHASIS;
   pdata->data_widget[widget_id] = menuitem =
     sii_submenu_item ( "Emphasis", submenu, widget_id
		       , sii_param_menu_cb, frame_num );
# endif

  check_item = pdata->data_widget[PARAM_ELECTRIC];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

  check_item = pdata->data_widget[PARAM_HILIGHT];
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

}

/* c---------------------------------------------------------------------- */

const gchar *sii_param_palette_name (guint frame_num)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   return pd->pal->p_name->str;
}

/* c---------------------------------------------------------------------- */

void sii_param_process_changes (guint frame_num)
{
   /*
      If you want to associate the current parameter name with a
      differen palette, type in the new palette name and press <enter>.

      Or if you want to define a new name and a new palette,
      type in the name but DO NOT press enter. Then type in
      the new palette name and press <enter>.
      
      Change the other parameters to suite your needs and click
      "Replot" or "OK" and the parameters will persist.
    */

   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   SiiPalette *pal, *pal0;
   gfloat f1, f2;
   guint jj, kk, wid;
   gchar *aa, *bb, *p_name, *name;
   gboolean ok;


   sii_param_dup_entries (frame_num);
   sii_param_check_changes (frame_num);
   name = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[PARAM_NAME]));

   if(pd->change_flag[PARAM_PALETTE]) {
      p_name = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[PARAM_PALETTE]));
      sii_new_palette_for_param (p_name, name);
   }
   else if(pd->change_flag[PARAM_NAME]) {
     /* the name changed but the palette didn't */
     pd->pal = sii_set_palette (name);
   }
   
   pal = pd->pal;
   jj = PARAM_CTRINC;
   kk = PARAM_MINMAX;
   
   if(pd->change_flag[jj]) {
      /* calculate min max */
     ok = sii_str_values (pd->txt[jj]->str, 2, &f1, &f2);

     if (ok) {
       pal->ctrinc[0] = f1; pal->ctrinc[1] = f2;
       bb = sii_set_string_from_vals
	 (pd->orig_txt[jj], 2, pal->ctrinc[0], pal->ctrinc[1]
	  , pd->precision[jj]);
       gtk_entry_set_text (GTK_ENTRY (pd->data_widget[jj]), bb);
       sii_min_max_from_ctr_inc (pal->num_colors
				 , &pal->ctrinc[0], &pal->ctrinc[1]
				 , &pal->minmax[0], &pal->minmax[1]);
       bb = sii_set_string_from_vals
	 (pd->orig_txt[kk], 2, pal->minmax[0], pal->minmax[1]
	  , pd->precision[kk]);
       gtk_entry_set_text (GTK_ENTRY (pd->data_widget[kk]), bb);
     }
   }
   else if(pd->change_flag[kk]) {
     /* calculate ctr inc */
     ok = sii_str_values (pd->txt[jj]->str, 2, &f1, &f2);

     if (ok) {
       pal->minmax[0] = f1; pal->minmax[1] = f2;
       bb = sii_set_string_from_vals
	 (pd->orig_txt[kk], 2, pal->minmax[0], pal->minmax[1]
	  , pd->precision[kk]);
       gtk_entry_set_text (GTK_ENTRY (pd->data_widget[kk]), bb);
       sii_ctr_inc_from_min_max (pal->num_colors
				 , &pal->ctrinc[0], &pal->ctrinc[1]
				 , &pal->minmax[0], &pal->minmax[1]);
       bb = sii_set_string_from_vals
	   (pd->orig_txt[jj], 2, pal->ctrinc[0], pal->ctrinc[1]
	    , pd->precision[jj]);
       gtk_entry_set_text (GTK_ENTRY (pd->data_widget[jj]), bb);
     }
   }
   
   for (jj=0; jj < PARAM_MAX_WIDGETS; jj++) {
      if (!pd->entry_flag[jj] || !pd->change_flag[jj])
	{ continue; }

      its_changed = TRUE;
      aa = pd->txt[jj]->str;

      switch (jj) {
	 
       case PARAM_GRID_CLR:
	 g_string_assign (pal->grid_color, aa);
	 break;
	 
       case PARAM_CLR_TBL_NAME:
	 g_string_assign (pal->color_table_name, aa);
	 break;
	 
       case PARAM_BND_CLR:
	 g_string_assign (pal->boundary_color, aa);
	 break;
	 
       case PARAM_XCEED_CLR:
	 g_string_assign (pal->exceeded_color, aa);
	 break;
	 
       case PARAM_MSSNG_CLR:
	 g_string_assign (pal->missing_data_color, aa);
	 break;

       case PARAM_ANNOT_CLR:
	 g_string_assign (pal->annotation_color, aa);
	 break;

       case PARAM_BACKG_CLR:
	 g_string_assign (pal->background_color, aa);
	 break;

       case PARAM_EMPH_MINMAX:
	 sii_str_values (aa, 2, &pal->emphasis_zone[0]
			 , &pal->emphasis_zone[1]);
	 break;

       case PARAM_EMPH_CLR:
	 g_string_assign (pal->emphasis_color, aa);
	 break;
      };
   }
   name = gtk_entry_get_text( GTK_ENTRY (pd->data_widget[PARAM_NAME]));
   sii_set_entries_from_palette (frame_num, name, NULL);
}

/* c---------------------------------------------------------------------- */

void sii_param_reset_changes (guint frame_num)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   guint jj;

   for (jj=0; jj < PARAM_MAX_WIDGETS; jj++) {
      pd->change_flag[jj] = FALSE;
   }
}

/* c---------------------------------------------------------------------- */

void sii_param_set_plot_field (int frame_num, char *fname)
{
  /* for toggling */

  ParamData *pd = frame_configs[frame_num]->param_data;
  SiiLinkedList *sll = pd->fields_list;
  ParamFieldInfo *pfi;
  char str[16];

  strcpy (str, fname);
  g_strstrip (str);

  if (!sll->data) {		/* first time */
    sll->data = (gpointer)g_malloc0 (sizeof (*pfi));
    pfi = (ParamFieldInfo *)sll->data;
    strcpy (pfi->name, str);
    return;
  }
  pfi = (ParamFieldInfo *)sll->data;
  if (strcmp (pfi->name, str) == 0) /* replotting the same field */
    { return;}

  pd->toggle_field = pd->fields_list;
  pd->fields_list = sll = sll->next;

  if (!sll->data) {
    sll->data = (gpointer)g_malloc0 (sizeof (*pfi));
  }
  pfi = (ParamFieldInfo *)sll->data;
  strcpy (pfi->name, str);
  return;
}

/* c---------------------------------------------------------------------- */

void sii_param_toggle_field (guint frame_num)
{
  ParamData *pd = frame_configs[frame_num]->param_data;
  SiiLinkedList *sll = pd->toggle_field;
  ParamFieldInfo *pfi, *pfi1, *pfi2;

  if (!sll)
    { return; }

  pfi = (ParamFieldInfo *)sll->data;
  gtk_entry_set_text (GTK_ENTRY (pd->data_widget[PARAM_NAME]), pfi->name);
  if (sii_set_param_info (frame_num))
    { sii_plot_data (frame_num, REPLOT_THIS_FRAME); }
  sii_redisplay_list (frame_num, PARAM_PALETTES_TEXT);

  pd->toggle_field = pd->fields_list->previous;
  pfi1 = (ParamFieldInfo *)pd->fields_list->data;
  pfi2 = (ParamFieldInfo *)pd->toggle_field->data;
}

/* c---------------------------------------------------------------------- */

void sii_param_widget( guint frame_num )
{
  GtkWidget *label;
  GtkWidget *widget;

  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *hbox0;
  GtkWidget *hbox2;
  GtkWidget *hbbox;

  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *vbox3;

  GtkWidget *text;
  GtkWidget *table;
  GtkWidget *table2;
  GtkWidget *hscrollbar;
  GtkWidget *vscrollbar;

  GtkWidget *entry;
  GtkWidget *entrybar;
  GtkWidget *entrybar2;
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
  guint widget_id;
  gfloat upper;
  GdkPoint *ptp;
  gint x, y;
  SiiPalette *pal;

				/* c...code starts here */

  pdata = (ParamData *)frame_configs[frame_num]->param_data;
  pal = pdata->pal;

  edit_font = med_pro_font;
  edit_fore = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"black");
  edit_back = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"white");


  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  sii_set_widget_ptr ( frame_num, FRAME_PARAMETERS, window );
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num * TASK_MODULO + FRAME_PARAMETERS));

  /* --- Title and border --- */
  bb = g_strdup_printf ("Frame %d  Parameter and Colors Widget", frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);
				/*
  sii_param_menubar (window, &menubar, frame_num);
				 */
  sii_param_menubar2 (window, &menubar, frame_num);
  gtk_container_add (GTK_CONTAINER(vbox), menubar);

  hbox0 = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (vbox), hbox0);

  widget = gtk_drawing_area_new ();
  gtk_box_pack_start (GTK_BOX (hbox0), widget, TRUE, TRUE, 0 );
  pdata->data_widget[PARAM_COLOR_TEST] = widget;

  g_string_truncate (pdata->orig_txt[PARAM_LABEL], 0);
  g_string_sprintfa (pdata->orig_txt[PARAM_LABEL], "   %d Colors   "
			, pal->num_colors);
  label = gtk_label_new ( pdata->orig_txt[PARAM_LABEL]->str );
  pdata->data_widget[PARAM_LABEL] = label;
  gtk_box_pack_start (GTK_BOX (hbox0), label, TRUE, TRUE, 0 );
  gtk_drawing_area_size (GTK_DRAWING_AREA (widget), 96, 0);


  hbbox = gtk_hbutton_box_new ();
  gtk_hbutton_box_set_spacing_default (4);
  gtk_box_pack_end (GTK_BOX (hbox0), hbbox, TRUE, TRUE, 0 );

  button = gtk_button_new_with_label ("Replot");
  gtk_box_pack_start (GTK_BOX (hbbox), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + PARAM_REPLOT_THIS;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("OK");
  gtk_box_pack_start (GTK_BOX (hbbox), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + PARAM_OK;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("Cancel");
  gtk_box_pack_start (GTK_BOX (hbbox), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + PARAM_CANCEL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_param_menu_cb
		      , (gpointer)nn
		      );
  gtk_object_set_data (GTK_OBJECT(button)
		       , "widget_id", (gpointer)PARAM_CANCEL);
  hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (vbox), hbox);
  gtk_container_border_width (GTK_CONTAINER (hbox), 4);

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (hbox), vbox2);

  label = gtk_label_new ( "Parameters" );
  gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER );
  gtk_container_add (GTK_CONTAINER (vbox2), label);

  table = gtk_table_new (2, 1, FALSE);
  gtk_box_pack_start (GTK_BOX (vbox2), table, TRUE, TRUE, 0 );

  aa = pdata->param_names_list->str;

  widget_id = PARAM_NAMES_TEXT;
  text = gtk_text_new (NULL,NULL);
  pdata->data_widget[widget_id] = text;
  gtk_widget_set_usize (table, 0, 288);
  gtk_widget_set_events (text, event_flags );
  gtk_signal_connect_after (GTK_OBJECT(text)
		      ,"button_press_event"
		      , (GtkSignalFunc) param_text_event_cb
		      , (gpointer)aa);

  gtk_object_set_data (GTK_OBJECT(text),
		       "frame_num",
		       (gpointer)frame_num);

  gtk_object_set_data (GTK_OBJECT(text),
		       "widget_id",
		       (gpointer)widget_id);

  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    xoptions, yoptions, 0, 0);
  
  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    xoptions, yoptions, 0, 0);


  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , aa, -1 );


  table = gtk_table_new (2, 16, FALSE);
  gtk_box_pack_start (GTK_BOX (hbox), table, TRUE, TRUE, 0 );
  row = -1;

  row++;
  label = gtk_label_new ( " Parameter Name " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_NAME]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_NAME, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Min & Max " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_MINMAX]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_MINMAX, frame_num, ENTRY_TWO_FLOATS );

  row++;
  label = gtk_label_new ( " Center & Increment " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_CTRINC]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_CTRINC, frame_num, ENTRY_TWO_FLOATS );

  row++;
  label = gtk_label_new ( " Color Palette " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_PALETTE]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_PALETTE, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Grid Color " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_GRID_CLR]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_GRID_CLR, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Color Table Name " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_CLR_TBL_NAME]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_CLR_TBL_NAME, frame_num, ENTRY_TXT_ONLY );


  row++;
  label = gtk_label_new ( " " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  row++;
  label = gtk_label_new ( " Boundary Color " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_BND_CLR]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_BND_CLR, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Exceeded Color " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_XCEED_CLR]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_XCEED_CLR, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Missing Data Color " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_MSSNG_CLR]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_MSSNG_CLR, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Annotation Color " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_ANNOT_CLR]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_ANNOT_CLR, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Background Color " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_BACKG_CLR]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_BACKG_CLR, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Emphasis Color " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_EMPH_CLR]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_EMPH_CLR, frame_num, ENTRY_TXT_ONLY );

  row++;
  label = gtk_label_new ( " Emphasis Min/Max " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  aa = pdata->orig_txt[PARAM_EMPH_MINMAX]->str;
  gtk_entry_set_text (GTK_ENTRY (entry), aa);
  gtk_table_attach (GTK_TABLE (table), entry, 1, 2, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  sii_param_entry_info( entry, PARAM_EMPH_MINMAX, frame_num, ENTRY_TWO_FLOATS );


  gtk_container_foreach (GTK_CONTAINER (table),
			 sii_set_widget_frame_num,
			 (gpointer)frame_num );

  sii_update_param_widget (frame_num);

 /* --- Make everything visible --- */
  gtk_widget_show_all (window);

  for (jj=0; jj < PARAM_MAX_WIDGETS; jj++ ) {

     if (pdata->entry_flag[jj]) {
	nn = frame_num * TASK_MODULO + jj;
	gtk_signal_connect (GTK_OBJECT(pdata->data_widget[jj])
			    ,"activate"
			    , (GtkSignalFunc) sii_param_menu_cb
			    , (gpointer)nn );
	gtk_signal_connect (GTK_OBJECT(pdata->data_widget[jj])
			    ,"paste_clipboard"
			    , (GtkSignalFunc) sii_param_entry_paste_cb
			    , (gpointer)nn );
     }
  }
}

/* c---------------------------------------------------------------------- */

void sii_redisplay_list (guint frame_num, guint widget_id)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   GtkWidget *text = pd->data_widget[widget_id];
   char *aa;

   if (!text)
     { return; }

   switch (widget_id) {

    case PARAM_PALETTES_TEXT:
      aa = param_palette_names;
      break;

    case PARAM_NAMES_TEXT:
      aa = param_names;
      break;

    case PARAM_CLR_TBL_TEXT:
      aa = param_color_table_names;
      break;

    default:
      return;
   };

   gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
   gtk_text_insert (GTK_TEXT (text)
		    , edit_font, edit_fore, edit_back, aa, -1);
   gtk_adjustment_set_value (GTK_ADJUSTMENT (GTK_TEXT (text)->vadj), 0);
}

/* c---------------------------------------------------------------------- */

void sii_reset_image (guint frame_num)
{
  SiiFrameConfig *sfc = frame_configs[frame_num];
  WW_PTR wwptr = solo_return_wwptr(frame_num);
  guchar *img;
  int jj;

  jj = wwptr->background_color_num;

  if (!(img = (guchar *)sfc->image->data))
    { return; }

  memset (img, jj, sfc->width * sfc->height);
}

/* c---------------------------------------------------------------------- */

void sii_set_entries_from_palette (guint frame_num, gchar *name, gchar *p_name)
{
   SiiPalette *pal = NULL;
   SiiLinkedList *item = NULL;
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   gchar *aa, p_name_new[16];
   guint jj, wid;

   if( name && strlen (name)) {
      pal = sii_set_palette (name);
   }
   else if( p_name && strlen (p_name)) {
      item = sii_palette_seek (p_name);
      pal = (SiiPalette *)item->data;
   }

   if (!pal)
     { pal = pd->pal; }
   pd->pal = pal;

# ifdef notyet

   aa = pal->->str;
   wid = ;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);
# endif

   aa = pal->grid_color->str;
   wid = PARAM_GRID_CLR;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->color_table_name->str;
   wid = PARAM_CLR_TBL_NAME;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->p_name->str;
   wid = PARAM_PALETTE;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->boundary_color->str;
   wid = PARAM_BND_CLR;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->exceeded_color->str;
   wid = PARAM_XCEED_CLR;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->missing_data_color->str;
   wid = PARAM_MSSNG_CLR;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->annotation_color->str;
   wid = PARAM_ANNOT_CLR;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->background_color->str;
   wid = PARAM_BACKG_CLR;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);

   aa = pal->emphasis_color->str;
   wid = PARAM_EMPH_CLR;
   g_string_assign (pd->orig_txt[wid], aa);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), aa);


   wid = PARAM_CTRINC;
   g_memmove (pd->orig_values[wid], pal->ctrinc, sizeof (*pal->ctrinc));
   sii_set_string_from_vals
     (pd->orig_txt[wid], 2, pal->ctrinc[0], pal->ctrinc[1]
      , pd->precision[wid]);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid]), pd->orig_txt[wid]->str);

   wid = PARAM_MINMAX;
   sii_min_max_from_ctr_inc (pal->num_colors, &pal->ctrinc[0], &pal->ctrinc[1],
			     &pal->minmax[0], &pal->minmax[1]);
   g_memmove (pd->orig_values[wid], pal->minmax, sizeof(*pal->minmax));
   aa = sii_set_string_from_vals
     (pd->orig_txt[wid], 2, pal->minmax[0], pal->minmax[1]
      , pd->precision[wid]);

   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid])
		       , pd->orig_txt[wid]->str);

   wid = PARAM_EMPH_MINMAX;
   g_memmove (pd->orig_values[wid], pal->emphasis_zone
	      , sizeof (*pal->emphasis_zone));
   sii_set_string_from_vals
     (pd->orig_txt[wid], 2, pal->emphasis_zone[0], pal->emphasis_zone[1]
      , pd->precision[wid]);
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid])
		       , pd->orig_txt[wid]->str);
}

/* c---------------------------------------------------------------------- */

SiiPalette *sii_set_palette (gchar *name)
{
   int nn, nt;
   SiiPalette *pal = NULL;
   SiiLinkedList *item;
   
   item = sii_palette_for_param (name);

   if (!item) {
      item = (SiiLinkedList *)g_malloc0 (sizeof (SiiLinkedList));
      pal = sii_malloc_palette();
      pal->p_name = g_string_new ("");
      item->data = (gpointer)pal;
      g_string_sprintfa (pal->p_name, "p_%s", name);
      pal->usual_parms = g_string_new ("");
      g_string_sprintfa (pal->usual_parms, "%s,", name);
   }
   else {
      sii_ll_remove (&palette_stack, item);
      pal = (SiiPalette *)item->data;
      sii_palette_prepend_usual_param (pal, name);
   }

   sii_ll_push (&palette_stack, item);
   sii_new_palettes_list ();
   return pal;
}

/* c---------------------------------------------------------------------- */

gboolean
sii_set_param_info (guint frame_num)
{
  GtkWidget *widget;
  LinksInfo *li;
  gfloat ff, gg;
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  struct parameter_widget_info pwi;
  char str[256], *sptrs[32], *aa, *bb, *cc;
  gint nt, nn, jj;
  WW_PTR wwptr = solo_return_wwptr(frame_num);

  widget = sii_get_widget_ptr (frame_num, FRAME_PARAMETERS);

  if (!widget)
    { return TRUE; }

  sii_param_process_changes (frame_num);

  memset (&pwi, 0, sizeof (pwi));

  pwi.frame_num = frame_num;
  pwi.changed = (its_changed) ? YES : NO;

  aa = gtk_entry_get_text (GTK_ENTRY (pd->data_widget[PARAM_PALETTE]));
  strcpy (pwi.palette_name, aa);

  aa = gtk_entry_get_text (GTK_ENTRY (pd->data_widget[PARAM_NAME]));
  strcpy (pwi.parameter_name, aa);

  aa = gtk_entry_get_text (GTK_ENTRY (pd->data_widget[PARAM_CTRINC]));
  sii_str_values (aa, 2, &pd->pal->ctrinc[0], &pd->pal->ctrinc[1]);
  sii_min_max_from_ctr_inc (pd->pal->num_colors
			    , &pd->pal->ctrinc[0], &pd->pal->ctrinc[1]
			    , &pd->pal->minmax[0], &pd->pal->minmax[1]);

  aa = gtk_entry_get_text (GTK_ENTRY (pd->data_widget[PARAM_EMPH_MINMAX]));
  sii_str_values (aa, 2, &ff, &gg);
  pwi.emphasis_zone_lower = ff;
  pwi.emphasis_zone_upper = gg;

  li = frame_configs[frame_num]->link_set[LI_PARAM];
  for (jj=0; jj < maxFrames; jj++) {
    pwi.linked_windows[jj] = (li->link_set[jj]) ? 1 : 0;
  }
  wwptr->color_bar_location = 0;
  if (pd->cb_loc == PARAM_CB_LEFT)
    {  wwptr->color_bar_location = -1; }
  else if (pd->cb_loc == PARAM_CB_RIGHT)
    {  wwptr->color_bar_location = 1; }
  
  wwptr->color_bar_symbols = (pd->toggle[PARAM_CB_SYMBOLS]) ? YES : NO;

  nn = solo_set_parameter_info (&pwi);

  sii_update_param_widget (frame_num);
  its_changed = FALSE;

  return (nn) ? FALSE : TRUE;
  
}

/* c---------------------------------------------------------------------- */

gchar *sii_set_param_names_list
(guint frame_num, GString *gs_list, guint count)
{
   GString *gs_str;
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;

   if (!pd) {
      pd = (ParamData *)g_malloc0 (sizeof(ParamData));
      frame_configs[frame_num]->param_data = (gpointer)pd;
   }

   if (!pd->param_names_list) {
      pd->param_names_list = g_string_new ("");
   }
   gs_str = pd->param_names_list;
   g_string_truncate (gs_str, 0);

   if (gs_list)
     { g_string_append (gs_str, gs_list->str); }

   return gs_str->str;
}

/* c------------------------------------------------------------------------ */

void
solo_get_palette(char *name, int frame_num)
{
  SiiPalette *pal;
  WW_PTR wwptr;
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  SiiLinkedList *item;
  gchar p_name[16];

  pal = pd->pal;
  wwptr = solo_return_wwptr(frame_num);

  item = sii_palette_for_param (name);

  if (item) {
    strcpy (p_name, "p_");
    strcat (p_name, name);
    item = sii_new_palette_for_param (p_name, name);
  }

}

/* c------------------------------------------------------------------------ */

int solo_hardware_color_table(gint frame_num)
{
  int jj, kk, mm, nc=0, nn, ndx = 0, start, end, cmax;
  struct solo_perusal_info *spi, *solo_return_winfo_ptr();
  WW_PTR wwptr;
  struct solo_list_mgmt *lm;
  SiiPalette *pal;
  gchar mess[256], *aa, *bb, *cc, line[256], *table, *name, *p_name;
  GdkColor *gcolor, *gclist;
  gboolean ok = TRUE, ok2;
  float red, green, blue, rnum, rden, gnum, gden, bnum, bden;
  GdkVisual *rgb_visual;
  GdkColormap *cmap;
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  GtkWidget *widget;

# ifdef obsolete
  if (!frame_configs[frame_num]->color_map) {
    rgb_visual = gdk_rgb_get_visual ();
    frame_configs[frame_num]->color_map = gdk_colormap_new (rgb_visual, FALSE);
  }
# endif
  cmap = frame_configs[frame_num]->color_map;

  widget = sii_get_widget_ptr (frame_num, FRAME_PARAMETERS);
 
  wwptr = solo_return_wwptr(frame_num);

  if (!sii_get_widget_ptr (frame_num, FRAME_PARAMETERS)) {
     /* the palette might be wrong for this parameter */
     pd->pal = sii_set_palette (wwptr->parameter->parameter_name);
  }
  else if (strcmp (wwptr->parameter->parameter_name
		   , pd->orig_txt[PARAM_NAME]->str) == 0) {
     pd->pal = sii_set_palette (wwptr->parameter->parameter_name);
  }
  pal = pd->pal;
  mess[0] = '\0';
  lm = wwptr->list_colors;
  wwptr->num_colors_in_table = lm->num_entries = 0;
  spi = solo_return_winfo_ptr();
  
  table = (gchar *)g_tree_lookup( ascii_color_tables
				  , (gpointer)pal->color_table_name->str );
  if (!table) {
    sprintf (mess, "Color table %s cannot be found"
	     , pal->color_table_name->str);
    sii_message (mess);
    return 0;
  }
  cmax = strlen (table);
  gclist = (GdkColor *)g_malloc0 (256 * sizeof (GdkColor));
  
  for(; ndx < cmax;) {
    
    ok = sii_nab_region_from_text (table, ndx, &start, &end);
    
    if (!ok || end <= start)
      { break; }
    /*
     * advance to first non-whiteout character
     */
    strncpy (line, table+start, end-start);
    line[end-start] = '\0';
    ndx = end +1;
    bb = table+ndx;
    g_strstrip (line);
    
    if(!strlen(line) || strstr(line, "colortable") ||
       strstr(line, "endtable") || strstr(line, "!")) {
      continue;
    }
    
    if(aa=strstr(line, "xcolor")) { /* X color name */
      aa += strlen ("xcolor");
      g_strstrip (aa);
      solo_add_list_entry(lm, aa, strlen(aa));
    }
    else if(strstr(line, "/")) { /* funkier zeb color files */
      sscanf(line, "%f/%f %f/%f %f/%f"
	     , &rnum, &rden
	     , &gnum, &gden
	     , &bnum, &bden);
      kk = nc++;
      gcolor = gclist +kk;
      gcolor->red =   (gushort)((rnum/rden)*(K64 - 1));
      gcolor->green = (gnum/gden)*(K64 - 1);
      gcolor->blue =  (bnum/bden)*(K64 - 1);
      ok2 = gdk_color_alloc (cmap, gcolor);
      gcolor->pixel = kk;

      if (!ok2)
	{ ok = NO; }
    }
    else {			/* assume rgb vals between 0 and 1 */
      sscanf(line, "%f %f %f", &red, &green, &blue);
      kk = nc++;
      gcolor = gclist +kk;
      gcolor->red =     red*(K64 - 1);
      gcolor->green = green*(K64 - 1);
      gcolor->blue =   blue*(K64 - 1);
      ok2 = gdk_color_alloc (cmap, gcolor);
      gcolor->pixel = kk;
      if (!ok2)
	{ ok = NO; }
    }		   
  }

  if(!nc) {
    lm = wwptr->list_colors;
    nc = lm->num_entries;
    for(kk=0; kk < nc; kk++) {

      aa = *(lm->list +kk);
      gcolor = 
	(GdkColor *)g_tree_lookup (colors_tree, (gpointer)aa);
      *(gclist +kk) = *gcolor;

      if (!gcolor)
	{ ok = NO; }
      ok2 = gdk_color_alloc (cmap, gcolor);
      gcolor->pixel = kk;
      
      if (!ok2)
	{ ok = NO; }
    }
  }

  if (!ok) {
    sprintf (mess, "Unable to allocate colors for %s"
	     , pal->color_table_name->str);
    sii_message (mess);
    return ok;
  }
  wwptr->num_colors_in_table = 
    pal->num_colors = nc;

  if (!pal->data_color_table[0]) {

    pal->data_color_table[0] = (GdkColor *)g_new
      (GdkColor, MAX_COLOR_TABLE_SIZE);
    for (kk=1; kk < MAX_COLOR_TABLE_SIZE; kk++)
      { pal->data_color_table[kk] = pal->data_color_table[0] +kk; }

    pal->feature_color[0] = g_new (GdkColor, MAX_FEATURE_COLORS);
    for (kk=0; kk < MAX_FEATURE_COLORS; kk++)
      { pal->feature_color[kk] = pal->feature_color[0] +kk; }
  }

  for (kk=0; kk < nc; kk++) {
    gcolor = gclist +kk;
    *pal->data_color_table[kk] = *gcolor;

    mm = gcolor->red;
    pal->color_table_rgbs[kk][0] = (guchar)(mm >> 8);
    mm = gcolor->green;
    pal->color_table_rgbs[kk][1] = (guchar)(mm >> 8);
    mm = gcolor->blue;
    pal->color_table_rgbs[kk][2] = (guchar)(mm >> 8);

    (wwptr->xcolors+kk)->pixel = kk;

# ifdef notyet
    gdk_color_change (cmap, gcolor);
# endif
  }

  g_free (gclist);

  if (!solo_palette_color_numbers(frame_num))
    { ok = NO; }

  if (ok)
    { pd->orig_pal = pal; }

  return ok;
				/*
Gdk-ERROR **: BadAccess (attempt to access private resource denied)
  serial 2508 error_code 10 request_code 89 minor_code 0
Gdk-ERROR **: BadAccess (attempt to access private resource denied)
  serial 2509 error_code 10 request_code 89 minor_code 0
				 */
}

/* c------------------------------------------------------------------------ */

int solo_palette_color_numbers(int frame_num)
{
  /* this routine assumes that all the color names have been checked
   */
  gint kk, ok=YES, nc, mm, nn, ct_ndx;
  gboolean ok2;
  WW_PTR wwptr, solo_return_wwptr();
  SiiPalette *pal;
  gchar mess[256];
  GdkColor *gcolor;
  GdkColormap *cmap;
  ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
  
  wwptr = solo_return_wwptr(frame_num);
  pal = pd->pal;
  nc = pd->pal->num_colors;
  cmap = frame_configs[frame_num]->color_map;
  cmap = gtk_widget_get_colormap (frame_configs[frame_num]->frame);
  cmap = gdk_rgb_get_cmap ();
  mess[0] = '\0';

  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->exceeded_color->str);
  if (!gcolor) {
    strcat (mess, pal->exceeded_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_EXCEEDED] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
# ifdef notyet
    gdk_color_change (cmap, gcolor);
# endif
  }

  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->missing_data_color->str);
  if (!gcolor) {
    strcat (mess, pal->missing_data_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_MISSING] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
# ifdef notyet
    gdk_color_change (cmap, gcolor);
# endif
  }
  
  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->background_color->str);
  if (!gcolor) {
    strcat (mess, pal->background_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_BACKGROUND] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
# ifdef notyet
    gdk_color_change (cmap, gcolor);
# endif
  }

  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->emphasis_color->str);
  if (!gcolor) {
    strcat (mess, pal->emphasis_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_EMPHASIS] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
# ifdef notyet
    gdk_color_change (cmap, gcolor);
# endif
  }

  /*
   * the following two colors are drawn directly
   * as opposed to being part of the rgb image
   */

  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->boundary_color->str);
  if (!gcolor) {
    strcat (mess, pal->boundary_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_BND] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
    wwptr->bnd_color_num = gcolor->pixel;
  }

  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->annotation_color->str);
  if (!gcolor) {
    strcat (mess, pal->annotation_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_ANNOTATION] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
    wwptr->annotation_color_num = gcolor->pixel;
  }

  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->boundary_color->str);
  if (!gcolor) {
    strcat (mess, pal->boundary_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_BND] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
  }

  gcolor = (GdkColor *)g_tree_lookup
    (colors_tree, (gpointer)pal->grid_color->str);
  if (!gcolor) {
    strcat (mess, pal->grid_color->str); strcat (mess, " ");
    ok = NO;
  }
  else {
    *pal->feature_color[FEATURE_RNG_AZ_GRID] = *gcolor;
    *pal->feature_color[FEATURE_TIC_MARKS] = *gcolor;
    ok2 = gdk_color_alloc (cmap, gcolor);
    wwptr->grid_color_num = gcolor->pixel;
  }

# ifdef notyet
  wwptr->bnd_color_num = gcolor->pixel;
  wwptr->bnd_alert_num = gcolor->pixel;
  wwptr->bnd_last_num = gcolor->pixel;
# endif


  ct_ndx = pal->num_colors -1;

  for (kk=0; kk < MAX_FEATURE_COLORS; kk++) {
    if (pal->feature_color[kk]->pixel) { /* color set */

      gcolor = pal->feature_color[kk];
      mm = gcolor->red;
      pal->feature_rgbs[kk][0] = (guchar)(mm >> 8);
      mm = gcolor->green;
      pal->feature_rgbs[kk][1] = (guchar)(mm >> 8);
      mm = gcolor->blue;
      pal->feature_rgbs[kk][2] = (guchar)(mm >> 8);

      switch (kk) {
      case FEATURE_BACKGROUND:
	wwptr->background_color_num = ++ct_ndx;
	memcpy (pal->color_table_rgbs[ct_ndx], pal->feature_rgbs[kk]
		, sizeof (pal->feature_rgbs[kk]));
	break;
      case FEATURE_EXCEEDED:
	wwptr->exceeded_color_num = ++ct_ndx;
	memcpy (pal->color_table_rgbs[ct_ndx], pal->feature_rgbs[kk]
		, sizeof (pal->feature_rgbs[kk]));
	break;
      case FEATURE_MISSING:
	wwptr->missing_data_color_num = ++ct_ndx;
	memcpy (pal->color_table_rgbs[ct_ndx], pal->feature_rgbs[kk]
		, sizeof (pal->feature_rgbs[kk]));
	break;
      case FEATURE_EMPHASIS:
	wwptr->emphasis_color_num = ++ct_ndx;
	memcpy (pal->color_table_rgbs[ct_ndx], pal->feature_rgbs[kk]
		, sizeof (pal->feature_rgbs[kk]));
	break;
      };
    }
  }

  if (!ok) {
    sprintf (mess+strlen(mess), "not useful color name(s) in frame %d"
	     , frame_num);
    sii_message (mess);
  }
  return ok;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_solo_palette_stack ()
{
    struct solo_palette s_pal, *solo_malloc_old_palette();
    SiiPalette *pal;
    SiiLinkedList *item = palette_stack, *item2;
    SiiLinkedList *sps = solo_palette_stack;
    int mm = 0, nn = 0;

    /* count sii palettes */

    if (!solo_palette_stack) {
      sps = solo_palette_stack = sii_ll_malloc_item ();
      sps->data = (gpointer)solo_malloc_old_palette();
    }
    sps = solo_palette_stack;
    item = palette_stack;

    for (; item; item = item->next) {
      if (item == palette_stack) {
	sii_param_dup_pal (item->data, sps->data);
	continue;
      }
      if (!sps->next) {
	sps->next = sii_ll_malloc_item ();
	sps->next->data = (gpointer)solo_malloc_old_palette();
	sps->next->previous = sps;
      }
      sps = sps->next;
      sii_param_dup_pal (item->data, sps->data);
    }
    return solo_palette_stack;
}

/* c---------------------------------------------------------------------- */

void sii_update_param_widget (guint frame_num)
{
   ParamData *pd = (ParamData *)frame_configs[frame_num]->param_data;
   GtkWidget *text;
   LinksInfo *li;
   GtkWidget *entry, *check_item;
   guint wid, jj;
   char *solo_list_entry (struct solo_list_mgmt *slm, int jj);
   void solo_gen_parameter_list (guint frme);
   gchar *p_name, *name;
   WW_PTR wwptr = solo_return_wwptr(frame_num);
   struct solo_list_mgmt *slm;
   struct solo_perusal_info *spi = solo_return_winfo_ptr();
   GString *gs;
   GtkAdjustment *adj;
   gint value;


   pd->toggle[PARAM_CB_SYMBOLS] = (wwptr->color_bar_symbols) ? TRUE : FALSE;

   if (wwptr->color_bar_location) {
      pd->cb_loc = (wwptr->color_bar_location > 0) ?
	PARAM_CB_RIGHT : PARAM_CB_LEFT;
   }
   else
     { pd->cb_loc = PARAM_CB_BOTTOM; }

   if (!sii_get_widget_ptr (frame_num, FRAME_PARAMETERS))
     { return; }


   check_item = pd->data_widget[pd->cb_loc];
   gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (check_item), TRUE );

   check_item = pd->data_widget[PARAM_CB_SYMBOLS];
   gtk_check_menu_item_set_active
     (GTK_CHECK_MENU_ITEM (check_item), pd->toggle[PARAM_CB_SYMBOLS]);

   text = pd->data_widget[PARAM_NAMES_TEXT];
   name = wwptr->parameter->parameter_name;
   p_name = wwptr->parameter->palette_name;
   g_string_assign (pd->orig_txt[PARAM_NAME], name);
   g_string_assign (pd->orig_txt[PARAM_PALETTE], p_name);
   g_string_sprintf (pd->orig_txt[PARAM_LABEL], "   %d Colors   "
		     , pd->pal->num_colors);
   gtk_label_set (GTK_LABEL (pd->data_widget[PARAM_LABEL])
		  , pd->orig_txt[PARAM_LABEL]->str);

   

   wid = PARAM_NAME;
   gtk_entry_set_text (GTK_ENTRY (pd->data_widget[wid])
		       , pd->orig_txt[wid]->str);
   sii_set_entries_from_palette (frame_num, name, p_name);

   li = frame_configs[frame_num]->link_set[LI_PARAM];
   for (jj=0; jj < maxFrames; jj++) {
     li->link_set[jj] = (wwptr->parameter->linked_windows[jj]) ? TRUE : FALSE;
   }

   solo_gen_parameter_list (frame_num);
   slm = spi->list_parameters;
   solo_sort_slm_entries (slm);
   gs = pd->param_names_list;
   g_string_truncate (gs, 0);

   for (jj=0; jj < slm->num_entries; jj++) {
     g_string_append (gs, solo_list_entry (slm, jj));
     g_string_append (gs, "\n");
   }
   adj = GTK_ADJUSTMENT (GTK_TEXT (text)->vadj);
   value = adj->value;

   gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
   gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , pd->param_names_list->str, -1 );

   gtk_adjustment_set_value (adj, value);
   sii_redisplay_list (frame_num, PARAM_PALETTES_TEXT);
}
/* c---------------------------------------------------------------------- */
/* c...mark */
/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

static GtkItemFactoryEntry param_menu_items[] = {
  { "/File",                    NULL,              NULL,                   0,  "<Branch>" },
  { "/File/Close",              NULL, sii_param_menu_cb,         PARAM_CLOSE,  NULL },
  { "/File/sep3",               NULL,              NULL,                   0,  "<Separator>" },
  { "/File/List Palettes",      NULL, sii_param_menu_cb,  PARAM_PALETTE_LIST,  NULL },
  { "/File/List Color Tables",  NULL, sii_param_menu_cb,  PARAM_CLR_TBL_LIST,  NULL },
  { "/File/Import Color Table", NULL, sii_param_menu_cb, PARAM_MPORT_CLR_TBL,  NULL },
  { "/File/sep3",               NULL,              NULL,                   0,  "<Separator>" },
  { "/File/List Colors",        NULL, sii_param_menu_cb,   PARAM_COLORS_LIST,  NULL },
  { "/Options",                 NULL,              NULL,                   0,  "<Branch>" },
  { "/Options/rePlot Links",    NULL, sii_param_menu_cb,  PARAM_REPLOT_LINKS,  NULL },
  { "/Options/rePlot All",      NULL, sii_param_menu_cb,    PARAM_REPLOT_ALL,  NULL },
  { "/Options/sep3",            NULL,              NULL,                   0,  "<Separator>" },
  { "/Options/Electric Params", NULL, sii_param_menu_cb,      PARAM_ELECTRIC,  "<CheckItem>" },

  { "/ParamLinks",              NULL,              NULL,                  0,  "<Branch>" },
  { "/ParamLinks/Set Links",    NULL, sii_param_menu_cb,        PARAM_LINKS,  NULL },

  { "/Help",                    NULL,              NULL,                   0,  "<Branch>" },
  { "/Help/Overview",           NULL, sii_param_menu_cb,      PARAM_OVERVIEW,  NULL },
  { "/Help/With ?",             NULL, sii_param_menu_cb,                   0,  NULL },
};

/* c---------------------------------------------------------------------- */



