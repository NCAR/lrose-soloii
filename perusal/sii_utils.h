/* 	$Id$	 */

# ifndef SII_UTILS_H
# define SII_UTILS_H

void
sii_glom_strings (const gchar **cptrs, int nn, GString *gs);

LinksInfo *
sii_new_links_info ( gchar *name, guint frame_num, guint widget_id
			       , gboolean this_frame_only );
void
show_links_widget (LinksInfo *linkfo);



int
dd_tokens( char *str, char **sptrs );

int
dd_tokenz( char *str, char **sptrs, char *delims );

SiiLinkedList *
insert_clicks_que ( SiiPoint *click );

int
sii_cmd_tmplt_tokens (gchar *str, gchar**sptrs, gint *data_types);

void
sii_bad_entry_message ( gchar *ee, guint items );

gchar *
sii_generic_gslist_insert ( gpointer name, gpointer data );

guint
sii_get_master_seq_num ();

GtkWidget *
sii_get_widget_ptr ( guint frame_num, gint window_id );

char *
sii_glom_tokens ( char *str, char **sptrs, int count, char *joint );

guint
sii_inc_master_seq_num ();

SiiLinkedList *
sii_init_circ_que (guint length);

SiiLinkedList *
sii_init_linked_list (guint length);

SiiLinkedList *
sii_ll_malloc_item ();

SiiLinkedList *
sii_ll_pop (SiiLinkedList **list );

SiiLinkedList *
sii_ll_push (SiiLinkedList **list, SiiLinkedList *link);

SiiLinkedList *
sii_ll_remove (SiiLinkedList **list, SiiLinkedList *link);

void
sii_message (const gchar *message );

gchar *
sii_nab_line_from_text (const gchar *txt, guint position );

gboolean
sii_nab_region_from_text (const gchar *txt, guint position
				   , gint *start, gint *end);
void
sii_nullify_widget_cb (GtkWidget *widget, gpointer data);

gchar *
sii_item_delims ();

gchar *
sii_set_string_from_vals (GString *gs, guint nvals, gfloat f1, gfloat f2,
			  guint precision);
void
sii_set_widget_frame_num ( GtkWidget *w, gpointer data);

void
sii_set_widget_ptr ( guint frame_num, gint window_id, GtkWidget *widget);

int
sii_str_seek ( char **sptrs, int count, char *sought);

gboolean
sii_str_values (gchar *line, guint nvals, gfloat *f1, gfloat *f2);

GtkWidget *
sii_submenu ( gchar *name, GtkWidget *mbar );

GtkWidget *
sii_submenu_item ( gchar *name, GtkWidget *submenu, guint widget_id
		  , GtkSignalFunc sigfun, guint frame_num );

GtkWidget *
sii_toggle_submenu_item ( gchar *name, GtkWidget *submenu, guint widget_id
			 , GtkSignalFunc sigfun, guint frame_num
			 , guint radio_item, GSList **radio_group_ptr );



# endif







