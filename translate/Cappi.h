/* 	$Id$	 */
/*
 * This is meant to describe the items in a cappi file 
 */

# define ORDERED_TRIPLE 3	/* two variables */
# define ORDERED_PENTUPLE 5	/* four variables */
# define MAX_LEVELS 8

struct cappi_header {
    char name_struct[4];	/* could be "CPIH" */
    long sizeof_struct;

    float parameter_scale[MAX_PARMS];
    float parameter_bias[MAX_PARMS];
    float level_alt[MAX_LEVELS];
    long parameter_organization;
    long num_parms;
    long sizeof_parm[MAX_PARMS];
    long num_levels;
    long offsets[12];
    char radar_name[8];
    char parameter_name[MAX_PARMS][8];
};

struct cappi_d {
    char name_struct[4];	/* could be "CAPI" */
    long sizeof_struct;		/* plus "num_cells" ordered triples */
    float latitude;
    float longitude;
    float altitude_msl;
    float azimuth;
    float elevation;
    float true_scan_rate;
    long time_stamp;
    short index_of_intxn[MAX_LEVELS];
    /* index of the ordered n-tuple
     * starting with zero that is closest
     * to the intersection of the ray
     * and the plane
     */
    short num_cells[MAX_LEVELS];
};

/*
 * "num_cells" of ordered triples are appended to this struct
 * the first is a 16-bit range value in km. scaled by 100
 * the second is an 8-bit refectivity scaled by 2 and
 * the third is an 8-bit velocity scaled by 2
 */

struct ordered_triple {
    short range;
    char dbz;
    char vel;
};



