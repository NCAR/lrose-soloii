/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "sii_utils.h"
# include <solo_window_structs.h>

gchar *sample_click_text = "caca";

enum {
   FRAME_MENU_ZERO,
   FRAME_MENU_CANCEL,
   FRAME_MENU_REPLOT,
   FRAME_MENU_SWEEPFILES,
   FRAME_MENU_SWPFI_LIST,
   FRAME_MENU_PARAMETERS,
   FRAME_MENU_VIEW,
   FRAME_MENU_CENTERING,
   FRAME_MENU_LOCKSTEPS,
   FRAME_MENU_EDITOR,
   FRAME_MENU_EXAMINE,
   FRAME_MENU_CLICK_DATA,
   FRAME_MENU_LAST_ENUM,
};

/* c---------------------------------------------------------------------- */

void frame_menu_widget ( guint frame_num );

void frame_click_data_widget ( guint frame_num );

void show_click_data_widget (guint frame_num);

void show_edit_widget (GtkWidget *text, gpointer data );

void show_exam_widget (GtkWidget *text, gpointer data );

void show_frame_menu (GtkWidget *text, gpointer data );

void show_param_widget (GtkWidget *text, gpointer data );

void show_swpfi_widget (GtkWidget *text, gpointer data );

void show_view_widget (GtkWidget *text, gpointer data );

/* c---------------------------------------------------------------------- */

static GdkFont *edit_font;
static GdkColor *edit_fore;
static GdkColor *edit_back;

static gchar *click_tmplt = "W0000.00km.-000.00 -000.00 -000.00W";

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

void sii_set_click_data_text (guint frame_num, GString *gs)
{
  GtkWidget *widget, *text;
  widget = sii_get_widget_ptr ( frame_num, FRAME_CLICK_DATA );

  g_string_assign (frame_configs[frame_num]->data_widget_text, gs->str);

  if (!widget)
    { return; }

  text = (GtkWidget *)gtk_object_get_data (GTK_OBJECT(widget), "text_widget");

  if (text) {
    gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
    gtk_text_insert (GTK_TEXT (text)
		     , edit_font, edit_fore, edit_back
		     , gs->str, gs->len);
  }
}

/* c---------------------------------------------------------------------- */

void frame_menu_cb (GtkWidget *frame, gpointer data )
{
  GtkWidget *widget; 
  guint num = GPOINTER_TO_UINT (data);
  guint frame_num, task, window_id;
  gint x, y;
  GdkPoint *ptp;

  frame_num = num/TASK_MODULO;
  window_id = task = num % TASK_MODULO;

  switch (task) {

  case FRAME_MENU_REPLOT:
    if (sii_set_param_info (frame_num))
      { sii_plot_data (frame_num, REPLOT_THIS_FRAME); }
    break;

  case FRAME_MENU_CANCEL:
    widget = sii_get_widget_ptr ( frame_num, FRAME_MENU );
    if( widget )
    { gtk_widget_hide (widget); }
    break;

  case FRAME_MENU_SWEEPFILES:
    widget = sii_get_widget_ptr ( frame_num, FRAME_MENU );
    gtk_widget_hide (widget);
    show_swpfi_widget (frame, (gpointer)frame_num );
    break;

  case FRAME_MENU_PARAMETERS:
    widget = sii_get_widget_ptr ( frame_num, FRAME_MENU );
    gtk_widget_hide (widget);
    show_param_widget (frame, (gpointer)frame_num );
    break;

  case FRAME_MENU_VIEW:
    widget = sii_get_widget_ptr ( frame_num, FRAME_MENU );
    gtk_widget_hide (widget);
    show_view_widget (frame, (gpointer)frame_num );
    break;

  case FRAME_MENU_LOCKSTEPS:
    break;

  case FRAME_MENU_EDITOR:
    widget = sii_get_widget_ptr ( frame_num, FRAME_MENU );
    gtk_widget_hide (widget);
    show_edit_widget (frame, (gpointer)frame_num );
    break;

  case FRAME_MENU_EXAMINE:
    widget = sii_get_widget_ptr ( frame_num, FRAME_MENU );
    gtk_widget_hide (widget);
    show_exam_widget (frame, (gpointer)frame_num );
    break;

  case FRAME_MENU_CLICK_DATA:
    /* Show the clicked data widget for this frame
     */
    frame_configs[frame_num]->show_data_widget =
      !frame_configs[frame_num]->show_data_widget;

    widget = sii_get_widget_ptr (frame_num, FRAME_CLICK_DATA);

    if (frame_configs[frame_num]->show_data_widget) {
      show_click_data_widget (frame_num);
    }
    else {
      if (widget) {
	gdk_window_get_origin ( widget->window, &x, &y );
	ptp = &frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA];
	ptp->x = x; ptp->y = y;
	gtk_widget_hide (widget);
      }
    }
    widget = sii_get_widget_ptr (frame_num, FRAME_MENU);
    gtk_widget_hide (widget);
    break;
  };
}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

void show_frame_menu (GtkWidget *text, gpointer data )
{
  guint frame_num = GPOINTER_TO_UINT (data);
  GtkWidget *widget;
  GdkPoint *ptp;
  gint x, y;

  widget = sii_get_widget_ptr (frame_num, FRAME_MENU);

  if( !widget )
    { frame_menu_widget (frame_num); }
  else {
     ptp = &frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA];
     x = ptp->x; y = ptp->y;
     /* Doesn't work on Solaris! */
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */
void show_all_click_data_widgets (gboolean really)
{
  GtkWidget *widget, *frame;
  guint frame_num;
  gint x, y;
  GdkPoint *ptp;

  for (frame_num=0; frame_num < sii_frame_count; frame_num++) {

    if (really) {
      if (frame_configs[frame_num]->show_data_widget) {
	continue;		/* already shown */
      }
      else {
	show_click_data_widget (frame_num);
      }
    }
    else {			/* hide it! */
      widget = sii_get_widget_ptr (frame_num, FRAME_CLICK_DATA);

      if (widget) {		
				/* save origin */
	gdk_window_get_origin ( widget->window, &x, &y );
	ptp = &frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA];
	ptp->x = x; ptp->y = y;
	gtk_widget_hide (widget);
      }
      frame_configs[frame_num]->show_data_widget = FALSE;
    }
  }
}

/* c---------------------------------------------------------------------- */

void show_click_data_widget (guint frame_num)
{
  GtkWidget *widget;
  GdkPoint *ptp;
  gint x, y;

  widget = sii_get_widget_ptr ( frame_num, FRAME_CLICK_DATA );
  frame_configs[frame_num]->show_data_widget = TRUE;

  if( !widget )
    { frame_click_data_widget (frame_num); }
  else {
     ptp = &frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA];
     x = ptp->x; y = ptp->y;
     /* Doesn't work on Solaris! */
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */

void frame_click_data_widget ( guint frame_num )
{
  GtkWidget *widget;
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *button;
  GtkWidget *table;
  GtkWidget *text;
  GtkWidget *vscrollbar;

  guint nn, height, nlines = 11;
  gchar *aa, *bb, str[128];
  GdkPoint *ptp;
  gint x, y, lbearing, rbearing, width, ascent, descent;


  edit_font = med_fxd_font;
  edit_fore = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"black");
  edit_back = (GdkColor *)g_hash_table_lookup ( colors_hash, (gpointer)"white");

  gdk_text_extents (edit_font, click_tmplt, strlen (click_tmplt)
		    , &lbearing, &rbearing, &width, &ascent, &descent);

  width = gdk_string_width( edit_font, click_tmplt );
  height = edit_font->ascent + edit_font->descent+1;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  sii_set_widget_ptr ( frame_num, FRAME_CLICK_DATA, window );

  widget = sii_get_widget_ptr (frame_num, FRAME_MENU);
  if (widget) {
    ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
    frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA].x = ptp->x;
    frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA].y = ptp->y;
  }
  else {
    widget = frame_configs[frame_num]->frame;
    gdk_window_get_origin ( widget->window, &x, &y );
    ptp = &frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA];
    ptp->x = x+64; ptp->y = y+64;
  }
  gtk_widget_set_uposition (window, ptp->x, ptp->y);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num * TASK_MODULO + FRAME_MENU));

  /* --- Title and border --- */
  sprintf (str, "Frame %d  Data", frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), str);
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  text = gtk_text_new (NULL,NULL);
  gtk_widget_set_usize (text, width, nlines * height);
  gtk_container_add (GTK_CONTAINER (vbox), text);

  aa = frame_configs[frame_num]->data_widget_text->str;
  if (!frame_configs[frame_num]->data_widget_text->len)
    { aa = sample_click_text; }

  gtk_object_set_data (GTK_OBJECT(window),
		       "text_widget",
		       (gpointer)text);

  gtk_text_insert (GTK_TEXT (text)
		   , edit_font, edit_fore, edit_back
		   , aa, -1);

  gtk_text_set_editable (GTK_TEXT (text), FALSE);

  /* --- Make everything visible --- */
  gtk_widget_show_all (window);
}

/* c---------------------------------------------------------------------- */

void frame_menu_widget ( guint frame_num )
{
  GtkWidget *label;
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *button;

  guint nn;
  gchar *aa, *bb;
  GdkPoint *ptp;
  gint x, y;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  sii_set_widget_ptr ( frame_num, FRAME_MENU, window );
  ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
  x = ptp->x; y = ptp->y;
  gtk_widget_set_uposition (window, ptp->x, ptp->y);

  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(sii_nullify_widget_cb),
		      (gpointer)(frame_num * TASK_MODULO + FRAME_MENU));

  /* --- Title and border --- */
  bb = g_strdup_printf ("Frame %d", frame_num+1 );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  button = gtk_button_new_with_label ("Cancel");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );

  nn = frame_num*TASK_MODULO + FRAME_MENU_CANCEL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );
# ifdef notyet
  button = gtk_button_new_with_label ("Replot");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + FRAME_MENU_REPLOT;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );
# endif
  button = gtk_button_new_with_label ("Sweepfiles");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );

  nn = frame_num*TASK_MODULO + FRAME_MENU_SWEEPFILES;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label (" Parameters + Colors ");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );

  nn = frame_num*TASK_MODULO + FRAME_MENU_PARAMETERS;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("View");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );

  nn = frame_num*TASK_MODULO + FRAME_MENU_VIEW;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );
# ifdef obsolete
  button = gtk_button_new_with_label ("Centering");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );
  button = gtk_button_new_with_label ("Lock Steps");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );
# endif

  button = gtk_button_new_with_label ("Editor");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );

  nn = frame_num*TASK_MODULO + FRAME_MENU_EDITOR;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("Examine");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );

  nn = frame_num*TASK_MODULO + FRAME_MENU_EXAMINE;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );

  button = gtk_button_new_with_label ("Data Widget");
  gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0 );

  nn = frame_num*TASK_MODULO + FRAME_MENU_CLICK_DATA;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) frame_menu_cb
		      , (gpointer)nn
		      );

  /* --- Make everything visible --- */
  gtk_widget_show_all (window);

}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

