/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "sii_utils.h"

enum {
   LINKS_ZERO,
   LINKS_OK,
   LINKS_CANCEL,
   LINKS_SET_ALL,
   LINKS_CLEAR_ALL,
   LINKS_LAST_ENUM,
};

void sii_links_widget( guint frame_num, guint widget_id, LinksInfo *linkfo );

/* c---------------------------------------------------------------------- */

void sii_set_links_from_solo_struct (int frame_num, int links_id,
				     long *linked_windows)
{
   LinksInfo *li = frame_configs[frame_num]->link_set[links_id];
   guint jj;

   for (jj=0; jj < maxFrames; jj++)
     { li->link_set[jj] = (linked_windows[jj]) ? TRUE : FALSE; }
}

/* c---------------------------------------------------------------------- */

void sii_links_set_foreach (GtkWidget *button, gpointer data )
{
   gint active = GPOINTER_TO_UINT (data);
   gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), active);
}

/* c---------------------------------------------------------------------- */

void  sii_links_set_from_struct (GtkWidget *button, gpointer data )
{
   LinksInfo *li = (LinksInfo *)data;
   gint button_num = GPOINTER_TO_UINT
     (gtk_object_get_data (GTK_OBJECT(button), "button_num"));

   gint active = li->link_set[button_num];
   gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button), active);
}

/* c---------------------------------------------------------------------- */

void  sii_links_get_foreach (GtkWidget *button, gpointer data )
{
   LinksInfo *li = (LinksInfo *)data;
   gint active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button));
   gint button_num = GPOINTER_TO_UINT
     (gtk_object_get_data (GTK_OBJECT(button), "button_num"));
   li->link_set[button_num] = active;
}

/* c---------------------------------------------------------------------- */

LinksInfo *sii_new_links_info ( gchar *name, guint frame_num, guint widget_id
			       , gboolean this_frame_only )
{
   LinksInfo *li = g_malloc0 (sizeof( LinksInfo ));
   int fn;

   li->name = g_strdup (name);
   li->frame_num = frame_num;
   li->widget_id = widget_id;
   li->num_links = (this_frame_only) ? 1 : maxFrames;

   if (this_frame_only) {
      li->link_set[frame_num] = TRUE;
   }
   else {
      for (fn=0; fn < maxFrames; fn++ ) {
	 li->link_set[fn] = TRUE;
      }
   }
   return li;
}

/* c---------------------------------------------------------------------- */

void show_links_widget (LinksInfo *linkfo)
{
  GtkWidget *widget;
  GtkWidget *table;
		
  guint frame_num = linkfo->frame_num;
  guint widget_id = linkfo->widget_id;
			
  GdkPoint *ptp;
  gint x, y;

  widget = sii_get_widget_ptr (frame_num, widget_id);

  if( !widget )
    { sii_links_widget( frame_num, widget_id, linkfo ); }
  else {
     ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
     x = ptp->x; y = ptp->y;
     gtk_container_foreach (GTK_CONTAINER (linkfo->table),
			 sii_links_set_from_struct,
			 (gpointer)linkfo );
     gtk_widget_set_uposition (widget, ptp->x, ptp->y);
     gtk_widget_show (widget);
  }
}

/* c---------------------------------------------------------------------- */

void sii_links_widget_cb ( GtkWidget *w, gpointer   data )
{
   GtkWidget *widget;
   GtkWidget *table;
   GtkWidget *button = GTK_WIDGET (data);
   LinksInfo *li = (LinksInfo *)gtk_object_get_data (GTK_OBJECT(button)
						     , "link_info");
   guint task = GPOINTER_TO_UINT (gtk_object_get_data (GTK_OBJECT(button)
						       , "button_wid"));
   gint active;

   switch (task) {

    case LINKS_OK:
      table = GTK_WIDGET
	(gtk_object_get_data (GTK_OBJECT(button), "table_widget"));
      gtk_container_foreach (GTK_CONTAINER (table),
			 sii_links_get_foreach,
			 (gpointer)li );
    case LINKS_CANCEL:
      if( widget = sii_get_widget_ptr (li->frame_num, li->widget_id))
	{ gtk_widget_hide (widget); }
      break;

    case LINKS_CLEAR_ALL:
    case LINKS_SET_ALL:
      active = (task == LINKS_SET_ALL);
      table = GTK_WIDGET
	(gtk_object_get_data (GTK_OBJECT(button), "table_widget"));
      gtk_container_foreach (GTK_CONTAINER (table),
			 sii_links_set_foreach,
			 (gpointer)active );
      break;

   };
}

/* c---------------------------------------------------------------------- */

void sii_links_cb ( GtkWidget *w, gpointer   data )
{
   GtkWidget *button = GTK_WIDGET (data);
   guint button_num = GPOINTER_TO_UINT (gtk_object_get_data (GTK_OBJECT(button)
			     , "button_num"));
   LinksInfo *li = (LinksInfo *)gtk_object_get_data (GTK_OBJECT(button)
			     , "link_info");

}

/* c---------------------------------------------------------------------- */

void sii_links_widget( guint frame_num, guint widget_id, LinksInfo *linkfo )
{
  GtkWidget *window;
  GtkWidget *vbox0;
  GtkWidget *hbox0;
  GtkWidget *button;
  GtkWidget *table;

  GtkAttachOptions xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  GtkAttachOptions yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
  guint xpadding = 0;
  guint ypadding = 0;
  GdkPoint *ptp;
  gint x, y, row, col, cols, jj, kk, nn, button_num, mark;
  gchar *bb, str[16];

 
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
  bb = g_strdup_printf ("Frame %d  %s", frame_num+1, linkfo->name );
  gtk_window_set_title (GTK_WINDOW (window), bb);
  g_free( bb );
  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Create a new vertical box for storing widgets --- */
  vbox0 = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER(window), vbox0);

  linkfo->table = 
    table = gtk_table_new (maxConfigRows-1, maxConfigCols, TRUE);
  gtk_container_add (GTK_CONTAINER(vbox0), table);
  gtk_object_set_data (GTK_OBJECT(window)
		       , "table_widget", (gpointer)table);

  xpadding = ypadding = 2;
  button_num = 0;
 
  for (row=0; row < maxConfigRows-1; row++ ) {
     for (col=0; col < maxConfigCols; col++, button_num++ ) {

	sprintf (str, "F%d", button_num+1 );
	button = gtk_toggle_button_new_with_label (str);
	gtk_widget_set_usize (button, 0, 40);
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button)
				      , linkfo->link_set[button_num]);

	gtk_table_attach (GTK_TABLE (table), button
			  , col, col+1, row, row+1
			  , xoptions, yoptions, xpadding, ypadding );

	gtk_signal_connect (GTK_OBJECT(button)
		      ,"toggled"
		      , (GtkSignalFunc) sii_links_cb
		      , (gpointer)button
		      );
	gtk_object_set_data (GTK_OBJECT(button)
			     , "button_num", (gpointer)(button_num));
	gtk_object_set_data (GTK_OBJECT(button)
			     , "link_info", (gpointer)linkfo);
     }
  }


  hbox0 = gtk_hbutton_box_new ();
  gtk_container_add (GTK_CONTAINER(vbox0), hbox0);


  button = gtk_button_new_with_label ("Clear All");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + LINKS_CLEAR_ALL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_links_widget_cb
		      , (gpointer)button
		      );
  gtk_object_set_data (GTK_OBJECT(button)
		       , "link_info", (gpointer)linkfo);
  gtk_object_set_data (GTK_OBJECT(button)
		       , "button_wid", (gpointer)LINKS_CLEAR_ALL);
  gtk_object_set_data (GTK_OBJECT(button)
		       , "table_widget", (gpointer)table);

  button = gtk_button_new_with_label ("Set All");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + LINKS_SET_ALL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_links_widget_cb
		      , (gpointer)button
		      );
  gtk_object_set_data (GTK_OBJECT(button)
		       , "link_info", (gpointer)linkfo);
  gtk_object_set_data (GTK_OBJECT(button)
		       , "button_wid", (gpointer)LINKS_SET_ALL);
  gtk_object_set_data (GTK_OBJECT(button)
		       , "table_widget", (gpointer)table);


  button = gtk_button_new_with_label ("OK");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + LINKS_OK;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_links_widget_cb
		      , (gpointer)button
		      );
  gtk_object_set_data (GTK_OBJECT(button)
		       , "link_info", (gpointer)linkfo);
  gtk_object_set_data (GTK_OBJECT(button)
		       , "button_wid", (gpointer)LINKS_OK);
  gtk_object_set_data (GTK_OBJECT(button)
		       , "table_widget", (gpointer)table);

  button = gtk_button_new_with_label ("Cancel");
  gtk_box_pack_start (GTK_BOX (hbox0), button, TRUE, TRUE, 0 );
  nn = frame_num*TASK_MODULO + LINKS_CANCEL;
  gtk_signal_connect (GTK_OBJECT(button)
		      ,"clicked"
		      , (GtkSignalFunc) sii_links_widget_cb
		      , (gpointer)button
		      );
  gtk_object_set_data (GTK_OBJECT(button)
		       , "link_info", (gpointer)linkfo);
  gtk_object_set_data (GTK_OBJECT(button)
		       , "button_wid", (gpointer)LINKS_CANCEL);


  /* --- Make everything visible --- */
  
  gtk_widget_show_all (window);
  mark = 0;

}

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */
