/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include "color_tables.h"

gboolean Xcolors_flag = no;

/* c---------------------------------------------------------------------- */
GdkColor *normalized_RGB_to_gdk( gfloat f_red, gfloat f_green, gfloat f_blue );

void sii_set_default_colors ();

void put_ascii_color_table( gchar *name, gchar *table );

void set_all_X_named_colors();

GdkColor *sii_set_gtk_color(  gchar *color_name
			       , gfloat f_red, gfloat f_green, gfloat f_blue );

GdkColor *tone_down_gdk_color( gchar *name, gfloat attenuation );


/* c---------------------------------------------------------------------- */
/* c------------------------------------------------------------------------ */
/* c---------------------------------------------------------------------- */

GdkColor *normalized_RGB_to_gdk( gfloat f_red, gfloat f_green, gfloat f_blue )
{
  GdkColor *c;
  gushort gus;
  gulong gul = 0xffff;
  gboolean ok;

  c = (GdkColor *) g_malloc (sizeof (GdkColor));

  c->pixel = 0;
  c->red = (gushort)(f_red * gul );
  c->green = (gushort)(f_green * gul );
  c->blue = (gushort)(f_blue * gul );
				/*
				 */
  ok = gdk_color_alloc (gdk_rgb_get_cmap (), c);
  return( c );
}

/* c---------------------------------------------------------------------- */

GdkColor *tone_down_gdk_color( gchar * name, gfloat atten )
{
				/* 0 < value < 1.0 */
  GdkColor *old, *c;
  gushort gus;
  gulong gul = 0xffff;
  gfloat value = 1. -atten;
  gchar new_name[80];
  gfloat r, g, b;

  old = (GdkColor *)g_hash_table_lookup( colors_hash, (gpointer)name );
  sprintf( new_name, "%s%02d", name, (gint)( atten * 100 +.5 ));

  r = (gfloat)old->red/gul * value;
  g = (gfloat)old->green/gul * value;
  b = (gfloat)old->blue/gul * value;

  c = sii_set_gtk_color( new_name, r, g, b );

  return( c );
}

/* c---------------------------------------------------------------------- */

GdkColor * sii_set_gtk_color( gchar *color_name
			      , gfloat f_red, gfloat f_green, gfloat f_blue )
{
  GSList *cnames;
  gchar *new_name;
  gint nn;

  GdkColor *gcolor = normalized_RGB_to_gdk( f_red, f_green, f_blue );
  GdkColor *gcolor2;

  new_name = (gchar *)g_malloc0( strlen( color_name ) +1 );
  strcpy( new_name, color_name );

  gcolor2 = (GdkColor *)g_tree_lookup( colors_tree, (gpointer)new_name );
  if( gcolor2 ) {
    return (gcolor);
  }
  
  if( Xcolors_flag ) {
    Xcolor_names = g_slist_insert_sorted
      ( Xcolor_names, new_name, (GCompareFunc)strcmp );
  }
  else {
    color_names = g_slist_insert_sorted
      ( color_names, new_name, (GCompareFunc)strcmp );
  }
  if( strstr( new_name, "seagreen" ))
    { nn = 0; }

  g_hash_table_insert( colors_hash, (gpointer)new_name, (gpointer)gcolor );
  g_tree_insert( colors_tree, (gpointer)new_name, (gpointer)gcolor );

  nn = g_tree_nnodes (colors_tree);
  gcolor2 = (GdkColor *)g_hash_table_lookup( colors_hash, (gpointer)new_name );
				/*
				 */
  return (gcolor);
}

/* c---------------------------------------------------------------------- */

gboolean sii_print_str( gpointer a_str, gpointer nada )
{
  printf( "It's: %s\n", (gchar *)a_str );
  return( FALSE );		/* to continue a tree traverse */
}

/* c---------------------------------------------------------------------- */

void sii_print_hash_str( gpointer key, gpointer value, gpointer user_data )
{
  printf( "It's: %s\n", (gchar *)key );
}

/* c---------------------------------------------------------------------- */

void sii_set_default_colors ()
{
  GdkColor *gcolor;
  GdkColor *gcolor2;
  gfloat norm = 255;
  gint ii, mark = 0, nn;
  GSList *sorted_colors = NULL;
  GdkColormap *cmap = gdk_colormap_get_system ();

  gcolor = sii_set_gtk_color( "red",   	 1., 0., 0. );
  gcolor = tone_down_gdk_color( "red", .1 );
  gcolor = tone_down_gdk_color( "red", .2 );
  gcolor = tone_down_gdk_color( "red", .3 );

  gcolor = sii_set_gtk_color( "green", 	 0., 1., 0. );
  gcolor = tone_down_gdk_color( "green", .1 );
  gcolor = tone_down_gdk_color( "green", .2 );
  gcolor = tone_down_gdk_color( "green", .3 );
  gcolor = tone_down_gdk_color( "green", .4 );
  gcolor = tone_down_gdk_color( "green", .5 );

  gcolor = sii_set_gtk_color( "blue",  	 0., 0., 1. );
  gcolor = tone_down_gdk_color( "blue", .1 );
  gcolor = tone_down_gdk_color( "blue", .2 );
  gcolor = tone_down_gdk_color( "blue", .3 );
  gcolor = tone_down_gdk_color( "blue", .4 );

  gcolor = sii_set_gtk_color( "yellow",	 1., 1., 0. );
  gcolor = tone_down_gdk_color( "yellow", .1 );
  gcolor = tone_down_gdk_color( "yellow", .2 );

  gcolor = sii_set_gtk_color( "cyan",  	 0., 1., 1. );
				/* default grid color */
  gcolor = tone_down_gdk_color( "cyan", .1 );
  gcolor = tone_down_gdk_color( "cyan", .25 );
  gcolor = tone_down_gdk_color( "cyan", .5 );

  gcolor = sii_set_gtk_color( "magenta", 1., 0., 1. );
  gcolor = tone_down_gdk_color( "magenta", .1 );
  gcolor = tone_down_gdk_color( "magenta", .25 );

  gcolor = sii_set_gtk_color( "black", 0.006, 0.006, 0.006 );
  gcolor = sii_set_gtk_color( "white", 1., 1., 1. );
  gcolor = tone_down_gdk_color( "white", .06 );
  gcolor = tone_down_gdk_color( "white", .1 );
  gcolor = tone_down_gdk_color( "white", .25 );
  gcolor = tone_down_gdk_color( "white", .44 );
  gcolor = tone_down_gdk_color( "white", .66);

  gcolor = sii_set_gtk_color( "chocolate", .67, .25, .25 );
  gcolor = sii_set_gtk_color( "chocolate", .64, .24, .24 );
  gcolor = sii_set_gtk_color( "coffee", .3, .1, .1 );
  gcolor = sii_set_gtk_color( "orange", 1., .64, .24 );

				/* background */
  gcolor = sii_set_gtk_color( "midnightblue", 25/norm,25/norm,112/norm );
				/* missing data */
  gcolor = sii_set_gtk_color( "darkslateblue", 71/norm,61/norm,139/norm );

  gcolor = sii_set_gtk_color( "gold", 255/norm,215/norm,0 );
  gcolor = sii_set_gtk_color( "thistle", 216/norm, 191/norm, 216/norm );
  gcolor = sii_set_gtk_color( "wheat", 245/norm, 222/norm, 179/norm );
  gcolor = sii_set_gtk_color( "hotpink", 1., .4, .7 );

  set_all_X_named_colors();
				/*
  g_hash_table_foreach ( colors_hash, sii_print_hash_str, 0 );
				 */
  ii = 0;
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"white" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"red" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"green" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"blue" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"magenta" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"cyan" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"orange" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"chocolate" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"wheat" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"seagreen" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"lavender" );
  frame_border_colors[ii++] = (GdkColor *)g_hash_table_lookup
    ( colors_hash, (gpointer)"salmon" );
				/* should be a 12 by now */

  /* set up a tree of the ascii versions of various color tables */

# ifdef obsolete
  put_ascii_color_table( "carbone17", carbone17 );
  put_ascii_color_table( "rrate11", rrate11 );
  put_ascii_color_table( "bluebrown10", bluebrown10 );
  put_ascii_color_table( "theodore16", theodore16 );
  put_ascii_color_table( "ewilson17", ewilson17 );
  put_ascii_color_table( "wild25", wild25 );
  put_ascii_color_table( "scook18", scook18 );
  put_ascii_color_table( "pd17", pd17 );
  put_ascii_color_table( "gray5", gray5 );
  put_ascii_color_table( "gray9", gray9 );
  put_ascii_color_table( "sym_gray12", sym_gray12 );
  put_ascii_color_table( "carbone42", carbone42 );
# else
  nn = sizeof (carbone17)/sizeof (char *);
  sii_param_add_ctbl (carbone17, nn);
  nn = sizeof (carbone11)/sizeof (char *);
  sii_param_add_ctbl (carbone11, nn);
  nn = sizeof (rrate11)/sizeof (char *);
  sii_param_add_ctbl (rrate11, nn);
  nn = sizeof (bluebrown10)/sizeof (char *);
  sii_param_add_ctbl (bluebrown10, nn);
  nn = sizeof (bluebrown11)/sizeof (char *);
  sii_param_add_ctbl (bluebrown11, nn);
  nn = sizeof (theodore16)/sizeof (char *);
  sii_param_add_ctbl (theodore16, nn);
  nn = sizeof (ewilson17)/sizeof (char *);
  sii_param_add_ctbl (ewilson17, nn);
  nn = sizeof (wild25)/sizeof (char *);
  sii_param_add_ctbl (wild25, nn);
  nn = sizeof (scook18)/sizeof (char *);
  sii_param_add_ctbl (scook18, nn);
  nn = sizeof (pd17)/sizeof (char *);
  sii_param_add_ctbl (pd17, nn);
  nn = sizeof (gray5)/sizeof (char *);
  sii_param_add_ctbl (gray5, nn);
  nn = sizeof (gray9)/sizeof (char *);
  sii_param_add_ctbl (gray9, nn);
  nn = sizeof (sym_gray12)/sizeof (char *);
  sii_param_add_ctbl (sym_gray12, nn);
  nn = sizeof (carbone42)/sizeof (char *);
  sii_param_add_ctbl (carbone42, nn);
# endif

  /*
  g_tree_traverse ( ascii_color_tables
		   , (gpointer)sii_print_str, G_IN_ORDER, 0 );
  */		   
  /* remember to put a "return (FALSE); in the function
   * if you want it to traverse the complete tree
   */
				/*
  g_slist_foreach ( color_table_names, (GFunc)sii_print_str, 0 );
  g_slist_foreach ( color_names, (GFunc)sii_print_str, 0 );
  g_slist_foreach ( color_names, (GFunc)sii_print_str, 0 );
  printf( "\nhey\n\n" );
  g_slist_foreach ( Xcolor_names, (GFunc)sii_print_str, 0 );
				 */
  mark = 0;

# ifdef notyet

  gcolor = sii_set_gtk_color( "",  );
  gcolor = sii_set_gtk_color( "", ., ., . );
  put_ascii_color_table( "",  );
# endif
}

/* c---------------------------------------------------------------------- */

void set_all_X_named_colors()
{
   char *aa, *bb, *cc;
   char str[80], *sptrs[16], name[32];
   int jj, nn, nt, count = 0;
   float r,g,b;
   GdkColor *gcolor;
   const gchar **cptrs = all_X_named_colors;

   Xcolors_flag = yes;
   nn = sizeof (all_X_named_colors)/sizeof (char *);

   for( ; nn-- ; cptrs++) {
      strcpy (str, *cptrs);
      nt = dd_tokens( str, sptrs );
      if( nt < 4 )
	{ break; }

      if( !strstr( str, "grey" ) ) { /* don't use grey */
	count++;
	jj = sscanf ( sptrs[1], "%f", &r );
	jj = sscanf ( sptrs[2], "%f", &g );
	jj = sscanf ( sptrs[3], "%f", &b );
	
	gcolor = sii_set_gtk_color( sptrs[0], r, g, b );
      }
   }
   Xcolors_flag = no;
}

/* c---------------------------------------------------------------------- */

void put_ascii_color_table (gchar *name, gchar *table)
{
   int nn;
   gchar *new_name = name, *aa;

   if (!(aa = g_tree_lookup( ascii_color_tables, (gpointer)name ))) {
     new_name = (gchar *)g_malloc0 (strlen (name) +1);
     strcpy( new_name, name );
     color_table_names = g_slist_insert_sorted
       ( color_table_names, new_name, (GCompareFunc)strcmp );
   }
   else {
      /* if the key already exists, it will point to the new table */
      g_free (aa);
   }
   g_tree_insert( ascii_color_tables, (gpointer)new_name, (gpointer)table );
   nn = g_tree_nnodes (ascii_color_tables);
				/*
   aa = (gchar *)g_tree_lookup( ascii_color_tables, (gpointer)"carbone17" );
				 */
}

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */



