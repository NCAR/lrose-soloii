/* 	$Id$	 */


#ifndef SII_WIDGET_CONTENT_STRUCTS_H

#define SII_WIDGET_CONTENT_STRUCTS_H

/* c------------------------------------------------------------------------ */

struct examine_widget_info {
    /*
     */
    int window_num;
    int whats_selected;
    int scroll_increment;
    int ray_num;
    int at_cell;
    int ray_count;
    int cell_count;
    int change_count;
    int typeof_change;
    int row_annotation;
    int col_annotation;

    char fields_list[128];
    char modify_val[16];
    char display_format[16];
};

/* c------------------------------------------------------------------------ */

struct landmark_widget_info {
    int frame_num;
    long linked_windows[SOLO_MAX_WINDOWS];
    int options;
    int reference_frame;
    int changed;
    double latitude;
    double longitude;
    double altitude;
};
/* c------------------------------------------------------------------------ */

struct centering_widget_info {
    int frame_num;
    long linked_windows[SOLO_MAX_WINDOWS]; /* wwptr->frame_ctr_info->linked_windows */
    int options;		/* wwptr->frame_ctr_info->centering_options */
    int reference_frame;	/* wwptr->frame_ctr_info->reference_frame */
    int changed;
    double latitude;		/* wwptr->center_of_view->latitude */
    double longitude;		/* wwptr->center_of_view->longitude */
    double altitude;		/* wwptr->center_of_view->altitude */
    float az_of_ctr;		/* wwptr->center_of_view->azimuth */
    float rng_of_ctr;		/* wwptr->center_of_view->range */
};
/* c------------------------------------------------------------------------ */

struct lock_step_info {
    int frame_num;
    int changed;
    long linked_windows[SOLO_MAX_WINDOWS]; /* wwptr->lock->linked_windows */
};
/* c------------------------------------------------------------------------ */

struct sweep_widget_info {
    int frame_num;
    int sweep_interval;		/* wwptr->sweep->sweep_skip */
    int sweep_num;		/* list element num */
    long linked_windows[SOLO_MAX_WINDOWS]; /* wwptr->sweep->linked_windows */
    long lockstep_links[SOLO_MAX_WINDOWS]; /* wwptr->locks->linked_windows */
    int timesync_flag;
    int timesync_ref_frame;
    long timesync_links[SOLO_MAX_WINDOWS]; 
    char directory_name[128];	/* wwptr->sweep->directory_name */
    char filename[128];		/* wwptr->show_file_info */
    int radar_num;		/* list element num */
    int changed;
    char start_time_text[32];	/* wwptr->start_time_text */
    char stop_time_text[32];	/* wwptr->stop_time_text */
    char radar_name[16];
    int filter_flag;
    float flter_fxd_ang;
    float flter_tolerance;
    char scan_modes[64];
};
/* c------------------------------------------------------------------------ */

struct parameter_widget_info {
    int frame_num;
    int changed;
    long linked_windows[SOLO_MAX_WINDOWS]; /* wwptr->parameter->linked_windows */

    int add_parameter_to_palette; /* yes or no */
    int color_file_num;		/* list element num */
    int palette_num;		/* list element num */
    int parameter_num;		/* list element num */

    float center;		/* wwptr->palette->center */
    float increment;		/* wwptr->palette->increment */
    float emphasis_zone_lower;	/* wwptr->palette->emphasis_zone_lower */
    float emphasis_zone_upper;	/* wwptr->palette->emphasis_zone_upper */
    /*
     * wwptr->palette->message is where we say the number of colors and
     * the min and max values of the colorbar
     */

    char parameter_name[16];	/* wwptr->parameter->parameter_name */
    char palette_name[16];	/* wwptr->palette->palette_name */
    char units[16];		/* wwptr->palette->units */
    /*
     * the information up to here should show up in the main
     * parameter widget. The following information should show up
     * in a seperate color info widget that can be poped up from
     * the parameter widget
     */
    char color_table_dir[128];	/* wwptr->palette->color_table_dir */
    char color_table_name[128];	/* wwptr->palette->color_table_name */
    char emphasis_color[SIZEOF_COLOR_NAMES];/* wwptr->palette->emphasis_color */
    char exceeded_color[SIZEOF_COLOR_NAMES];/* wwptr->palette->exceeded_color */
    char missing_data_color[SIZEOF_COLOR_NAMES];/* wwptr->palette->missing_data_color */
    char background_color[SIZEOF_COLOR_NAMES];/* wwptr->palette->background_color */
    char annotation_color[SIZEOF_COLOR_NAMES];/* wwptr->palette->annotation_color */
    char grid_color[SIZEOF_COLOR_NAMES];/* wwptr->palette->grid_color */
    /* boundary colors
     */
    char bnd_color[SIZEOF_COLOR_NAMES];/* wwptr->palette->bnd_color */
    char bnd_alert[SIZEOF_COLOR_NAMES];/* wwptr->palette->bnd_alert */
    char bnd_last_line[SIZEOF_COLOR_NAMES];/* wwptr->palette->bnd_last_line */
    char min_max_string[48];	/* wwptr->palette->message */

    int color_bar_location;
    int color_bar_symbols;
};
/* c------------------------------------------------------------------------ */

struct view_widget_info {
    int frame_num;
    int changed;
    long linked_windows[SOLO_MAX_WINDOWS]; /* wwptr->view->linked_windows */
    int auto_clear;		/* YES or NO wwptr->view->auto_clear */
    int auto_grid;		/* YES or NO wwptr->view->auto_grid */
    int type_of_centering;
    int type_of_grid;		/* wwptr->view->type_of_grid */
    int type_of_plot;
    int frame_config_num;
    int ctr_from_last_click;	/* YES or NO */
    int width;
    int height;
    int left_attach;
    int right_attach;
    int top_attach;
    int bottom_attach;
    int magic_rng_annot;

    float magnification;	/* wwptr->view->magnification */
    float az_line_int_deg;	/* wwptr->view->az_line_int_deg */
    float rng_ring_int_km;	/* wwptr->view->rng_ring_int_km */
    float horiz_tic_mark_km;	/* wwptr->view->horiz_tic_mark_km */
    float vert_tic_mark_km;	/* wwptr->view->vert_tic_mark_km */
    /*
     * Grid size comes from wwptr->view->message
     */
    float az_annot_at_km;	/* wwptr->view->az_annot_at_km */
    float rng_annot_at_deg;	/* wwptr->view->rng_annot_at_deg */

    float rhi_x_ctr_km;		/* center for rhi plots */
    float rhi_y_ctr_km;		/* center for rhi plots */

    float ts_magnification;	/* time series has its own mag. */
    float ts_ctr_km;		/* center for time-series plots */
    float x_tic_mag;
    float y_tic_mag;
};
/* c------------------------------------------------------------------------ */




# endif 






