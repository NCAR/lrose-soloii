/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "sii_enums.h"
# include <gdk/gdkkeysyms.h>

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

void sii_frame_expose_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_frame_config_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_focus_in_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_focus_out_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_enter_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void sii_leave_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data );
void checkup_cb( GtkWidget *w, gpointer   data );

void sii_plot_data (guint frame_num, guint plot_function);

void solo_set_halt_flag(); 

void sii_set_geo_coords ( int frame_num, gdouble dx, gdouble dy);

void sii_check_def_widget_sizes ();

/* c---------------------------------------------------------------------- */

void center_frame_cb( GtkWidget *w, gpointer data )
{
   guint nn = GPOINTER_TO_UINT (data);

   if (nn < 1) {
     sii_default_center (-1);	/* do every frame */
     return;
   }
   sii_center_on_clicks (nn);
}

/* c---------------------------------------------------------------------- */

void sii_blow_up_expose_event (GtkWidget *frame, GdkEvent *event
			       , gpointer data )
{
   GdkDrawable *drawable = frame->window;
   GtkWidget*	drawing_area = frame;
   guint frame_num = GPOINTER_TO_UINT (data); /* frame number */
   SiiFrameConfig *sfc, *sfc0;
   guint ix=0, iy=0, width, height, ncols, nrows, cn, mm, nn, jj;
   sii_table_parameters *stp;
   GdkGC *gc;
   GdkColor *gcolor;
   GdkEventExpose *expose = (GdkEventExpose *)event;
   GtkAllocation alloc;
   gboolean reconfigured, uncovered, totally_exposed;
   static gboolean new_frames = FALSE;
   GdkRectangle *rect, area;
   gchar *aa, str[256];

  frame_num = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(frame), "frame_num" ));

   sfc = frame_configs[frame_num];
   sfc0 = frame_configs[0];
   ++sfc0->big_expose_count;

   alloc = sfc0->blow_up_frame->allocation;
   area = expose->area;

   reconfigured = alloc.width != sfc0->big_width ||
      alloc.height != sfc0->big_height;

   reconfigured = sfc0->big_reconfig_count > 0;

   uncovered = alloc.width != expose->area.width ||
     alloc.height != expose->area.height;

   totally_exposed = alloc.width == expose->area.width &&
     alloc.height == expose->area.height;

   *str = '\0';
   if (reconfigured) {
      strcat (str, "reconfig ");
   }
   if (uncovered) {
     strcat (str, "uncovered ");
   }
   if (totally_exposed) {
     strcat (str, "totally ");
   }
   if (!(reconfigured || uncovered || totally_exposed)) {
     strcat (str, "neither ");
   }

   g_message
     ("bup expose: %s frm:%d gtk:%dx%d sii:%dx%d exp:%dx:%d rc:%d xp:%d"
	      , str
	      , frame_num
	      , alloc.width
	      , alloc.height
	      , sfc0->big_width, sfc0->big_height
	      , expose->area.width, expose->area.height
	      , sfc0->big_reconfig_count
	      , sfc0->big_expose_count
	      );
   sfc0->big_reconfig_count = 0;

				/*
				 */
   if (!reconfigured) {
   }
     sii_really_xfer_images (frame_num, &area, TRUE);
}

/* c---------------------------------------------------------------------- */

void sii_frame_expose_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
				/* c...expose */
   GdkDrawable *drawable = frame->window;
   GtkWidget*	drawing_area = frame;
   guint frame_num = GPOINTER_TO_UINT (data); /* frame number */
   SiiFrameConfig *sfc;
   guint ix=0, iy=0, width, height, ncols, nrows, cn, mm, nn, jj;
   gint mark;
   sii_table_parameters *stp;
   GdkGC *gc;
   GdkColor *gcolor;
   GdkEventExpose *expose = (GdkEventExpose *)event;
   GtkAllocation alloc;
   gboolean reconfigured, uncovered, totally_exposed;
   static gboolean new_frames = FALSE;
   GdkRectangle *rect, area;
   gchar mess[256];
   static GString *gs=NULL;


   if(!gs)
     { gs = g_string_new (""); }
   g_string_truncate (gs, 0);

   sfc = frame_configs[frame_num];
   ++sfc->expose_count;
   alloc = sfc->frame->allocation;
   area = expose->area;

   reconfigured = sfc->colorize_count == 0;
   if (reconfigured)
     { g_string_append (gs, " REC"); }

   uncovered = alloc.width != expose->area.width ||
     alloc.height != expose->area.height;
   if (uncovered)
     { g_string_append (gs, " UNC"); }

   totally_exposed = alloc.width == expose->area.width &&
     alloc.height == expose->area.height;
   if (totally_exposed)
     { g_string_append (gs, " TOT"); }

   sfc->most_recent_expose = expose->area;

   g_string_sprintfa
     (gs, " frm:%d frm:%dx%d  gtk:%dx%d  ec:%d cc:%d nf:%d lc:%d"
      , frame_num
      , sfc->width, sfc->height
      , alloc.width
      , alloc.height
      , sfc->expose_count
      , sfc->reconfig_count
      , sfc->new_frame_count
      , sfc->local_reconfig
      );
# ifdef obsolete
     g_string_sprintfa
       (gs, "\n  x:%d y:%d %dx%d"
	, expose->area.x, expose->area.y
	, expose->area.width, expose->area.height
	);
# endif

   if (reconfigured) {
     mark = 0;
   }
   else if (uncovered) {
     sii_xfer_images (frame_num, &area);
   }
   else if (!sfc->reconfig_count && totally_exposed) {
      sii_xfer_images (frame_num, &area);
   }
# ifdef config_dbug
   g_message (gs->str);
# endif

   while (sfc->reconfig_count) {
     
     if (sfc->local_reconfig && sfc->expose_count == sfc->reconfig_count)
       {
	 /* reconfig_count == 1 for first time through
	  * other local reconfigs emit two exposes
	  * one for the new drawable and one for the config
	  */
	 sfc->width = alloc.width;
	 sfc->height = alloc.height;
	 sfc->data_width = alloc.width;
	 sfc->data_height = alloc.height;
	 sfc->local_reconfig = FALSE;
	 g_string_append (gs, " X0");
	 sii_plot_data (frame_num, REPLOT_THIS_FRAME);
	 break;
       }
     
     if (sfc->reconfig_count > 2)
       {
	 /* We've rebuilt the tables and this is the last expose,
	  * now plot the data
	  */
	 sfc->width = alloc.width;
	 sfc->height = alloc.height;
	 sfc->data_width = alloc.width;
	 sfc->data_height = alloc.height;
	 sii_plot_data (frame_num, REPLOT_THIS_FRAME);
	 break;
       }

     if (sfc->expose_count == 2)
       {

	 if (frame_num == 0) {
	   /* recalculate basic table cell size
	    * should be a multiple of 4
	    */
	   stp = &frame_configs[0]->tbl_parms;
	   mm = stp->right_attach - stp->left_attach;
	   nn = stp->bottom_attach - stp->top_attach;
	   sii_table_widget_width =
	     (guint)((gdouble)alloc.width/mm);
	   sii_table_widget_height =
	     (guint)((gdouble)alloc.height/nn);
	   
	   sii_check_def_widget_sizes ();
	   sii_new_frames ();
	 }
	 break;
       }
     break;
   }
}

/* c---------------------------------------------------------------------- */

void sii_blow_up_config_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
				/* c...config */
  guint frame_num = GPOINTER_TO_UINT (data), mark;
  GdkEventConfigure *ce;
  gchar *aa = GTK_WIDGET_SENSITIVE( frame ) ? "sensitive" : "insensitive" ;
  gdouble d;

  frame_num = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(frame), "frame_num" ));

  ce = (GdkEventConfigure *)event;
  ++frame_configs[0]->big_reconfig_count;
  frame_configs[0]->big_expose_count = 0;
  frame_configs[0]->big_colorize_count = 0;

  g_message ("bup configure frm %d %dx%d"
	     , frame_num
	     , frame->allocation.width
	     , frame->allocation.height
	     );
}

/* c---------------------------------------------------------------------- */

void sii_frame_config_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
				/* c...config */
  guint frame_num = GPOINTER_TO_UINT (data), mark;
  GdkEventConfigure *ce;
  gchar *aa = GTK_WIDGET_SENSITIVE( frame ) ? "sensitive" : "insensitive" ;
  gchar *bb, mess[256];
  gdouble d;

  strcpy (mess, " ");
  ce = (GdkEventConfigure *)event;
  ++frame_configs[frame_num]->reconfig_count;
  frame_configs[frame_num]->colorize_count = 0;

# ifdef conf_dbug
  g_message ("CONFIG frm:%d %dx%d gtk:%dx%d  ec:%d cc:%d nf:%d et:%d se:%d"
	     , frame_num
	     , frame_configs[frame_num]->width
	     , frame_configs[frame_num]->height
	     , frame->allocation.width
	     , frame->allocation.height
	     , frame_configs[frame_num]->expose_count
	     , frame_configs[frame_num]->reconfig_count
	     , frame_configs[frame_num]->new_frame_count
	     , event->type
	     , ce->send_event
	     );
# endif
  frame_configs[frame_num]->expose_count = 0;
}

/* c---------------------------------------------------------------------- */

void sii_enter_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
  gint xx, yy;
  GdkModifierType state;
  guint frame_num = GPOINTER_TO_UINT (data), mark;
  gdk_window_get_pointer( frame->window, &xx, &yy, &state );
  cursor_frame = frame_num +1;
				/*
  g_message ("frame %d enter", frame_num );
				 */
}

/* c---------------------------------------------------------------------- */

void sii_leave_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
  guint frame_num = GPOINTER_TO_UINT (data), mark;
  cursor_frame = 0;
# ifdef obsolete
  g_message ("frame %d leave", frame_num );
# endif
}

/* c---------------------------------------------------------------------- */

void sii_blow_up_mouse_button_event (GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
  /* GDK_BUTTON1_MASK GDK_BUTTON2_MASK GDK_BUTTON3_MASK */
  guint frame_num = GPOINTER_TO_UINT (data), mark, width, height;
  gint x, y, jj;
  gboolean the_same;
  gint xx, yy;
  GdkModifierType state, statexy;
  SiiPoint pt, *prev_pt;
  gchar str[16], *aa, mess[256];
  GdkPoint *ptp;
  gboolean double_click, triple_click;
  gchar *clicks = "";
  gdouble dx, dy, d_xx, d_yy, d_zz;
  SiiFrameConfig *sfc;
  SiiFrameConfig *sfc0;


  frame_num = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(frame), "frame_num" ));
 
# ifdef notyet
  g_message (" ");
  gdk_window_get_origin ( frame->window, &x, &y );
  g_message (" origin  %d %d %d", x, y, statexy );

  gdk_window_get_deskrelative_origin ( frame->window, &x, &y );
  g_message (" deskrelative_origin  %d %d %d", x, y, statexy );

  gdk_window_get_root_origin ( frame->window, &x, &y );
  g_message (" root_origin  %d %d %d", x, y, statexy );

  gdk_window_get_pointer( NULL, &x, &y, &statexy );
  g_message ("pointer NULL window %d %d %d", x, y, statexy );
# endif

  /* relative to the origin of this window */
  gdk_window_get_pointer( frame->window, &xx, &yy, &state );
  sprintf (mess, "xx:%d yy:%d state:%d", xx, yy, state);
  g_message (mess);

  xx /= 2;
  yy /= 2;
  sfc = frame_configs[frame_num];
  sfc0 = frame_configs[0];

  d_xx = sfc->ulc_radar.xx;
  d_yy = sfc->ulc_radar.yy;
  sfc->click_loc.x = xx;
  sfc->click_loc.y = yy;
  width = sfc0->data_width;
  height = sfc0->data_height;


  /* set offsets from the center of the frame
   */
  dx = xx -.5 * width;
  dy = (height -1 -yy) -.5 * height;

  *str = '\0';
  pt.button_mask = 0;

  if( state & GDK_BUTTON1_MASK || state & GDK_BUTTON2_MASK) {
    pt.button_mask |= (state & GDK_BUTTON1_MASK)
      ? GDK_BUTTON1_MASK : GDK_BUTTON2_MASK;

    /* send to editor which should ignore it if
     * not shown or the suspend boundary definition
     * flag is set ("No BND" button)
       void sii_editor_data_click (SiiPoint *point);
     */
    strcat( str, (state & GDK_BUTTON1_MASK) ? "B1 ": "B2 ");

    pt.frame_num = frame_num;
    pt.x = xx;
    pt.y = yy;
    pt.dtime = pt.lat = pt.lon = pt.alt = 0;
    the_same = 0;
    if (clicks_in_que) {
      prev_pt = (SiiPoint *)sii_click_que->data;
      the_same = (memcmp (prev_pt, &pt, sizeof (SiiPoint)) == 0);
    }
    if (!the_same)
      { insert_clicks_que (&pt); }

    /* set the geographical coordinates of the click
     */
    sii_set_geo_coords ((int)frame_num, dx, dy);
  }

  if( state & GDK_BUTTON3_MASK ) {
    strcat( str, "B3 " );
    pt.button_mask |= GDK_BUTTON3_MASK;
  }

   if( state & GDK_BUTTON3_MASK ) {
      jj = FRAME_MENU;
      gdk_window_get_pointer (NULL, &x, &y, NULL);
      ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
      ptp->x = x; ptp->y = y;
      ptp = &frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA];
      ptp->x = x; ptp->y = y;
      show_frame_menu ( frame, data );
   }

}

/* c---------------------------------------------------------------------- */

void sii_mouse_button_event (GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
  /* GDK_BUTTON1_MASK GDK_BUTTON2_MASK GDK_BUTTON3_MASK */
  guint frame_num = GPOINTER_TO_UINT (data), mark, width, height;
  gint x, y, jj;
  gboolean the_same;
  gint xx, yy;
  GdkModifierType state, statexy;
  SiiPoint pt, *prev_pt;
  gchar str[16], *aa;
  GdkPoint *ptp;
  gboolean double_click, triple_click;
  gchar *clicks = "";
  gdouble dx, dy, d_xx, d_yy;


# ifdef notyet
  g_message (" ");
  gdk_window_get_origin ( frame->window, &x, &y );
  g_message (" origin  %d %d %d", x, y, statexy );

  gdk_window_get_deskrelative_origin ( frame->window, &x, &y );
  g_message (" deskrelative_origin  %d %d %d", x, y, statexy );

  gdk_window_get_root_origin ( frame->window, &x, &y );
  g_message (" root_origin  %d %d %d", x, y, statexy );

  gdk_window_get_pointer( NULL, &x, &y, &statexy );
  g_message ("pointer NULL window %d %d %d", x, y, statexy );
# endif

  /* relative to the origin of this window */
  gdk_window_get_pointer( frame->window, &xx, &yy, &state );

  d_xx = frame_configs[frame_num]->ulc_radar.xx;
  d_yy = frame_configs[frame_num]->ulc_radar.yy;
  frame_configs[frame_num]->click_loc.x = xx;
  frame_configs[frame_num]->click_loc.y = yy;
  width = frame_configs[frame_num]->data_width;
  height = frame_configs[frame_num]->data_height;

  /* set offsets from the center of the frame
   */
  dx = xx -.5 * width;
  dy = (height -1 -yy) -.5 * height;
  g_string_sprintf (gs_complaints, 
"click:%dx%d(%d,%d)(%.4f,%.4f)ulcR:(%.4f,%.4f)"
		    , width, height, xx, yy, dx, dy, d_xx, d_yy
		    );

  double_click = event->type == GDK_2BUTTON_PRESS;
  if (double_click)
    { clicks = "double"; }
  triple_click = event->type == GDK_3BUTTON_PRESS;
  if (triple_click)
    { clicks = "triple"; }

  *str = '\0';
  pt.button_mask = 0;

  if( state & GDK_BUTTON1_MASK || state & GDK_BUTTON2_MASK) {
    pt.button_mask |= (state & GDK_BUTTON1_MASK)
      ? GDK_BUTTON1_MASK : GDK_BUTTON2_MASK;

    /* send to editor which should ignore it if
     * not shown or the suspend boundary definition
     * flag is set ("No BND" button)
       void sii_editor_data_click (SiiPoint *point);
     */
    strcat( str, (state & GDK_BUTTON1_MASK) ? "B1 ": "B2 ");

    pt.frame_num = frame_num;
    pt.x = xx;
    pt.y = yy;
    pt.dtime = pt.lat = pt.lon = pt.alt = 0;
    the_same = 0;
    if (clicks_in_que) {
      prev_pt = (SiiPoint *)sii_click_que->data;
      the_same = (memcmp (prev_pt, &pt, sizeof (SiiPoint)) == 0);
# ifdef notyet
      g_message ("Prev x:%d  y:%d %d", prev_pt->x, prev_pt->y, the_same);
# endif
    }
    if (!the_same)
      { insert_clicks_que (&pt); }

    /* set the geographical coordinates of the click
     */
    sii_set_geo_coords ((int)frame_num, dx, dy);
  }

  if( state & GDK_BUTTON3_MASK ) {
    strcat( str, "B3 " );
    pt.button_mask |= GDK_BUTTON3_MASK;
  }

# ifdef notyet
  g_message ("frame %d clicked in at %d %d %d %s %d %x %s"
	     , frame_num
	     , xx, yy, state, clicks, the_same, pt.button_mask
	     , (strlen(str)) ? str : "?");
# endif
   if( state & GDK_BUTTON3_MASK ) {
      jj = FRAME_MENU;
      gdk_window_get_pointer (NULL, &x, &y, NULL);
      ptp = &frame_configs[frame_num]->widget_origin[FRAME_MENU];
      ptp->x = x; ptp->y = y;
      ptp = &frame_configs[frame_num]->widget_origin[FRAME_CLICK_DATA];
      ptp->x = x; ptp->y = y;
      show_frame_menu ( frame, data );
   }

 
}

/* c---------------------------------------------------------------------- */

gint sii_blow_up_keyboard_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
  gchar *aa, *bb, *mod = "nomod";
  GdkEventKey *kbev = (GdkEventKey *)event;
  guint frame_num = GPOINTER_TO_UINT (data), mark, jj;
  guint F_frame_id = 0;
  gboolean modifier_only = FALSE;
  gboolean modified = FALSE;
  gboolean plot_command = FALSE;

  frame_num = GPOINTER_TO_UINT
    (gtk_object_get_data (GTK_OBJECT(frame), "frame_num" ));

  modified = (kbev->state & GDK_CONTROL_MASK || kbev->state & GDK_SHIFT_MASK);

  switch( kbev->keyval ) {

  case GDK_T:
  case GDK_t:
    sii_param_toggle_field (frame_num);
    break;

  case GDK_D:
  case GDK_d:
    sii_edit_zap_last_bnd_point();
    break;

  case GDK_Control_L:
  case GDK_Control_R:
  case GDK_Shift_L:
  case GDK_Shift_R:
    /* Lone modifier key. Ignore */
    modifier_only = TRUE;
    break;
    
  case GDK_F1:
  case GDK_F2:
  case GDK_F3:
  case GDK_F4:
  case GDK_F5:
  case GDK_F6:
  case GDK_F7:
  case GDK_F8:
  case GDK_F9:
  case GDK_F10:
  case GDK_F11:
  case GDK_F12:
    /* Function buttons
  case GDK_L1:
  case GDK_L2:
     */
    if( kbev->keyval == GDK_L1 )
      { F_frame_id = 11; }
    else if( kbev->keyval == GDK_L2 )
      { F_frame_id = 12; }
    else
      { F_frame_id = kbev->keyval - GDK_F1 +1; }
    g_message( "bup Caught F key:%d frm:%d", F_frame_id, frame_num );
    break;

  case 268828432:		/* SunF36 when you punch F11 on Sun Solaris */
    g_message ("bup Caught funky SunF36 for F11");
    F_frame_id = 11; 
    break;

  case 268828433:		/* SunF37 when you punch F12 on Sun Solaris */
    g_message ("bup Caught funky SunF37 for F12");
    F_frame_id = 12; 
    break;
  };

  if (F_frame_id > 0)
    { sii_blow_up (F_frame_id-1); }
}

/* c---------------------------------------------------------------------- */

gint sii_frame_keyboard_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
 /* Middle mouse button centers the lockstep on the clicked point
  * There will also be a option to set the limits of the plot
  * (center and magnification) by using the last N points clicked.
  *
  */

# ifdef just_for_info
  /* GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK */
  case GDK_BackSpace:
  case GDK_Tab:
  jj = gdk_keyval_from_name ( "Escape" );
  jj = gdk_keyval_from_name ( "leftarrow" );
  jj = gdk_keyval_from_name ( "uparrow" );
  jj = gdk_keyval_from_name ( "rightarrow" );
  jj = gdk_keyval_from_name ( "downarrow" );
  jj = gdk_keyval_from_name ( "Return" );
  jj = gdk_keyval_from_name ( "F1" );
  jj = gdk_keyval_from_name ( "Delete" );
  jj = gdk_keyval_from_name ( "BackSpace" );
# endif


  gchar *aa, *bb, *mod = "nomod";
  GdkEventKey *kbev = (GdkEventKey *)event;
  guint frame_num = GPOINTER_TO_UINT (data), mark, jj;
  guint F_frame_id = 0;
  gboolean modifier_only = FALSE;
  gboolean modified = FALSE;
  gboolean plot_command = FALSE;


  modified = (kbev->state & GDK_CONTROL_MASK || kbev->state & GDK_SHIFT_MASK);

  if( kbev->state & GDK_CONTROL_MASK )
    { mod = "Control"; }
  if( kbev->state & GDK_SHIFT_MASK )
    { mod = "Shift"; }

  switch( kbev->keyval ) {

  case GDK_T:
  case GDK_t:
    sii_param_toggle_field (cursor_frame-1);
    break;

  case GDK_D:
  case GDK_d:
    sii_edit_zap_last_bnd_point();
    break;

  case GDK_Control_L:
  case GDK_Control_R:
  case GDK_Shift_L:
  case GDK_Shift_R:
    /* Lone modifier key. Ignore */
    modifier_only = TRUE;
    break;
    
  case GDK_F1:
  case GDK_F2:
  case GDK_F3:
  case GDK_F4:
  case GDK_F5:
  case GDK_F6:
  case GDK_F7:
  case GDK_F8:
  case GDK_F9:
  case GDK_F10:
  case GDK_F11:
  case GDK_F12:
    /* Function buttons
  case GDK_L1:
  case GDK_L2:
     */
    if( kbev->keyval == GDK_L1 )
      { F_frame_id = 11; }
    else if( kbev->keyval == GDK_L2 )
      { F_frame_id = 12; }
    else
      { F_frame_id = kbev->keyval - GDK_F1 +1; }
    
    g_message( "Caught F key:%d ", F_frame_id );
    break;

  case 268828432:		/* SunF36 when you punch F11 on Sun Solaris */
    g_message ("Caught funky SunF36 for F11");
    F_frame_id = 11; 
    break;

  case 268828433:		/* SunF37 when you punch F12 on Sun Solaris */
    g_message ("Caught funky SunF37 for F12");
    F_frame_id = 12; 
    break;

  case GDK_Escape:		/* Stop display or editing  */
    g_message( "Caught Escape " );
    solo_set_halt_flag ();
    break;

  case GDK_Delete:		/* delete the last boundary point */
    g_message( "Caught Delete " );
      break;

  case GDK_Right:
    /* if the cursor is currently residing in a frame, then
     * plot the next sweep for the set of frames (lockstep)
     * of which this frame is a member
     */
    if( kbev->state & ( GDK_SHIFT_MASK | GDK_CONTROL_MASK )) {
      /* fast forward mode (escape interrupts this mode) */
      g_message( "Caught Control/Shift Right " );

      if (cursor_frame) {
	sii_plot_data (cursor_frame-1, FORWARD_FOREVER);
      }
    }
    else {
      if (cursor_frame) {
	sii_plot_data (cursor_frame-1, FORWARD_ONCE);
      }
      g_message( "Caught Right " );
    }
    break;
    
  case GDK_Left:
    /* Same as Right only the other direction */
    if( kbev->state & ( GDK_SHIFT_MASK | GDK_CONTROL_MASK )) {
      g_message( "Caught Control/Shift Left " );

      if (cursor_frame) {
	sii_plot_data (cursor_frame-1, BACKWARDS_FOREVER);
      }
    }
    else {
      if (cursor_frame) {
	sii_plot_data (cursor_frame-1, BACKWARDS_ONCE);
      }
      g_message( "Caught Left " );
    }
    break;

  case GDK_Up:
    /* Next sweep at same fixed angle */
      break;

  case GDK_Down:
    /* Previous sweep at same fixed angle */
    break;

    
  case GDK_Return:
      g_message( "Caught Return " );
      if (cursor_frame) {
	sii_plot_data (cursor_frame-1, REPLOT_LOCK_STEP);
      }
      else
	{ sii_plot_data (cursor_frame-1, REPLOT_ALL); }
    break;


  default:
    break;
  };


  if (F_frame_id > 0)		/* Blowup or pop down a big image */
    { sii_blow_up (F_frame_id-1); }


  aa = gdk_keyval_name( kbev->keyval );
  if (!aa)
    { aa = "unk"; }

  g_message ("data %d frame: %d keyed k:%d s:%d n:%s aa:%s mod:%s"
	     , frame_num, cursor_frame
	     , kbev->keyval, kbev->state, kbev->string, aa, mod);

  return (frame_num > 2 * maxFrames) ? FALSE : TRUE;
}

/* c---------------------------------------------------------------------- */

void custom_config_cb( GtkWidget *w, gpointer   data )
{
  guint cfg = GPOINTER_TO_UINT (data);
  guint ncols = cfg/10;
  guint nrows = cfg % 10;
  g_message ("Hello, Custom Config World! cols: %d  rows: %d\n"
	     , ncols, nrows );
# define ONE_BIG_TWOV_SMALL   1012
# define ONE_BIG_TWO_SMALL   2012
# define ONE_BIG_THREE_SMALL 1013
# define ONE_BIG_FIVE_SMALL  1015
# define ONE_BIG_SEVEN_SMALL 1017
# define TWO_BIG_FOURV_SMALL  1024
# define TWO_BIG_FOUR_SMALL  2024

  sii_reset_config_cells();

  if( cfg == ONE_BIG_TWOV_SMALL ) {
    config_cells[0]->frame_num = 1;
    config_cells[1]->frame_num = 1;
    config_cells[4]->frame_num = 1;
    config_cells[2]->frame_num = 2;
    config_cells[6]->frame_num = 3;
  }
  else if( cfg == ONE_BIG_TWO_SMALL ) {
    config_cells[0]->frame_num = 1;
    config_cells[1]->frame_num = 1;
    config_cells[4]->frame_num = 1;
    config_cells[8]->frame_num = 2;
    config_cells[9]->frame_num = 3;
  }
  else if( cfg == ONE_BIG_THREE_SMALL ) {
    config_cells[0]->frame_num = 1;
    config_cells[2]->frame_num = 1;
    config_cells[8]->frame_num = 1;
    config_cells[3]->frame_num = 2;
    config_cells[7]->frame_num = 3;
    config_cells[11]->frame_num = 4;
  }    
  else if( cfg == ONE_BIG_FIVE_SMALL ) {
    config_cells[0]->frame_num = 1;
    config_cells[1]->frame_num = 1;
    config_cells[4]->frame_num = 1;
    config_cells[2]->frame_num = 2;
    config_cells[6]->frame_num = 3;
    config_cells[8]->frame_num = 4;
    config_cells[9]->frame_num = 5;
    config_cells[10]->frame_num = 6;
  }
  else if( cfg == ONE_BIG_SEVEN_SMALL ) {
    config_cells[0]->frame_num = 1;
    config_cells[2]->frame_num = 1;
    config_cells[8]->frame_num = 1;
    config_cells[3]->frame_num = 2;
    config_cells[7]->frame_num = 3;
    config_cells[11]->frame_num = 4;
    config_cells[12]->frame_num = 5;
    config_cells[13]->frame_num = 6;
    config_cells[14]->frame_num = 7;
    config_cells[15]->frame_num = 8;
  }
  else if( cfg == TWO_BIG_FOURV_SMALL ) {
    config_cells[0]->frame_num = 1;
    config_cells[1]->frame_num = 1;
    config_cells[4]->frame_num = 1;

    config_cells[2]->frame_num = 2;
    config_cells[3]->frame_num = 2;
    config_cells[6]->frame_num = 3;
    config_cells[7]->frame_num = 3;

    config_cells[8]->frame_num = 4;
    config_cells[9]->frame_num = 4;
    config_cells[12]->frame_num = 4;

    config_cells[10]->frame_num = 5;
    config_cells[11]->frame_num = 5;
    config_cells[14]->frame_num = 6;
    config_cells[15]->frame_num = 6;
  }
  else if( cfg == TWO_BIG_FOUR_SMALL ) {
    config_cells[0]->frame_num = 1;
    config_cells[1]->frame_num = 1;
    config_cells[4]->frame_num = 1;
    config_cells[2]->frame_num = 2;
    config_cells[3]->frame_num = 2;
    config_cells[6]->frame_num = 2;

    config_cells[8]->frame_num = 3;
    config_cells[9]->frame_num = 4;
    config_cells[10]->frame_num = 5;
    config_cells[11]->frame_num = 6;
  }
  else {
    printf( "Bogus custom config #: %d\n", cfg );
    return;
  }

  sii_set_config();
  sii_new_frames ();
# ifdef notyet
# endif
}

/* c---------------------------------------------------------------------- */

void zoom_data_cb( GtkWidget *w, gpointer data )
{
   guint pct = GPOINTER_TO_UINT (data);
   gfloat factor = 0;


   if( pct > 10000 ) {
     pct %= 10000;
     factor = 1. - pct*.01;
   }
   else if (pct != 0)
     { factor = 1. + pct * .01; }

   sii_blanket_zoom_change (factor);
}

/* c---------------------------------------------------------------------- */

void zoom_config_cb( GtkWidget *w, gpointer   data )
{
   guint pct = GPOINTER_TO_UINT (data), mm, nn;
   gdouble factor, zoom;
   gint jj;

   if( pct > 10000 ) {
     pct %= 10000;
     if( pct == 5 )
       { factor = 100./106; }
     if( pct == 11 )
       { factor = 100./112; }
     else if( pct == 33 )
       { factor = 100./150; }
     else
       { factor = 1. - pct*.01; }
   }
   else
     { factor = 1. + pct * .01; }
   
   sii_table_widget_width = (guint)(factor * sii_table_widget_width +.5 );
   sii_table_widget_height =(guint)(factor * sii_table_widget_height +.5 );

   for (jj=0; jj < sii_return_frame_count(); jj++) {
	frame_configs[jj]->local_reconfig = TRUE;
	frame_configs[jj]->reconfig_count = 0;
   }
   sii_check_def_widget_sizes ();

   if (!pct) {
     sii_table_widget_width += 4;
     sii_table_widget_height += 4;
   }
   sii_new_frames ();
}

/* c---------------------------------------------------------------------- */

void shape_cb( GtkWidget *w, gpointer   data )
{
   guint cfg = GPOINTER_TO_UINT (data), jj;
   gdouble factor;

# ifdef obsolete
   if( cfg == 11 ) {
     sii_table_widget_width = (guint)(DEFAULT_WIDTH * sii_config_w_zoom);
     sii_table_widget_height = (guint)(DEFAULT_WIDTH * sii_config_h_zoom);
   }
   else if(cfg == 43 ) {
     sii_table_widget_width = (guint)(DEFAULT_WIDTH * sii_config_w_zoom);
     sii_table_widget_height =
       (guint)(DEFAULT_SLIDE_HEIGHT * sii_config_h_zoom);
   }
   else if(cfg == 34 ) {
     sii_table_widget_width = (guint)(DEFAULT_WIDTH * sii_config_w_zoom);
     sii_table_widget_height =
       (guint)(DEFAULT_VSLIDE_HEIGHT * sii_config_h_zoom);
   }
# else
   factor = (gdouble)sii_table_widget_width/DEFAULT_WIDTH;
   
   if( cfg == 11 ) {
     sii_table_widget_height = sii_table_widget_width;
   }
   else if(cfg == 43 ) {
     sii_table_widget_height =
       (guint)(.75 * sii_table_widget_width);
   }
   else if(cfg == 34 ) {
     sii_table_widget_height =
       (guint)((gdouble)4./3. * sii_table_widget_width);
   }
# endif   
   for (jj=0; jj < sii_return_frame_count(); jj++) {
	frame_configs[jj]->local_reconfig = TRUE;
	frame_configs[jj]->reconfig_count = 0;
   }
   sii_check_def_widget_sizes ();
   sii_new_frames ();
} 
/* c---------------------------------------------------------------------- */

void config_cb( GtkWidget *w, gpointer   data )
{
   guint cfg = GPOINTER_TO_UINT (data);
   guint ncols = cfg/10;
   guint nrows = cfg % 10;
   guint ii, jj, kk, frame_num = 0;
   gboolean result;

# ifdef obsolete
   g_message ("Hello, Config World! cols: %d  rows: %d\n", ncols, nrows );
# endif

  if( cfg == 99 ) {		/* the config cells have been set already */
     result = sii_set_config();
     return;
  }
  
  sii_reset_config_cells();

  for( jj = 0; jj < nrows; jj++ ) {
    for( ii = 0; ii < ncols; ii++ ) {
      kk = ii + jj * maxConfigCols;
      config_cells[kk]->frame_num = ++frame_num; /* tag the cell with the frame no. */
    }
  }
  result = sii_set_config();
  sii_new_frames ();
}

# ifdef obsolete
/* c---------------------------------------------------------------------- */

static GtkItemFactoryEntry editor_menu_items[] = {
  { "/File",                NULL,          NULL,    0,  "<Branch>" },
  { "/File/Open Cmd File", NULL,   checkup_cb,    0,  NULL },
  { "/File/Save Cmd File", NULL,   checkup_cb,    0,  NULL },
  { "/File/sep3",          NULL,      NULL,    0,  "<Separator>" },
  { "/File/Open Bnd File", NULL,   checkup_cb,    0,  NULL },
  { "/File/Save Bnd File", NULL,   checkup_cb,    0,  NULL },
  { "/File/sep3",          NULL,      NULL,    0,  "<Separator>" },
  { "/File/Extend Edit",   NULL,   checkup_cb,    0,  NULL },
  { "/File/sep3",          NULL,      NULL,    0,  "<Separator>" },
  { "/File/Close",         NULL,   checkup_cb,    0,  NULL },
  { "/Boundary",           NULL,          NULL,    0,  "<Branch>" },
  { "/Boundary/List Points",NULL,   checkup_cb,    0,  NULL },
  { "/Boundary/sep3",      NULL,      NULL,    0,  "<Separator>" },
  { "/Boundary/Edit Inside",NULL,   checkup_cb,    0,  NULL },
  { "/Boundary/Edit Outside",NULL,   checkup_cb,    0,  NULL },
  { "/Help",                NULL,          NULL,    0,  "<Branch>" },
  { "/Help/Cmd Editing",        NULL,   checkup_cb,    0,  NULL },
  { "/Help/On Boundaries",        NULL,   checkup_cb,    0,  NULL },

};

/* c---------------------------------------------------------------------- */

void sii_get_edit_menubar( GtkWidget  *window,
                    GtkWidget **menubar )
{
  GtkItemFactory *item_factory;
  GtkAccelGroup *accel_group;
  gint nmenu_items = sizeof (editor_menu_items) / sizeof (editor_menu_items[0]);
  
  
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

  gtk_item_factory_create_items (item_factory, nmenu_items, editor_menu_items, NULL);

  /* Attach the new accelerator group to the window. */
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

  if (menubar)
    /* Finally, return the actual menu bar created by the item factory. */ 
    *menubar = gtk_item_factory_get_widget (item_factory, "<main>");

}
# endif

/* c---------------------------------------------------------------------- */
guint test_event_cb(GtkWidget *label, gpointer data )
{
  guint position = GPOINTER_TO_UINT (data);
  g_message( "layout widget line: %d"
	    , position
	    );
  return FALSE;
}

/* c---------------------------------------------------------------------- */

void test_widget( GtkWidget *w, gpointer   data )
{
  GtkWidget *label;

  GtkWidget *window;
  GtkWidget *scrolledwindow;
  GtkWidget *vbox;
  GtkWidget *edit_menubar;
  GtkWidget *hbox;

  GtkWidget *vbox1;
  GtkWidget *vbox2;
  GtkWidget *vbox3;

  GtkWidget *text;
  GtkWidget *table;
  GtkWidget *hscrollbar;
  GtkWidget *vscrollbar;

  GtkWidget *menubar;
  GtkWidget *menu;
  GtkWidget *menuitem;
  GtkWidget *layout;

  GtkWidget *button;


  gchar str[256], *aa, *bb, *ee;
  guint frame_num = GPOINTER_TO_UINT (data);
  gint event_flags = GDK_BUTTON_PRESS_MASK;
  const gchar *cc, *tt = "00/00/2000 00:00:00 HHHHHHHH 000.0 HHH HHHHHHHH";
  gint length, width, height, mm, nn, mark;
  gfloat upper;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      gtk_signal_connect (GTK_OBJECT (window), "destroy",
			  GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			  &window);

  /* --- Title and border --- */

  gtk_container_border_width (GTK_CONTAINER (window), 0);

  /* --- Set the window title and size --- */
  gtk_widget_set_usize (window, 320, 600);
  gtk_window_set_title (GTK_WINDOW (window), "Test Widget");

  /* --- Create a new vertical box for storing widgets --- */
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER (vbox), scrolledwindow);

  nn = 500;
  height = 18;

  layout = gtk_layout_new (NULL, NULL);
  gtk_container_add (GTK_CONTAINER(scrolledwindow), layout);
  gtk_layout_set_size (GTK_LAYOUT (layout), 380, nn*height);


  /* We set step sizes here since GtkLayout does not set
   * them itself.
   */
  GTK_LAYOUT (layout)->hadjustment->step_increment = 10.0;
  GTK_LAYOUT (layout)->vadjustment->step_increment = 10.0;
  
  aa = "00/00/2000 00:00:00.000 HHHHHHHH";
  bb = " HHH V0000";

  for (mm=0; mm < nn; mm++ ) {
     sprintf (str, "%s%6.1f%s               ", aa, (gfloat)mm, bb );
     button = gtk_button_new_with_label (str);
     gtk_layout_put (GTK_LAYOUT (layout), button,
		     0, mm*height);

     gtk_signal_connect (GTK_OBJECT(button)
			 ,"clicked"
			 , (GtkSignalFunc) test_event_cb
			 , (gpointer)button);

     gtk_object_set_data (GTK_OBJECT(button),
			  "frame_num",
			  (gpointer)frame_num);
     gtk_object_set_data (GTK_OBJECT(button),
			  "swpfi_index",
			  (gpointer)mm);
  }

  /* --- Make everything visible --- */
  
  gtk_widget_show_all (window);
  mark = 0;
}

/* c---------------------------------------------------------------------- */

void dialog_a_cb( GtkWidget *w, gpointer   data )
{
  guint itype = GPOINTER_TO_UINT (data);
  GtkWidget *dialog, *label, *button, *button2;

# ifdef obsolete
  dialog = gtk_dialog_new_with_buttons
    ( "Test Dialog",
      main_window, 
      GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_STOCK_BUTTON_OK,
      GTK_RESPONSE_NONE,
      NULL );
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
		     label );
# endif
  dialog = gtk_dialog_new ();
    
  label = gtk_label_new ( "This is the label" );
  gtk_misc_set_padding( GTK_MISC (label), 10, 10 );
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
		      label, TRUE, TRUE, 0 );
  gtk_widget_show( label );

  button = gtk_button_new_with_label ("Cancel");
  GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->action_area),
		      button, TRUE, TRUE, 0 );
  gtk_widget_grab_default (button);
  gtk_widget_show (button);

  gtk_signal_connect_object ( GTK_OBJECT (button), "clicked",
			      GTK_SIGNAL_FUNC (gtk_widget_destroy ),
			      GTK_OBJECT (dialog));
  button2 = gtk_button_new_with_label ("Finish");
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->action_area),
		      button2, TRUE, TRUE, 0 );
  gtk_widget_show (button2);

  gtk_widget_show_all (dialog);
}


/* c---------------------------------------------------------------------- */

void checkup_cb( GtkWidget *w, gpointer   data )
{
  guint itype = GPOINTER_TO_UINT (data);
  g_message ("Not yet... Just checking");
}

/* c---------------------------------------------------------------------- */

void image_cb( GtkWidget *w, gpointer   data )
{
  guint itype = GPOINTER_TO_UINT (data);
  gchar *aa;

  switch (itype ) {
  case 0:
    aa = "dir";
    break;
  case MAIN_PNG:
    aa = "png";
    break;
  case MAIN_GIF:
    aa = "gif";
    break;
  case MAIN_PRNT:
    aa = "Print";
    break;
  default:
    break;
  }
  g_message ("Hello, Image World! %s", aa );
}

/* c---------------------------------------------------------------------- */

void sii_focus_in_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
  gint xx, yy;
  GdkModifierType state;
  guint frame_num = GPOINTER_TO_UINT (data), mark;
  gdk_window_get_pointer( frame->window, &xx, &yy, &state );
# ifdef obsolete
  g_message ("frame %d focus in at %d %d %d\n", frame_num, xx, yy, state );
# endif
}

/* c---------------------------------------------------------------------- */

void sii_focus_out_event(GtkWidget *frame, GdkEvent *event
			      , gpointer data )
{
  guint frame_num = GPOINTER_TO_UINT (data), mark;
# ifdef obsolete
  g_message ("frame %d focus out\n", frame_num );
# endif
}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

