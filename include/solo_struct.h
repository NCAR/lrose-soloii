/* 	$Id$	 */
/******************************************************************************
*  solo_struct.h
*
*  L. Nettleton 6/92
*
*  structures used by SOLO in connection with respect to all aspects of its 
*  functioning.
*******************************************************************************/

# include <X11/Intrinsic.h>
# include "solo_window_structs.h"  /* for SOLO_MAX_WINDOWS definition */

#ifndef SOLO_GLOBAL
#define SOLO_GLOBAL extern
#endif

typedef int FUZZY;

/**  Structure used to gather values from SOLO User Interface              **/

struct PERUSAL_WIDGET{
    FUZZY clrbar_center;	/** Colorbar center     **/
    FUZZY clrbar_step;		/** Colorbar step       **/
    FUZZY antenna;		/** Antenna option      **/
    FUZZY azimuth;		/** Draw azimuth option **/
    FUZZY range_rings;		/** Range rings option  **/
    FUZZY magnification;	/** Magnification       **/
    char fld_opt[8];		/** Field option        **/
    int  bndry_opt;		/** Mouse boundary opt  **/
  
};

typedef struct PERUSAL_WIDGET P_WIDGET;

SOLO_GLOBAL P_WIDGET *pwptr;


/**  Structures to pass values to Perusal                                  **/

typedef struct{                                    
    int      parameters;	/** dz,NCP,velocity... **/
    FUZZY    inc;		/** Colorbar step      **/
    FUZZY    center;		/** Colorbar center    **/
    int      aftflag;		/** Fore/Aft flag      **/
    int      changed;  /**UNNECESSARY - redundant to update_frame          **/
    FUZZY    azimuth_inc;	/** Azimuth increment  **/
    FUZZY    range_rgs_inc;	/** Range rings inc.   **/
    FUZZY    magnify;		/** Magnification      **/
    char     pfilename[80];	/** Fore or aft        **/
 }FIELD;


typedef struct{
       FIELD frame[SOLO_MAX_WINDOWS];               /** Frame parameters   **/
       int   no_frames;                             /** No. of frames      **/
       int   update_frame;                          /** Display frame no.  **/
       int   scan_int;                              /** Scan display intrvl**/
       int   no_cont_scans;                         /** No. of cont. scans **/
       int   window_open;                           /** Already open flag  **/
       char  date[10];
   }FRAMES;

SOLO_GLOBAL FRAMES *frameptr;


/** Structures and Xtoolkit definitions to pass values to I/O utilities    **/

SOLO_GLOBAL Widget Top;                             /** Parent for Xtoolkt **/
SOLO_GLOBAL XtAppContext Appc;                      /** Application context**/

struct INPUT_PATH{
   long start_time;
   long stop_time;
   char path[80];
   char radar[56];
   int  path_flag;
};

typedef struct INPUT_PATH IN_PATH;

SOLO_GLOBAL IN_PATH *pathptr;

SOLO_GLOBAL char infile[256];


/**  Structures to pass values to the Editor                               **/

struct EDITOR_STRUCT{
     char fields[150];
     char targets[50];
     char ed_names[50];
     char keep_del[7];
     int  ed_open;
     int  no_of_scan;
     int  antennas;                                 /** fore or aft        **/
     int  option;
     int  ed_function;
     int  inside_out;
};

typedef struct EDITOR_STRUCT EDIT;

SOLO_GLOBAL EDIT *EDITptr;  

struct POINT_DELETE{
     int flag;
     float azimuth;
     int range_gate;
};

typedef struct POINT_DELETE PT_DELETE;

SOLO_GLOBAL PT_DELETE *PtDelptr;

/** Structure to for utility routines (stacks, queues, etc)                 **/

struct STACK{
     char *string;
     struct STACK *next;
};

typedef struct STACK STACKNODE;

SOLO_GLOBAL STACKNODE *stackptr;

struct PARSE_TREE{
    char symbol[5];
    struct PARSE_TREE *left;
    struct PARSE_TREE *right;
};

typedef struct PARSE_TREE TREE;

typedef struct{
     char        pfield[8];
     int         edit_index;
     int         num_data;
     float       pscale;
     float       pbias;
     unsigned short *fld_ptr;
}FLD_STR;

SOLO_GLOBAL FLD_STR *fore_edptr, *aft_edptr;

struct CATALOG_FUNCTIONS{
     char in_cat[80];
     char out_cat[80];
     char cat_rname[12];
     char cat_intvl[10];
     char strt_cat[10];
     char stop_cat[10];
};

typedef struct CATALOG_FUNCTIONS CAT;

SOLO_GLOBAL CAT *catptr;

struct CAPPI_PARAMETERS{
     float  altitude;
     float  region;
     char disp_field[4];
     float  minvalue;
     float  maxvalue;
     int d_size;
     int area_len;
     int area_wd;
     int cscans;
     int antenna;
     int open_flag;
};

typedef struct CAPPI_PARAMETERS CAPPI;

SOLO_GLOBAL CAPPI *capptr;


struct AVAILABLE_FIELDS{
     char fore_flds[150];                            /** dz,vt,ncp,sw       **/
     char aft_flds[150];                             /** dz,vt,ncp,sw       **/
};

typedef struct AVAILABLE_FIELDS FIELDS;
SOLO_GLOBAL FIELDS *FLDptr;



typedef struct{
   char string[5];
}LIST;

typedef struct{
   int  threshold;
   int  boundary_del;
   int  point_del;
   int  unfold_del;
}EDIT_FLAG;

SOLO_GLOBAL EDIT_FLAG *EDITflgptr;
SOLO_GLOBAL Widget Widgetptr;
SOLO_GLOBAL Widget Wtwoptr;
