/* 	$Id$	 */

# ifndef SOLOII_H
# define SOLOII_H

#include <gtk/gtk.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

# include <sii_image.h>

# define yes TRUE
# define no FALSE

# define MAX_TOP_WINDOWS 64
# define maxFrames 12
# define maxConfigRows 4
# define maxConfigCols 4
# define maxConfigCells maxConfigRows*maxConfigCols
# define maxLinkSets 16
# define CFG_QUE_SIZE 3

# define GDK_COLOR_NORM 0xffff
# define TASK_MODULO 100

# define DEFAULT_SLIDE_HEIGHT 224
# define DEFAULT_VSLIDE_HEIGHT 400

# define DEFAULT_WIDTH 300
# define DEFAULT_HEIGHT 180

enum {
   MAIN_ZERO,
   MAIN_PNG,
   MAIN_GIF,
   MAIN_PRNT,
   MAIN_LAST_ENUM,
}MainOps;

enum {				/* top level window ids for each frame */
   FRAME_ZERO,
   FRAME_MENU,
   FRAME_CLICK_DATA,

   FRAME_SWPFI_MENU,
   FRAME_SWPFI_DISPLAY,
   FRAME_SWPFI_RADAR_LIST,
   FRAME_SWPFI_ZAP_LIST,

   FRAME_PARAMETERS,
   FRAME_PALETTE_LIST,
   FRAME_COLR_TBL_LIST,
   FRAME_COLORS_LIST,
   FRAME_PARAM_ENTRY,

   FRAME_EDITOR,
   FRAME_EDIT_EXAMPLES,
   FRAME_EDIT_EXTEND,
   FRAME_EDIT_FILES,
   FRAME_EDIT_CMDS_HELP,

   FRAME_VIEW,
   FRAME_EXAM_MENU,
   FRAME_EXAM_DISPLAY,
   
   FRAME_SWPFI_LINKS,
   FRAME_PARAM_LINKS,
   FRAME_VIEW_LINKS,
   FRAME_CTR_LINKS,
   FRAME_LMRK_LINKS,
   FRAME_LOCKSTEP_LINKS,

   FRAME_SOLO_INIT,
   FRAME_CONFIGS,
   FRAME_ZOOMS,

   FRAME_HELP,
   FRAME_HELP_SWPFI,
   FRAME_HELP_PARAM,
   FRAME_HELP_VIEW,
   FRAME_HELP_EDIT,
   FRAME_HELP_EXAM,
   FRAME_LAST_ENUM,

} sii_TopLevelWwindowIds;

enum {
   ENTRY_ZERO,
   ENTRY_ONE_FLOAT,
   ENTRY_TWO_FLOATS,
   ENTRY_ONE_INT,
   ENTRY_TWO_INTS,
   ENTRY_TXT_ONLY,
   ENTRY_LAST_ENUM,

} EntryOpts;

enum {
   LI_ZERO,
   LI_SWPFI,
   LI_LOCKSTEP,
   LI_PARAM,
   LI_VIEW,
   LI_CENTER,
   LI_LANDMARK,
   LI_LAST_ENUM,

}LinksIds;

typedef struct _GdkRectangle	  Rectangle;
typedef struct _GdkSegment	  Segment;
typedef struct _GdkPoint	  Point;
# ifdef obsolete
typedef struct _GdkRegion         Region;
# endif

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

typedef struct {
  gdouble lat;
  gdouble lon;
  gfloat  alt;
} siiLatLonAlt;

/* c---------------------------------------------------------------------- */

typedef struct {
   double x;
   double y;
   double z;
} Dxyz;

/* c---------------------------------------------------------------------- */

typedef struct {

  guint frame_num;
  guint button_mask;
  gboolean intxn;
  gint x;			/* screen coordinates */
  gint y;			/* y = 0 => top & y = height => bottom */

  gdouble xx;			/* cartesian coords in km. */
  gdouble yy;
  gdouble zz;

  gdouble dtime;
  gdouble lat;
  gdouble lon;
  gdouble alt;

  gdouble rng_km;
  gdouble az;
  gdouble el;
  gdouble rotation_angle;
  gdouble tilt;

} SiiPoint;

/* c---------------------------------------------------------------------- */

typedef struct _sii_linked_list SiiLinkedList;

struct _sii_linked_list {
  gpointer data;
  gpointer data2;
  SiiLinkedList *previous;
  SiiLinkedList *next;
};

/* c---------------------------------------------------------------------- */

typedef struct {

   guint left_attach;
   guint right_attach;
   guint top_attach;
   guint bottom_attach;

} sii_table_parameters;

/* c---------------------------------------------------------------------- */

typedef struct {
   gchar *name;
   guint frame_num;
   guint widget_id;
   guint lead_frame;
   guint num_links;
   gboolean link_set[maxFrames];
   GtkWidget *table;

} LinksInfo;

/* c---------------------------------------------------------------------- */

typedef struct {		/* frame info */
   
  /* parameters for each frame */
  guint frame_num;
  guint blow_up_frame_num;
  SiiPoint click_loc;
  guint ncols;
  guint nrows;
   
  gboolean blow_up;
  gboolean changed;
  gboolean local_reconfig;
  guint width;
  guint height;
  guint data_width;
  guint data_height;
  guint lock_step;
  guint sync_num;
  gint reconfig_count;
  gint expose_count;
  gint prev_expose_num;
  gint new_frame_count;
  gint colorize_count;
  gint max_lbl_wdt;
  gint big_max_lbl_wdt;
  gint points_in_bnd;

  gint big_reconfig_count;
  gint big_expose_count;
  gint big_colorize_count;

  sii_table_parameters tbl_parms; /* left_attach, right_attach, etc */

  GtkAttachOptions xoptions;
  GtkAttachOptions yoptions;
  guint xpadding;
  guint ypadding;

  GtkStyle *style;
  GdkFont *font;
  GdkFont *big_font;
  guint font_height;
  guint big_font_height;
  GdkGC *gc;
  GdkGC *big_gc;
  GdkGC *border_gc;
  GdkColormap *color_map;
  GtkAccelGroup *accel_group;

  gchar frame_title[128];
   
  GtkWidget *frame;
  GtkWidget *blow_up_window;
  GtkWidget *blow_up_frame;
  GtkWidget *toplevel_windows[MAX_TOP_WINDOWS];
  guint active_frame_id[MAX_TOP_WINDOWS]; /* always frame_num +1 */

  gboolean show_data_widget;
  GString *data_widget_text;
  guint data_widget_show_count;

  gpointer swpfi_data;
  gpointer param_data;
  gpointer view_data;
  gpointer edit_data;
  gpointer exam_data;

  LinksInfo *link_set[maxLinkSets];
  GdkPoint widget_origin[MAX_TOP_WINDOWS];
  GdkPoint frame_origin;
  GdkPoint big_frame_origin;

  GdkRectangle top_label;
  GdkRectangle color_bar_labels;
  GdkRectangle most_recent_expose;
  SiiPoint corner[4];		/* (ul,ur,lr,ll */
  SiiPoint center;
  SiiPoint radar;
  SiiPoint ulc_radar;
  SiiPoint landmark;
  SiiPoint ulc_bnd;		/* corners of boundary rectangle */
  SiiPoint lrc_bnd;

  guint cb_pattern_len;
  guint cb_thickness;
  guchar *cb_pattern;

  SiiImage *image;		/* check each time for match with width x height */
  guint image_size;
  guint image_stride;
  guint max_image_size;
   
  guint big_sync_num;
  guint big_width;
  guint big_height;
  guint big_mag_multiple;

  SiiImage *big_image;
  guint big_image_size;
  guint big_image_stride;
  guint max_big_image_size;
  guint big_image_show_count;
   
  guint cfg_que_ndx;
  guint cfg_width[CFG_QUE_SIZE];
  guint cfg_height[CFG_QUE_SIZE];

} SiiFrameConfig;

/* c---------------------------------------------------------------------- */

typedef struct {

  guint cell_num;
  guint row;
  guint col;

  guint frame_num;
  gboolean processed;

} sii_config_cell;

/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

# endif




