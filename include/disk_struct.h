/* 	$Id$	 */
/*disk structure*/
  
typedef struct{
    int    year[4];
    int    month[4];
    int    day[4];
    int    which_radar[4];      /*set to 70 for forward,65 for aft*/
    char   diskfilename[4][80];
    char   volume_filename[4][80];
    int    filedesc;
    int    called;
    int    paramsize;
    int    redraw;
 }DISK_STRUCT;
