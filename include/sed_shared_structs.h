/* 	$Id$	 */
# include <point_in_space.h>

# ifndef SED_SHARED_STRUCTS
# define SED_SHARED_STRUCTS
/* c------------------------------------------------------------------------ */

struct solo_click_info {
    int which_widget;		/* widget id */
    int which_list;		/* list id */
    int which_widget_button;
    int which_mouse_button;
    int which_list_entry;
    int which_character;

    char click_string[88];
    char command_buffer[88];

    int frame;
    long x;			/* in meters */
    long y;			/* in meters */
    long z;			/* in meters */

    int second_list_entry;
    long ival0;
    long ival1;
    float f_val0;
    float f_val1;
};
/* c------------------------------------------------------------------------ */

struct swp_file_input_control {
    double start_time;
    double stop_time;
    int frame;			/* from which frame the source
				 * data widget was invoked */

    int version;		/* -1 to catch earliest version
				 * 99999 to catch latest version
				 */
    char radar_name[12];
    char directory[88];
    int radar_num;
    /*
     * items from the source data info widget
     */
    int new_version_flag;	/* non-zero creates a new version
				 * need to be able to toggle on and of
				 * in case it's hit inadvertantly
				 * i.e. need an on/off indicator */
    char directory_text[88];
    char radar_names_text[64];
    char first_sweep_text[64];
    char last_sweep_text[64];
    char version_text[64];
    int clicked_frame;		/* used as a flag (frame number plus one)
				 * nonzero indicates sweep file
				 * information has come from the frame
				 * from which the editor widget was called up
				 * If it's set to zero we are either in batch
				 * mode or the user has modified the sfic
				 * widget */
};
/* c------------------------------------------------------------------------ */

struct sed_command_files {
    char directory_text[128];
    char file_name_text[128];
    char comment_text[88];
    int omit_source_file_info;
};
/* c------------------------------------------------------------------------ */

struct solo_frame_state_files {
    char directory_text[128];
    char file_name_text[128];
    char comment_text[88];
    int omit_sweep_info;
};
/* c------------------------------------------------------------------------ */

# define	  VERT_XY_PLANE 0
# define	  HORZ_XY_PLANE 1
  
# define     BND_POINT_APPENDED 0
# define     BND_POINT_INSERTED 1
# define      BND_POINT_DELETED 2
# define	BND_POINT_MOVED 3
# define	    BND_CLEARED 4
# define	      BND_SHIFT 5
# define	      BND_OTHER 6

# define   BND_FIX_RADAR_INSIDE 0x0001
# define  BND_FIX_RADAR_OUTSIDE 0x0002

struct boundary_header {
    char name_struct[4];	/* "BDHD" */
    long sizeof_struct;
    long time_stamp;
    long offset_to_origin_struct; /* should be a PISP */
    long type_of_info;
    long num_points;
    long offset_to_first_point;
    long sizeof_boundary_point;
    long xy_plane_horizontal;
    long open_boundary;
    long version;
    char radar_name[12];
    char user_id[16];
    long force_inside_outside;

};

struct bnd_point_mgmt {
    struct bnd_point_mgmt *last;
    struct bnd_point_mgmt *next;
    long x;			/* meters */
    long y;			/* meters */
    long z;			/* meters */
    long r;			/* range in meters */

    struct bnd_point_mgmt *last_intxn;
    struct bnd_point_mgmt *next_intxn;

    struct bnd_point_mgmt *x_parent;
    struct bnd_point_mgmt *x_left;
    struct bnd_point_mgmt *x_right;

    struct bnd_point_mgmt *y_parent;
    struct bnd_point_mgmt *y_left;
    struct bnd_point_mgmt *y_right;

    float slope;
    float slope_90;		/* slope of line perpendicular
				 * to this line*/
    float len;

    long x_mid;			/* midpoint of the line */
    long y_mid;			/* midpoint of the line */
    long dy;			/* last->y - this->y */
    long dx;			/* last->x - this->x */
    long rx;			/* intersection with a ray */

    int bnd_num;
    int segment_num;
    int what_happened;
    int which_frame;
    int which_side;
    int mid_level;
    struct point_in_space *pisp;

    long _x;		/* x shifted */
    long _y;		/* y shifted */
    long _z;		/* z shifted */
    /*
     * time series stuff...range and time will be in pisp.
     */
    double t_mid;
    float dt;
    float r_mid;
    float dr;
  
    int screen_x;
    int screen_y;
};

struct one_boundary {
    struct one_boundary *last;
    struct one_boundary *next;

    struct bnd_point_mgmt *top_bpm;
    struct bnd_point_mgmt *x_mids;
    struct bnd_point_mgmt *y_mids;
    struct bnd_point_mgmt *first_intxn;
    struct bnd_point_mgmt *next_segment;
    struct bnd_point_mgmt *last_line;
    struct bnd_point_mgmt *last_point;

    float r0;
    float r1;
    int num_segments;
    int num_intxns;
    int num_points;
    double min_x;			/* meters */
    double max_x;			/* meters */
    double min_y;			/* meters */
    double max_y;			/* meters */
    double min_z;			/* meters */
    double max_z;			/* meters */
    int radar_inside_boundary;
    int open_boundary;
    struct boundary_header *bh;
};

struct boundary_stuff {
    struct one_boundary *first_boundary;

    struct boundary_header *bh;
    struct point_in_space *origin;

    int operate_outside_bnd;	/* zero implies inside the boundary
				 * non-zero implies outside the boundary */

    int num_points;		/* used to indicate the presence or
				 * absence of a boundary(s) */

    int line_thickness;		/* in pixels */
    int sizeof_mod_list;
    int num_mods;
    int last_operation;
    struct bnd_point_mgmt **mod_list;
    struct bnd_point_mgmt *last_line;
    struct bnd_point_mgmt *last_point;

    char directory_text[128];
    char file_name_text[128];
    char comment_text[88];
    char last_boundary_point_text[88];

    int num_boundaries;
    int total_boundaries;
    struct one_boundary *current_boundary;
    int proximity_parameter;	/* the mouse is considered to be within
				 * to proximity of a line if it within
				 * a rectangle centered on the line
				 * whose length is the length
				 * of the line and whose width is two
				 * times the PP unless the line width is
				 * less than the PP. Then the PP is defined
				 * as .666 times the length of the line */
    struct point_in_space *pisp;
    long linked_windows[16];
    int view_bounds;
    int absorbing_boundary;
};
/* c------------------------------------------------------------------------ */
# endif

