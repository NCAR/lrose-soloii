/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"

# ifndef notyet
# include <gdk/gdkx.h>
# endif

static gchar *def_param_names[] =
{ "DZ", "VE", "NCP", "SW", "DM", "ZDR", "LDR", "PHIDP", "RHOHV", "DX",
    "KDP", "PD", "DCZ", "DZ", "VE", "NCP", "SW", "", "", "", "", };

/* c---------------------------------------------------------------------- */

void sii_initialize_parameter (guint frame_num, gchar *name);
void sii_initialize_view (guint frame_num);
void sii_frame_expose_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_frame_config_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_mouse_button_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_focus_in_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_focus_out_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_enter_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_leave_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
gint sii_frame_keyboard_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_new_frames ();

void sii_check_def_widget_sizes ();

SiiLinkedList *sii_init_linked_list (guint length);

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

void sii_check_for_illegal_shrink ()
{
  SiiFrameConfig *sfc = frame_configs[0];
  sii_table_parameters *stp;
  gint jj, mm;
  gdouble d;

  for (jj=0; jj < sii_return_frame_count(); jj++) {
    if (frame_configs[jj]->reconfig_count)
      { return; }
    /* don't do anything in the middle of a reconfig */
  }
  stp = &sfc->tbl_parms;
  mm = stp->right_attach - stp->left_attach;
  d = (gdouble)sfc->width/mm;
  if (d < sii_table_widget_width)
    { sii_new_frames (); }
}

/* c---------------------------------------------------------------------- */

void sii_check_def_widget_sizes ()
{
  int nn;

  if (sii_table_widget_width < DEFAULT_WIDTH)
    { sii_table_widget_width = DEFAULT_WIDTH; }

  if (sii_table_widget_height < DEFAULT_HEIGHT)
    { sii_table_widget_height = DEFAULT_HEIGHT; }

  nn = (sii_table_widget_width -1)/4 +1;
  sii_table_widget_width = nn * 4;

  nn = (sii_table_widget_height -1)/4 +1;
  sii_table_widget_height = nn * 4;
}

/* c---------------------------------------------------------------------- */

void sii_init_frame_configs()
{
  int fn, jj, kk;
  GtkAccelGroup *accel_group;
  guint accel_mods;
  guint key;
  GtkStyle *  defstyle = gtk_widget_get_default_style ();
  GString *gstr = NULL;
  SiiLinkedList *sll;
  
  if (gstr == NULL)
    { gstr = g_string_new (""); }

  for( fn = 0; fn < maxFrames; fn++ ) {
    frame_configs[fn] = (SiiFrameConfig *)
      g_malloc0( sizeof( SiiFrameConfig ));
    frame_configs[fn]->frame_num = fn;
    frame_configs[fn]->style = gtk_style_copy (defstyle);
    frame_configs[fn]->accel_group = accel_group = gtk_accel_group_new ();
    frame_configs[fn]->data_widget_text = g_string_new ("");

    sii_initialize_parameter (fn, def_param_names[fn]);
    sii_initialize_view (fn);

    g_string_sprintfa (gstr, "%s\n", def_param_names[fn]);

				/*
    gtk_accel_group_add ( accel_group);
				 */
  }
  for( fn = 0; fn < maxFrames; fn++ ) {
     sii_set_param_names_list (fn, gstr, maxFrames);   
  }
  g_string_free (gstr, TRUE);	/* free string struct and char data */
}

/* c---------------------------------------------------------------------- */

void sii_reset_config_cells()
{
  int cn;

  if( !config_cells ) {		/* allocate them */

    config_cells = (sii_config_cell **)
      g_malloc0( maxConfigCells * sizeof( sii_config_cell *));

    for( cn = 0; cn < maxConfigCells; cn++ ) {
      config_cells[cn] = (sii_config_cell *)
	g_malloc0( sizeof( sii_config_cell ));
      config_cells[cn]->cell_num = cn;
      config_cells[cn]->row = cn/maxConfigRows;
      config_cells[cn]->col = cn % maxConfigCols;
    }
  }

  for( cn = 0; cn < maxConfigCells; cn++ ) {
    config_cells[cn]->frame_num = 0;
    config_cells[cn]->processed = no;
  }
}

/* c---------------------------------------------------------------------- */

void sii_new_frames ()
{
   int fn, jj, kk, mm, nn, width, height;
   gboolean nonhomogeneous = FALSE, homogeneous = TRUE;
   SiiFrameConfig *sfc, *sfc0 = frame_configs[0];
   sii_table_parameters *stp;
   GtkWidget *frame;
   GtkAllocation allocation;
   gdouble wzoom, hzoom;
   gpointer gptr;
   gchar str[256];
   guint att_opts = GTK_EXPAND | GTK_SHRINK | GTK_FILL;

   gint event_flags = GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK
     | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK
     | GDK_FOCUS_CHANGE_MASK;

   
   /* c...code sii_new_frames */

   if( !sii_config_count++ ) {
# ifdef obsolete
     sii_reset_config_cells();
     config_cb (0, (gpointer)22 ); /* 2x2 */
# endif
   }

   if( main_table ) {		/* this is a gtk table type container */
      gtk_widget_destroy( main_table );
   }

   main_table = gtk_table_new (sfc0->nrows, sfc0->ncols, homogeneous);
   gtk_container_add (GTK_CONTAINER (main_event_box), main_table);
# ifdef notyet
   gtk_box_pack_start (GTK_BOX (main_event_box), main_table, TRUE, TRUE, 0);
   xwin_table = GDK_WINDOW_XWINDOW (GTK_WIDGET (main_table->window));
# endif

  for (fn=0; fn < sii_frame_count; fn++ ) {

    sfc = frame_configs[fn];
    ++sfc->new_frame_count;
    sfc->frame = frame = gtk_drawing_area_new ();
    gtk_widget_set_events (frame, event_flags );
    
    stp = &sfc->tbl_parms;
    mm = stp->right_attach - stp->left_attach;
    nn = stp->bottom_attach - stp->top_attach;

    width = mm * sii_table_widget_width;
    height = nn * sii_table_widget_height;
    sprintf (str, "new_frame %d  %dx%d", fn, width, height );
    sii_append_debug_stuff (str);

    sfc->width = sfc->data_width = width;
    sfc->height = sfc->data_height = height;

    gtk_drawing_area_size (GTK_DRAWING_AREA (frame)
			   , width
			   , height );
    
    /* --- Signals! --- */
    gtk_signal_connect (GTK_OBJECT (frame), "expose_event",
			(GtkSignalFunc) sii_frame_expose_event, (gpointer)fn);
    gtk_signal_connect (GTK_OBJECT(frame),"configure_event",
			(GtkSignalFunc) sii_frame_config_event, (gpointer)fn);

    gtk_signal_connect (GTK_OBJECT(frame),"button_press_event",
			(GtkSignalFunc) sii_mouse_button_event, (gpointer)fn);
    gtk_signal_connect (GTK_OBJECT(frame),"key_press_event",
			(GtkSignalFunc) sii_frame_keyboard_event, (gpointer)fn);
    
    gtk_signal_connect (GTK_OBJECT(frame),"enter_notify_event",
			(GtkSignalFunc) sii_enter_event, (gpointer)fn);
    gtk_signal_connect (GTK_OBJECT(frame),"leave_notify_event",
			(GtkSignalFunc) sii_leave_event, (gpointer)fn);

    gtk_signal_connect (GTK_OBJECT(frame),"focus_in_event",
			(GtkSignalFunc) sii_focus_in_event, (gpointer)fn);
    gtk_signal_connect (GTK_OBJECT(frame),"focus_out_event",
			(GtkSignalFunc) sii_focus_out_event, (gpointer)fn);
    
# ifdef notyet
    xpadding = ypadding = 0;
    xoptions = yoptions = GTK_FILL | GTK_EXPAND | GTK_SHRINK;
# endif

    gtk_table_attach (GTK_TABLE (main_table), frame
		      , stp->left_attach, stp->right_attach
		      , stp->top_attach, stp->bottom_attach
		      , att_opts, att_opts, 0, 0
		      );
    
  }

   gtk_widget_show_all (main_window);
   nn = 0;
}

/* c---------------------------------------------------------------------- */

gboolean sii_set_config()
{
  int cll, cll2, jj, kk, ll, mm, nn, mark, frame, row, col;
  int row2, col2, ncols = 1, nrows = 1;
  int frame_count = 0;
  sii_config_cell * scc, *scc2;
  SiiFrameConfig *sfc;
  sii_table_parameters tbl_parms[maxFrames], *stp;

				/* start in the upper left hand corner */
				/* moving accross the columns in each row */

  for( cll = 0; cll < maxConfigCells; cll++ ) {
    scc = config_cells[cll];

    if( !scc->frame_num )
      { continue; }
    if(  scc->processed )
      { continue; }
				/* at next cell to process */
    stp = &tbl_parms[frame_count++];
    scc->processed = yes;
    frame = scc->frame_num;	/* 1-12 */
    row = row2 = cll/maxConfigRows;
    col = col2 = cll % maxConfigCols;

    stp->left_attach = col;
    stp->right_attach = col +1;
    stp->top_attach = row;
    stp->bottom_attach = row +1;
    if( stp->bottom_attach > nrows )
      { nrows = stp->bottom_attach; }
    if( stp->right_attach > ncols )
      { ncols = stp->right_attach; }
    
				/* see if this number appears again */
    for( jj = cll+1; jj < maxConfigCells; jj++ ) {
      if( config_cells[jj]->frame_num != frame || config_cells[jj]->processed )
	{ continue; }

      if( jj/maxConfigRows > row2 )
	{ row2 = jj/maxConfigRows; }

      if( jj % maxConfigCols > col2 )
	{ col2 = jj % maxConfigCols; }
    }

    if( row2 > row || col2 > col ) {

      for( ; row <= row2; row++ ) {
	if( row +1 > stp->bottom_attach ) {
	   stp->bottom_attach = row+1;
	   if( stp->bottom_attach > nrows )
	     { nrows = stp->bottom_attach; }
	}

	for( jj = col; jj <= col2; jj++ ) {
	  cll2 = row * maxConfigRows + jj;
	  config_cells[cll2]->processed = yes;

	  if( jj +1 > stp->right_attach ) {
	     stp->right_attach = jj+1;
	     if( stp->right_attach > ncols )
	       { ncols = stp->right_attach; }
	  }
	}
      }
      mark = 0;
    } /* row2 > row || col2 > col */
  } /* for( cll = 0; cll < maxConfigCells; */

  if( frame_count > 0 ) {
     for( jj = 0; jj < frame_count; jj++ ) {

	mm = tbl_parms[jj].right_attach - tbl_parms[jj].left_attach;
	nn = tbl_parms[jj].bottom_attach - tbl_parms[jj].top_attach;

	frame_configs[jj]->tbl_parms = tbl_parms[jj];
	frame_configs[jj]->ncols = ncols;
	frame_configs[jj]->nrows = nrows;
	frame_configs[jj]->local_reconfig = TRUE;
	frame_configs[jj]->reconfig_count = 0;

	/* Fix this! */
	frame_configs[jj]->lock_step = jj;
     }
     sii_frame_count = frame_count;
     return( yes );
  }

  return( no );
}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */








