
/* etl lidar structs */

struct  data_file_info {
   short file_number;
   short version_number;
   short lidar_number;
   short data_type;
   short number_data_fields;
   short start_year;
   short start_month;
   short start_day;
   short start_hour;
   short start_minute;
   short start_second;
   short start_hund_sec;
   long time_utc;
   long number_of_records;
   short spare ;
   short platform_type;
   float filler1[4];
};

struct  descriptive_info {
   char exp_name[32];
   char metalog[248];
};

struct  gps_info_parm {
   long gps_time_sec;
   long gps_time_nsec;
   float gps_heading;
   float gps_pitch;
   float gps_roll;
   float gps_mrms;
   float gps_brms;
   long  gps_bad_altitude_flag;
   long filler;
   float gps_stddev_delta_heading;
   float gps_stddev_delta_pitch;
   float gps_stddev_delta_roll;
   float gps_utc_time;
   float gps_latitude;
   float gps_longitude ;
   float gps_altitude;
   float gps_course_over_ground;
   float gps_speed_over_ground;
   float gps_vertical_velocity;
   float gps_pdop;
   float gps_hdop;
   float gps_vdop;
   float gps_tdop;
   float spare[2];
};

struct  lidar_info_parm {
   float rep_rate;
   float operating_wavelength;
   float zero_beat_freq;
   long mode;
   short actual_num_bad_pulses;
   short filler[3];
   float pulse_frequency;
   float qs_but;
   float pzt_volt;
   float pulse_energy;
   float freq_mean;
   float freq_variance;
   long spare[4];
};

struct  ocs_info_parm {
   short ocs_status;
   short data_recording;
   short current_file_no;
   short num_gates;
   short gate_width;
   short range_to_first_gate;
   short num_to_integrate;
   short spare1;
   long current_rec;
   long current_beam_rec;
   short spare2[10];
};

struct  plat_env_parm {
   float static_temp;
   float dewpt_temp;
   float sfc_temp;
   float wind_speed;
   float wind_direction;
   float pressure;
   float spare[4];
};

struct  position_info_parm {
   long utc_time;
   float latitude;
   float longitude;
   float spare1;
   short month;
   short day;
   short year;
   short hour;
   short minute;
   short second;
   short millisecond;
   short spare2[3];
   long microsecond;
};

struct  scan_info_parm {
   float azimuth;
   float elevation;
   short scan_type;
   short spare[5];
};

struct  inu_info_parm {
   long inu_time_sec;
   long inu_time_nsec;
   float inu_pitch;
   float inu_roll;
   float inu_yaw;
   float inu_pitch_rate;
   float inu_roll_rate;
   float inu_yaw_rate;
   float inu_x_velocity;
   float inu_y_velocity;
   float inu_z_velocity;
   float inu_x_accel;
   float inu_y_accel;
   float inu_z_accel;
   float spare1[2];
   short inu_ang_tilt;
   short spare2[3];
};

struct  scan_control_parm {
   short scan_type;
   short scanner;
   short compensation_flag;
   short coord_file_input;
   short profiler_sync_mode;
   short spare1;
   char scan_name[24];
   float beg_azimuth;
   float end_azimuth;
   float azimuth_step;
   float beg_elevation;
   float end_elevation;
   float elevation_step;
   float coord_buf[120];
   long num_steps;
   long spare2[5];
};

struct  noise_block_parm {
   float noise_values[14];
};

struct  dsp_control_parm {
   long num_beams_to_send;
   short data_type;
   short AD_rate;
   short number_lags;
   short num_pulses_avg;
   short gate_width;
   short range_to_first_gate;
   short num_gates_pulse;
   short num_to_integrate;
   short freq_corr;
   short start_pt_freq_corr;
   short num_freq_corr_pts;
   short raw_start_pt;
   short raw_num_samples;
   short enable_bad_pul_reject;
   short pulse_rejection_thres;
   short num_bad_pulses;
   short noise_corr_flag;
   short start_noise_gate ;
   short num_noise_gates;
   short spare[5];
};

struct  site_control_parm {
   short lidar_number;
   short altitude;
   char sitename[16];
   short latitude_deg;
   short latitude_min;
   short latitude_sec;
   short longitude_deg;
   short longitude_min;
   short longitude_sec;
   short spare[4];
};

struct flight_info_parm_mcws {
	unsigned long	end_time;	/* Time in seconds GMT from 1970	*/
	unsigned short	flight_number;	/* Flight number assigned to flight	*/
	unsigned short	run_number;	/* Run number assigned to each run 1-999*/
	unsigned short	start_msec;	/* Time at start in milliseconds	*/
	unsigned short	end_msec;	/* Time at end in milliseconds	*/
};

/*== Gegraphic information =====================================================*/

struct location_info_parm_mcws {
	int 	lat;			/* Latitude in 32 bit angle -180 to 180	*/ 
	int 	lati;			/* Latitude in 32 bit angle -180 to 180	*/ 
	int 	lng;			/* Latitude in 32 bit angle -180 to 180	*/ 
	int 	lngi;			/* Latitude in 32 bit angle -180 to 180	*/ 
	unsigned short p_alitude;	/* Pressure alitude (ft)  at end 	*/
	unsigned short r_alitude;	/* Radar alitude (ft) at end	 	*/
};

/*== Platform atitude and velocity =============================================*/

struct plat_info_parm_mcws {
	short	pitch;			/* Pitch angle, signed, postive nose up */
					/* fixed pt 1/10 degree			*/
	short	pitchi;			/* Pitch angle, signed, postive nose up */
					/* fixed pt 45/8192 degree		*/
	short	roll;			/* Roll angle, signed, + roll right	*/
					/* fixed pt 1/10 degree			*/
	short	rolli;			/* Roll angle, signed, + roll right	*/
					/* fixed pt 45/8192 degree		*/
	unsigned short	true_hdg;	/* True heading 0-359.9 fixed pt 1/10	*/
	unsigned short	true_hdgi;	/* True heading 0-359.95 		*/
					/* fixed pt 45/8192 degree		*/
	short	drift_angle;		/* Drift angle, signed, + wind from left*/ 
					/* fixed pt 1/10 degree			*/
	short	drift_anglei;		/* Drift angle, signed, + wind from left*/ 
					/* fixed pt 45/8192 degree		*/
	unsigned short	gnd_speed;	/* Ground speed 1kt			*/
	unsigned short 	gnd_speedi;	/* Ground speed 0.01 m/s		*/
	unsigned short	track;		/* track angle relative to N 1/10 deg	*/
	unsigned short	true_aspeed;	/* True airspeed measured kt		*/
	unsigned short	true_aspeedc;	/* True airspeed computed kt		*/
	short	u_roll;			/* Rolled that is used fx_pt 1/100	*/
	short	u_pitch;		/* Pitch that is used fx_pt 1/100	*/
	short	u_drift;		/* Drift that is used fx_pt 1/100	*/
	unsigned short	u_trhdg;	/* TRHDG that is used fx_pt 1/100	*/
	short	u_gndspd;		/* Ground speed 0.01 fx_pt m/s		*/
	short 	u_tas;			/* True airspeed .01 fx_pt m/s		*/
	short 	spare;			/* Spare word				*/
};

struct arinc_info_parm_mcws {
	int utc;			/* Time in hr:min:sec int 1.0 sec units	*/
	int gps_lat;			/* Lat in 32 bit angle +-180 degrees	*/
	int gps_lng;			/* Lat in 32 bit angle +-180 degrees	*/
	int gps_alt;			/* Gps altitude in 0.125 ft		*/
	unsigned short gps_trk;		/* Gps track in 0.0055 units		*/
	unsigned short gps_gnspd;	/* Ground speed in knots 0.125 units	*/
};

struct scanner_info_parm_mcws {
	double PITCH;
	double ROLL;
	double DRIFT;
	double THDG;
	double TAS;
	double GNDSPD;
	double LOSD;
	double LOSE;
	double ASIM;
	double ELEV;
	double DEVN;
	double THETA;
	double T1A;
	double T2A;
	double T1B;
	double T2B;
	double DTHETA;
	double THETA1;
	double THETA2;
	double LOMHZ;
	double VSIGN;
	double VCORN;
	double ASCOMP;
	double GSCOMP;
	double OFFSET;
	float  FREQ;
	int	LO20KHZ;
	int	TTPI;
	int	TTPO;
	int	WTIMEO;
	int	WTIMEI;
	int 	bad_pulse;
	int 	lo_read;
};

/*== Platform environment ======================================================*/

struct plat_env_parm_mcws {
	short	static_temp;		/* Static air temp, signed 1/10 C	*/
	short	static_tempc;		/* Static air temp, signed calc 1/10 C	*/
	short 	total_temp;		/* Total air temperature, sigend 1/10 C */
	short	dewpt_ge;		/* Dew/frost point, signed GE 1/10 C	*/
	short	dewpt_egg;		/* Dew/frost point, signed EGG 1/10 C 	*/
	short	sfc_temp;		/* IR surface temperature, signed 1/10 C*/
	short	wind_speed;		/* INS wind speed magnitude	kt	*/
	short	wind_dirn;		/* INS wind speed direction 0 to 3599   */
					/* fixed pt 1/10 degree			*/
};
	
/*== Beam location relative to run =============================================*/

struct beam_loc_parm_mcws {
	unsigned short 	beam_index;	/* Beam position in scan cycle		*/
	short		spare1;		/* East distance from start 20m 	*/
	short		spare2;		/* North distance from start 20m 	*/
	unsigned short	beam_losa;	/* az angle rel north	.010000 deg	*/
	short		beam_lose;	/* el angle rel horz+up	.010000 deg	*/
	short		scan_type;
};

/*== House keeeping info OCS system ============================================*/

struct ocs_info_parm_mcws {
	unsigned short	ocs_status;	/* OCS status Coded			*/
	unsigned short	sources;	/* Souces for g_spd,pitch,roll,etc...	*/
	unsigned short	theta1;		/* Inner wedge position BCD 1/10 deg	*/
	unsigned short	theta2;		/* Outer wedge position BCD 1/10 deg	*/
	unsigned short	wedge_ttp;	/* Wedge time to position in ms		*/
	unsigned short	rfif_lo;	/* RF/IF unit LO freq 1/100 MHZ		*/
	unsigned short  rfif_lo_out;	/* RF/IF unit LO freq 1/100 MHZ as read	*/
	unsigned short	rfif_gain;	/* RF/IF unit gain in units 0 - 1023	*/
	unsigned short 	num_bad_pulses;	/* Number of bad pulses allowed		*/
	unsigned short	num_pulses;	/* Number of pulses to integrate	*/
	unsigned short	num_lags;	/* Number of lags			*/
	unsigned short	num_gates;	/* Number of range gates		*/
	unsigned short	gate_width;	/* Range gate width in meters		*/
	unsigned short	num_posns;	/* Number of beam positions per scan	*/
	unsigned short	bad_data;	/* Indicates a bad data sets		*/
	unsigned short  bad_pulses_set;	/* Number of bad pulses for data set	*/
	unsigned short	off_gscomp;	/* OFFSET + GSCOMP 1/100		*/
	unsigned short	spare[3];	/* Spare				*/
};

/*== scanner inu data ==========================================================*/

struct scan_inu_parm_mcws {
	int   time[10];			/* Inu time 1/100 of second 		*/
	int   lati[10];			/* Latitude in 32bit angle -180 to 180	*/
	int   lngi[10];			/* Latitude in 32bit angle -180 to 180	*/
	short   pitchi[20];             /* Pitch angle, signed, postive nose up */
					/* fixed pt 45/8192 degree              */
	short   rolli[20];              /* Roll angle, signed, + roll right     */
					/* fixed pt 45/8192 degree              */
	unsigned short   true_hdgi[20]; /* True heading 0-359.95                */
					/* fixed pt 45/8192 degree              */
	short   drift_anglei[10];       /* Drift angle, signed, + wind from left*/
					/* fixed pt 45/8192 degree              */
	unsigned short 	gnd_speedi[10];	/* Ground speed 0.01 m/s		*/
};

/*== Dads data =================================================================*/

struct dads_inu_parm_mcws {
	int   time;			/* Inu time second			*/
	int   lat;			/* Latitude in 32bit angle -180 to 180	*/
	int   lng;			/* Latitude in 32bit angle -180 to 180	*/
	unsigned short p_alitude;       /* Pressure alitude (ft)  at end        */
	unsigned short r_alitude;       /* Radar alitude (ft) at end            */
	short   pitch;                  /* Pitch angle, signed, postive nose up */
					/* fixed pt 1/10 degree                 */
	short   roll;                   /* Roll angle, signed, + roll right     */
					/* fixed pt 1/10 degree                 */
	short   true_hdg;               /* True heading 0-359.9 fixed pt 1/10   */
	short   drift_angle;            /* Drift angle, signed, + wind from left*/
					/* fixed pt 1/10 degree                 */
	unsigned short  gnd_speed;      /* Ground speed 1kt                     */
	unsigned short  track;          /* track angle relative to N 1/10 deg   */
	unsigned short  true_aspeed;    /* True airspeed measured kt            */
	unsigned short  true_aspeedc;   /* True airspeed computed kt            */
};

/*== Lidar housekeeping info ===================================================*/

struct lidar_info_parm_mcws {
	int io_pzt;		/* injection oscillator pzt drive voltage */
	int lo_pzt;		/* local oscillator pzt drive voltage */
	int po_pzt;		/* power oscillator pzt drive voltage */
	int io_error;
	int lo_error;
	int po_error;
	int hv_on;		/* High voltage status: 0 = hv off, 1 = on */
	int loops;		/* loop status: 0 = loops off, 1 = on 2 locking */
	int setup;		/* setup file number 				*/
	int amb_temp;		/* ambient temperature				*/
	int gas_temp;		/* gas temperature				*/
	int fan_volt;		/* fan voltage					*/
	float io_det;		/* injection oscillator detctor			*/
	float lo_det;		/* locat oscillator detector			*/
	float po_det;		/* power oscillator detector			*/
	float rep_rate;		/* Laser prf	(hz)				*/
	float laser_power;	/* Laser power (watts)				*/
};


/* end etl lidar structs */

struct mrd_head2 {
    long year;
    long month;
    long day;
    long hour;
    long minute;
    long second;
    long word_7;
    long word_8;
    long word_9;
    long word_10;
    long word_11;
    long word_12;
    long word_13;
    long word_14;
    long word_15;
    long word_16;
    long word_17;
    long word_18;
    long word_19;
    long word_20;
    long word_21;
    long word_22;
    long word_23;
    long word_24;
    long word_25;
    long word_26;
    long word_27;
    long word_28;
    long word_29;
    long word_30;
    long word_31;
    long word_32;
    long word_33;
    long word_34;
    long word_35;
    long word_36;
    long word_37;
    long word_38;
    long word_39;
    long word_40;
};

struct mrd_head {
    short word_1;
    short word_2;
    short word_3;
    short raw_rot_ang_x10;	/* deg. */
    short lat_deg;
    short lat_min;
    short lat_sec_x10;
    short lon_deg;
    short lon_min;
    short lon_sec_x10;
    short altitude;		/* meters */
    short roll_x10;		/* deg. */
    short heading_x10;		/* deg. */
    short drift_x10;		/* deg. */
    short pitch_x10;		/* deg. */
    short raw_tilt_x10;
    short nyq_vel_x10;		/* m/s */
    short julian_date;
    short azimuth_samples;
    short gate_length;		/* m */
    short range_delay;		/* m */
    short ground_speed_x64;	/* m/s */
    short vert_airspeed_x64;	/* m/s */
    char flight_number[8];
    char storm_name[12];
    short wind_dir_x10;		/* deg. */
    short nav_flag;
    short wind_speed_x10;	/* m/s */
    short noise_threshold;
    short corrected_tilt_x10;	/* deg. */
    short num_good_gates;
    short gspd_vel_corr_x10;	/* m/s */
    short sweep_num;
    short max_gates;
    short tilt_corr_flag;
    short altitude_flag;
    char aircraft_id[2];
    short place_holder[80];	/* second header has to be copied in here */
				/* to avoid alignment problems */
};

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
    long linked_windows[12];
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

struct solo_parameter_info {
    char name_struct[4];        /* "SPMI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[12];
    char parameter_name[16];
    char palette_name[16];
};

struct solo_parameter_info0 {
    char name_struct[4];        /* "SPMI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[6];
    char parameter_name[16];
    char palette_name[16];
};

struct solo_sweep_file {
    char name_struct[4];        /* "SSFI" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    double start_time;
    float fixed_angle;
    long linked_windows[12];
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

struct solo_sweep_file0 {
    char name_struct[4];        /* "SSFI" */
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
    char emphasis_color[32];
    char exceeded_color[32];
    char missing_data_color[32];
    char background_color[32];
    char annotation_color[32];
    char grid_color[32];
    /* boundary info */
    char bnd_color[32];
    char bnd_alert[32];
    char bnd_last_line[32];
    char message[48];		/* display min/max colorbar vals */
};
struct solo_plot_lock {
    char name_struct[4];	/* "SPTL" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[12];
};
struct solo_plot_lock0 {
    char name_struct[4];	/* "SPTL" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[6];
};

struct frame_ctr_info {
    char name_struct[4];	/* "SCTR" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[12];
    long centering_options;
    long reference_frame;
};

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

struct landmark_info {
    char name_struct[4];	/* "SLMK" */
    long sizeof_struct;
    long window_num;
    long time_modified;
    long changed;
    long always_popup;

    long linked_windows[12];
    long landmark_options;
    long reference_frame;
};

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

struct rot_table_entry {
    float rotation_angle;
    long offset;
    long size;
};

struct rot_ang_table {
    char name_struct[4];	/* "RKTB" */
    long sizeof_struct;
    float angle2ndx;
    long ndx_que_size;
    long first_key_offset;
    long angle_table_offset;
    long num_rays;
};


struct super_SWIB {
    char name_struct[4];	/* "SSWB" */
    long sizeof_struct;
    /* parameters from the first version */
    long last_used;		/* Unix time */
    long start_time;
    long stop_time;
    long sizeof_file;
    long compression_flag;
    long volume_time_stamp;	/* to reference current volume */
    long num_params;		/* number of parameters */

    /* end of first version parameters */

    char radar_name[8];

    double d_start_time;
    double d_stop_time;
    /*
     * "last_used" is an age off indicator where > 0 implies Unix time
     * of the last access and
     * 0 implies this sweep should not be aged off
     */
    long version_num;
    long num_key_tables;
    long status;
    long place_holder[7];
    long keyt_table[24];
    /*
     * offset and key info to a table containing key value such as
     * the rot. angle and the offset to the corresponding ray
     * in the disk file
     */
};

struct cell_spacing_d {
    char   cell_spacing_des[4]; /* Identifier for a cell spacing descriptor */
				/* (ascii characters CSPD). */
    long   cell_spacing_des_len; /* Cell Spacing descriptor length in bytes. */
    short  num_segments;	/* Number of segments that contain cells of */
				/* equal widths. */
    short  distToFirst;		/* Distance to first gate in meters. */
    short  spacing[6];		/* Width of cells in each segment in m. */
    short  num_cells[6];	/* Number of cells in each segment. */
};				/* End of Structure */

struct lidar_d {    
    char lidar_des[4];          /* Identifier  a lidar descriptor */
                                /* block (four ASCII characters */
                                /* "LIDR"). */
    long lidar_des_length;      /* Length of a lidar descriptor block. */
    char lidar_name[8];         /* Eight character lidar */
                                /* name. (Characters SABL) */
    float lidar_const;          /* Lidar constant. */
    float pulse_energy;         /* Typical pulse energy of the lidar. */
    float peak_power;           /* Typical peak power of the lidar. */
    float pulsewidth;           /* Typical pulse width. */
    float aperature_size;       /* Diameter of the lidar aperature. */
    float field_of_view;        /* Field of view of the receiver. mra; */
    float aperature_eff;        /* Aperature efficiency. */
    float beam_divergence;      /* Beam divergence. */
    short lidar_type;           /* Lidar type: 0) Ground,  1) Airborne */
                                /* fore,  2) Airborne aft,  3) */
                                /* Airborne tail,  4) Airborne lower */
                                /* fuselage,  5) Shipborne. 6) */
                                /* Airborne Fixed */
    short scan_mode;            /* Scan mode:  0) Calibration,  1) PPI */
                                /* (constant elevation),  2) Co-plane, */
                                /* 3) RHI (Constant azimuth),  4) */
                                /* Vertical pointing up,  5) Target */
                                /* (stationary),  6) Manual,  7) Idle */
                                /* (out of control), 8) Surveillance, */
                                /* 9) Vertical sweep, 10) Vertical */
                                /* scan. 11) Vertical pointing down, */
                                /* 12 Horizontal pointing right, 13) */
                                /* Horizontal pointing left */
    float req_rotat_vel;        /* Requested rotational velocity of */
                                /* the scan mirror. */
    float scan_mode_pram0;      /* Scan mode specific parameter #0 */
                                /* (Has different meanings for */
                                /* different scan modes) (Start angle */
                                /* for vertical scanning). */
    float scan_mode_pram1;      /* Scan mode specific parameter #1 */
                                /* (Has different meaning for */
                                /* different scan modes) (Stop angle */
                                /* for vertical scanning). */
    short num_parameter_des;    /* Total number of parameter */
                                /* descriptor blocks for this lidar. */
    short total_number_des;     /* Total number of all descriptor */
                                /* blocks for this lidar. */
    short data_compress;        /* Data compression scheme in use:  0) */
                                /* no data compression, 1) using HRD */
                                /* compression scheme. */
    short data_reduction;       /* Data reduction algorithm in use: */
                                /* 0) None, 1) Between two angles, 2) */
                                /* Between concentric circles. 3) */
                                /* Above and below certain altitudes. */
    float data_red_parm0;       /* Data reduction algorithm specific */
                                /* parameter  #0:  0) Unused, 1) */
                                /* Smallest positive angle in degrees, */
                                /* 2) Inner circle diameter in km,  3) */
                                /* Minimum altitude in km. */
    float data_red_parm1;       /* Data reduction algorithm specific */
                                /* parameter  #1 0) unused, 1) Largest */
                                /* positive angle in degrees, 2) Outer */
                                /* circle diameter in km,  3) Maximum */
                                /* altitude in km. */
    float lidar_longitude;      /* Longitude of airport from which */
                                /* aircraft took off  northern */
                                /* hemisphere is positive, southern */
                                /* negative. */
    float lidar_latitude;       /* Latitude of airport from which */
                                /* aircraft took off eastern */
                                /* hemisphere is positive, western */
                                /* negative. */
    float lidar_altitude;       /* Altitude of airport from which */
                                /* aircraft took off up is positive, */
                                /* above mean sea level. */
    float eff_unamb_vel;        /* Effective unambiguous velocity.. */
    float eff_unamb_range;      /* Effective unambiguous range. */
    int num_wvlen_trans;        /* Number of different wave lengths */
                                /* transmitted. */
    float prf;                  /* Pulse repetition frequency. */
    float wavelength[10];       /* Wavelengths of all the different */
                                /* transmitted light. */
};

struct field_parameter_data {
    char  field_param_data[4];	/* Field parameter data identifier */
				/* (ascii characters FRAD) */
    long field_param_data_len;	/* Length of the field parameter */
				/* data block in bytes */
    long data_sys_status;	/* Status word, bits will be assigned */
                                /*  particular status when needed */
    char  radar_name[8];	/* Name of radar from which this data ray  */
				/* came from */
    float test_pulse_level;	/* Test pulse power level as measured by the */
                                /*  power meter in dbm */
    float test_pulse_dist;	/* Distance from antenna to middle of */
				/* test pulse in km */
    float test_pulse_width;	/* Test pulse width in m  */
    float test_pulse_freq;	/* Test pulse frequency in Ghz */
    short test_pulse_atten;	/* Test pulse attenuation in db */
    short test_pulse_fnum;	/* Frequency number being calibrated */
				/* with the test pulse (what mux on */
				/* timing module is set to) */
    float noise_power;		/* Total estimated noise power in dbm */
    long  ray_count;		/* Data Ray counter For this */
				/* particular type of data ray */
    short first_rec_gate;	/* First recorded gate number (N) */
    short last_rec_gate;	/* Last recorded gate number (M) */
};				/* End of Structure */


struct lidar_parameter_data {
    char lidar_param_data[4];   /* Lidar parameter data identifier */
                                /* (four ASCII characters "LDAT"). */
    long lidar_param_data_len;  /* Length of the lidar parameter data */
                                /* block in bytes. */
    long data_sys_status;       /* Status word, bits will be assigned */
                                /* particular status when needed. */
    char lidar_name[8];         /* Name of lidar from which this data */
                                /* ray came from. Directly corresponds */
                                /* to a name given in one of the lidar */
                                /* descriptors. */
    float pulse_energy[10];     /* Pulse energy for each transmitted */
                                /* wavelength. */
    long ray_count;             /* Data ray counter for this */
                                /* particular type of data ray. */
    float derived_param[10];    /* Parameters derived form the data */
                                /* set in real time.  The definition */
                                /* and units of these parameters are */
                                /* given in the field lidar */
                                /* descriptor. */
    long event;                 /* Event counter is incremented */
                                /* everytime operator hits event */
                                /* button */
    long pmt_voltage;           /* Control voltage to the PMT power */
                                /* supply */
    long apd_voltage;           /* Control voltage to the APD power */
                                /* supply */
    long flashlmp_voltage;      /* Flash lamp power supply voltage */
    long temperatures[10];      /* Value of the monitored temperatures */
                                /* (names in the field lidar info */
                                /* block) */
    short first_rec_gate;       /* First recorded gate number (N). */
    short last_rec_gate;        /* Last recorded gate number (M) */
};


struct rsf_house {
/* 1*/	unsigned short rec_num;		/* log rec# for this beam, MOD 32768 */
/* 2*/	unsigned short field_tape_seq;	/* field tape sequence # =0 */
/* 3*/	unsigned short rec_type;	/* record type; 0 = data */
/* 4*/	unsigned short year;		/* last two digits */
/* 5*/	unsigned short month;
/* 6*/	unsigned short day;
/* 7*/	unsigned short hour;
/* 8*/	unsigned short minute;
/* 9*/	unsigned short second;
/*10*/	unsigned short az_xCF;		/* degrees * CF */
/*11*/	unsigned short el_xCF;		/* degrees * CF */
/*12*/	unsigned short rhozero1;	/* range to leading edge of 1st gate*/
/*13*/	unsigned short rhozero2;	/* = rhozero1 + rhozero2/1000 (in km)*/
/*14*/	unsigned short gs;		/* gate spacing (m) = */
/*15*/	unsigned short num_gates;	/* gates per beam */
/*16*/	unsigned short samples_per_beam;
/*17*/	unsigned short test_pulse_level; /* 0 */
/*18*/	unsigned short atp;		/* average transmitted power   */
/*19*/	unsigned short pulse_width;	/* 1.67 us */
/*20*/	unsigned short prfx10;		/* PRF (Hz * 10), typ. 1235 Hz */
/*21*/	unsigned short wavelength;	/* wavelength (cm * 100) */
/*22*/	unsigned short swp_num;		/* running counter of elevation scans
					 * since last start of operations */
/*23*/	unsigned short sweep_index;	/* identifies the sweep in the volume*/
/*24*/	unsigned short unused1[2];
/*26*/	unsigned short scan_mode;	/* 8 = Surveillance */
/*27*/	unsigned short cw_az_lim;	/* azimuth angle of first dwell */
/*28*/	unsigned short ccw_az_lim;	/* azimuth angle of last dwell */
/*29*/	unsigned short up_elev_lim;	/* 0 */
/*30*/	unsigned short lo_elev_lim;	/* 0 */
/*31*/	unsigned short fixed_ang;	/* the elevation angle from the
					 * scan strategy table */
/*32*/	unsigned short sig_source;	/*  0 = radar */
/*33*/	unsigned short coupler_loss;
/*34*/	unsigned short tp_strt;
/*35*/	unsigned short tp_width;
/*36*/	unsigned short pri_co_bl;
/*37*/	unsigned short scnd_co_bl;
/*38*/	unsigned short tp_atten;
/*39*/	unsigned short sys_gain;
/*40*/	unsigned short fix_tape;
/*41*/	unsigned short tp_freq_off;
/*42*/	unsigned short log_bw;
/*43*/	unsigned short lin_bw;
/*44*/	unsigned short ant_bw;
/*45*/	unsigned short ant_scan_rate;
/*46*/	unsigned short unused2[2];
/*48*/	unsigned short vol_num;		/* running count of full or partial
					 * volume scans since last start of 
					 * operations */
/*49*/	unsigned short clut_filt;
/*50*/	unsigned short polarization;	/*0 = horizontal */
/*51*/	unsigned short prf1;
/*52*/	unsigned short prf2;
/*53*/	unsigned short prf3;
/*54*/	unsigned short prf4;
/*55*/	unsigned short prf5;
/*56*/	unsigned short unused3;
/*57*/	unsigned short rec_num_d32768;	/* record count overflow */
/*58*/	unsigned short altitude;
/*59*/	unsigned short latitude;
/*60*/	unsigned short longitude;
/*61*/	unsigned short transit;		/* 0 = in a scan */
/*62*/	unsigned short ds_id;		/* -1 */
/*63*/	unsigned short rs_id;		/* 0x4d48 - 'MH' */
/*64*/	unsigned short proj_num;	
/*65*/	unsigned short hsk_len;		/* # of words of housekeeping = 100 */
/*66*/	unsigned short log_rec_len;	/* 100 + 4*512 */
/*67a*/	unsigned char  num_lrecs;	/* number of log recs in phys rec */
/*67b*/ unsigned char  lrec_num;	/* number of current logical rec  */
/*68*/	unsigned short num_fields;
/*69-74*/	unsigned short parm_desc1[6];
/*75*/	unsigned short tp_max;
/*76*/	unsigned short tp_min;
/*77*/	unsigned short tp_step;
/*78*/	unsigned short vol_scan_prg;
/*79-90*/
	short cal_info1[12];
/*91*/	unsigned short rnoise_bdcal;	/* words 91-100 are MHR words */
/*92*/	unsigned short rsolar;
/*93*/	unsigned short rgcc;
/*94*/	unsigned short rvtime;
/*95*/	unsigned short rcrec;
/*96*/	unsigned short unused4[4];
/*100*/	unsigned short live_or_sim;	/* LIVE or SIM */
/*101-160*/ unsigned short unused5[60];
/*161-170*/ unsigned short parm_desc2[10];
/*171*/ unsigned short src_test_bus;
/*172*/ unsigned short add_test_bus;
/*173*/ unsigned short half_prf;
/*174*/ unsigned short ptape_unit;
/*175*/ unsigned short stape_unit;
/*176*/ unsigned short word_176;
/*177*/ unsigned short word_177;
/*178*/ unsigned short word_178;
/*179*/ unsigned short cal_attn_step;
/*180*/ unsigned short cal_freq_step;
/*181*/ unsigned short r_sq_offset;
/*182*/ unsigned short refl_thres;
/*183*/ unsigned short shifter_cnts;
/*184*/ unsigned short attn_setting;
/*185*/ unsigned short swp_center;
/*186*/ unsigned short cp2_mode;
/*187*/ unsigned short non_dual_mode;
/*188*/ unsigned short word_188;
/*189-200*/ unsigned short unused6[12];
/*201*/ unsigned short wavelength2;	/* wavelength, secondary system */
/*202*/ unsigned short atp2;		/* average tx pwr, secondary wavlen */
/*203*/ unsigned short pulse_width2;
/*204*/ unsigned short prf_secondary;
/*205*/ unsigned short sys_gain2;
/*206*/ unsigned short log_bw2;
/*207*/ unsigned short ant_bw2;
/*208*/ unsigned short polarization2;
/*209-211*/ unsigned short unused7[3];
/*212-231*/
 	short cal_info2[20];
/*232-240*/ unsigned short unused8[9];
/*241-246*/ unsigned short aircraft_info[6];
/*247*/ unsigned short dual_pol_mode;
/*248*/ unsigned short dual_pol_switch;
/*249-256*/ unsigned short unused9[8];

};


struct volume_d {
    char  volume_des[4];	/* Volume descriptor identifier: ASCII */
				/* characters "VOLD" stand for Volume */
				/* Descriptor. */
    long  volume_des_length;	/* Volume descriptor length in bytes. */
    short format_version;	/* ELDORA/ASTRAEA field tape format */
				/* revision number. */
    short volume_num;		/* Volume Number into current tape. */
    long  maximum_bytes;	/* Maximum number of bytes in any. */
				/* physical record in this volume. */
    char  proj_name[20];		/* Project number or name. */
    short year;			/* Year data taken in years. */
    short month;		/* Month data taken in months. */
    short day;			/* Day data taken in days. */
    short data_set_hour;	/* hour data taken in hours. */
    short data_set_minute;	/* minute data taken in minutes. */
    short data_set_second;	/* second data taken in seconds. */
    char  flight_num[8];	/* Flight number. */
    char  gen_facility[8];	/* identifier of facility that */
				/* generated this recording. */
    short gen_year;		/* year this recording was generated */
				/* in years. */
    short gen_month;		/* month this recording was generated */
				/* in months. */
    short gen_day;		/* day this recording was generated in days. */
    short number_sensor_des;	/* Total Number of sensor descriptors */
				/* that follow. */
}; /* End of Structure */

struct correction_d {
    char correction_des[4];	/* Correction descriptor identifier: ASCII */
				/* characters "CFAC" stand for Volume */
				/* Descriptor. */
    long  correction_des_length; /*Correction  descriptor length in bytes. */
    float azimuth_corr;          /* Correction added to azimuth[deg]*/
    float elevation_corr;        /* Correction added to elevation[deg]*/
    float range_delay_corr;      /* Correction used for range delay[m]*/
    float longitude_corr;        /* Correction added to radar longitude*/
    float latitude_corr;         /* Correction added to radar latitude*/
    float pressure_alt_corr;     /* Correction added to pressure altitude*/
                                 /* (msl)[km]*/
    float radar_alt_corr;        /* Correction added to radar altitude above */
                                 /* ground level(agl) [km]*/
    float ew_gndspd_corr;       /* Correction added to radar platform*/
                                   /*ground speed(E-W)[m/s]*/
    float ns_gndspd_corr;          /* Correction added to radar platform*/
                                   /* ground speed(N-S)[m/s]*/
    float vert_vel_corr;          /* Correction added to radar platform */
                                   /*vertical velocity[m/s]*/
    float heading_corr;            /* Correction added to radar platform */
                                   /* heading [deg])*/
    float roll_corr;               /* Correction added to radar platform*/
                                   /* roll[deg]*/
    float pitch_corr;              /* Correction added to radar platform*/
                                   /* pitch[deg]*/
    float drift_corr;              /* Correction added to radar platform*/
                                   /*drift[deg]*/
    float rot_angle_corr;          /* Corrrection add to radar rotation angle*/
                                   /*[deg]*/
    float tilt_corr;              /* Correction added to radar tilt angle*/

}; /* End of Structure */

struct cell_d {
    char cell_spacing_des[4];	/* Cell descriptor identifier: ASCII */
				/* characters "CELV" stand for cell*/
				/* vector. */
    long  cell_des_len   ;	/* Comment descriptor length in bytes*/
    long  number_cells   ;	/*Number of sample volumes*/
    float dist_cells[1];	/*Distance from the radar to cell*/
				/* n in meters*/

}; /* End of Structure */


struct parameter_d {
    char  parameter_des[4];	/* Parameter Descriptor identifier */
				/* (ascii characters "PARM"). */
    long parameter_des_length;	/* Parameter Descriptor length in */
				/* bytes.*/
    char  parameter_name[8];	/* Name of parameter being described. */
    char  param_description[40]; /* Detailed description of this parameter. */
    char  param_units[8];	/* Units parameter is written in. */
    short interpulse_time;	/* Inter-pulse periods used. bits 0-1 */
				/* = frequencies 1-2. */
    short xmitted_freq;		/* Frequencies used for this */
				/* parameter. */
    float recvr_bandwidth;	/* Effective receiver bandwidth for */
				/* this parameter in MHz.*/
    short pulse_width;		/* Effective pulse width of parameter */
				/* in m. */
    short polarization;		/* Polarization of the radar beam for */
				/* this parameter (0 Horizontal,1 */
				/* Vertical,2 Circular,3 Elliptical) in na. */
    short num_samples;		/* Number of samples used in estimate */
				/* for this parameter. */
    short binary_format;	/* Binary format of radar data. */
    char  threshold_field[8];	/* Name of parameter upon which this */
				/* parameter is thresholded (ascii */
				/* characters NONE if not */
				/* thresholded). */
    float threshold_value;	/* Value of threshold in ? */
    float parameter_scale;	/* Scale factor for parameter. */
    float parameter_bias;	/* Bias factor for parameter. */
    long  bad_data;		/* Bad data flag. */
    
				/* 1995 extension #1 */

    long extension_num;
    char  config_name[8];	/* used to identify this set of
				 * unique radar characteristics */
    long  config_num;
    long offset_to_data;	/* bytes added to the data struct pointer
				 * to point to the first datum whether it's
				 * an RDAT or a QDAT
				 */
    float mks_conversion;
    long num_qnames;		
    char qdata_names[32];	/* each of 4 names occupies 8 characters
				 * of this space
				 * and is blank filled. Each name identifies
				 * some interesting segment of data in a
				 * particular ray for this parameter.
				 */
    long num_criteria;
    char criteria_names[32];	/* each of 4 names occupies 8 characters
				 * and is blank filled. These names identify
				 * a single interesting floating point value
				 * that is associated with a particular ray
				 * for a this parameter. Examples might
				 * be a brightness temperature or
				 * the percentage of cells above or
				 * below a certain value */
    long number_cells;
    float meters_to_first_cell;	/* center! */
    float meters_between_cells;
    float eff_unamb_vel;	/* Effective unambiguous velocity, m/s. */

}; /* End of Structure */


struct sweepinfo_d {
    char sweep_des[4];	      /* Comment descriptor identifier: ASCII */
			      /* characters "SWIB" stand for sweep info */
			      /* block Descriptor. */
    long  sweep_des_length;   /* Sweep  descriptor length in bytes. */
    char  radar_name[8];      /* comment*/
    long  sweep_num;          /*Sweep number from the beginning of the volume*/
    long  num_rays;            /*number of rays recorded in this sweep*/
    float start_angle;         /*true start angle [deg]*/
    float stop_angle;          /*true stop angle  [deg]*/
    float fixed_angle;
    long  filter_flag;

}; /* End of Structure */


struct platform_i {
    char  platform_info[4];	/* Identifier for the aircraft/ship */
				/* parameters block (ascii characters ASIB) */
    long platform_info_length;	/* Length in Bytes of the */
				/* aircraft/ship arameters block */
    float longitude;		/* Antenna Longitude (Eastern */
				/* Hemisphere is positive, West */
				/* negative) in degrees */
    float latitude;		/* Antenna Latitude (Northern */
				/* Hemisphere is positive, South */
				/* Negative) in degrees */
    float altitude_msl;		/* Antenna Altitude above mean sea */
				/* level (MSL) in m */
    float altitude_agl;		/* Antenna Altitude above ground level */
				/* (AGL) in m */
    float ew_velocity;		/* Antenna east-west ground speed */
				/* (towards East is positive) in m/sec */
    float ns_velocity;		/* Antenna north-south ground speed */
				/* (towards North is positive) in m/sec */
    float vert_velocity;	/* Antenna vertical velocity (Up is */
				/* positive) in m/sec */
    float heading;		/* Antenna heading (angle between */
				/* rotodome rotational axis and true */
				/* North, clockwise (looking down) */
				/* positive) in degrees */
    float roll;			/* Roll angle of aircraft tail section */
				/* (Horizontal zero, Positive left wing up) */
				/* in degrees */
    float pitch;		/* Pitch angle of rotodome (Horizontal */
				/* is zero positive front up) in degrees*/
    float drift_angle;		/* Antenna drift Angle. (angle between */
				/* platform true velocity and heading, */
				/* positive is drift more clockwise */
				/* looking down) in degrees */
    float rotation_angle;	/* Angle of the radar beam with */
				/* respect to the airframe (zero is */
				/* along vertical stabilizer, positive */
				/* is clockwise) in deg */
    float tilt;			/* Angle of radar beam and line normal */
				/* to longitudinal axis of aircraft, */
				/* positive is towards nose of */
				/* aircraft) in degrees */
    float ew_horiz_wind;	/* east - west wind velocity at the */
				/* platform (towards East is positive) */
				/* in m/sec */
    float ns_horiz_wind;	/* North - South wind velocity at the */
				/* platform (towards North is */
				/* positive) in m/sec */
    float vert_wind;		/* Vertical wind velocity at the */
				/* platform (up is positive) in m/sec */
    float heading_change;	/* Heading change rate in degrees/second. */
    float pitch_change;		/* Pitch change rate in degrees/second. */
}; /* End of Structure */


struct ray_i {
    char  ray_info[4];		/* Identifier for a data ray info. */
				/* block (ascii characters "RYIB"). */
    long ray_info_length;	/* length of a data ray info block in */
				/* bytes. */
    long  sweep_num;		/* sweep number for this radar. */
    long  julian_day;		/* Guess. */
    short hour;			/* Hour in hours. */
    short minute;		/* Minute in minutes. */
    short second;		/* Second in seconds. */
    short millisecond;		/* Millisecond in milliseconds. */
    float azimuth;		/* Azimuth in degrees. */
    float elevation;		/* Elevation in degrees. */
    float peak_power;		/* Last measured peak transmitted */
				/* power in kw. */
    float true_scan_rate;	/* Actual scan rate in degrees/second. */
    long  ray_status;		/* 0 = normal, 1 = transition, 2 = bad. */
}; /* End of Structure */


struct qparamdata_d {		/* new for 1995 */

    char pdata_desc[4];		/* parameter data descriptor identifier: ASCII
				 * characters "QDAT" for a block that contains
				 * the data plus some supplemental and
				 * identifying information */
    
    long  pdata_length;		/* parameter data descriptor length in bytes.
				 * this represents the size of this header
				 * information plus the data
				 *
				 * for this data block the start of the data
				 * is determined by using "offset_to_data"
				 * in the corresponding parameter descriptor
				 * "struct parameter_d"
				 * the offset is from the beginning of
				 * this descriptor/block
				 */
    
    char  pdata_name[8];	/* name of parameter */

    long  extension_num;
    long  config_num;		/* facilitates indexing into an array
				 * of radar descriptors where the radar
				 * characteristics of each ray and each
				 * parameter might be unique such as phased
				 * array antennas
				 */
    short first_cell[4];
    short num_cells[4];
				/* first cell and num cells demark
				 * some feature in the data and it's
				 * relation to the cell vector
				 * first_cell[n] = 0 implies the first datum
				 * present corresponds to "dist_cells[0]
				 * in "struct cell_d"
				 * for TRMM data this would be the
				 * nominal sample where the cell vector is
				 * at 125 meter resolution instead of 250 meters
				 * and identified segments might be the
				 * rain echo oversample "RAIN_ECH" and the
				 * surface oversample "SURFACE"
				 */
    float criteria_value[4];	/* criteria value associated
				 * with a criteria name
				 * in "struct parameter_d"
				 */
}; /* End of Structure */

struct cbd_header {
    unsigned short message_id;
    unsigned short sizeof_message;
    unsigned short volume_scan_count;
    unsigned short pw0;
    unsigned short peak_xmittd_pwr_kw;
    unsigned short pw1;
    unsigned long pw2;
    float current_elevation;
    float angular_scan_rate;
    unsigned short pri_xe6;	/* pulse repetition interval in microsec. */
    unsigned short pw3;
    unsigned short final_range_sample;
    unsigned short range_samples_per_dwell;
    float current_azimuth;
    float total_noise_power;
    unsigned long time_stamp;
    unsigned short base_data_type;
    unsigned short pw4;
    unsigned short integer_azimuth;
    unsigned short load_shed_final_sample;
};

struct hrd_header {
    /* tape header general information */
    short header_flag;		/* should be a 0 */
    short sizeof_header;
    short tape_num;
    short hd_fmt_ver;
    short word_5;
    short year;		
    short month;	
    short day;		
    short hour;		
    short minute;	
    short second;	
    char LF_menu[16];
    char TA_menu[16];
    char Data_menu[16];
    short word_36;
    short nav_system;		/* 0=ONE,1=INE1,2=INE2 */
    short LU_tape_drive;
    short aircraft_id;		/* 42,43 (0 for ground) */
    char flight_id[8];
    short data_header_len;	/* # 16-bit words */
    short ray_header_len;	/* # 16-bit words */
    short time_zone_offset;	/* minutes ahead of GMT */

    /* Power up test results */
    short word_47;
    short word_48;
    short word_49;
    short word_50;
    short word_51;
    short word_52;
    short word_53;
    short word_54;

    /* Display configuration when header written */
    short word_55;
    short word_56;
    short word_57;
    short word_58;
    short word_59;
    short word_60;
    short word_61;
    short word_62;
    short word_63;
    short word_64;
    short word_65;
    short word_66;
    short word_67;
    short word_68;
    short word_69;
    short word_70;
    short word_71;
    short word_72;
    short word_73;
    short word_74;
    short word_75;
    short word_76;
    short word_77;
    short word_78;
    short word_79;
    short word_80;
    char project_id[8];
    short word_85;
    short word_86;
    short word_87;
    short word_88;
    short word_89;
    short word_90;
    short word_91;
    short word_92;
    short word_93;
    short word_94;
    short word_95;
    short word_96;
    short word_97;
    short word_98;
    short word_99;
    short word_100;

    /* LF Radar Information words 101-400 */
    /* TA Radar Information words 401-700 */

};
/* c...mark */

struct hrd_radar_info {
    short sample_size;
    short DSP_flag;
    /*
     * 0: Range normalization 
     * 1: Doppler channel speckle remover 
     * 2: Log channel speckle remover 
     * 3: Pulse at end of ray 
     * 4: Pulse at beginning of ray 
     * 6: Use AGC (TA only) 
     * 8-9: 0:single PRF, 1:dual PRF 2/3, 2:dual PRF 3/4
     */
    short refl_slope_x4096;	/* dB per A/D count */
    short refl_noise_thr_x16;	/* dB above noise */
    short clutter_cor_thr_x16;	/* signed dB */
    short SQI_thr;		/* same units as DSP */
    short width_power_thr_x16;	/* dBZ */
    short calib_refl_x16;	/* dBZ */
    short AGC_decay_code;
    short dual_PRF_stabil_delay; /* pulses */
    short thr_flags_uncorr_refl;
    short word_112;
    short thr_flags_vel;
    short thr_flags_width;
    short data_mode;		/* 1:processed data, 2:time series */
    short word_116;
    short word_117;
    short word_118;
    short word_119;
    short word_120;
    short word_121;
    short word_122;
    short word_123;
    short word_124;
    short word_125;
    short word_126;
    short word_127;
    short word_128;
    short word_129;
    short word_130;
    short word_131;
    short word_132;
    short word_133;
    short word_134;
    short word_135;
    short word_136;
    short word_137;
    short word_138;
    short word_139;
    short word_140;

    /* Range Mask Configuration */
    short range_b1;		/* km. portion */
    short variable_spacing_flag; /* 1:var, 0:fixed */
    /*
     * if variable gate spacing then the gate spacing is
     *   75 m for the first 256 gates
     *  150 m for the next  128 gates
     *  300 m for the next  128 gates
     */
    short bin_spacing_xe3;
    short num_input_bins;
    short range_avg_state;	/* (1,2,3,4)(undefined in var mode) */
    short b1_adjust_xe4;
    short word_147;
    short word_148;
    short num_output_bins;
    short word_150;

    /* Noise sample information */
    short PRT_noise_sample;
    short range_noise_sample;	/* km. */
    short log_rec_noise_x64;	/* A2D units */
    short I_A2D_offset_x256;	/* A2D units */
    short Q_A2D_offset_x256;	/* A2D units */
    short word_156;
    short word_157;
    short word_158;
    short word_159;
    short word_160;

    /* DSP Diagnostics */
    short word_161;
    short word_162;
    short word_163;
    short word_164;
    short word_165;
    short word_166;
    short word_167;
    short word_168;
    short word_169;
    short word_170;

    /* Miscellaneous Information */
    short word_171;
    short waveln_xe4;		/* m. */
    short pulse_width_xe8;	/* sec. */
    short PRF;
    short word_175;
    short DSS_flag;		/* 0:off, 1:on */
    short trans_recv_number;
    short transmit_power;
    short gain_control_flag;	/* 0:full, 1:STC, 2:AGC */
    short word_180;
    short word_181;
    short word_182;
    short word_183;
    short word_184;
    short word_185;
    short word_186;
    short word_187;
    short word_188;
    short word_189;
    short word_190;

    /* Antenna Scanning Information */
    short scan_mode;
    short sweep_speed_x10;	/* RPM */
    unsigned short tilt_angle;	/* binary angle */
    short sector_center;	/* degrees */
    short sector_width;		/* degrees */
    short word_196;
    short word_197;
    short word_198;
    short word_199;
    short word_200;

    /* Real Time Display Color Configuration */
    short word_201;
    short word_202;
    short word_203;
    short word_204;
    short word_205;
    short word_206;
    short word_207;
    short word_208;
    short word_209;
    short word_210;
    short word_211;
    short word_212;
    short word_213;
    short word_214;
    short word_215;
    short word_216;
    short word_217;
    short word_218;
    short word_219;
    short word_220;
    short word_221;
    short word_222;
    short word_223;
    short word_224;
    short word_225;
    short word_226;
    short word_227;
    short word_228;
    short word_229;
    short word_230;
    short word_231;
    short word_232;
    short word_233;
    short word_234;
    short word_235;
    short word_236;
    short word_237;
    short word_238;
    short word_239;
    short word_240;
    short word_241;
    short word_242;
    short word_243;
    short word_244;
    short word_245;
    short word_246;
    short word_247;
    short word_248;
    short word_249;
    short word_250;
    short word_251;
    short word_252;
    short word_253;
    short word_254;
    short word_255;
    short word_256;
    short word_257;
    short word_258;
    short word_259;
    short word_260;
    short word_261;
    short word_262;
    short word_263;
    short word_264;
    short word_265;
    short word_266;
    short word_267;
    short word_268;
    short word_269;
    short word_270;
    short word_271;
    short word_272;
    short word_273;
    short word_274;
    short word_275;
    short word_276;
    short word_277;
    short word_278;
    short word_279;
    short word_280;
    short word_281;
    short word_282;
    short word_283;
    short word_284;
    short word_285;
    short word_286;
    short word_287;
    short word_288;
    short word_289;
    short word_290;
    short word_291;
    short word_292;
    short word_293;
    short word_294;
    short word_295;
    short word_296;
    short word_297;
    short word_298;
    short word_299;
    short word_300;
    short word_301;
    short word_302;
    short word_303;
    short word_304;
    short word_305;
    short word_306;
    short word_307;
    short word_308;
    short word_309;
    short word_310;
    short word_311;
    short word_312;
    short word_313;
    short word_314;
    short word_315;
    short word_316;
    short word_317;
    short word_318;
    short word_319;
    short word_320;
    short word_321;
    short word_322;
    short word_323;
    short word_324;
    short word_325;
    short word_326;
    short word_327;
    short word_328;
    short word_329;
    short word_330;
    short word_331;
    short word_332;
    short word_333;
    short word_334;
    short word_335;
    short word_336;
    short word_337;
    short word_338;
    short word_339;
    short word_340;
    short word_341;
    short word_342;
    short word_343;
    short word_344;
    short word_345;
    short word_346;
    short word_347;
    short word_348;
    short word_349;
    short word_350;
    short word_351;
    short word_352;
    short word_353;
    short word_354;
    short word_355;
    short word_356;
    short word_357;
    short word_358;
    short word_359;
    short word_360;
    short word_361;
    short word_362;
    short word_363;
    short word_364;
    short word_365;
    short word_366;
    short word_367;
    short word_368;
    short word_369;
    short word_370;
    short word_371;
    short word_372;
    short word_373;
    short word_374;
    short word_375;
    short word_376;
    short word_377;
    short word_378;
    short word_379;
    short word_380;
    short word_381;
    short word_382;
    short word_383;
    short word_384;
    short word_385;
    short word_386;
    short word_387;
    short word_388;
    short word_389;
    short word_390;
    short word_391;
    short word_392;
    short word_393;
    short word_394;
    short word_395;
    short word_396;
    short word_397;
    short word_398;
    short word_399;
    short word_400;
};

/* c...mark */

struct hrd_data_rec_header {
    short data_record_flag;	/* should be a 1 */
    unsigned short sizeof_rec;
    short sweep_num;
    short rec_num;
    char radar_num;		/* 1:LF, 2:TA */
    char rec_num_flag;		/* 1:first, 0:middle, 2:last */
};

struct hrd_ray_header {
    unsigned short sizeof_ray;
    unsigned char code;
    /* bit 15 set: reflectivity */
    /* bit 14 set: velocity */
    /* bit 13 set: width */
    /* bit 12 set: data from TA DSP */
    /* bit 11 set: data from LF DSP */
    /* bit 10 set: time series */
    char year;
    char month;
    char day;
    unsigned char raycode;
    signed char hour;
    short minute;
    short seconds_x100;
    short latitude;	/* (binary angle) */
    short longitude;	/* (binary angle) */
    short altitude_xe3;
    /*
     * executables of the translaters earlier than 1200 Apr.18, 1994
     * will produce files with ns and ew velocities and winds
     * swapped
     */
    short ac_vew_x10;		/* east-west velocity */
    short ac_vns_x10;		/* north-south velocity */
    short ac_vud_x10;		/* vertical velocity */
    short ac_ui_x10;		/* east-west wind */
    short ac_vi_x10;		/* north-south wind */
    short ac_wi_x10;		/* vertical wind */
    short RCU_status;
    short elevation;	/* (binary angle) */
    short azimuth;	/* (binary angle) */
    short ac_pitch;	/* (binary angle) */
    short ac_roll;	/* (binary angle) */
    short ac_drift;	/* (binary angle) */
    short ac_heading;	/* (binary angle) */
};

struct hrd_generic_header {
    short header_id;
    short sizeof_rec;
};


struct prqnu_ray_hdr {
  char            desc[4];
  short           recordlen;
  short           gates,hits;
  short           spad1;
  float           rcvr_pulsewidth,prt,delay; /* delay to first gate */
  char            clutterfilter,timeseries;
  short           tsgate;
  unsigned int    time;      /* seconds since 1970 */
  unsigned short           subsec;    /* fractional seconds (.1) */
  short           spad2;
  float           az,el;
  float           radar_longitude; 
  float           radar_latitude;
  float           radar_altitude;
  float           ew_velocity;
  float           ns_velocity;
  float           vert_velocity;
  char            dataformat;     /* 0 = abp, 1 = abpab (poly), 2 
				     = abpab (dual prt) */
  char            pad1[3];
  float           prt2;
  float           fxd_angle;
  unsigned char   scan_type;
  unsigned char   scan_num;
  unsigned char   vol_num;
  char            pad2;
  unsigned int    ray_count;
  char            transition;
  char            pad3[3];
  float           hxmit_power;    /* on the fly hor power */
  float           vxmit_power;    /* on the fly ver power */
  float           yaw;            /* platform heading in degrees */
  float           pitch;          /* platform pitch in degrees */
  float           roll;           /* platform roll in degrees */
  char            spare[88];
};

struct prq_ray_hdr {
		char            desc[4];
		short           recordlen;
		short           gates,hits;
		float           rcvr_pulsewidth,prt,delay; /* delay to first gate */
		char            clutterfilter,timeseries;
		short           tsgate;
		unsigned int    time;      /* seconds since 1970 */
		unsigned short           subsec;    /* fractional seconds (.1 mS) */
		float           az,el;
		float           radar_longitude; 
		float           radar_lattitude;
		float           radar_altitude;
		float           ew_velocity;
		float           ns_velocity;
		float           vert_velocity;
		char            dataformat;     /* 0 = abp, 1 = abpab (poly), 2 = abpab (dual prt) */
		float           prt2;
		float           fxd_angle;
		unsigned char   scan_type;
		unsigned char   scan_num;
		unsigned char   vol_num;
		unsigned int    ray_count;
		char            transition;
		float           hxmit_power;    /* on the fly hor power */
		float           vxmit_power;    /* on the fly ver power */
                float           yaw;		/* platform heading in degrees */
                float           pitch;	/* platform pitch in degrees */
                float           roll;	/* platform roll in degrees */
                char            spare[88];
		};

struct prqnu_radar_hdr  {
  char    desc[4];
  short   recordlen;
  short   rev;
  short   year;           /* this is also in the dwell as sec from
			     1970 */
  short   spad;
  char    radar_name[8];
  char    polarization;   /* H or V */
  char    pad1[3];
  float   test_pulse_pwr; /* power of test pulse (refered to anten
			     na flange) */
  float   test_pulse_frq; /* test pulse frequency */
  float   frequency;      /* transmit frequency */
  float   peak_power;     /* typical xmit power (at antenna flange
			     ) read from config.rdr file */
  float   noise_figure;
  float   noise_power;    /* for subtracting from data */
  float   receiver_gain;  /* hor chan gain from antenna flange to 
			     VIRAQ input */
  float   data_sys_sat;   /* VIRAQ input power required for full scale */
  float   antenna_gain;
  float   horz_beam_width;
  float   vert_beam_width;
  float   xmit_pulsewidth; /* transmitted pulse width */
  float   rconst;         /* radar constant */
  float   phaseoffset;    /* offset for phi dp */
  float   vreceiver_gain; /* ver chan gain from antenna flange to 
			     VIRAQ */
  float   vtest_pulse_pwr; /* ver test pulse power refered to antenna flange */
  float   vantenna_gain;
  float   misc[6];        /* 9 more misc floats */
  char    text[960];
};

struct prq_radar_hdr  {
		char    desc[4];
		short   recordlen;
		short   rev;
		short   year;           /* this is also in the dwell as sec from 1970 */
		char    radar_name[8];
		char    polarization;   /* H or V */
		float   test_pulse_pwr; /* power of test pulse (refered to antenna flange) */
		float   test_pulse_frq; /* test pulse frequency */
		float   frequency;      /* transmit frequency */
		float   peak_power;     /* typical xmit power (at antenna flange) read from config.rdr file */
		float   noise_figure;
		float   noise_power;    /* for subtracting from data */
		float   receiver_gain;  /* hor chan gain from antenna flange to VIRAQ input */
		float   data_sys_sat;   /* VIRAQ input power required for full scale */
		float   antenna_gain;
		float   horz_beam_width;
		float   vert_beam_width;
		float   xmit_pulsewidth; /* transmitted pulse width */
		float   rconst;         /* radar constant */
		float   phaseoffset;    /* offset for phi dp */
		float   vreceiver_gain; /* ver chan gain from antenna flange to VIRAQ */
		float   vtest_pulse_pwr; /* ver test pulse power refered to antenna flange */
		float   vantenna_gain;  
		float   vnoise_power;   /* for subtracting from data */
		float   zdr_fudge_factor; /* what else? */
		float   misc[4];        /* 4 more misc floats */
		char    text[960];
		};


struct prq_radar_hdr_save  {
		char    desc[4];
		short   recordlen;
		short   rev;
		short   year;           /* this is also in the dwell as sec from 1970 */
		char    radar_name[8];
		char    polarization;   /* H or V */
		float   test_pulse_pwr; /* power of test pulse (refered to antenna flange) */
		float   test_pulse_frq; /* test pulse frequency */
		float   frequency;      /* transmit frequency */
		float   peak_power;     /* typical xmit power (at antenna flange) read from config.rdr file */
		float   noise_figure;
		float   noise_power;    /* for subtracting from data */
		float   receiver_gain;  /* hor chan gain from antenna flange to VIRAQ input */
		float   data_sys_sat;   /* VIRAQ input power required for full scale */
		float   antenna_gain;
		float   horz_beam_width;
		float   vert_beam_width;
		float   xmit_pulsewidth; /* transmitted pulse width */
		float   rconst;         /* radar constant */
		float   phaseoffset;    /* offset for phi dp */
		float   vreceiver_gain; /* ver chan gain from antenna flange to VIRAQ */
		float   vtest_pulse_pwr; /* ver test pulse power refered to antenna flange */
		float   vantenna_gain;  
		float   misc[6];        /* 9 more misc floats */
		char    text[960];
		};


struct nexrad_id_rec {          /* "ARCHIVE2" */
    char filename[8];
};

struct nexrad_vol_scan_title {  /* "ARCHIVE2.001,..." */
    char filename[9];
    char extension[3];
    long julian_date;           /* from 1/1/70 */
    long milliseconds_past_midnight;
    long filler_1;
};

struct CTM_info {               /* ignored */
    short CTM_word_1;
    short CTM_word_2;
    short CTM_word_3;
    short CTM_word_4;
    short CTM_word_5;
    short CTM_word_6;
};

struct nexrad_message_header {
    short message_len;          /* in 16-bit words */
    unsigned char channel_id;
    unsigned char message_type;
    short seq_num;              /* mod 0x7fff */
    short julian_date;          /* from 1/1/70 */
    long milliseconds_past_midnight;
    short num_message_segs;
    short message_seg_num;
};

struct rda_status_info {
   short rda_status;		/* ( 1) halfword location */
   short oper_status;		/* ( 2) */
   short cntrl_status;		/* ( 3) */
   short aux_pwr_gen_state;	/* ( 4) */
   short atp;			/* ( 5) */
   short ref_cal_corr;		/* ( 6) */
   short dte;			/* ( 7) */
   short vcp;			/* ( 8) */
   short rds_cntl_auth;		/* ( 9) */
   short intefr_det_rate;	/* (10) */
   short op_mode;		/* (11) */
   short intefr_suppr_unit;	/* (12) */
   short arch2status;		/* (13) */
   short arch2vols;		/* (14) */
   short rda_alarms;		/* (15) */
   short command_ak;		/* (16) */
   short ch_cntrl_stat;		/* (17) */
   short spol_blnk_stat;	/* (18) */
   short bypass_map_date;	/* (19) */
   short bypass_map_time;	/* (20) */
   short notch_map_date;	/* (21) */
   short notch_map_time;	/* (22) */
   short tps_stat;		/* (23) */
   short spare1;		/* (24) */
   short spare2;		/* (25) */
   short spare3;		/* (26) */
   short alarm_codes[14];	/* (27-40) */
};

struct drdh {
    long milliseconds_past_midnight; /* (15-16) */
    short julian_date;          /* (17) from 1/1/70 */
    short unamb_range_x10;      /* (18) km. */
    unsigned short azimuth;     /* (19) binary angle */
    short radial_num;           /* (20) */
    short radial_status;        /* (21) */
    unsigned short elevation;   /* (22) binary angle */
    short elev_num;             /* (23) */
    short ref_gate1;            /* (24) meters */
    short vel_gate1;            /* (25) meters */
    short ref_gate_width;       /* (26) meters */
    short vel_gate_width;       /* (27) meters */
    short ref_num_gates;        /* (28) */
    short vel_num_gates;        /* (29) */
    short sector_num;           /* (30) */
    float sys_gain_cal_const;   /* (31-32) */
    short ref_ptr;              /* (33) byte count from start of drdh */
    short vel_ptr;              /* (34) byte count from start of drdh */
    short sw_ptr;               /* (35) byte count from start of drdh */
    short velocity_resolution;  /* (36) */
    short vol_coverage_pattern; /* (37) */
    short VNV1;                 /* V & V simulator reserved */
    short VNV2;
    short VNV3;
    short VNV4;
    short ref_data_playback;    /* (42) */
    short vel_data_playback;    /* (43) */
    short sw_data_playback;     /* (44) */
    short nyquist_vel_x100;     /* (45)m/s */
    short atmos_atten_factor_x1000; /* (46) dB/km */
    short threshold_parameter;  /* (47) */
    /* c...mark */
    /* word_?? are meant as fillers and correspond to the documentation */
    short word_48;
    short word_49;
    short word_50;
    short word_51;
    short word_52;
    short word_53;
    short word_54;
    short word_55;
    short word_56;
    short word_57;
    short word_58;
    short word_59;
    short word_60;
    short word_61;
    short word_62;
    short word_63;
    short extended_header_ptr;	/* byte count from start of drdh */
};

struct uf_man{		   /* UF MANDITORY HEADER */
    /*
     * numbers in comments are the corresponding word numbers from the
     * Nov.'80 BAMS SESAME News writeup
     */
    char  id[2];                   /* 1 */
    short rec_len;		   /* 2 */
    /*
     * UF_FMT pointers are integer*2 (FORTRAN) type subscripts
     */
    short opt_hed_ptr;		   /* 3 */
    short loc_hed_ptr;		   /* 4 */
    short data_hed_ptr;		   /* 5 */
    short rec_num;		   /* 6 */
    short vol_num;		   /* 7 */
    short ray_num;		   /* 8 */
    short prec_in_ray;		   /* 9 */
    /*
     * the original spec provided for rays to occupy more than one record
     */
    short swp_num;		   /* 10 */
    char  radar[8];		   /* 11 */
    char  site[8];		   /* 15 */
    short lat_deg;		   /* 19 */
    short lat_min;		   /* 20 */
    short lat_sec_x64;		   /* 21 */
    /*
     * x64 implies that this parameter has been scaled (multiplied) by 64
     */
    short lon_deg;		   /* 22 */
    short lon_min;		   /* 23 */
    short lon_sec_x64;		   /* 24 */
    short altitude_msl;		   /* 25 */
    short year;			   /* 26 */
    short month;		   /* 27 */
    short day;			   /* 28 */
    short hour;			   /* 29 */
    short minute;		   /* 30 */
    short second;		   /* 31 */
    char  time_zone[2];		   /* 32 */
    short azimuth_x64;		   /* 33 */
    short elevation_x64;	   /* 34 */
    short sweep_mode;		   /* 35 */
    /*
     * 0 = Calibration
     * 1 = PPI
     * 2 = Coplane
     * 3 = RHI
     * 4 = Vertical
     * 5 = Target
     * 6 = Manual
     * 7 = Idle
     * 8 = Surveilance
     */
    short fixed_angle_x64;	   /* 36 */
    short sweep_rate_x64;	   /* 37 */
    short gen_date_yy;		   /* 38 */
    short gen_date_mm;		   /* 39 */
    short gen_date_dd;		   /* 40 */
    char  facility_id[8];	   /* 41 */
    short missing_data_flag;	   /* 45 */
    short word_46;		/* filler to an even # of 16-bit words */

};


struct uf_opt {		   /* UF OPTIONAL HEADER */
    char  project_id[8];	   /* 1 */
    short baseline_az_x64;	   /* 5 */
    short baseline_el_x64;	   /* 6 */
    short vol_hour;		   /* 7 */
    short vol_minute;		   /* 8 */
    short vol_second;		   /* 9 */
    char  fld_tape_id[8];	   /* 10 */
    short range_info_flag;	   /* 14 */
    /*
     * 0 = range info the same for all fields in this volume
     * 1 = range info the same only within each sweep
     * 2 = range info the same only within each ray
     */
    short seconds_xe3;		   /* 15 */
    short software_rev;		   /* 16 */

};

struct uf_opt_old {		   /* UF OPTIONAL HEADER */
    char  project_id[8];	   /* 1 */
    short baseline_az_x64;	   /* 5 */
    short baseline_el_x64;	   /* 6 */
    short vol_hour;		   /* 7 */
    short vol_minute;		   /* 8 */
    short vol_second;		   /* 9 */
    char  fld_tape_id[8];	   /* 10 */
    short range_info_flag;	   /* 14 */
    /*
     * 0 = range info the same for all fields in this volume
     * 1 = range info the same only within each sweep
     * 2 = range info the same only within each ray
     */

} OLD_UF_OPTIONAL;


struct uf_loc_use {
    char id[4];
    short array[6];		   /* reserve space for 6 words */
};

struct uf_loc_use_ncar {
    /*
     * Aircraft postitioning information
     */
    char id[4];			/* should contain "AIR" NULL */
    short ac_pitch_x64;		/* (3) */
    short ac_roll_x64;		/* (4) */

    short ac_phdg_x64;		/* (5) platform heading */
    short ac_vns_x10;		/* (6) north/south velocity */
    short ac_vew_x10;		/* (7) east/west velocity */
    short ac_wp3_x10;		/* (8) corrected vertical velocity */

    short ac_hgme;		/* (9) geometric altitude in meters */
    short ac_ui_x10;		/* (10) east-west wind */
    short ac_vi_x10;		/* (11) north-south wind */
    short ac_wi_x10;		/* (12) vertical wind */

    short ac_drift_x64;		/* (13) drift angle */
    short ac_rotation_x64;	/* (14) rotation angle */
    short ac_tilt_x64;		/* (15) tilt angle */
    short ac_phdg_change_x64;	/* (16) platform heading change deg per sec */

    short ac_pitch_change_x64;	/* (17) platform pitch change deg per sec */
    short ac_ant_rot_angle_x64;	/* (18) rotation angle corrected for roll */
    short ac_tilt_angle_x64;	/* (19) corrected tilt angle */
    short word_20;		/* filler */

};

struct uf_loc_use_ucla {
    /*
     * UCLA aircraft postitioning information
     */
    char id[10];		/* should contain "INE VALUES"  */
    char flight_id[8];

    short word_10;		/* filler */

    short word_11;		/* filler */
    short ac_phdg_x64;		/* (12) platform heading */
    short ac_roll_x64;		/* (13) */
    short ac_pitch_x64;		/* (14) */
    short ac_drift_x64;		/* (15) drift angle */
    short ac_vud_x64;		/* (16) vertical velocity */
    short tail_vew_x64;		/* (17) east/west velocity */
    short tail_vns_x64;		/* (18) north/south velocity */
    short tail_vud_x64;		/* (19) up/down velocity */
    short word_20;		/* filler */

    short word_21;		/* filler */
    short ac_ant_rot_angle_x64;	/* (22) rotation angle corrected for roll */
    short ac_tilt_angle_x64;	/* (23) corrected tilt angle */
    short word_24;		/* filler */
    short word_25;		/* filler */

};


struct uf_loc_use_ship {
    char id[4];			/* should be "SHIP" */
    short  train_ord;
    short  elev_ord;

    short  pitch;
    short  roll;
    short  heading;
    short  azm_rate;

    short  elev_rate;
    short  pitch_rate;
    short  roll_rate;
    short  heading_rate;

    short  vel_e;
    short  vel_n;
    short  vel_u;

    short  nav_sys_flag;
    short  rad_vel_cor;
};

struct uf_fld_id {
    char  id[2];		   /* 1 */
    short fld_hed_ptr;		   /* 2 */
};

struct uf_data_hed {
    short num_flds_this_ray;	   /* 1 */
    short num_recs_this_ray;	   /* 2 */
    short num_flds_this_rec;	   /* 3 */
    /*
     * followed by the requisit number of field ids
     */
};

/*
 * xe6 is short hand for scaling by 1.0e6
 */

struct uf_fld_hed {		   /* UF FIELD DATA HEADER */
    unsigned short fld_data_ptr;   /* 1 */
    short scale;		   /* 2 (if -1 use alt_scale and alt_bias) */
    short range_g1;		   /* 3 */
    short g1_adjust_xe3;	   /* 4 */
    short gate_spacing_xe3;	   /* 5 */
    short num_gates;		   /* 6 */
    unsigned short sample_vol_xe3; /* 7 */
    unsigned short h_beam_width_x64; /* 8 */
    unsigned short v_beam_width_x64; /* 9 */
    unsigned short rec_bw_xe6;	   /* 10 */
    short polarization;		   /* 11 */
    unsigned short waveln_x6400;   /* 12 */
    short num_samples;		   /* 13 */
    char  thr_fld[2];		   /* 14 */
    short thr_val;		   /* 15 */
    short thr_val_scale;	   /* 16 */
    char  edit_code[2];		   /* 17 */
    unsigned short prT_xe6;	   /* 18 */
    short bits_per_datum;	   /* 19 */
/*
 * place holders for radar parameters that are field type dependent
 */
    short word_20;
    short word_21;
    short word_22;
    short word_23;
    short word_24;
    short word_25;
/*
 * more field specific radar parameters (FSRPs)
 */
    short word_26;
    short word_27;
    short az_adjust_xe3;	   /* 28 */
    short el_adjust_xe3;	   /* 29 */
    short gt_sp_adjust_xe6;	   /* 30 */
    short alt_scale;               /* 31 (+ or - power of 2) */
    short alt_bias;		   /* 32 (scaled by alt_scale) */

};


struct radar_d {
    char  radar_des[4];		/* Identifier for a radar descriptor */
				/* block (ascii characters "RADD"). */
    long  radar_des_length;	/* Length of a radar descriptor block */
				/* in bytes. */
    char  radar_name[8];	/* Eight character radar name. */
    float radar_const;		/* Radar/lidar constant in ?? */
    float peak_power;		/* Typical peak power of the sensor in kw. */
				/* Pulse energy is really the
				 * peak_power * pulse_width */
    float noise_power;		/* Typical noise power of the sensor in dBm. */
    float receiver_gain;	/* Gain of the receiver in db. */
    float antenna_gain;		/* Gain of the antenna in db. */
    float system_gain;		/* System Gain in db.
				 * (Ant G - WG loss) */
    float horz_beam_width;	/* Horizontal beam width in degrees. */
				/* beam divergence in milliradians
				 * is equivalent to beamwidth */
    float vert_beam_width;	/* Vertical beam width in degrees. */
    short radar_type;		/* Radar Type (0)Ground, 1)Airborne */
				/* Fore, 2)Airborne Aft, 3)airborne */
				/* Tail, 4)Airborne Lower Fuselage, */
				/* 5)Shipborne. */
    short scan_mode;		/* Scan Mode (0)Calibration, 1)PPI */
				/* (constant Elevation) 2)Coplane, */
				/* 3)RHI (Constant Azimuth), 4) */
				/* Vertical Pointing, 5)Target */
				/* (Stationary), 6)Manual, 7)Idle (Out */
				/* of Control). */
    float req_rotat_vel;	/* Requested rotational velocity of */
				/* the antenna in degrees/sec. */
    float scan_mode_pram0;	/* Scan mode specific parameter #0 */
				/* (Has different meaning for */
				/* different scan modes). */
    float scan_mode_pram1;	/* Scan mode specific parameter #1. */
    short num_parameter_des;	/* Total number of parameter */
				/* descriptor blocks for this radar. */
    short total_num_des;	/* Total number of additional */
				/* descriptor block for this radar. */
    short data_compress;	/* Data compression. 0 = none, 1 = HRD */
				/* scheme. */
    short data_reduction;	/* Data Reduction algorithm: 1 = none, */
				/* 2 = between 2 angles, 3 = Between */
				/* concentric circles, 4 = Above/below */
				/* certain altitudes.*/
    float data_red_parm0;	/* 1 = smallest positive angle in */
				/* degrees, 2 = inner circle diameter, */
				/* km, 4 = minimum altitude, km. */
    float data_red_parm1;	/* 1 = largest positve angle, degress, */
				/* 2 = outer cicle diameter, km, 4 = */
				/* maximum altitude, km. */
    float radar_longitude;	/* Longitude of radar in degrees. */
    float radar_latitude;	/* Latitude of radar in degrees. */
    float radar_altitude;	/*  Altitude of radar above msl in km. */
    float eff_unamb_vel;	/* Effective unambiguous velocity, m/s. */
    float eff_unamb_range;	/* Effective unambiguous range, km. */
    short num_freq_trans;	/* Number of frequencies transmitted. */
    short num_ipps_trans;	/* Number of different inter-pulse */
				/* periods transmitted. */
    float freq1;		/* Frequency 1. */
    float freq2;		/* Frequency 2. */
    float freq3;		/* Frequency 3. */
    float freq4;		/* Frequency 4. */
    float freq5;		/* Frequency 5. */
    float interpulse_per1;	/* Interpulse period 1. */
    float interpulse_per2;	/* Interpulse period 2. */
    float interpulse_per3;	/* Interpulse period 3. */
    float interpulse_per4;	/* Interpulse period 4. */
    float interpulse_per5;	/* Interpulse period 5. */

				/* 1995 extension #1 */
    long  extension_num;
    char  config_name[8];	/* used to identify this set of
				 * unique radar characteristics */
    long  config_num;		/* facilitates a quick lookup of radar
				 * characteristics for each ray */
    /*
     * extend the radar descriptor to include unique lidar parameters
     */
    float aperature_size;       /* Diameter of the lidar aperature in cm. */
    float field_of_view;        /* Field of view of the receiver. mra; */
    float aperature_eff;        /* Aperature efficiency in %. */
    float freq[11];		/* make space for a total of 16 freqs */
    float interpulse_per[11];	/* and ipps */
    /*
     * other extensions to the radar descriptor
     */
    float pulsewidth;           /* Typical pulse width in microseconds. */
				/* pulse width is inverse of the
				 * band width */
    float primary_cop_baseln;	/* coplane baselines */
    float secondary_cop_baseln;
    float pc_xmtr_bandwidth;	/* pulse compression transmitter bandwidth */
    long  pc_waveform_type;	/* pulse compression waveform type */
    char  site_name[20];

}; /* End of Structure */



struct field_radar_i {
    char  field_radar_info[4];	/* Identifier for a field written */
				/* radar information block */
				/* (ascii characters FRIB). */
    long field_radar_info_len;	/* Length of this field written radar */
				/* information block in bytes. */
    long data_sys_id;		/* Data system identification. */
    float loss_out;		/* Waveguide Losses between Transmitter and */
				/* antenna in db. */
    float loss_in;		/* Waveguide Losses between antenna and Low */
				/* noise amplifier in db. */
    float loss_rjoint;		/* Losses in the rotary joint in db. */
    float ant_v_dim;		/* Antenna Vertical Dimension in m. */
    float ant_h_dim;		/* Antenna Horizontal Dimension in m. */
    float ant_noise_temp;	/* Antenna Noise Temperature in degrees K. */
    float r_noise_figure;	/* Receiver noise figure in dB*/
    float xmit_power[5];        /* Nominal Peak transmitted power in dBm
                                   by channel */
    float x_band_gain;          /* X band gain in dB */
    float receiver_gain[5];     /* Measured receiver gain in dB (by channel) */
    float if_gain[5];           /* Measured IF gain in dB (by channel) */
    float conversion_gain;      /* A to D conversion gain in dB */
    float scale_factor[5];      /* Scale factor to account for differences in
                                   the individual channels, and the inherent
                                   gain due to summing over the dwell time */
    float processor_const;      /* Constant used to scale dBz to
				   units the display processors understand */
    long dly_tube_antenna;	/* Time delay from RF being applied to
				   tube and energy leaving antenna in ns. */
    long dly_rndtrip_chip_atod;/* Time delay from a chip generated in the 
				   timing module and the RF pulse entering
                                   the A to D converters.  Need to take the
                                   RF input to the HPA and inject it into 
                                   the waveguide back at the LNA to make this
                                   measurement  in ns*/
    long dly_timmod_testpulse; /* Time delay from timing Module test
				   pulse edge and test pulse arriving at
				   the A/D converter in ns. */
    long dly_modulator_on;	/* Modulator rise time (Time between
				   video on into HPA and modulator full up in
				   the high power amplifier) in ns. */
    long dly_modulator_off;	/* Modulator fall time (Time between
				   video off into the HPA
				   and modulator full off) in ns. */
    float peak_power_offset;     /*Added to the power meter reading of the
                                   peak output power this yields actual
				   peak output power (in dB) */ 
    float test_pulse_offset;    /* Added to the power meter reading of the
                                   test pulse this yields actual injected
                                   test pulse power (dB) */
    float E_plane_angle;           /* E-plane angle (tilt) this is the angle in
				   the horizontal plane (when antennas are
				   vertical) between a line normal to the
				   aircraft's longitudinal axis and the radar
				   beam in degrees.  Positive is in direction
				   of motion (fore) */
    float H_plane_angle;         /* H plane angle in degrees - this follows
				    the sign convention described in the
				    DORADE documentation for ROLL angle */
    float encoder_antenna_up;   /* Encoder reading minus IRU roll angle
				   when antenna is up and horizontal */
    float pitch_antenna_up;     /* Antenna pitch angle (measured with
				   transit) minus IRU pitch angle when
				   antenna is pointing up */
    short indepf_times_flg;	/* 0 = neither recorded, 1 = independent
				   frequency data only, 3 = independent 
				   frequency and time series data recorded */
    short indep_freq_gate;	/* gate number where the independent frequency
                                   data comes from */
    short time_series_gate;	/* gate number where the time series data come
                                   from */
    short num_base_params;      /* Number of base parameters. */
    char  file_name[80];	/* Name of this header file. */
}; /* End of Structure */
