/* 	$Id$	 */

# ifndef SII_EXTERNALS_H
# define SII_EXTERNALS_H


extern GtkWidget *main_window;
extern GtkWidget *main_vbox;
extern GtkWidget *main_event_box;
extern GtkWidget *menubar;

extern GtkWidget *main_table;

GtkAllocation *sii_frame_alloc[maxFrames];
				/*
extern GtkAllocation *main_table_alloc;
extern gdouble main_table_width_change;
extern gdouble main_table_height_change;
				
 */

extern GHashTable *colors_hash;

extern GSList *gen_gslist;	/* generic GSList */

extern GSList *color_names;
extern GSList *Xcolor_names;
extern GSList *color_table_names;
extern GSList *color_palette_names;

extern GTree *colors_tree;
extern GTree *ascii_color_tables;
extern GTree *color_tables;
extern GTree *color_palettes_tree;

extern guint sii_config_count;
extern gfloat sii_config_w_zoom;
extern gfloat sii_config_h_zoom;
extern sii_config_cell **config_cells;

extern guint sii_frame_count;
extern SiiFrameConfig *frame_configs[2*maxFrames +1];
extern GdkColor *frame_border_colors[maxFrames];

extern guint sii_table_widget_width;
extern guint sii_table_widget_height;

extern previous_F_keyed_frame;
extern gint cursor_frame;

extern guint click_count;
extern guint clicks_in_que;

extern click_que_size;
extern SiiLinkedList *sii_click_que;

extern guint sii_seq_num;

extern GdkFont *small_fxd_font;
extern GdkFont *med_fxd_font;
extern GdkFont *big_fxd_font;

extern GdkFont *small_pro_font;
extern GdkFont *med_pro_font;
extern GdkFont *big_pro_font;  

extern gchar *title_tmplt;

extern guint screen_width_mm;
extern guint screen_height_mm;
extern guint screen_width_px;
extern guint screen_height_px;

extern gboolean time_series_mode;

extern GString *gs_complaints;

extern gboolean sii_electric_centering;

extern gboolean nexrad_mode_set;

extern gdouble angular_fill_value;

extern gboolean uniform_frame_shape;

extern batch_mode;

extern GString *gs_image_dir;

extern gfloat batch_threshold_value;

extern GString *batch_threshold_field;

extern gboolean sii_center_crosshairs;

extern GString *gen_gs;

# endif



