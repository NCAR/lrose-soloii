/* 	$Id$	 */


#ifndef SOLO_WINDOW_STRUCTS_H

#define SOLO_WINDOW_STRUCTS_H


/* FIXME! X dependencies to be removed */

# include <sii_image.h>

typedef struct {
  int pixel;
  int red;
  int green;
  int blue;
}XXColor;

typedef struct {
  unsigned long pixel;
  unsigned short red;
  unsigned short green;
  unsigned short blue;
}XColor;

typedef struct {
  int bits_per_pixel;
  int bytes_per_line;
  int byte_order;
  int width;
  void *data;
} XImage;

typedef int Window;

/* end FIXME */

# define          SOLO_MAX_WINDOWS 12
# define            SOLO_UI_WINDOW SOLO_MAX_WINDOWS
# define        SOLO_TOTAL_WINDOWS SOLO_MAX_WINDOWS+1
# define           MAX_EXM_COLUMNS 48
# define             MAX_EXM_CHARS 256

# define              SOLO_NO_GRID 0
# define        SOLO_RANGE_AZIMUTH 0x1
# define            SOLO_TIC_MARKS 0x2

# define         SOLO_BAD_DIR_NAME 1
# define      SOLO_BAD_COLOR_TABLE 2

# define         SIZEOF_COLOR_NAMES 32
# define                        K64 65536
# define                        K32 32768
# define                          K 1024

/* frame configs
 */
# define            SMALL_RECTANGLE 0
# define          DOUBLE_WIDE_FRAME 1    
# define               SQUARE_FRAME 2
# define          LARGE_SLIDE_FRAME 3
# define         LARGE_SQUARE_FRAME 4
# define    LARGE_HALF_HEIGHT_FRAME 5
# define     LARGE_HALF_HEIGHT_REPL 6

# define SOLO_IDS "SPAL,SPTL,SSFI,SRDN,SCPM,SWVI,PISP,"

# ifndef DD_DEFINES
# define YES 1
# define NO 0
# define RESET_IN |
# define SET_IN &
# define INC_NDX(i,que_size) ( ((i)+1) % (que_size) )
# define DEC_NDX(i,que_size) ( ((i)+(que_size)-1) % (que_size) )
# define PIOVR2 1.570796327
# define PI 3.141592654
# define SQ(x) ((x)*(x))
# define TWOPI 6.283185307
# define DD_SCALE(x,scale,offset) ((x)*(scale)+(offset)+.5)
# define DD_UNSCALE(x,rcp_scale,offset) (((x)-(offset))*(rcp_scale))

# define      DD_8_BITS 1
# define     DD_16_BITS 2

# define CART_ANGLE(x) ((double)90.-(x))
# define RADIANS(x) ((x)*0.017453292)
# define DEGREES(x) ((x)*57.29577951)

# define M_TO_KM(x) ((x) * .001)
# define KM_TO_M(x) ((x) * 1000.)
# define MAX_PARMS 64

# endif


# ifndef NULL
# define NULL 0
# endif

# define     SOLO_LOCAL_POSITIONING 0
# define     SOLO_FIXED_POSITIONING 1
# define    SOLO_LINKED_POSITIONING 2


# include <gtk/gtk.h>
# include "sii_widget_content_structs.h"
# include "sii_enums.h"
# include "./point_in_space.h"

/* c------------------------------------------------------------------------ */

struct solo_window_ptrs *solo_return_wwptr (int frame_num);
struct solo_perusal_info *solo_return_winfo_ptr ();

/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */

struct solo_sweep_file {
    char name_struct[4];	/* "SSFI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    double start_time;
    float fixed_angle;
    long linked_windows[SOLO_MAX_WINDOWS];
    long sweep_count;
    long sweep_skip;

    long time_stamp;
    long latest_version;
    long input_fmt;
    long io_type;
    long millisecond;
    long radar_type;
    long version_num;
    char directory_name[128];
    char file_name[128];
    char scan_type[16];
    char radar_name[16];
    long radar_num;
    long sweep_keep_count;
    double stop_time;
};
/* c------------------------------------------------------------------------ */

struct solo_plot_lock {
    char name_struct[4];	/* "SPTL" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[SOLO_MAX_WINDOWS];
};
/* c------------------------------------------------------------------------ */

struct solo_parameter_info {
    char name_struct[4];	/* "SPMI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[SOLO_MAX_WINDOWS];
    char parameter_name[16];
    char palette_name[16];
};
/* c------------------------------------------------------------------------ */

struct solo_view_info {
    char name_struct[4];	/* "SWVI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long height_in_pix;
    long width_in_pix;

    long auto_clear;
    long auto_grid;
    long frame_config;
    long linked_windows[SOLO_MAX_WINDOWS];
    long type_of_annot;
    long type_of_grid;
    long type_of_plot;

    float angular_fill_deg;
    float angular_fill_pct;
    float az_line_int_deg;
    float az_annot_at_km;
    float rng_ring_int_km;
    float rng_annot_at_deg;
    float grid_line_width_pix;
    float horiz_tic_mark_km;
    float vert_tic_mark_km;
    float radar_altitude_km;
    float border_minor_tic_km;
    float border_major_tic_km;

    float magnification;
    float vertical_mag;

    /* there are also three points in space associtated with view info.
     * they are the radar_location, center_of_view, and a landmark.
     * they reside in the solo_window_ptrs struct.
     */
    long offset_to_pisps;
    long num_pisps;
    char message[48];		/* display length of x and y axes in km. */
    /*
     * "SOL_RAD_LOC" id contains the radar location for this window
     * "SOL_WIN_LOC" id contains the location of the center of this window
     * "SOL_LANDMARK" id contains a landmark for this window
     */
    long type_of_centering;
    float rhi_x_ctr_km;		/* center for rhi plots */
    float rhi_y_ctr_km;		/* center for rhi plots */

    float ts_magnification;	/* time series has its own mag. */
    float ts_ctr_km;		/* center for time-series plots */
    float x_tic_mag;
    float y_tic_mag;

};
/* c------------------------------------------------------------------------ */

struct frame_ctr_info {
    char name_struct[4];	/* "SCTR" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[SOLO_MAX_WINDOWS];
    long centering_options;
    long reference_frame;
};

/* c------------------------------------------------------------------------ */

struct landmark_info {
    char name_struct[4];	/* "SLMK" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[SOLO_MAX_WINDOWS];
    long landmark_options;
    long reference_frame;
};
/* c------------------------------------------------------------------------ */

struct solo_palette {
    char name_struct[4];	/* "SPAL" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;

    float center;
    float increment;
    float emphasis_zone_lower;
    float emphasis_zone_upper;
    float bnd_line_width_pix;
    long num_colors;
    char usual_parms[128];
    char palette_name[16];
    char units[16];
    char color_table_dir[128];
    char color_table_name[128];
    char emphasis_color[SIZEOF_COLOR_NAMES];
    char exceeded_color[SIZEOF_COLOR_NAMES];
    char missing_data_color[SIZEOF_COLOR_NAMES];
    char background_color[SIZEOF_COLOR_NAMES];
    char annotation_color[SIZEOF_COLOR_NAMES];
    char grid_color[SIZEOF_COLOR_NAMES];
    /* boundary info */
    char bnd_color[SIZEOF_COLOR_NAMES];
    char bnd_alert[SIZEOF_COLOR_NAMES];
    char bnd_last_line[SIZEOF_COLOR_NAMES];
    char message[48];		/* display min/max colorbar vals */
};
/* c------------------------------------------------------------------------ */


# ifndef SE_CHANGEZ
# define SE_CHANGEZ
/* c------------------------------------------------------------------------ */

struct se_changez {
    struct se_changez *last;
    struct se_changez *next;
    float f_old_val;
    float f_new_val;

    int cell_num;
    int col_num;
    int field_num;
    int flagged_for_deletion;	/* display "-------" */
    int ray_num;
    int row_num;
    int sweep_num;
    int window_num;
    int typeof_change;
    int second_cell_num;
};
# endif
/* c------------------------------------------------------------------------ */

struct solo_examine_info {
    char name_struct[4];	/* "SXMN" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    float modify_val;
    long whats_selected;	/* 0 for data */
    long scroll_increment;	/* 19 default */
    long ray_num;		/* 0 default */
    long ray_count;		/* 3 or less default */
    long cell_count;		/* 21 default */
    long field_count;		/* 3 or less */
    long at_cell;		/* 0 default */
    long change_count;
    long typeof_change;
    long row_annotation;
    long col_annotation;
    char fields_list[128];
    char display_format[16];	/* 7.2f */
    long sweep_num;
};
/* c------------------------------------------------------------------------ */

struct examine_control {
    int window_num;
    int actual_ray_num;
    int actual_at_cell;
    int actual_num_cells;
    int actual_ray_count;
    int actual_num_fields;
    int actual_field_num[16];
    int num_data_cols;		/* actual_num_rays * actual_num_fields */
    int num_cols;		/* = non_data_col_count + num_data_cols
				 */
    int col_lims[MAX_EXM_COLUMNS];
    char *col_ptrs[MAX_EXM_COLUMNS];

    int bad_click;
    int heading_row_count;
    int non_data_col_count;
    int num_chars_rng_label;
    int num_chars_per_row;
    int num_chars_per_cell;
    int rays_in_sweep;
    int cells_in_ray;
    int ctr_on_click;

    struct se_changez *tmp_change;

    float click_angle;
    float click_range;

    float rotation_angles[MAX_EXM_COLUMNS];
    float bad_data[MAX_EXM_COLUMNS]; /* bad data flag for each field */
    float ac_vel[MAX_EXM_COLUMNS];
    float *data_ptrs[MAX_EXM_COLUMNS];
    float *data_space;
    int max_cells;

    float *ranges;
    int max_rngs;

    char fields_list[128];
    char *fld_ptrs[16];

    char rng_fmt[16];
    char names_header[256];
    char rotang_header[256];
    char data_line[256];
    char actual_data_format[16];
    char del_str[24];
};
/* c------------------------------------------------------------------------ */

# ifndef SOLO_LIST_MGMT
# define SOLO_LIST_MGMT

struct solo_list_mgmt {
    int num_entries;
    int sizeof_entries;
    int max_entries;
    char **list;
};
/* c------------------------------------------------------------------------ */

struct solo_str_mgmt {
    struct solo_str_mgmt *last;
    struct solo_str_mgmt *next;
    char *at;
};
# endif

/* c------------------------------------------------------------------------ */

struct landmark_info0 {
    char name_struct[4];	/* "SLMK" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[6];
    long landmark_options;
    long reference_frame;
};
/* c------------------------------------------------------------------------ */

struct frame_ctr_info0 {
    char name_struct[4];	/* "SCTR" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[6];
    long centering_options;
    long reference_frame;
};

/* c------------------------------------------------------------------------ */

struct solo_generic_window_struct {
    char name_struct[4];
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
};

typedef struct solo_generic_window_struct *GWS_PTR;

/*
 * most of these descriptors here are meant to go in a
 * save solo window info file
 */
/* c------------------------------------------------------------------------ */

struct solo_sweep_file0 {
    char name_struct[4];	/* "SSFI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    double start_time;
    float fixed_angle;
    long linked_windows[6];
    long sweep_count;
    long sweep_skip;

    long time_stamp;
    long latest_version;
    long input_fmt;
    long io_type;
    long millisecond;
    long radar_type;
    long version_num;
    char directory_name[128];
    char file_name[128];
    char scan_type[16];
    char radar_name[16];
    long radar_num;
    long sweep_keep_count;
    double stop_time;
};
/* c------------------------------------------------------------------------ */

struct solo_parameter_info0 {
    char name_struct[4];	/* "SPMI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[6];
    char parameter_name[16];
    char palette_name[16];
};
/* c------------------------------------------------------------------------ */

struct solo_palette_ptr {
    struct solo_palette_ptr *last;
    struct solo_palette_ptr *next;
    struct solo_palette* at;
    long user_ww[SOLO_MAX_WINDOWS];
};
/* c------------------------------------------------------------------------ */

struct solo_radar_name {
    char name_struct[4];	/* "SRDN" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[SOLO_MAX_WINDOWS];
    char radar_name[16];
};
/* c------------------------------------------------------------------------ */

struct solo_radar_name0 {
    char name_struct[4];	/* "SRDN" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[6];
    char radar_name[16];
};
/* c------------------------------------------------------------------------ */

struct solo_view_info0 {
    char name_struct[4];	/* "SWVI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long height_in_pix;
    long width_in_pix;

    long auto_clear;
    long auto_grid;
    long frame_config;
    long linked_windows[6];
    long type_of_annot;
    long type_of_grid;
    long type_of_plot;

    float angular_fill_deg;
    float angular_fill_pct;
    float az_line_int_deg;
    float az_annot_at_km;
    float rng_ring_int_km;
    float rng_annot_at_deg;
    float grid_line_width_pix;
    float horiz_tic_mark_km;
    float vert_tic_mark_km;
    float radar_altitude_km;
    float border_minor_tic_km;
    float border_major_tic_km;

    float magnification;
    float vertical_mag;

    /* there are also three points in space associtated with view info.
     * they are the radar_location, center_of_view, and a landmark.
     * they reside in the solo_window_ptrs struct.
     */
    long offset_to_pisps;
    long num_pisps;
    char message[48];		/* display length of x and y axes in km. */
    /*
     * "SOL_RAD_LOC" id contains the radar location for this window
     * "SOL_WIN_LOC" id contains the location of the center of this window
     * "SOL_LANDMARK" id contains a landmark for this window
     */
    long type_of_centering;
    float rhi_x_ctr_km;		/* center for rhi plots */
    float rhi_y_ctr_km;		/* center for rhi plots */

    float ts_magnification;	/* time series has its own mag. */
    float ts_ctr_km;		/* center for time-series plots */
    float x_tic_mag;
    float y_tic_mag;

};
/* c------------------------------------------------------------------------ */
 
struct ts_ray_info {
    short ray_num;
    short sweep_num;
};
/* c------------------------------------------------------------------------ */
 
struct ts_sweep_info {
    int dir_num;
    int ray_count;
    char directory[128];
    char filename[88];
};
/* c------------------------------------------------------------------------ */
 
struct ts_ray_table {
    int max_ray_entries;
    int max_sweep_entries;
    int max_list_entries;
    int max_sxm_entries;
    int ray_count;
    int sweep_count;
    struct ts_ray_info *tsri_top;
    struct ts_sweep_info *tssi_top;
    struct ts_ray_info **tsri_list;
    struct ts_ray_info *tsri_sxm;
};
/* c------------------------------------------------------------------------ */
 
struct solo_window_ptrs {
    long window_num;
    long time_modified;

    float parameter_scale;
    float parameter_bias;

    long active;
    long changed;
    long ddir_radar_num;
    long file_id;
    long menus_popped_up;
    long no_more_data;
    long num_colors_in_table;
    long parameter_num;
    long parameter_bad_val;
    long prev_time_stamp;
    long selected_sweep_num;

    long annotation_color_num;
    long background_color_num;
    long bnd_alert_num;
    long bnd_color_num;
    long bnd_last_num;
    long emphasis_color_num;
    long exceeded_color_num;
    long grid_color_num;
    long lock_color_num;
    long missing_data_color_num;
    /* c...mark
     */
    struct solo_plot_lock *lock;
    struct solo_sweep_file *sweep;
    struct solo_radar_name *radar;
    struct solo_parameter_info *parameter;
    struct solo_palette *palette;
    struct solo_view_info *view;

    struct point_in_space *radar_location;
    struct point_in_space *center_of_view;
    struct point_in_space *landmark;

    struct solo_field_vals *field_vals;

    struct solo_list_mgmt *list_colors;
    XColor *xcolors;

    unsigned long *data_color_lut;	/* 64K*sizeof(long) */
    short *data_cell_lut;

    char show_file_info[64];
    float uniform_cell_spacing;
    float uniform_cell_one;
    int number_cells;
    unsigned long *cell_colors;
    char top_line[128];
    unsigned char *color_bar;
    float clicked_angle;
    float clicked_range;
    struct point_in_space *pisp_click;
    int lock_num;
    int clicked_frame_num;
    int clicked_list_id;
    int clicked_list_char_num;
    int clicked_button_id;
    int clicked_list_entry_num;
    int xy_plane_horizontal;

    struct solo_window_ptrs *next_lock;
    struct solo_window_ptrs *next_sweep;
    struct solo_window_ptrs *lead_sweep;
    int constrained_rasterz;
    SiiImage *image;
    SiiImage *big_image;
    int big_width_in_pix;
    int big_height_in_pix;
    int pixel_depth;
    int test_mode;

    double boundary_x_offset;	/* meters */
    double boundary_y_offset;	/* meters */
    double landmark_x_offset;	/* meters */
    double landmark_y_offset;	/* meters */

    struct landmark_info *landmark_info;
    struct frame_ctr_info *frame_ctr_info;
    struct point_in_space *clicked_ctr;
    int clicked_ctr_of_frame;
    int sweep_file_modified;
    int scan_mode;

    struct solo_examine_info *examine_info; 
    struct solo_list_mgmt *examine_list;
    int examine_line_len;
    struct se_changez *changez_list;
    int num_changez;
    struct examine_control *examine_control;

    struct solo_list_mgmt *list_field_vals;
    char start_time_text[32];
    char stop_time_text[32];
    char ts_start_time_text[32];
    char ts_stop_time_text[32];
    struct ts_ray_table *tsrt;	/* ray table for time series plots */
    struct ts_ray_info *clicked_tsri;
    int clicked_x_pxl;
    int clicked_y_pxl;

    float color_bar_min_val;
    float color_bar_max_val;
    float emphasis_min;
    float emphasis_max;
    double d_prev_time_stamp;
    double d_sweepfile_time_stamp;
    double d_sweepfile_time_inc;

    int swpfi_filter_set;
    int swpfi_time_sync_set;
    double clicked_time;
    int magic_rng_annot;
    float filter_fxd_ang;
    float filter_tolerance;
    char filter_scan_modes[64];
    int color_bar_location;
    int color_bar_symbols;
};

typedef struct solo_window_ptrs *WW_PTR;

/* c------------------------------------------------------------------------ */

struct solo_perusal_info {
    int active_windows[SOLO_MAX_WINDOWS];

    int work_sheet[SOLO_MAX_WINDOWS][SOLO_MAX_WINDOWS];
    int first_ww_for_sweep[SOLO_MAX_WINDOWS];

    int save_windows;
    struct solo_window_ptrs *solo_windows[SOLO_TOTAL_WINDOWS];

    XColor lock_colors[SOLO_MAX_WINDOWS];

    /* c...mark */
    char winfo_comment[48];
    char winfo_directory[128];
    char winfo_filename[128];
    char ed_info_directory[128];
    char ed_info_filename[128];

    struct solo_list_mgmt *list_sweeps;
    struct solo_list_mgmt *list_radars;
    struct solo_list_mgmt *list_parameters;
    struct solo_list_mgmt *list_color_tables;
    struct solo_list_mgmt *list_palettes;
    struct solo_list_mgmt *list_winfo_files;
    struct solo_list_mgmt *list_ed_info_files;
    struct solo_list_mgmt *list_boundary_files;
    struct solo_list_mgmt *list_lock_colors;

    int theScreen;
    struct solo_list_mgmt *list_a_message;
    struct solo_list_mgmt *list_scratch;
    struct solo_list_mgmt *list_sample_colors;
    struct solo_list_mgmt *list_text;
    int num_locksteps;
    int first_ww_for_lockstep[SOLO_MAX_WINDOWS];
    int sweeps_in_lockstep[SOLO_MAX_WINDOWS];
    struct solo_list_mgmt *list_all_files;
    struct solo_list_mgmt *list_select_files;

    int defeat_nexrad_mode;
};
/* c------------------------------------------------------------------------ */

# define   SFV_GARBAGE 0x1
# define    SFV_CENTER 0x2

struct solo_field_vals {
    struct solo_field_vals *next;
    struct solo_field_vals *last;
    float earth_r;
    float lat;
    float lon;
    float alt;
    float az;
    float el;
    float rot_ang;
    float rng;
    float bad_flag;
    float field_vals[7];
    int num_rays;
    int num_vals;
    int state;
    int items_per_ray;
    int sizeof_item;
    double time;
    char **list;
    float tilt;
    float ranges[7];
    float heading;
    float agl;
};
/* c------------------------------------------------------------------------ */

struct point_in_space_ptrs {
    struct point_in_space_ptrs *last;
    struct point_in_space_ptrs *next;
    struct point_in_space *at;
};
/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */
/* c------------------------------------------------------------------------ */

/* c...mark */

/*
 * zeb colortables are in "/zeb/lib/colortables/"
 * colortable input routines and ui code are in "/zeb/src/dm/"
 *
 * ui_perform(char *cmd); 
 *     e.g. ui_perform("read /rdss/solo/colors/original_solo.colors");
 *
 * see state DMC_COLORTABLE (22) in dm.state plus the following code in dm.c
 *
 *	   case DMC_COLORTABLE:
 *	   	dc_Define (UPTR (cmds[1]));
 *		break;
 * 
 * dc_Define is in dm_color.c as is dc_InTable and dc_ColorValue
 * 
 */


# ifdef FYI

/*
 * for information only--do not use!
 */

/*
 * The internal format of a zeb color table.
 */
# define MAXCT 256		/* Max number of colors			*/
typedef struct color_table
{
	char	ct_name[40];		/* The name of this table	*/
	int	ct_ncolor;		/* The number of colors		*/
	XColor	*ct_colors;		/* The actual color entries	*/
} CTable;

/*
 * Data structure used by color operations
 */
typedef struct {
	unsigned long pixel;
	unsigned short red, green, blue;
	char flags;  /* do_red, do_green, do_blue */
	char pad;
} XColor;

/* c------------------------------------------------------------------------ */
typedef struct _XImage {
    int width, height;          /* size of image */
    int xoffset;                /* number of pixels offset in X direction */
    int format;                 /* XYBitmap, XYPixmap, ZPixmap */
    char *data;                 /* pointer to image data */
    int byte_order;             /* data byte order, LSBFirst, MSBFirst */
    int bitmap_unit;            /* quant. of scanline 8, 16, 32 */
    int bitmap_bit_order;       /* LSBFirst, MSBFirst */
    int bitmap_pad;             /* 8, 16, 32 either XY or ZPixmap */
    int depth;                  /* depth of image */
    int bytes_per_line;         /* accelarator to next line */
    int bits_per_pixel;         /* bits per pixel (ZPixmap) */
    unsigned long red_mask;     /* bits in z arrangment */
    unsigned long green_mask;
    unsigned long blue_mask;
    XPointer obdata;            /* hook for the object routines to hang on */
    struct funcs {              /* image manipulation routines */
        struct _XImage *(*create_image)();
#if NeedFunctionPrototypes
        int (*destroy_image)        (struct _XImage *);
        unsigned long (*get_pixel)  (struct _XImage *, int, int);
        int (*put_pixel)            (struct _XImage *, int, int, unsigned long);
        struct _XImage *(*sub_image)(struct _XImage *, int, int, unsigned int, unsigned 
int);
        int (*add_pixel)            (struct _XImage *, long);
#else
        int (*destroy_image)();
        unsigned long (*get_pixel)();
        int (*put_pixel)();
        struct _XImage *(*sub_image)();
        int (*add_pixel)();
#endif
        } f;
} XImage;


/* c------------------------------------------------------------------------ */

struct point_in_space {
    char name_struct[4];	/* "PISP" */
    long sizeof_struct;
    double time;		/* unix time */
    double earth_radius;
    double latitude;
    double longitude;
    double altitude;
    float roll;
    float pitch;
    float drift;
    float heading;
    float x;
    float y;
    float z;
    float azimuth;
    float elevation;
    float range;
    float rotation_angle;
    float tilt;
    long cell_num;
    long ndx_last;
    long ndx_next;
    long state;			/* which bits are set */
    char id[16];
};

/* c------------------------------------------------------------------------ */
/* set this bit if there is a lat/lon/alt/earthr */
# define   PISP_EARTH   0x00000001

/* set this bit if x,y,z vals relative to lat/lon */
# define   PISP_XYZ     0x00000002

/* set this bit if az,el,rng relative to lat/lon */
# define   PISP_AZELRG  0x00000004

/* set this bit for aircraft postitioning relative to lat/lon
 * rotation angle, tilt, range, roll, pitch, drift, heading
 */
# define   PISP_AIR     0x00000008

# endif



#endif /* ifndef SOLO_WINDOW_STRUCTS_H */



