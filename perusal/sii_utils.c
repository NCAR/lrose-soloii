/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "sii_utils.h"
# include "sii_overlays.h"
# include <stdio.h>
static GString *dbgs = NULL;
static gint nlines = 0;

/* c---------------------------------------------------------------------- */

void sii_dump_debug_stuff ()
{
   if (dbgs && dbgs->len > 0) {
      printf ("%s", dbgs->str);
      g_string_truncate (dbgs, 0);
      g_string_append (dbgs, "\n");
      nlines = 0;
   }
}

/* c---------------------------------------------------------------------- */

void sii_append_debug_stuff (gchar *gch)
{
   gchar *aa, *bb;
   gint len, mm, nn, max=777, slack=111;
   /*
   printf ("%s\n", gch);
    */
   
   if (!dbgs)
     { dbgs = g_string_new (""); }

   if (gch && strlen (gch) > 0) {
      mm = dbgs->len;
      g_string_append (dbgs, gch);
      g_string_append (dbgs, "\n");
      nn = dbgs->len;

      for (aa = dbgs->str; aa && mm < nn; mm++) {
	 if (aa[mm] == '\n')
	   { ++nlines; }
      }

      if (nlines > max+slack) {
	 while (nlines > max) {
	    nn = dbgs->len;
	    for (aa=dbgs->str,mm=0; mm < nn; mm++) {
	       if (aa[mm] == '\n') {
		  mm++;
		  break;
	       }
	    }
	    if (mm < dbgs->len) {
	       g_string_erase (dbgs, 0, mm);
	       --nlines;
	    }
	 }	 
      }
   }
}

/* c------------------------------------------------------------------------ */

void sii_glom_strings (const gchar **cptrs, int nn, GString *gs)
{
   g_string_truncate (gs, 0);
   for (; nn--; cptrs++) {
      g_string_append (gs, *cptrs);
      if (nn)
	{ g_string_append (gs, "\n"); }
   }
}

/* c------------------------------------------------------------------------ */

int sii_absorb_zeb_map (gchar *path_name, siiOverlay *overlay)
{
  /* routine to read in a zeb map overlay */

    int ii, jj, kk, nn, np, nt, mark, count = 0;
    char *aa, *bb, str[256], str2[256], *sptrs[64];
    FILE *stream;
    gboolean new = TRUE;
    siiPolyline *top, *next, *prev=NULL;


    if(!(stream = fopen(path_name, "r"))) {
	printf("Unable to open %s", path_name);
	return(-1);
    }
    top = next = overlay->plines;

    /* read in the new strings
     */
    for(nn=0;; nn++) {
	if(!(aa = fgets(str, (int)128, stream))) {
	    break;
	}
	if ((nt = dd_tokens (str, sptrs)) < 2)
	  { continue; }
	if (new) {
	  np = atoi (sptrs[0])/2;
	  if (!next) {
	    next = g_malloc0 (sizeof (siiPolyline));
	    if (!top)
	      { top = overlay->plines = next; }
	    next->lat = (gdouble *)g_malloc0 (np * sizeof (gdouble));
	    next->lon = (gdouble *)g_malloc0 (np * sizeof (gdouble));
	    next->x = (int *)g_malloc0 (np * sizeof (int));
	    next->y = (int *)g_malloc0 (np * sizeof (int));
	  }
	  next->urc.lat = atof (sptrs[1]);
	  next->urc.lon = atof (sptrs[3]);
	  next->llc.lat = atof (sptrs[2]);
	  next->llc.lon = atof (sptrs[4]);

	  next->num_points = np;
	  if (np > overlay->max_pline_length)
	    { overlay->max_pline_length = np; }

	  overlay->num_plines++;
	  if (prev)
	    { prev->next = next; }
	  prev = next;
	  new = FALSE;
	  jj = 0;
	  continue;
	}
	for (ii=0; ii < nt; ii +=2, jj++, np--) {
	  next->lat[jj] = atof (sptrs[ii]);
	  next->lon[jj] = atof (sptrs[ii+1]);
	  count++;
	}
	if (np == 0)
	  { new = TRUE; next = next->next; }
    }
    fclose(stream);
    return(count);
}

/* c---------------------------------------------------------------------- */

guint sii_inc_master_seq_num ()
{
   return ++sii_seq_num;
}

/* c---------------------------------------------------------------------- */

guint sii_get_master_seq_num ()
{
   return sii_seq_num;
}

/* c---------------------------------------------------------------------- */

void sii_zap_quick_message (GtkWidget *w, gpointer data )
{
   gtk_widget_destroy (GTK_WIDGET (data));
}


/* c---------------------------------------------------------------------- */

void sii_message (const gchar *message )
{
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *button;
  GtkWidget *dialog, *label;

  if (batch_mode)
    { return; }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Soloii Message");
  gtk_container_border_width (GTK_CONTAINER (window), 10);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		      &window);
  
  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_add (GTK_CONTAINER(window), vbox);

  label = gtk_label_new (message);
  gtk_label_set_justify ((GtkLabel *)label, GTK_JUSTIFY_LEFT );
  gtk_container_add (GTK_CONTAINER (vbox), label );

# ifdef obsolete
  label = gtk_label_new ("");
  gtk_container_add (GTK_CONTAINER (vbox), label );
# endif

  button = gtk_button_new_with_label (" Dismiss ");
  gtk_container_add (GTK_CONTAINER (vbox), button);

  gtk_signal_connect (GTK_OBJECT(button)
		      , "clicked"
		      , GTK_SIGNAL_FUNC(sii_zap_quick_message)
		      , window);

  gtk_widget_show_all (window);
}

/* c---------------------------------------------------------------------- */

int sii_str_seek ( char **sptrs, int count, char *sought )
{
   int ii = 0;

   if (!sptrs || count < 1 || !sought || strlen (sought) < 1)
     { return -1; }

   for(; ii < count; ii++) {
      if (sptrs[ii] && strcmp (sptrs[ii], sought) == 0)
	{ return ii; }
   }
   return -1;
}

/* c---------------------------------------------------------------------- */

char *sii_glom_tokens ( char *str, char **sptrs, int count, char *joint )
{
   /* tokens with a NULL pointer or zero length will be omitted
    */
   int ii = 0;

   if (!str || !sptrs || count < 1 )
     { return NULL; }

   str[0] = '\0';

   for(; ii < count; ii++ ) {
      if (sptrs[ii] && strlen (sptrs[ii])) {
	 strcat (str, sptrs[ii]);
	 if (joint && strlen (joint))
	   { strcat (str, joint); }
      }
   }
   return str;
}

/* c---------------------------------------------------------------------- */

gchar *sii_set_gtktext_data (GSList *list)
{
   
}

/* c---------------------------------------------------------------------- */

gchar *sii_generic_gslist_insert ( gpointer name, gpointer data )
{
   gen_gslist = g_slist_insert_sorted
     ( gen_gslist, name, (GCompareFunc)strcmp );

   return( FALSE );		/* to continue a tree traverse */
}

/* c---------------------------------------------------------------------- */

void sii_set_widget_frame_num ( GtkWidget *w, gpointer data )
{
   guint frame_num = GPOINTER_TO_UINT (data);
   gtk_object_set_data (GTK_OBJECT(w),
			"frame_num",
			(gpointer)frame_num);
}

/* c---------------------------------------------------------------------- */

gboolean sii_str_values ( gchar *line, guint nvals, gfloat *f1
			 , gfloat *f2 )
{
   gchar str[128], *sptrs[16];
   gint nt;

   strcpy (str, line);
   nt = dd_tokenz (str, sptrs, " \t\n," );
   if( nt < nvals )
     { return FALSE; }

   nt = sscanf ( sptrs[0], "%f", f1 );
   if( nt != 1 )
     { return FALSE; }

   if( nvals == 1 )
     { return TRUE; }

   nt = sscanf ( sptrs[1], "%f", f2 );
   if( nt != 1 )
     { return FALSE; }

   return TRUE;
}

/* c---------------------------------------------------------------------- */

void sii_nullify_widget_cb (GtkWidget *widget, gpointer data)
{
  guint num = GPOINTER_TO_UINT (data);
  guint frame_num, task, window_id;

  frame_num = num/TASK_MODULO;
  window_id = num % TASK_MODULO;

  frame_configs[frame_num]->toplevel_windows[window_id] = NULL;
}

/* c---------------------------------------------------------------------- */

GtkWidget *sii_get_widget_ptr ( guint frame_num, gint window_id )
{
   return frame_configs[frame_num]->toplevel_windows[window_id];
}

/* c---------------------------------------------------------------------- */

void sii_set_widget_ptr ( guint frame_num, gint window_id, GtkWidget *widget )
{
   frame_configs[frame_num]->toplevel_windows[window_id] = widget;
}

/* c---------------------------------------------------------------------- */

gchar *sii_nab_line_from_text (const gchar *txt, guint position )
{
   static gchar line[256];
   gint mm, nn = position;

   if( !txt || strlen( txt ) <= position || position < 0 )
      { return NULL; }
   
   for (; txt[nn] && txt[nn] != '\n'; nn++ ); /* find end of line */
   for (mm = nn; mm > 0 && txt[mm-1] != '\n'; mm-- ); /* find BOL */

   line[0] = '\0';
   strncpy (line, &txt[mm], nn-mm );
   line[nn-mm] = '\0';
   return line;
}

/* c---------------------------------------------------------------------- */

gboolean sii_nab_region_from_text (const gchar *txt, guint position
				   , gint *start, gint *end)
{
   static gchar line[256];
   gint mm, nn = position;

   if( !txt || strlen( txt ) <= position || position < 0 )
      { return FALSE; }
   
   for (; txt[nn] && txt[nn] != '\n'; nn++ ); /* find end of line */
   for (mm = nn; mm > 0 && txt[mm-1] != '\n'; mm-- ); /* find BOL */

   *start = mm; *end = nn;
   return TRUE;
}

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_ll_queue (SiiLinkedList **list, SiiLinkedList *link)
{
   SiiLinkedList *this=link, *previous, *next;

   if (!link)
     { return link; }

   if (!(*list))  {
      *list = link;
      link->previous = link->next = link;
   }
   else {			/* add to end of que */
      link->previous = (*list)->previous; /* (*list)->previous is of queue */
      link->next = NULL;
      (*list)->previous->next = link;
      (*list)->previous = link;
   }
   return link;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_ll_dequeue (SiiLinkedList **list)
{
  SiiLinkedList *this;
  
  if (!(*list))  {
    return NULL;
  }
  /* remove top item  */
  this = *list;
  if (!(*list)->next)		/* last item in queue */
    { *list = NULL; }
  else {
    (*list)->next->previous = (*list)->previous;
    *list = (*list)->next;
  }
  this->previous = this->next = NULL;
  return this;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_ll_push (SiiLinkedList **list, SiiLinkedList *link)
{
   SiiLinkedList *this=link, *previous, *next;

   if (!link)
     { return link; }

   if (!(*list))  {
      *list = link;
      link->previous = link->next = NULL;
   }
   else {
      link->previous = NULL;
      link->next = *list;
      (*list)->previous = link;
      *list = link;
   }
   return link;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_ll_remove (SiiLinkedList **list, SiiLinkedList *link)
{
   if (!link)
     { return NULL; }

   if (link == *list ) {		/* top of list */
      *list = link->next;
      if (link->next)
	{ link->next->previous = NULL; }
   }
   else if (!link->next) {	/* bottom of list */
      link->previous->next = NULL;
   }
   else {
      link->previous->next = link->next;
      link->next->previous = link->previous;
   }
   return link;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_ll_pop (SiiLinkedList **list )
{
   SiiLinkedList *this=sii_ll_remove (list, *list);
   return this;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_init_linked_list (guint length)
{
  SiiLinkedList *this, *first=NULL, *previous = NULL;
  guint cn = 0;

  for( ; cn < length; cn++ ) {
    this = (SiiLinkedList *)g_malloc0 (sizeof (SiiLinkedList));

    if( previous ) {
      previous->next = this;
      this->previous = previous;
    }
    else {
      first = this;
    }
    previous = this;
  }
  return first;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_ll_malloc_item ()
{
   SiiLinkedList *item = (SiiLinkedList *)g_malloc0 (sizeof (SiiLinkedList));;
   return item;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *sii_init_circ_que (guint length)
{
  SiiLinkedList *this, *first=NULL, *previous = NULL;

  first = sii_init_linked_list (length);
  this = previous = first;

  for(; this; previous = this, this = this->next ); /* loop to end */

  if (previous) {
     first->previous = previous;
     previous->next = first;
  }
  return first;
}

/* c---------------------------------------------------------------------- */

SiiLinkedList *insert_clicks_que ( SiiPoint *click )
{
  SiiPoint *click2, *click1, *click0;

  if( !sii_click_que ) {
    sii_click_que = sii_init_circ_que (click_que_size);
  }
  sii_click_que = sii_click_que->next;

  if( !sii_click_que->data ) {
    sii_click_que->data = g_memdup ((gpointer)click, sizeof (*click));
  }
  else {
    g_memmove( sii_click_que->data, (gpointer)click, sizeof (*click));
 }
  click2 = (SiiPoint *)sii_click_que->data;
  click1 = (SiiPoint *)sii_click_que->previous->data;
  click0 = (SiiPoint *)sii_click_que->previous->previous->data;

  if( clicks_in_que < click_que_size )
    { clicks_in_que++; }
  
  return sii_click_que;
}

/* c------------------------------------------------------------------------ */
/* c---------------------------------------------------------------------- */

gchar *
sii_item_delims ()
{
  return " \t\n,";
}

/* c---------------------------------------------------------------------- */

gchar *sii_set_string_from_vals (GString *gs, guint nvals,
				 gfloat f1, gfloat f2,
				 guint precision)
{
   gchar fmt[64], prec[16];

   g_string_truncate (gs, 0);
   sprintf (prec, ".%df", precision);
   strcpy (fmt, "%");
   strcat (fmt, prec);
   if (nvals > 1) {
      strcat (fmt, ", %");
      strcat (fmt, prec);
      g_string_sprintfa (gs, fmt, f1, f2);
   }
   else {
      g_string_sprintfa (gs, fmt, f1);
   }
   return gs->str;
}

/* c---------------------------------------------------------------------- */

void sii_bad_entry_message ( gchar *ee, guint items )
{
   gchar str[256];

   str[0] = '\0';
   if (ee && strlen (ee))
     { strcat( str, ee ); }
   sprintf (str+strlen(str)
	    , "\n Entry could not be interpreted. Requires %d item(s)"
	    , items );
   sii_message (str);
}

/* c---------------------------------------------------------------------- */

GtkWidget *sii_submenu ( gchar *name, GtkWidget *mbar )
{
   GtkWidget *menuitem, *submenu;
   menuitem = gtk_menu_item_new_with_label (name);
   gtk_menu_bar_append (GTK_MENU_BAR (mbar), menuitem );
   submenu = gtk_menu_new ();
   gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
   return submenu;
}

/* c---------------------------------------------------------------------- */

GtkWidget *
sii_submenu_item ( gchar *name, GtkWidget *submenu, guint widget_id
		  , GtkSignalFunc sigfun, guint frame_num )
{
   GtkWidget *next_item;
   guint nn = frame_num * TASK_MODULO + widget_id;

   if (!name) {
      next_item = gtk_menu_item_new ();	/* seperater */
   }
   else {
      next_item = gtk_menu_item_new_with_label (name);
      gtk_signal_connect (GTK_OBJECT(next_item)
			  ,"activate"
			  , (GtkSignalFunc) sigfun
			  , (gpointer)nn);
      gtk_object_set_data (GTK_OBJECT(next_item)
			   , "widget_id", (gpointer)widget_id);
      gtk_object_set_data (GTK_OBJECT(next_item)
			   , "frame_num", (gpointer)frame_num);
   }
   gtk_menu_append (GTK_MENU (submenu), next_item );

   return next_item;
}
/* c---------------------------------------------------------------------- */

GtkWidget *
sii_toggle_submenu_item ( gchar *name, GtkWidget *submenu, guint widget_id
			 , GtkSignalFunc sigfun, guint frame_num
			 , guint radio_item, GSList **radio_group_ptr )
{
   /* be sure and set radio_group = NULL;
    * before creating the first radio button
    */
   GtkWidget *next_item;
   GSList *radio_group;
   guint nn = frame_num * TASK_MODULO + widget_id;

   
   if (radio_item) {
      if (radio_item == 1 )
	{ *radio_group_ptr = NULL; }

      radio_group = *radio_group_ptr;
      next_item = gtk_radio_menu_item_new_with_label (radio_group, name);
      radio_group = gtk_radio_menu_item_group
	(GTK_RADIO_MENU_ITEM (next_item));
      *radio_group_ptr = radio_group;
   }
   else {
      next_item = gtk_check_menu_item_new_with_label (name);
   }
   gtk_signal_connect (GTK_OBJECT(next_item)
		       ,"toggled"
		       , (GtkSignalFunc) sigfun
		       , (gpointer)nn);
  gtk_object_set_data (GTK_OBJECT(next_item)
		       , "widget_id", (gpointer)widget_id);
  gtk_object_set_data (GTK_OBJECT(next_item)
		       , "frame_num", (gpointer)frame_num);

   gtk_menu_append (GTK_MENU (submenu), next_item );
   return next_item;
}

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */

/* c---------------------------------------------------------------------- */


