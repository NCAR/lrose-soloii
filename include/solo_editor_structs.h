/* 	$Id$	 */

# define             SE_FRAME  SOLO_MAX_WINDOWS+1
# define         SE_MAX_GATES  2048
# define        SE_MAX_STRING  256
# define   SE_INSIDE_BOUNDARY  1
# define  SE_OUTSIDE_BOUNDARY -1
# define     MAX_SETUP_INPUTS  7

# include <sed_shared_structs.h>
# include <solo_window_structs.h>
# include <point_in_space.h>
# include <uii.h>

/* c------------------------------------------------------------------------ */

struct zmap_points {
    struct zmap_points *next_list;
    struct zmap_points *next;
    double lat;
    double lon;
    float src_val;
    float curr_val;
    float azm;
    float rng;
    float x;
    float y;
    int number;
    char id[48];
    char list_id[16];
};

/* c------------------------------------------------------------------------ */

struct prev_bnd_sets {
    struct prev_bnd_sets *last;
    struct prev_bnd_sets *next;
    int sizeof_set;
    char *at;
};
/* c------------------------------------------------------------------------ */

struct se_pairs {
    int low;
    int high;
    int sum;
    float f_low;
    float f_high;
    float f_sum;
    struct se_pairs *last;
    struct se_pairs *next;
};
/* c------------------------------------------------------------------------ */

struct solo_edit_stuff {

    struct solo_str_mgmt *radar_stack;
    int num_radars;
    struct swp_file_input_control *sfic;
    int se_frame;		/* frame number to use for editing
				 */

				/* BB unfolding */
    float ew_wind;
    float ns_wind;
    float ud_wind;
    int BB_avg_count;
    int BB_init_on;
    int BB_max_neg_folds;
    int BB_max_pos_folds;
    int num_unfolded;
    char BB_unfold_field[12];
				/* despeckling */
    int a_speckle;
    char despeckle_field[12];
    int num_despeckled;
				/* forced unfolding */
    float forced_unfold_center;
    char forced_unfolding_field[12];
				/* simple thresholding */
    float threshold_val;
    int threshold_where;	/* 0 => "below" */
    int num_thresholded;
    char threshold_target[12];
    char threshold_field[12];
				/* defreckling */
    float freckle_threshold;
    int dual_average_offset;
    int freckle_avg_count;
    int num_defreckled;
    char defreckle_field[12];
				/* remove sidelobe ring */
    int sidelobe_ring_gate_count;
    char ac_sidelobe_ring_field[12];
				/* remove ac motion */
    char remove_ac_motion_field[12];

				/* histogram stuff */
# define      H_BIN_MAX 100
# define         H_BINS 0x01
# define        H_AREAS 0x02
# define          H_REG 0x04
# define        H_IRREG 0x08

    double histo_start_time;
    double histo_stop_time;
    struct se_pairs *h_pairs;

    int histo_key;
    int histo_num_bins;
    int histo_output_key;
    int histo_output_lines;

    int low_count;
    int medium_count;
    int high_count;
    int histo_missing_count;
    int num_irreg_bins;
    int *counts_array;		/* low, high and between are tacked
				 * onto the end of this array */
    float *areas_array;		/* same here */
    float low_area;
    float high_area;
    float histo_low;
    float histo_high;
    float histo_increment;
    float histo_missing_area;
    float histo_sum;
    float histo_sum_sq;

    char histogram_field[12];
    char histo_directory[128];
    char histo_filename[88];
    char histo_comment[88];
    struct solo_str_mgmt *h_output;
				/* denotching */
    float notch_max;
    float notch_shear;
    int num_denotched;
    char denotch_field[12];
				/*
				 */
    float optimal_beamwidth;
				/*
				 */
    float add_constant;
    float multiply_constant;
				/*
				 */




    int finish_up;		/* every operation may have a final step
				 * which this flag should trigger
				 */
    int check_command;		/* runs the command(s) through the ui
				 * for syntax checking but does not
				 * execute them
				 */
    int punt;
    int batch_mode;
    int ac_data;

    int bad_flags_initialized;
    int boundary_exists;
    int boundary_use;
    int first_good_gate;
    int gate_diff_interval;
    int initialize_on_fgg;
    int min_bad_count;
    int modified;
    int num_bins_averaged;
    int num_deleted;
    int num_segments;
    int process_ray_count;
    int sweep_count;
    int sweep_ray_count;
    int use_boundary;
    int use_bad_flag_mask;
    int volume_count;
    int volume_ray_count;

    unsigned short *bad_flag_mask;
    unsigned short *boundary_mask;
    unsigned short *condition_mask;

    unsigned short     *all_zeroes_array;
    unsigned short       *all_ones_array;
    unsigned short  *bad_flag_mask_array;
    unsigned short  *boundary_mask_array;

    struct solo_click_info *click;

    struct solo_str_mgmt *cmdz; /* linked list of all commands */

    int num_fer_cmds;
    struct solo_str_mgmt *fer_cmds; /* points to start of FER commands
				     */
    int num_once_cmds;
    struct solo_str_mgmt *once_cmds; /* points to commands executed
				      * only once */
    int num_bnd_cmds;
    struct solo_str_mgmt *boundary_cmds;
    /*
     * c...mark
     */
    int num_setup_input_cmds;
    int setup_input_ndx;	/* set to zero when the process begins */
    struct solo_str_mgmt *setup_inputs[MAX_SETUP_INPUTS];
    struct solo_str_mgmt *scratch1;
    struct solo_str_mgmt *scratch2;
    struct solo_str_mgmt *edit_summary;
    /*
     * list of commands for use by widgets
     */
    struct solo_list_mgmt *all_other_cmds;
    struct solo_list_mgmt *all_fer_cmds;
    struct solo_list_mgmt *edit_log;
    struct solo_list_mgmt *current_cmds;
    struct solo_list_mgmt *cmd_examples;
    struct solo_list_mgmt *help_info;
    struct solo_list_mgmt *list_ed_cmd_files;
    struct solo_list_mgmt *boundary_points;
    struct solo_list_mgmt *list_boundary_files;
    struct solo_list_mgmt *list_winfo_files;
    struct solo_list_mgmt *list_sweeps;
    struct solo_list_mgmt *list_radars;
    struct solo_list_mgmt *radar_data;

    char help_directory[128];
    char last_directory[128];
    struct solo_str_mgmt *last_radar_stack;
    int last_new_version_flag;
    long time_modified;

    char histo_radar_name[12];
    float histo_fixed_angle;

    int surface_gate_shift;
    float surface_shift;

    int popup_frame;
    int focus_frame;
    int manifest_editing;	/* sweep file input control info (sfic)
				 * matches frame + replot after pass */
    int num_prev_bnd_sets;
    struct prev_bnd_sets *last_pbs;
    struct prev_bnd_sets *pbs;

    float nyquist_velocity;

    int max_gates;
    float old_scale[MAX_PARMS];
    float old_bias[MAX_PARMS];
    struct zmap_points *top_zmap_list;
    struct zmap_points *curr_zmap_list;

    int gates_of_shift;
    int deglitch_min_bins;
    int deglitch_radius;
    float deglitch_threshold;

    struct ui_cmd_mgmt * first_input_cmd; /* input commands */
    struct ui_cmd_mgmt * first_oto_cmd;	/* one time only */
    struct ui_cmd_mgmt * first_fer_cmd;	/* for each ray */
    struct ui_cmd_mgmt * all_cmds; /* exhaustive list of commands */

    int check_cmd_flag;
    char error[128];

    char *fer_lines;
    char *oto_lines;
    
    int histo_flush;
    struct solo_list_mgmt *all_templates;
};
/* c------------------------------------------------------------------------ */

