/* 	$Id$	 */
#include "eldora.h"
#include "read_defines.h"

 struct SENSEDESC{
      RADARDESC       *radar;
      FIELDRADAR      *fradar;
      CELLSPACING     *cellspacing;
      CELLVECTOR      *cellvector;
      PARAMETER       *paramdesc[MAXPARAMS];
      CORRECTION      *correct;
      int             bytes_per_cell[MAXPARAMS];
      int             type;
  };
  typedef struct SENSEDESC SENSEDESC;
  typedef struct SENSEDESC sensdesc;

  struct DORADE {
      VOLUME         *vp;
      COMMENT        *comment;
      WAVEFORM       *wave;
      SENSEDESC      sensors[MAXPARAMS];
      SWEEPINFO      *sweep;
      RAY            *ray;
      PLATFORM       *platform;
      FIELDPARAMDATA  *fp;
      PARAMDATA      *paramdata[MAXPARAMS];
      /*void(*uncompress());*/
      unsigned char  volume_header[BLOCKSIZE];
      unsigned char  data_area[BLOCKSIZE];
      int            filept;      /*file pointer to data set*/
      int            num_sensors;
      int            num_parameters;
      int            eldora;
      int            ray_count;
      int            ray_sweep_count;
      int            filetype;
      int            num_cells;
      int            radar_type;
      long           rayarray[MAXRAYS];
      long           swibarray[MAXSWIBS];
      int            rayarrayindex;
      int            swibarrayindex;
      int            byteoffset;
      int            bytesread;
      int            timesread;
      SWEEPINFO      *sweep_skips[MAXSWIBS];
  };


  typedef struct DORADE DORADE;
  typedef struct DORADE dorade;



/*function prototypes*/
int header_dorade(DORADE *dp,SENSEDESC *sd,char filename[20]);
int open_dorade(DORADE *,char *,int);
int retrieve_param_data(SENSEDESC *,DORADE *,int,void *,int,int);
int skip_fore_ray(DORADE *,int);
int skip_fore_sweep(DORADE *,int);


/*prototyes for getting address of structures*/
VOLUME        *get_volume(DORADE *);
COMMENT       *get_comment(DORADE *);
WAVEFORM      *get_waveform(DORADE *);
RADARDESC     *get_radardesc(SENSEDESC *);
SENSEDESC     *get_sensordesc(DORADE *,int);
FIELDRADAR    *get_fradardesc(SENSEDESC *);
CELLSPACING   *get_cellspacing(SENSEDESC *);
CELLVECTOR    *get_cellvector(SENSEDESC *);
PARAMETER     *get_parameter(SENSEDESC *,int);
SWEEPINFO     *get_sweep(DORADE *);
RAY           *get_ray(DORADE *);
PLATFORM      *get_platform(DORADE *);
PARAMDATA     *get_paramdata(DORADE *,int param_num);
FIELDPARAMDATA *get_fparamdata(DORADE *);



/*prototypes for printing structure fields*/
void print_volume_descriptor(VOLUME *);
void fprint_volume_descriptor(VOLUME *);
void print_field_radar_info(FIELDRADAR *);
void print_cellspacing_descriptor(CELLSPACING *);
void print_cellvector_descriptor(CELLVECTOR *);
void print_parameter_descriptor(PARAMETER *);




