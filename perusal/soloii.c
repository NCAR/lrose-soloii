/* 	$Id$	 */

# include "soloii.h"
# include "help_sii.h"
# include <stdlib.h>
# include <date_stamp.h>

GtkWidget *main_window = NULL;
GtkWidget *main_vbox = NULL;
GtkWidget *main_event_box;
GtkWidget *menubar = NULL;

GtkWidget *main_table = NULL;

GtkAllocation *sii_frame_alloc[maxFrames];
				/*
GtkAllocation *main_table_alloc = NULL;
gdouble main_table_width_change = 0;
gdouble main_table_height_change = 0;
				 */

GHashTable *colors_hash;

GSList *gen_gslist = NULL;	/* generic GSList */

GSList *color_names = NULL;
GSList *Xcolor_names = NULL;
GSList *color_table_names = NULL;
GSList *color_palette_names = NULL;

GTree *colors_tree = NULL;
GTree *ascii_color_tables = NULL;
GTree *color_tables = NULL;
GTree *color_palettes_tree = NULL;


guint sii_config_count = 0;
gfloat sii_config_w_zoom = 1.0;
gfloat sii_config_h_zoom = 1.0;
sii_config_cell **config_cells = NULL;

guint sii_frame_count = 0;
SiiFrameConfig *frame_configs[2*maxFrames +1];
GdkColor *frame_border_colors[maxFrames];

guint sii_table_widget_width = DEFAULT_WIDTH;
guint sii_table_widget_height = DEFAULT_WIDTH;

guint previous_F_keyed_frame = 0;

gint cursor_frame = 0;
/* 0 => cursor not in any frame
 * otherwise frame_num = cursor_frame-1;
 */

guint click_count = 0;
guint clicks_in_que = 0;

const guint click_que_size = 123;
SiiLinkedList *sii_click_que = NULL;

guint sii_seq_num = 0;

GdkFont *small_fxd_font = NULL;
GdkFont *med_fxd_font = NULL;
GdkFont *big_fxd_font = NULL;

GdkFont *small_pro_font = NULL;
GdkFont *med_pro_font = NULL;
GdkFont *big_pro_font = NULL;

gchar *title_tmplt = "00 00/00/2000 00:00:00 HHHHHHHH 000.0 HHH HHHHHHHH";

guint screen_width_mm = 0;
guint screen_height_mm = 0;
guint screen_width_px = 0;
guint screen_height_px = 0;

gboolean time_series_mode = FALSE;

GString *gs_complaints = NULL;

gboolean sii_electric_centering = FALSE;

gboolean nexrad_mode_set = FALSE;

gboolean uniform_frame_shape = TRUE;

gboolean batch_mode = FALSE;

GString *gs_image_dir = NULL;

gfloat batch_threshold_value = -32768;

GString *batch_threshold_field = NULL;

gboolean sii_center_crosshairs = FALSE;

GString *gen_gs = NULL;

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

enum {
  SII_MAIN_ZERO,

  SII_MAIN_DIR,
  SII_MAIN_IMAGE_DIR,
  SII_MAIN_CONFIG_DIR,
  SII_MAIN_IGNORE_SWPIFS,
  SII_MAIN_CONFIG_FILE,
  SII_MAIN_CONFIG_COMMENT,
  SII_MAIN_SAVE_CONFIG,
  SII_MAIN_LIST_CONFIGS,
  SII_MAIN_CANCEL,
  SII_MAIN_CANCEL_IMAGE_GEN,
  SII_MAIN_CTR_TOGGLE,
  SII_MAIN_XHAIRS_TOGGLE,
  SII_MAIN_PNG,
  SII_MAIN_FILESEL_OK,
  SII_MAIN_SWPFI_FILESEL,
  SII_MAIN_CONFIG_FILESEL,
  SII_MAIN_COLORS_FILESEL,

  SII_MAIN_EVENT_ENTER,
  SII_MAIN_EVENT_LEAVE,

  SII_MAIN_HLP_ZERO,
  SII_MAIN_HLP_BASIC,
  SII_MAIN_HLP_FILE,
  SII_MAIN_HLP_ZOOM,
  SII_MAIN_HLP_CTR,
  SII_MAIN_HLP_CONFIG,
  SII_MAIN_HLP_SHORTCUTS,
  SII_MAIN_HLP_COMPARE,
  SII_MAIN_HLP_ABOUT,
  SII_MAIN_HLP_LAST_ENUM,

  SII_MAIN_LAST_ENUM,
};

static GdkFont *edit_font;
static GdkColor *edit_fore;
static GdkColor *edit_back;
static GtkWidget *main_config_text = NULL;
static GtkWidget *main_config_window = NULL;
static GtkWidget *main_image_gen_window = NULL;
static GString *gs_config_file_names = NULL;
static GString *gs_initial_dir_name = NULL;
static GString *gs_scratch = NULL;

GtkWidget * nexrad_check_item;
GtkWidget * anglr_fill_item;
static int ignore_swpfi_info = 0;

/* c---------------------------------------------------------------------- */

static void print_hello( GtkWidget *w, gpointer   data );
static void sii_get_main_menu( GtkWidget  *window, GtkWidget **menubar );
void show_solo_init();
void show_solo_image_gen();
void sii_initialization_widget();
void sii_image_gen_widget();
void show_all_click_data_widgets (gboolean really);
int sii_default_startup (const gchar *swpfi_dir);
void sii_init_frame_configs();
void sii_reset_config_cells();
void soloii_get_main_menu(GtkWidget *window, GtkWidget **menubar);
void sii_set_default_colors ();
void custom_config_cb( GtkWidget *w, gpointer data );
void zoom_config_cb( GtkWidget *w, gpointer data );
void zoom_data_cb( GtkWidget *w, gpointer data );
void center_frame_cb( GtkWidget *w, gpointer   data );
void config_cb( GtkWidget *w, gpointer data );
void shape_cb( GtkWidget *w, gpointer data );
void test_widget( GtkWidget *w, gpointer data );
void image_cb( GtkWidget *w, gpointer data );
static void sii_menu_cb( GtkWidget *w, gpointer data );

gint sii_frame_keyboard_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
const GString *sii_return_config_files (const char *dir);
gchar *sii_get_swpfi_dir (gchar *dir);
void sp_change_cursor (int normal);
void solo_save_window_info (const char *dir, const char *a_comment);
int solo_absorb_window_info (const char *dir, const char *fname, int ignore_swpfi);
gchar *sii_nab_line_from_text (const gchar *txt, guint position );
void sii_png_image_prep (char *dir);
gboolean sii_batch (gpointer argu);
GtkWidget *sii_filesel (gint which_but, const gchar * dirroot);
/* c---------------------------------------------------------------------- */
/* c...events */
/* c---------------------------------------------------------------------- */

/* Obligatory basic callback */

static void print_hello( GtkWidget *w, gpointer   data )
{
  g_message ("Hello, Callback! data: %d\n", GPOINTER_TO_UINT (data) );
}
/* c---------------------------------------------------------------------- */

static void sii_menu_cb( GtkWidget *w, gpointer data )
{
  guint id = GPOINTER_TO_UINT (data), mark;
  int nn;

  switch (id) {

  case SII_MAIN_CTR_TOGGLE:
    sii_electric_centering = !sii_electric_centering;
    break;

  case SII_MAIN_XHAIRS_TOGGLE:
    sii_center_crosshairs = !sii_center_crosshairs;
    break;

  case SII_MAIN_HLP_BASIC:
    nn = sizeof (hlp_sii_basics)/sizeof (char *);
    sii_glom_strings (hlp_sii_basics, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case SII_MAIN_HLP_FILE:
    nn = sizeof (hlp_sii_file)/sizeof (char *);
    sii_glom_strings (hlp_sii_file, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case SII_MAIN_HLP_ZOOM:
    nn = sizeof (hlp_sii_zoom)/sizeof (char *);
    sii_glom_strings (hlp_sii_zoom, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case SII_MAIN_HLP_CTR:
    nn = sizeof (hlp_sii_ctr)/sizeof (char *);
    sii_glom_strings (hlp_sii_ctr, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case SII_MAIN_HLP_CONFIG:
    nn = sizeof (hlp_sii_config)/sizeof (char *);
    sii_glom_strings (hlp_sii_config, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case SII_MAIN_HLP_SHORTCUTS:
    nn = sizeof (hlp_sii_shortcuts)/sizeof (char *);
    sii_glom_strings (hlp_sii_shortcuts, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case SII_MAIN_HLP_COMPARE:
    nn = sizeof (hlp_sii_comparisons)/sizeof (char *);
    sii_glom_strings (hlp_sii_comparisons, nn, gen_gs);
    sii_message (gen_gs->str);
    break;

  case SII_MAIN_HLP_ABOUT:
    nn = sizeof (hlp_sii_about)/sizeof (char *);
    sii_glom_strings (hlp_sii_about, nn, gen_gs);
    g_string_append (gen_gs, sii_date_stamp);
    sii_message (gen_gs->str);
    break;
  };

}
/* c---------------------------------------------------------------------- */

void sii_main_event_box_cb(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
				/* c...config main window */
  guint eid = GPOINTER_TO_UINT (data), mark, cid;
  GtkWidget *framex;
  gdouble d;


  if (eid == SII_MAIN_EVENT_ENTER) {
# ifdef obsolete
    g_message ("Main event box entered");
# endif
    cid = (solo_busy()) ? 0 : 1;
    sp_change_cursor (cid);
  }
# ifdef obsolete
  else if (eid == SII_MAIN_EVENT_LEAVE) {
    g_message ("Main event box left");
    sp_change_cursor (mb_cursor);
  }
# endif
  else if (eid == 1) {
    g_message ("main event box configured %d x %d ",
	       main_event_box->allocation.width,
	       main_event_box->allocation.height);
  }
  mark = 0;
}

/* c---------------------------------------------------------------------- */

void show_solo_init (GtkWidget *w, gpointer data)
{
   GtkWidget *text = main_config_text;
   guint len;

   if (!main_config_window) {
      sii_initialization_widget();
      return;
   }
   gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
   gtk_text_insert (GTK_TEXT (text)
		    , edit_font, edit_fore, edit_back
		    , gs_config_file_names->str, -1);

   gtk_widget_show (main_config_window);
}

/* c---------------------------------------------------------------------- */

void show_solo_image_gen (GtkWidget *w, gpointer data)
{
   GtkWidget *text = main_config_text;
   guint len;

   if (!main_image_gen_window) {
      sii_image_gen_widget();
      return;
   }
   gtk_widget_show (main_image_gen_window);
}

/* c---------------------------------------------------------------------- */

int sii_initialize_cb (GtkWidget *w, gpointer data)
{
   guint task = GPOINTER_TO_UINT (data);
   GtkWidget *entry;
   GtkWidget *button;
   gint nn;
   gboolean active;
   const gchar *aa, *bb;
   const gchar *cc;
   const GString *cgs;


   switch (task) {

    case SII_MAIN_IGNORE_SWPIFS:
      button = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_config_window), "ignore");
      ignore_swpfi_info =
	gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
      break;


    case SII_MAIN_CONFIG_COMMENT:
    case SII_MAIN_SAVE_CONFIG:
      entry = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_config_window), "config_dir");
      aa = gtk_entry_get_text( GTK_ENTRY (entry));
      entry = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_config_window), "comment");
      bb = gtk_entry_get_text( GTK_ENTRY (entry));
      solo_save_window_info (aa, bb);

    case SII_MAIN_LIST_CONFIGS:
    case SII_MAIN_CONFIG_DIR:
      entry = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_config_window), "config_dir");
      aa = gtk_entry_get_text( GTK_ENTRY (entry));
      cgs = sii_return_config_files (aa);

      gtk_editable_delete_text (GTK_EDITABLE (main_config_text), 0, -1);
      gtk_text_insert (GTK_TEXT (main_config_text)
		       , edit_font, edit_fore, edit_back
		       , cgs->str, -1);
      g_string_assign (gs_config_file_names, cgs->str);
      break;

    case SII_MAIN_SWPFI_FILESEL:
    case SII_MAIN_CONFIG_FILESEL:
      if (!(aa =getenv ("INIT_FILESEL")))
	{ aa = "/"; }
      sii_filesel (task, aa);
      break;

    case SII_MAIN_DIR:
      if (sii_frame_count > 0) {
	 sii_message (
"Use Sweepfiles Widget to change directories"
		      );
	 return;
      }

      entry = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_config_window), "swpfi_dir");

      aa = gtk_entry_get_text( GTK_ENTRY (entry));
      nn = sii_default_startup (aa);
      if (nn < 1 ) {
	g_string_sprintf (gs_complaints
		      , "\nNo sweepfiles in %s\n", aa);
	sii_message (gs_complaints->str);
	break;
      }
      config_cb (0, (gpointer)22 ); /* 2x2 */

    case SII_MAIN_CANCEL:
      if (main_config_window)
	{ gtk_widget_hide (main_config_window); }
      break;

    case SII_MAIN_CANCEL_IMAGE_GEN:
      if (main_image_gen_window)
	{ gtk_widget_hide (main_image_gen_window); }
      break;

    case SII_MAIN_IMAGE_DIR:
    case SII_MAIN_PNG:
      entry = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_image_gen_window), "image_dir");

      aa = gtk_entry_get_text( GTK_ENTRY (entry));
      if (!gs_image_dir)
	{ gs_image_dir = g_string_new (aa); }
      else
	{ g_string_assign (gs_image_dir, aa); }

      if (task == SII_MAIN_IMAGE_DIR)
	{ break; }

      sii_png_image_prep (gs_image_dir->str);
      break;
   };
}

/* c---------------------------------------------------------------------- */

void sii_main_text_event_cb (GtkWidget *text, GdkEvent *event
			      , gpointer data )
{
  GtkWidget *entry, button;
  gint nn = GTK_TEXT (text)->cursor_mark.index, ii;
  gchar *line;
  const gchar *aa, *bb = gs_config_file_names->str;
   
  line = sii_nab_line_from_text (bb, nn);
  g_strstrip (line);

  entry = (GtkWidget *)gtk_object_get_data
    (GTK_OBJECT(main_config_window), "config_dir");
  aa = gtk_entry_get_text( GTK_ENTRY (entry));
  
  ii = solo_absorb_window_info
    (aa, line, (sii_frame_count)? ignore_swpfi_info : 0);
}

/* c---------------------------------------------------------------------- */

void click_data_cb (GtkWidget *text, gpointer data )
{
  gboolean really = GPOINTER_TO_UINT (data) == 1;
  int mark;

  show_all_click_data_widgets (really);
}

/* c---------------------------------------------------------------------- */

int main( int argc,
          char *argv[] )	/* c...main */
{
  GtkStyle *defstyle;
  GtkStyle *style;
  gint event_flags = GDK_LEAVE_NOTIFY_MASK;
  gint screen_width, screen_height, pix_per_mm, jj, kk, ll, mm, nn;
  gchar font_name[128], *font_name_fmt, *aa, *bb, *cc;
  gchar swi_dir[256], fname[256], str[256];
  GdkFont *font;

				/* c...code */

  colors_hash = g_hash_table_new (g_str_hash, g_str_equal);
  colors_tree = g_tree_new ((GCompareFunc)strcmp);
  ascii_color_tables = g_tree_new ((GCompareFunc)strcmp);
  color_tables = g_tree_new ((GCompareFunc)strcmp);
  gs_complaints = g_string_new ("\n");
  gen_gs = g_string_new ("");
  gs_config_file_names = g_string_new ("");

  bb = gtk_set_locale ();
  gtk_init (&argc, &argv);
  gdk_rgb_init ();

  dd_intset();			/* catch Control-C */

  screen_width_mm = gdk_screen_width_mm();
  screen_height_mm = gdk_screen_height_mm();
  screen_width_px = gdk_screen_width(); /* Sun 1280 */
  screen_height_px = gdk_screen_height();	/* Sun 1024 */

  font_name_fmt = "-adobe-courier-medium-r-normal--*-%d-*-*-m-*-iso8859-1";
  sprintf( font_name, font_name_fmt, 100 ); /* point size */
  small_fxd_font = gdk_font_load ( font_name );
  if (! small_fxd_font) {
      uii_printf("Unable to load font '%s'\n", font_name);
      exit(1);
  }

  sprintf( font_name, font_name_fmt, 120 ); /* point size */
  med_fxd_font = gdk_font_load ( font_name );
  if (! med_fxd_font) {
      uii_printf("Unable to load font '%s'\n", font_name);
      exit(1);
  }

  sprintf( font_name, font_name_fmt, 140 ); /* point size */
  big_fxd_font = gdk_font_load ( font_name );
  if (! big_fxd_font) {
      uii_printf("Unable to load font '%s'\n", font_name);
      exit(1);
  }
                 
  font_name_fmt = "-adobe-helvetica-medium-r-normal--*-%d-*-*-p-*-iso8859-1";
  sprintf( font_name, font_name_fmt, 100 ); /* point size */
  small_pro_font = gdk_font_load ( font_name );
  if (! small_pro_font) {
      uii_printf("Unable to load font '%s'\n", font_name);
      exit(1);
  }

  sprintf( font_name, font_name_fmt, 120 ); /* point size */
  med_pro_font = gdk_font_load ( font_name );
  if (! med_pro_font) {
      uii_printf("Unable to load font '%s'\n", font_name);
      exit(1);
  }

  sprintf( font_name, font_name_fmt, 140 ); /* point size */
  big_pro_font = gdk_font_load ( font_name );
  if (! big_pro_font) {
      uii_printf("Unable to load font '%s'\n", font_name);
      exit(1);
  }

				/*
      GDK_VISUAL_DIRECT_COLOR,
      GDK_VISUAL_TRUE_COLOR,
      GDK_VISUAL_PSEUDO_COLOR,
      GDK_VISUAL_STATIC_COLOR,
      GDK_VISUAL_GRAYSCALE,
      GDK_VISUAL_STATIC_GRAY
				 */
  gtk_widget_set_default_visual (gdk_rgb_get_visual ());
  gtk_widget_set_default_colormap (gdk_rgb_get_cmap ());
  gdk_rgb_set_min_colors (216);       /* 6x6x6 = 216; 7x7x7 = 343 */

  sii_set_default_colors ();
  sii_init_frame_configs();
  sii_reset_config_cells();

 
  main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  gtk_signal_connect (GTK_OBJECT (main_window), "destroy", 
		      GTK_SIGNAL_FUNC (gtk_main_quit), 
		      "WM destroy");
  gtk_window_set_title (GTK_WINDOW(main_window), "SoloII");
  gtk_widget_set_events (main_window, GDK_KEY_PRESS_MASK
			 | GDK_STRUCTURE_MASK);
  gtk_signal_connect (GTK_OBJECT(main_window),"key_press_event",
		      (GtkSignalFunc) sii_frame_keyboard_event, (gpointer)77);


  /* default for GTK windows is "user resizeable"
  gtk_window_set_policy( GTK_WINDOW(main_window), FALSE, TRUE, TRUE );
  gtk_window_set_policy( GTK_WINDOW(main_window), TRUE, TRUE, FALSE );
   */
  gtk_window_set_policy( GTK_WINDOW(main_window), TRUE, TRUE, TRUE );

  main_vbox = gtk_vbox_new (FALSE,0);
  gtk_signal_connect (GTK_OBJECT(main_vbox),"key_press_event",
		      (GtkSignalFunc) sii_frame_keyboard_event, (gpointer)78);

  gtk_container_add (GTK_CONTAINER (main_window), main_vbox);
  gtk_container_border_width (GTK_CONTAINER (main_vbox), 0);
  gtk_widget_show (main_vbox);
  
  sii_get_main_menu (main_window, &menubar);
  gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, TRUE, 0);

  main_event_box = gtk_event_box_new ();
  gtk_container_add (GTK_CONTAINER (main_vbox), main_event_box);
  gtk_widget_set_events (main_event_box
			 , GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK
			 | GDK_CONFIGURE);
  gtk_signal_connect (GTK_OBJECT(main_event_box),"enter_notify_event",
		      (GtkSignalFunc) sii_main_event_box_cb
		      , (gpointer)SII_MAIN_EVENT_ENTER);
  gtk_signal_connect (GTK_OBJECT(main_event_box),"leave_notify_event",
		      (GtkSignalFunc) sii_main_event_box_cb
		      , (gpointer)SII_MAIN_EVENT_LEAVE);
  gtk_signal_connect (GTK_OBJECT(main_event_box),"configure_event",
		      (GtkSignalFunc) sii_main_event_box_cb, (gpointer)1);
  

  gs_initial_dir_name = g_string_new ("./");
  batch_mode = (getenv ("BATCH_MODE")) ? TRUE : FALSE;


  if (aa = getenv ("WINDOW_DUMP_DIR")) {
    if (strlen (aa))
      { gs_image_dir = g_string_new (aa); }
  }

  if ((aa = getenv ("swi")) || (aa = getenv ("SOLO_WINDOW_INFO"))) {
    if (strlen (aa)) {
      bb = strrchr (aa, '/');
      if (!bb)
	{ strcpy (swi_dir, "./"); }
      else
	{ for (cc = swi_dir; aa <= bb; *cc++ = *aa++); *cc = '\0'; }
      jj = solo_absorb_window_info (swi_dir, aa, 0);

      if (batch_mode && jj < 0 ) {
	g_message
	  ("Unable to start BATCH_MOD soloii from config file");
	exit (1);
      }
    }
    else
      { aa = NULL; }
  }

  if (!aa && (aa = sii_get_swpfi_dir (NULL))) {
    sii_default_startup (aa);
    config_cb (0, (gpointer)22 ); /* 2x2 */
    g_string_assign (gs_initial_dir_name, aa);
  }
  else if (!aa) {
     g_string_truncate (gen_gs, 0);
     g_string_append
       (gen_gs, "The environment variable DORADE_DIR has not been\n");
     g_string_append
       (gen_gs, "set to point to a directory containing sweepfiles or the\n");
     g_string_append
       (gen_gs, "local directory does not contain any sweepfiles.\n");
# ifdef notyet
    sii_message (gen_gs->str);
# endif
    show_solo_init (main_window,(gpointer)main_window);
  }

  gtk_widget_show_all (main_window);

# ifdef notyet
  create_cursors ();
# endif

  g_idle_add (sii_batch, 0);	/* routine to control batch mode */

  gtk_main ();

  return(0);  /* c...end */

}

/* c---------------------------------------------------------------------- */

void 
sii_param_colors_filesel (const gchar *str, GtkWidget *fs);

/* c---------------------------------------------------------------------- */

int sii_return_colors_filesel_wid() { return (SII_MAIN_COLORS_FILESEL); }

/* c---------------------------------------------------------------------- */

int sii_filesel_cb (GtkWidget *fs, gpointer data)
{
   const gchar *name, *aa;
   gchar str[256], str2[256], *bb;
   GtkWidget *entry;
   gint task, ii;
   const GString *cgs;

   task = GPOINTER_TO_UINT (gtk_object_get_data (GTK_OBJECT(fs), "task"));
   name = gtk_file_selection_get_filename ((GtkFileSelection *)fs);
   strcpy (str, name);
   strcpy (str2, name);
   gtk_widget_destroy (fs);

   switch (task) {
    case SII_MAIN_SWPFI_FILESEL:
      entry = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_config_window), "swpfi_dir");
      gtk_entry_set_text (GTK_ENTRY (entry), str);
      ii = sii_default_startup (str);
      config_cb (0, (gpointer)22 ); /* 2x2 */
      break;
      
   case SII_MAIN_CONFIG_FILESEL:
      aa = str + strlen (str) -1; /* last char. */
      bb = strrchr (str, '/');
      *bb = '\0';
      cgs = sii_return_config_files (str);
      gtk_editable_delete_text (GTK_EDITABLE (main_config_text), 0, -1);
      gtk_text_insert (GTK_TEXT (main_config_text)
		       , edit_font, edit_fore, edit_back
		       , cgs->str, -1);
      g_string_assign (gs_config_file_names, cgs->str);
      if (aa != bb) {		/* this is a full path name */
	 bb++;
	 ii = solo_absorb_window_info
	   (str, bb, (sii_frame_count)? ignore_swpfi_info : 0);
      }
      entry = (GtkWidget *)gtk_object_get_data
	(GTK_OBJECT(main_config_window), "config_dir");
      gtk_entry_set_text (GTK_ENTRY (entry), str);
      break;

   case SII_MAIN_COLORS_FILESEL:
      sii_param_colors_filesel (str, fs);
      break;
   }
}

/* c---------------------------------------------------------------------- */

GtkWidget *sii_filesel (gint which_but, const gchar * dirroot)
{
   GtkWidget *fs;
   gchar str[256], *aa="/", *bb;

   if (which_but == SII_MAIN_SWPFI_FILESEL) {
      bb = "swpfi_dir";
   }
   else if (which_but == SII_MAIN_CONFIG_FILESEL) {
      bb = "config_dir";
   }
   else if (which_but == SII_MAIN_COLORS_FILESEL) {
      bb = "colors_dir";
   }
   
   strcpy (str, bb);
   strcat (str, " file selection dialog");
   fs = gtk_file_selection_new (str);

   gtk_file_selection_hide_fileop_buttons ((GtkFileSelection *)fs);
   gtk_signal_connect_object (GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button),
			      "clicked",GTK_SIGNAL_FUNC(sii_filesel_cb),
			      GTK_OBJECT (fs));
   gtk_signal_connect_object (GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
			      "clicked",GTK_SIGNAL_FUNC(gtk_widget_destroy),
			      GTK_OBJECT (fs));
   g_string_truncate (gen_gs, 0);
   if (dirroot && strlen (dirroot)) {
      g_string_append (gen_gs, dirroot);
   }
   else
     { g_string_append (gen_gs, "/"); }
   gtk_file_selection_set_filename ((GtkFileSelection *)fs, gen_gs->str);
   gtk_object_set_data (GTK_OBJECT(fs)
			, "task", (gpointer)which_but);
   gtk_widget_show (fs);
   return (fs);
}

/* c---------------------------------------------------------------------- */

void sii_initialization_widget()
{
  GtkWidget *window;

  GtkWidget *vbox;
  GtkWidget *vbox0;
  GtkWidget *vbox1;
  GtkWidget *vbox2;

  GtkWidget *hbox;
  GtkWidget *hbox0;
  GtkWidget *hbox1;
  GtkWidget *hbox2;

  GtkWidget *entry;
  GtkWidget *entry_keep;
  GtkWidget *label;
  GtkWidget *button;
  GtkWidget *text;
  GtkWidget *table;
  GtkWidget *hscrollbar;
  GtkWidget *vscrollbar;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 0;
  guint ypadding = 0;

  gint event_flags = GDK_BUTTON_PRESS_MASK;
  gchar str[128], *aa;
  gint mark, row;
  const GString *cgs;
				/* c...code */
  edit_font = med_pro_font;
  edit_fore = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"black");
  edit_back = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"white");

  main_config_window =
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_window_set_title (GTK_WINDOW (window), "SoloII Initialization");

  vbox0 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(window), vbox0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_container_border_width (GTK_CONTAINER (hbox), 4);
  gtk_container_add (GTK_CONTAINER(vbox0), hbox);

  button = gtk_button_new_with_label (" Save Config ");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_SAVE_CONFIG
		      );
  button = gtk_button_new_with_label (" List Configs ");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_LIST_CONFIGS
		      );
  button = gtk_button_new_with_label (" Cancel ");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_CANCEL
		      );

  g_string_truncate (gen_gs, 0);
  g_string_append (gen_gs, 
"For default startup, type the sweepfile directory and hit <enter>\n");
  g_string_append (gen_gs, 
"or enter the directory with your confguration files and hit <enter>\n");
  g_string_append (gen_gs, 
"and then click on the desired file name.\n");
  label = gtk_label_new (gen_gs->str);

  gtk_label_set_justify ((GtkLabel *)label, GTK_JUSTIFY_LEFT );
  gtk_container_add (GTK_CONTAINER(vbox0), label);


  table = gtk_table_new (8, 5, FALSE);
  gtk_container_border_width (GTK_CONTAINER (table), 4);
  gtk_container_add (GTK_CONTAINER(vbox0), table);
  row = -1;
  ypadding = 3;

  row++;
  label = gtk_label_new ( " Swpfi Dir " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  gtk_entry_set_text (GTK_ENTRY (entry), gs_initial_dir_name->str);
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_initialize_cb
		      , (gpointer)SII_MAIN_DIR);
  gtk_object_set_data (GTK_OBJECT(window)
		       , "swpfi_dir", (gpointer)entry);
  row++;
  label = gtk_label_new ( " " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  button = gtk_button_new_with_label (" Swpfi File Select ");
  gtk_table_attach (GTK_TABLE (table), button, 3, 5, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_SWPFI_FILESEL
		      );

  row++;
  label = gtk_label_new ( " Config Dir " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  gtk_entry_set_text (GTK_ENTRY (entry), gs_initial_dir_name->str);
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_initialize_cb
		      , (gpointer)SII_MAIN_CONFIG_DIR);
  gtk_object_set_data (GTK_OBJECT(window)
		       , "config_dir", (gpointer)entry);

# ifdef obsolete
  row++;
  label = gtk_label_new ( "Configuration" );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  gtk_entry_set_editable (GTK_ENTRY (entry), FALSE ); 
  gtk_object_set_data (GTK_OBJECT(window)
		       , "config_file_name", (gpointer)entry);
# endif

  row++;
  label = gtk_label_new ( " Comment " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 6, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  gtk_entry_set_text (GTK_ENTRY (entry), "_no_comment");
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_initialize_cb
		      , (gpointer)SII_MAIN_CONFIG_COMMENT);
  gtk_object_set_data (GTK_OBJECT(window)
		       , "comment", (gpointer)entry);
  
  row++;
  button = gtk_check_button_new_with_label (" Ignore Swpfi Info ");
  gtk_table_attach (GTK_TABLE (table), button, 1, 3, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"toggled"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_IGNORE_SWPIFS
		      );
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), FALSE);
  ignore_swpfi_info = 0;
  gtk_object_set_data (GTK_OBJECT(window)
		       , "ignore", (gpointer)button);

  button = gtk_button_new_with_label (" Config File Select ");
  gtk_table_attach (GTK_TABLE (table), button, 3, 5, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_CONFIG_FILESEL
		      );



  

  table = gtk_table_new (2, 1, FALSE);
  gtk_container_border_width (GTK_CONTAINER (table), 4);
  gtk_widget_set_usize (table, 500, 0);
  gtk_container_add (GTK_CONTAINER (vbox0), table);

  main_config_text = 
    text = gtk_text_new (NULL,NULL);
  gtk_widget_set_events (text, event_flags );
  gtk_signal_connect_after (GTK_OBJECT(text)
		      ,"button_press_event"
		      , (GtkSignalFunc) sii_main_text_event_cb
		      , (gpointer)gs_config_file_names);
  
  gtk_object_set_data (GTK_OBJECT(text),
		       "file_names_gs",
		       (gpointer)gs_config_file_names);

  gtk_table_attach (GTK_TABLE (table), text, 0, 1, 0, 1,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		    GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);

  vscrollbar = gtk_vscrollbar_new (GTK_TEXT (text)->vadj);
  gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1,
		    GTK_FILL, GTK_EXPAND | GTK_SHRINK | GTK_FILL, 0, 0);
  gtk_text_set_editable (GTK_TEXT (text), FALSE);

  cgs = sii_return_config_files (gs_initial_dir_name->str);
  g_string_assign (gs_config_file_names, cgs->str);
  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , gs_config_file_names->str, -1);

  /* --- Make everything visible --- */
  
  gtk_widget_show_all (window);
  mark = 0;
}

/* c---------------------------------------------------------------------- */

void sii_image_gen_widget()
{
  GtkWidget *window;

  GtkWidget *vbox;
  GtkWidget *vbox0;
  GtkWidget *vbox1;
  GtkWidget *vbox2;

  GtkWidget *hbox;
  GtkWidget *hbox0;
  GtkWidget *hbox1;
  GtkWidget *hbox2;

  GtkWidget *entry;
  GtkWidget *entry_keep;
  GtkWidget *label;
  GtkWidget *button;
  GtkWidget *text;
  GtkWidget *table;
  GtkWidget *hscrollbar;
  GtkWidget *vscrollbar;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 0;
  guint ypadding = 0;

  gint event_flags = GDK_BUTTON_PRESS_MASK;
  gchar str[128], *aa;
  gint mark, row;
  const GString *cgs;
				/* c...code */

  main_image_gen_window =
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_window_set_title (GTK_WINDOW (window), "SoloII PNG Images");

  vbox0 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(window), vbox0);

  hbox = gtk_hbox_new (FALSE, 2);
  gtk_container_border_width (GTK_CONTAINER (hbox), 4);
  gtk_container_add (GTK_CONTAINER(vbox0), hbox);

  button = gtk_button_new_with_label (" Make PNG Image ");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_PNG
		      );
  button = gtk_button_new_with_label (" Cancel ");
  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0 );
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc)sii_initialize_cb
		      , (gpointer)SII_MAIN_CANCEL_IMAGE_GEN
		      );

  table = gtk_table_new (1, 5, FALSE);
  gtk_container_border_width (GTK_CONTAINER (table), 4);
  gtk_container_add (GTK_CONTAINER(vbox0), table);
  row = -1;
  ypadding = 3;

  row++;
  label = gtk_label_new ( " Image Dir " );
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  entry = gtk_entry_new ();
  gtk_table_attach (GTK_TABLE (table), entry, 1, 5, row, row+1,
		    xoptions, yoptions, xpadding, ypadding );

  aa = (gs_image_dir) ? gs_image_dir->str : gs_initial_dir_name->str;
  gs_image_dir = g_string_new (aa);
  gtk_entry_set_text (GTK_ENTRY (entry), gs_image_dir->str);
  gtk_signal_connect (GTK_OBJECT(entry)
		      ,"activate"
		      , (GtkSignalFunc) sii_initialize_cb
		      , (gpointer)SII_MAIN_IMAGE_DIR);

  gtk_object_set_data (GTK_OBJECT(window)
		       , "image_dir", (gpointer)entry);

  /* --- Make everything visible --- */
  
  gtk_widget_show_all (window);
  mark = 0;
}

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

/* This is the GtkItemFactoryEntry structure used to generate new menus.
   Item 1: The menu path. The letter after the underscore indicates an
           accelerator key once the menu is open.
   Item 2: The accelerator key for the entry
   Item 3: The callback function.
   Item 4: The callback action.  This changes the parameters with
           which the function is called.  The default is 0.
   Item 5: The item type, used to define what kind of an item it is.
           Here are the possible values:

           NULL               -> "<Item>"
           ""                 -> "<Item>"
           "<Title>"          -> create a title item
           "<Item>"           -> create a simple item
           "<CheckItem>"      -> create a check item
           "<ToggleItem>"     -> create a toggle item
           "<RadioItem>"      -> create a radio item
           <path>             -> path of a radio item to link against
           "<Separator>"      -> create a separator
           "<Branch>"         -> create an item to hold sub items (optional)
           "<LastBranch>"     -> create a right justified branch 
*/

static GtkItemFactoryEntry menu_items[] = {
  { "/File",                   NULL,              NULL,	    0,  "<Branch>" },
  { "/File/Exit",              NULL,     gtk_main_quit,	    0,  NULL },
  { "/File/sep2",              NULL,              NULL,	    0,  "<Separator>" },
# ifdef obsolete
  { "/File/Test",              NULL,  sii_filesel_test,     0,  NULL },
# endif
  { "/File/Set Source Dir",    NULL,    show_solo_init,	    0,  NULL },
  { "/File/Config Files",      NULL,    show_solo_init,	    0,  NULL },
  { "/File/Set Image Dir",     NULL,show_solo_image_gen,    0,  NULL },
  { "/File/sep2",              NULL,              NULL,	    0,  "<Separator>" },
  { "/File/Hide Data Widgets", NULL,     click_data_cb,	    0,  NULL },
  { "/File/Show Data Widgets", NULL,     click_data_cb,	    1,  NULL },
# ifdef notyet
  { "/File/sep2",              NULL,              NULL,	    0,  "<Separator>" },
  { "/File/Make PNG Image",    NULL, sii_menu_cb, SII_MAIN_PNG,  NULL },
  { "/File/sep2",              NULL,              NULL,	    0,  "<Separator>" },
  { "/File/NEXRAD Mode",       NULL,       print_hello,	    0,  "<CheckItem>" },
  { "/File/Disable Rescan",    NULL,       print_hello,	    0,  "<CheckItem>" },
  { "/File/Always Clear Bnd",  NULL,       print_hello,	    0,  "<CheckItem>" },
  { "/File/Auto Angular Fill", NULL,       print_hello,	    0,  "<CheckItem>" },
  { "/File/sep4",              NULL,              NULL,	    0,  "<Separator>" },
  { "/File/Set Image Dir",     NULL,          image_cb,	    0,  NULL },
  { "/File/Make a png",        NULL,          image_cb,	    0,  NULL },
  { "/File/Make a gif",        NULL,          image_cb,	    0,  NULL },
  { "/File/sep3",              NULL,              NULL,	    0,  "<Separator>" },
  { "/File/Print",             NULL,          image_cb,	    0,  NULL },
  { "/File/sep3a",             NULL,              NULL,	    0,  "<Separator>" },
  { "/File/Import",            NULL,       print_hello,	    0,  NULL },
  { "/File/Export",            NULL,       print_hello,	    0,  NULL },
  { "/File/sep3b",             NULL,              NULL,	    0,  "<Separator>" },
  { "/File/Preferences",       NULL,       print_hello,	    0,  NULL },
  { "/File/sep3b",             NULL,              NULL,	    0,  "<Separator>" },
# endif
  { "/File/Exit",              NULL,     gtk_main_quit,	    0,  NULL },
  { "/Zoom",                   NULL,              NULL,	    0,  "<Branch>" },
  { "/Zoom/Data   ",           NULL,              NULL,	    0,  "<Title>" },
  { "/Zoom/Default",           NULL,      zoom_data_cb,     0,  NULL },
  { "/Zoom/ +200%",            NULL,      zoom_data_cb,	  200,  NULL },
  { "/Zoom/ +100%",            NULL,      zoom_data_cb,	  100,  NULL },
  { "/Zoom/  +50%",            NULL,      zoom_data_cb,	   50,  NULL },
  { "/Zoom/  +25%",            NULL,      zoom_data_cb,	   25,  NULL },
  { "/Zoom/  +10%",            NULL,      zoom_data_cb,	   10,  NULL },
  { "/Zoom/sep2",              NULL,              NULL,	    0,  "<Separator>" },
  { "/Zoom/  -10%",            NULL,      zoom_data_cb,	10010,  NULL },
  { "/Zoom/  -25%",            NULL,      zoom_data_cb,	10025,  NULL },
  { "/Zoom/  -50%",            NULL,      zoom_data_cb,	10050,  NULL },
  { "/Zoom/  -80%",            NULL,      zoom_data_cb,	10080,  NULL },
  { "/Zoom/sep3",              NULL,              NULL,	    0,  "<Separator>" },
  { "/Zoom/Config",            NULL,              NULL,	    0,  "<Title>" },
# ifdef notyet
  { "/Zoom/ +100%",            NULL,    zoom_config_cb,	  100,  NULL },
# endif
  { "/Zoom/  +50%",            NULL,    zoom_config_cb,	   50,  NULL },
  { "/Zoom/  +25%",            NULL,    zoom_config_cb,	   25,  NULL },
  { "/Zoom/  +17%",            NULL,    zoom_config_cb,	   10,  NULL },
  { "/Zoom/  +06%",            NULL,    zoom_config_cb,	    6,  NULL },
  { "/Zoom/sep3",              NULL,              NULL,	    0,  "<Separator>" },
  { "/Zoom/  -05%",            NULL,    zoom_config_cb,	10005,  NULL },
  { "/Zoom/  -11%",            NULL,    zoom_config_cb,	10011,  NULL },
# ifdef notyet
  { "/Zoom/  -20%",            NULL,    zoom_config_cb,	10020,  NULL },
  { "/Zoom/  -33%",            NULL,    zoom_config_cb,	10033,  NULL },
  { "/Zoom/  -50%",            NULL,    zoom_config_cb,	10050,  NULL },
# endif
  { "/Center",                 NULL,              NULL,	    0,  "<Branch>" },
  { "/Center/Center on",       NULL,              NULL,	    0,  "<Title>" },
  { "/Center/Last click",      NULL,   center_frame_cb,	    1,  NULL },
  { "/Center/Last 2 clicks",   NULL,   center_frame_cb,	    2,  NULL },
  { "/Center/Last 4 clicks",   NULL,   center_frame_cb,	    4,  NULL },
  { "/Center/sep3",            NULL,              NULL,	    0,  "<Separator>" },
  { "/Center/Radar",           NULL,   center_frame_cb,	    0,  NULL },
  { "/Center/sep3",            NULL,              NULL,	    0,  "<Separator>" },
  { "/Center/Electric",        NULL,       sii_menu_cb,SII_MAIN_CTR_TOGGLE,  "<CheckItem>" },
  { "/Center/Ctr CrossHairs",  NULL,       sii_menu_cb,SII_MAIN_XHAIRS_TOGGLE,  "<CheckItem>" },
  { "/Config",                 NULL,              NULL,	    0,  "<Branch>" },
  { "/Config/Default Config",  NULL,  custom_config_cb,	 2022,  NULL },
  { "/Config/1x1",             NULL,         config_cb,	   11,  NULL },
  { "/Config/1x2",             NULL,         config_cb,	   12,  NULL },
  { "/Config/1x3",             NULL,         config_cb,	   13,  NULL },
  { "/Config/1x4",             NULL,         config_cb,	   14,  NULL },
  { "/Config/sep1",            NULL,              NULL,	    0,  "<Separator>" },
  { "/Config/2x1",             NULL,         config_cb,	   21,  NULL },
  { "/Config/2x2",             NULL,         config_cb,	   22,  NULL },
  { "/Config/2x3",             NULL,         config_cb,	   23,  NULL },
  { "/Config/2x4",             NULL,         config_cb,	   24,  NULL },
  { "/Config/sep1",            NULL,              NULL,	    0,  "<Separator>" },
  { "/Config/3x1",             NULL,         config_cb,	   31,  NULL },
  { "/Config/3x2",             NULL,         config_cb,	   32,  NULL },
  { "/Config/3x3",             NULL,         config_cb,	   33,  NULL },
  { "/Config/3x4",             NULL,         config_cb,	   34,  NULL },
  { "/Config/sep1",            NULL,              NULL,	    0,  "<Separator>" },
  { "/Config/4x1",             NULL,         config_cb,	   41,  NULL },
  { "/Config/4x2",             NULL,         config_cb,	   42,  NULL },
  { "/Config/4x3",             NULL,         config_cb,	   43,  NULL },
  { "/Config/sep2",            NULL,              NULL,	    0,  "<Separator>" },
  { "/Config/Square Shaped",   NULL,          shape_cb,	   11,  NULL },
  { "/Config/Slide Shaped",    NULL,          shape_cb,	   43,  NULL },
  { "/Config/VSlide Shaped",   NULL,          shape_cb,	   34,  NULL },
  { "/Config/sep3",            NULL,              NULL,	    0,  "<Separator>" },
# ifdef notyet
  { "/Config/2x2 512x512",     NULL,  custom_config_cb,	 4022,  NULL },
# endif
  { "/Config/1 Big 2v Small",  NULL,  custom_config_cb,	 1012,  NULL },
  { "/Config/1 Big 2h Small",  NULL,  custom_config_cb,	 2012,  NULL },
  { "/Config/1 Big 3 Small",   NULL,  custom_config_cb,	 1013,  NULL },
  { "/Config/1 Big 5 Small",   NULL,  custom_config_cb,	 1015,  NULL },
  { "/Config/1 Big 7 Small",   NULL,  custom_config_cb,	 1017,  NULL },
  { "/Config/2 Big 4v Small",  NULL,  custom_config_cb,	 1024,  NULL },
  { "/Config/2 Big 4h Small",  NULL,  custom_config_cb,	 2024,  NULL },
  { "/Help",                   NULL,              NULL,	    0,  "<Branch>" },
# ifdef notyet
  { "/Help/Overview",          NULL,              NULL,	    0,  NULL },
  { "/Help/Getting Started",   NULL,              NULL,	    0,  NULL },
# endif
  { "/Help/Basics",            NULL, sii_menu_cb, SII_MAIN_HLP_BASIC,  NULL },
  { "/Help/Files",              NULL, sii_menu_cb, SII_MAIN_HLP_FILE,  NULL },
  { "/Help/Zooming",           NULL, sii_menu_cb, SII_MAIN_HLP_ZOOM,  NULL },
  { "/Help/Centering",         NULL, sii_menu_cb, SII_MAIN_HLP_CTR,  NULL },
  { "/Help/Configuring",       NULL, sii_menu_cb, SII_MAIN_HLP_CONFIG,  NULL },
  { "/Help/Shortcuts",         NULL, sii_menu_cb, SII_MAIN_HLP_SHORTCUTS,  NULL },
  { "/Help/Comparisons",       NULL, sii_menu_cb, SII_MAIN_HLP_COMPARE,  NULL },
  { "/Help/About",             NULL, sii_menu_cb, SII_MAIN_HLP_ABOUT,  NULL },
};

/* c---------------------------------------------------------------------- */

void sii_get_main_menu( GtkWidget  *window,
                    GtkWidget **menubar )
{
  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;
  GtkWidget *check_item;
  gint nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);
  
  
  accel_group = gtk_accel_group_new ();

  /* This function initializes the item factory.
     Param 1: The type of menu - can be GTK_TYPE_MENU_BAR, GTK_TYPE_MENU,
              or GTK_TYPE_OPTION_MENU.
     Param 2: The path of the menu.
     Param 3: A pointer to a gtk_accel_group.  The item factory sets up
              the accelerator table while generating menus.
  */

  item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", 
				       accel_group);

  /* This function generates the menu items. Pass the item factory,
     the number of items in the array, the array itself, and any
     callback data for the the menu items. */
  gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);

  /* Attach the new accelerator group to the window. */
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

  if (menubar)
    /* Finally, return the actual menu bar created by the item factory. */ 
    *menubar = gtk_item_factory_get_widget (item_factory, "<main>");


  check_item = gtk_item_factory_get_item
    (item_factory, "/Center/Electric");
  gtk_check_menu_item_set_active ((GtkCheckMenuItem*)check_item, TRUE );
# ifdef obsolete
  check_item = gtk_item_factory_get_item
    (item_factory, "/Center/Ctr CrossHairs");
  gtk_check_menu_item_set_active ((GtkCheckMenuItem*)check_item, TRUE );
# endif
}

/* c------------------------------------------------------------------------ */
void sp_change_cursor (int normal) {
  GdkCursor *cursor;
  guint jj, kk;
   /*
     ChangeCursor(widget,NormalCursor);
     ChangeCursor(widget,BusyCursor);
     const guint mb_cursor = 132;
    */
  jj = (normal != 1) ? normal : 86; /* pencil */
  kk = 150;			/* watch */
  cursor = gdk_cursor_new ((normal) ? jj : kk);
  gdk_window_set_cursor (main_event_box->window, cursor);
  gdk_cursor_destroy (cursor);
				/*
  gdk_flush();
				 */
}
/* c---------------------------------------------------------------------- */

/*
 * Cursors
 */

static gint
cursor_expose_event (GtkWidget *widget,
                     GdkEvent  *event,
                     gpointer   user_data)
{
  GtkDrawingArea *darea;
  GdkDrawable *drawable;
  GdkGC *black_gc;
  GdkGC *gray_gc;
  GdkGC *white_gc;
  guint max_width;
  guint max_height;

  g_return_val_if_fail (widget != NULL, TRUE);
  g_return_val_if_fail (GTK_IS_DRAWING_AREA (widget), TRUE);

  darea = GTK_DRAWING_AREA (widget);
  drawable = widget->window;
  white_gc = widget->style->white_gc;
  gray_gc = widget->style->bg_gc[GTK_STATE_NORMAL];
  black_gc = widget->style->black_gc;
  max_width = widget->allocation.width;
  max_height = widget->allocation.height;

  gdk_draw_rectangle (drawable, white_gc,
                      TRUE,
                      0,
                      0,
                      max_width,
                      max_height / 2);

  gdk_draw_rectangle (drawable, black_gc,
                      TRUE,
                      0,
                      max_height / 2,
                      max_width,
                      max_height / 2);

  gdk_draw_rectangle (drawable, gray_gc,
                      TRUE,
                      max_width / 3,
                      max_height / 3,
                      max_width / 3,
                      max_height / 3);

  return TRUE;
}

static void
set_cursor (GtkWidget *spinner,
            GtkWidget *widget)
{
  guint c;
  GdkCursor *cursor;
  GtkWidget *label;
  GtkFlagValue *vals;

  c = CLAMP (gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (spinner)), 0, 152);
  c &= 0xfe;

  label = gtk_object_get_user_data (GTK_OBJECT (spinner));
  vals = gtk_type_enum_get_values (GTK_TYPE_GDK_CURSOR_TYPE);
  while (vals && vals->value != c)
    vals++;
  if (vals)
    gtk_label_set (GTK_LABEL (label), vals->value_nick);
  else
    gtk_label_set (GTK_LABEL (label), "<unknown>");

  cursor = gdk_cursor_new (c);
  gdk_window_set_cursor (widget->window, cursor);
  gdk_cursor_destroy (cursor);
}

static gint
cursor_event (GtkWidget          *widget,
              GdkEvent           *event,
              GtkSpinButton      *spinner)
{
  if ((event->type == GDK_BUTTON_PRESS) &&
      ((event->button.button == 1) ||
       (event->button.button == 3)))
    {
      gtk_spin_button_spin (spinner, event->button.button == 1 ?
                            GTK_SPIN_STEP_FORWARD : GTK_SPIN_STEP_BACKWARD, 0);
      return TRUE;
    }

  return FALSE;
}


static void
create_cursors (void)
{
  static GtkWidget *window = NULL;
  GtkWidget *frame;
  GtkWidget *hbox;
  GtkWidget *main_vbox;
  GtkWidget *vbox;
  GtkWidget *darea;
  GtkWidget *spinner;
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *any;
  GtkAdjustment *adj;

  if (!window)
    {
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      
      gtk_signal_connect (GTK_OBJECT (window), "destroy",
                          GTK_SIGNAL_FUNC (gtk_widget_destroyed),
                          &window);
      
      gtk_window_set_title (GTK_WINDOW (window), "Cursors");
      
      main_vbox = gtk_vbox_new (FALSE, 5);
      gtk_container_border_width (GTK_CONTAINER (main_vbox), 0);
      gtk_container_add (GTK_CONTAINER (window), main_vbox);

      vbox =
        gtk_widget_new (gtk_vbox_get_type (),
                        "GtkBox::homogeneous", FALSE,
                        "GtkBox::spacing", 5,
                        "GtkContainer::border_width", 10,
                        "GtkWidget::parent", main_vbox,
                        "GtkWidget::visible", TRUE,
                        NULL);

      hbox = gtk_hbox_new (FALSE, 0);
      gtk_container_border_width (GTK_CONTAINER (hbox), 5);
      gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
      
      label = gtk_label_new ("Cursor Value : ");
      gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
      gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, TRUE, 0);
      
      adj = (GtkAdjustment *) gtk_adjustment_new (0,
                                                  0, 152,
                                                  2,
                                                  10, 0);
      spinner = gtk_spin_button_new (adj, 0, 0);
      gtk_box_pack_start (GTK_BOX (hbox), spinner, TRUE, TRUE, 0);

      frame =
        gtk_widget_new (gtk_frame_get_type (),
                        "GtkFrame::shadow", GTK_SHADOW_ETCHED_IN,
                        "GtkFrame::label_xalign", 0.5,
                        "GtkFrame::label", "Cursor Area",
                        "GtkContainer::border_width", 10,
                        "GtkWidget::parent", vbox,
                        "GtkWidget::visible", TRUE,
                        NULL);

      darea = gtk_drawing_area_new ();
      gtk_widget_set_usize (darea, 80, 80);
      gtk_container_add (GTK_CONTAINER (frame), darea);
      gtk_signal_connect (GTK_OBJECT (darea),
                          "expose_event",
                          GTK_SIGNAL_FUNC (cursor_expose_event),
                          NULL);
      gtk_widget_set_events (darea, GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK);
      gtk_signal_connect (GTK_OBJECT (darea),
                          "button_press_event",
                          GTK_SIGNAL_FUNC (cursor_event),
                          spinner);
      gtk_widget_show (darea);

      gtk_signal_connect (GTK_OBJECT (spinner), "changed",
                          GTK_SIGNAL_FUNC (set_cursor),
                          darea);

      label = gtk_widget_new (GTK_TYPE_LABEL,
                              "visible", TRUE,
                              "label", "XXX",
                              "parent", vbox,
                              NULL);
      gtk_container_child_set (GTK_CONTAINER (vbox), label,
                               "expand", FALSE,
                               NULL);
      gtk_object_set_user_data (GTK_OBJECT (spinner), label);

      any =
        gtk_widget_new (gtk_hseparator_get_type (),
                        "GtkWidget::visible", TRUE,
                        NULL);
      gtk_box_pack_start (GTK_BOX (main_vbox), any, FALSE, TRUE, 0);
  
      hbox = gtk_hbox_new (FALSE, 0);
      gtk_container_border_width (GTK_CONTAINER (hbox), 10);
      gtk_box_pack_start (GTK_BOX (main_vbox), hbox, FALSE, TRUE, 0);

      button = gtk_button_new_with_label ("Close");
      gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                                 GTK_SIGNAL_FUNC (gtk_widget_destroy),
                                 GTK_OBJECT (window));
      gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 5);

      gtk_widget_show_all (window);

      set_cursor (spinner, darea);
    }
  else
    gtk_widget_destroy (window);
}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c...end */

