#ifndef INCcappis
#define INCcappis

#define MAX_PLANES 16
#define MAX_GATES 1024
#define RAD       0.017452007
#define XDIM      101
#define YDIM      101

# ifndef S100
# define S100(x) (((x) * 100.) +.5)
# endif
# ifndef K32
# define K32 32768
# endif

# define BAD_VAL -K32
# define VARIABLE_GRIDS


struct cappi_info {
    int num_cappis;
    int cappi_num;
    DGI_PTR dgi;
    char file_prefix[8];
    char radar_name[16];
    struct cappi_control *first;
    struct cappi_control *current;
    char refl_field[16];
    char  vel_field[16];
    char file_name[80];
    char directory_name[128];

    int  refv[XDIM][YDIM];
    int  velv[XDIM][YDIM];

    double grid_spacing;	/* in meters */
    double x_axis_length;	/* in meters */
    double y_axis_length;	/* in meters */
    int num_xvals;
    int num_yvals;

    short **ref_flds;
    short **vel_flds;
    float *levels;
    short *y_offset;

    int sizeof_grid;
    int num_levels;
    int bad_val;
    float f_bad_val;
    float pct_origin_shift;
};

struct cappi_control {
    struct cappi_control *last;
    struct cappi_control *next;
    double t_start;
    double t_stop;
    double lat_origin;
    double lon_origin;
    double grid_spacing;	/* in meters */
    double x_axis_length;	/* in meters */
    double y_axis_length;	/* in meters */
    double levels[MAX_PLANES];	/* cappi altitudes in meters */
    int num_levels;
    double heading;
    double lat_limit;
    double lon_limit;
    int num_xvals;
    int num_yvals;
    int ray_num;
};


/***************************************************************************/
/*FUNCTION PROTOTYPTES*/
/***************************************************************************/
void initialize_distance(double distance[XDIM][YDIM]);
void initialize_field(int ref[XDIM][YDIM], int vel[XDIM][YDIM]); 
int  on_grid(double lat, double lon, double glat, double glong,
                          double grid_size);
void get_gate_ranges(short nseg, short r0, short ngates[6],short 
                         gate_length[6],int gate_ranges[MAX_GATES]);
void    get_platform_info(double *alt, double *lat, double *lon, 
                          double *rotation, double *heading, int *type);
void    get_ray_info(double *azi, double *ele);
double  calc_slant_range(double ele, double deltaz,double angle,
                         int flag);
double  convert_to_meters(double *arg);
int     grid_gate_number(double deltaz, double ele,double r,
                         short nseg,int gate_ranges[MAX_GATES],
                         short gate_length[6],short r0);
void cappi_ranges(double deltaz, double ele, double eta, double *r1,
                  double *pr1, double *pr2, int *flag);
double get_projected_range(double slant_range, double ele);
double get_deltax(double proj_range, double azi);
double get_deltay(double proj_range, double azi);
double xaircraft_pos(double lon, double glong,double lat);
double yaircraft_pos(double lat, double glat);
double calc_air_distance(double air, double delta);
double grid_pos(double distance, double grid_spacing, double advect);
double calc_distance(double xdistance, double ydistance, int x, int y);
void gridded_field_value(int xgrid, int ygrid, float ze[1024], float vr[1024],
                         int gates[1024],int number);
void write_data(int ref[XDIM][YDIM],int vel[XDIM][YDIM]);
void convert_latitude(struct cappi_info *cin, short *deg,
                      short *min, short *sec );
void convert_longitude(struct cappi_info *cin, short *deg,
                      short *min, short *sec );
#endif
