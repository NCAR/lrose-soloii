/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

# include <signal.h>
# include <stdio.h>
# include <string.h>
# include <dd_defines.h>
# include "input_limits.h"
# include "dd_stats.h"
# include <date_stamp.h>

#include <unistd.h>
#include <stdlib.h>

# ifdef linux
# include <endian.h>

# if BYTE_ORDER == LITTLE_ENDIAN
int LittleEndian = 1;
# else
int LittleEndian = 0;
# endif

# else
int LittleEndian = 0;
# endif
 

# define         BATCH_MODE 0
# define   INTERACTIVE_MODE 1

static struct dd_input_filters *difs;
static struct dd_stats *dd_stats;

void nex_dd_conv();
void uf_dd_conv();
void hrd_dd_conv();
void sigmet_dd_conv();
void ddin_dd_conv();
void eld_dd_conv();
void toga_dd_conv();
void fof_dd_conv();
void piraq_dd_conv();
void piraqx_dd_conv();
void tdwr_dd_conv();
void ddout_loop();
void dd_uf_conv();
void dd_loop_ground_echo();
void dd_mrd_conv();
void dd_shane_conv();
void dd_flush();
void dd_get_difs();
void dd_input_strings();
void etl_dd_conv();
int cdcode();
int getreply();

# ifdef sparc
int Sparc_Arch=YES;
# else
int Sparc_Arch=NO;
# endif


/* c---------------------------------------------------------------------- */

main()
{

    int ii, nn, ival, which_mode;
    float val;
    char str[256];

    struct dd_input_filters *dd_return_difs_ptr();
    struct dd_stats *dd_return_stats_ptr();
    char *a, *afmt, *get_tagged_string();
    int sweep_files=NO, mark;
    int interactive_mode=YES;

    difs = dd_return_difs_ptr();
    dd_stats = dd_return_stats_ptr();

    /* set up input filters */
    dd_input_strings();
    dd_get_difs(difs);

    if(a=get_tagged_string("BATCH_MODE")) { 
	interactive_mode = NO;
    }
    else {
	printf( "\nNCAR/ATD SoloII translaters as of %s\n\n", sii_date_stamp );
    }
    if(!(afmt=get_tagged_string("INPUT_FORMAT"))) {
	printf("No input format defined!\n");
	exit(0);
    }

    for(;;) {
	which_mode = interactive_mode;
	if(interactive_mode) {
	    printf("\n-1 = exit program (-2 exits from some other prompts)\n");
	    printf(  " 0 = continue\n");
	    printf(  " 1 = repeat last run\n Option = ");
	    nn = getreply(str, sizeof(str));
	    if(cdcode(str, nn, &ival, &val) != 1)
		  continue;
	    if(ival == -1)
		  break;
	    if(ival > 1)
		  continue;
	    if(interactive_mode && ival == 1)
		  which_mode = BATCH_MODE;
	}
	
	if(strstr(afmt, "WSR_88D_FORMAT")) {
	    nex_dd_conv(which_mode);
	}
	else if(strstr(afmt, "UF_FORMAT")) {
	    uf_dd_conv(which_mode);
	}
	else if(strstr(afmt, "HRD_FORMAT")) {
	    hrd_dd_conv(which_mode);
	}
	else if(strstr(afmt, "SIGMET_FORMAT")) {
	    sigmet_dd_conv(which_mode);
	}
	else if(strstr(afmt, "DORADE_FORMAT")) {
	    ddin_dd_conv(which_mode);
	}
	else if(strstr(afmt, "ELDORA_FORMAT")) {
	    eld_dd_conv(which_mode);
	}
	else if(strstr(afmt, "TOGA_FORMAT")) {
	    toga_dd_conv(which_mode);
	}
	else if(strstr(afmt, "FOF_FORMAT")) {
	    fof_dd_conv(which_mode);
	}
	else if(strstr(afmt, "PIRAQ_FORMAT")) {
	    piraq_dd_conv(which_mode);
	}
	else if(strstr(afmt, "PIRAQX_FORMAT")) {
	    piraqx_dd_conv(which_mode);
	}
	else if(strstr(afmt, "TDWR_FORMAT")) {
	    tdwr_dd_conv(which_mode);
	}
	else if(strstr(afmt, "ETL2M_FORMAT")) {
	    etl_dd_conv(which_mode);
	}
	else if(strstr(afmt, "CIMM_FORMAT")) {
	    cimm_dd_conv(which_mode);
	}
	else if(strstr(afmt, "NETCDF_FORMAT")) {
	    nc_dd_conv(which_mode);
	}

	else if(strstr(afmt, "SWEEP_FILES")) {
	    sweep_files = YES;
	    if(a=get_tagged_string("OUTPUT_FLAGS")) {
		if(strstr(a, "DORADE_DATA")) {
		    ddout_loop();
		}
		else if(strstr(a, "UF_DATA")) {
		    dd_uf_conv();
		}
		else if(strstr(a, "GECHO_DATA")) {
		    dd_loop_ground_echo();
		}
		else if(strstr(a, "NSSL_MRD")) {
		    dd_mrd_conv();
		}
		else if(strstr(a, "SHANES_DATA")) {
		    dd_shane_conv();
		}
		else if(strstr(a, "NETCDF_DATA")) {
		    dd_ncdf_conv();
		}
		else if(strstr(a, "SWEEP_FILES")) {
		  /* special program to pass thru sweepfile
		   * updating the correction parameters
		   */
		    dd_swpfi_conv();
		}
		else {
		    mark = 0;
		}
	    }
	    else {
		mark = 0;
	    }
	}
	else {
	    printf("Unrecognizable INPUT_FORMAT\n");
	    break;
	}
	if(!sweep_files)
	      dd_flush(DD_FLUSH_ALL);

	if(!interactive_mode)
	      break;
    }


    printf("Read: %d recs %d rays %d sweeps %d vols %d files %.2f MB\n"
	   , dd_stats->rec_count
	   , dd_stats->ray_count
	   , dd_stats->sweep_count
	   , dd_stats->vol_count
	   , dd_stats->file_count
	   , dd_stats->MB
	   );
    return(0);
}
/* c------------------------------------------------------------------------ */

void solo_message(message)
  char *message;
{
    printf("%s", message);
}
/* c---------------------------------------------------------------------- */



