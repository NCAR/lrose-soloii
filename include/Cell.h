/* 	$Id$	 */
#define MAXGATES 1500
struct cell_d {
    char cell_spacing_des[4];	        /* Cell descriptor identifier: ASCII */
				        /* characters "CELV" stand for cell*/
				        /* vector. */
    long  cell_des_len   ;	        /* Comment descriptor length in bytes*/
    long  number_cells   ;              /*Number of sample volumes*/
    float dist_cells[MAXGATES];         /*Distance from the radar to cell*/
                                        /* n in meters*/

}; /* End of Structure */



typedef struct cell_d cell_d;
typedef struct cell_d CELLVECTOR;

