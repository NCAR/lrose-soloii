/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

/*
 * This file contains
 * 
 * solo_gen_parameter_list
 * solo_gen_radar_list
 * solo_gen_swp_list
 * solo_get_files
 * solo_return_radar_list
 * solo_return_sweep_list
 * 
 */

# include <errno.h>
# include <dirent.h>
# include <solo_defs.h>
# include <dd_files.h>
# include <dorade_headers.h>
# include <dd_defines.h>
# include <solo_window_structs.h>
# include <dd_general_info.h>
# include <dgi_func_decl.h>
# include <function_decl.h>

/* external functions */
void solo_sort_strings();       /* se_basics.c */
void solo_message();		/* solo.c */


/* internal functions */
void solo_gen_parameter_list();
void solo_gen_radar_list();
void solo_gen_swp_list();
int solo_get_files();
struct solo_list_mgmt *solo_return_radar_list();
struct solo_list_mgmt *solo_return_sweep_list();


/* c------------------------------------------------------------------------ */

void solo_gen_parameter_list(frme)
  int frme;
{
    struct solo_list_mgmt *lm;
    int ww, i, j, n;
    char str[256];
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct dd_general_info *dgi, *dd_window_dgi();
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);
    ww = wwptr->lead_sweep->window_num;
    dgi = dd_window_dgi(ww, "");
    spi = solo_return_winfo_ptr();
    lm = spi->list_parameters;

    lm->num_entries = 0;
    for(i=0; i < dgi->num_parms; i++) {
	str_terminate(str, dgi->dds->parm[i]->parameter_name, 8);
	solo_add_list_entry(lm, str, strlen(str));
    }
}
/* c------------------------------------------------------------------------ */

void solo_gen_radar_list(frme)
  int frme;
{
    struct solo_list_mgmt *lm;
    int i, j, n;
    char str[256], *mddir_radar_name_v3();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();

    spi = solo_return_winfo_ptr();
    lm = spi->list_radars;
    n = mddir_num_radars_v3(frme);

    lm->num_entries = 0;
    for(i=0; i < n; i++) {
	strcpy(str, mddir_radar_name_v3(frme, i));
	solo_add_list_entry(lm, str, strlen(str));
    }
}
/* c------------------------------------------------------------------------ */

void solo_gen_swp_list(frme)
  int frme;
{
    struct dd_file_name **fnp, **mddir_return_swp_list();
    struct solo_list_mgmt *lm;
    int i, j, n, num_swps;
    char str[256], *dts_print();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    WW_PTR wwptr, solo_return_wwptr();
    DD_TIME dts, *d_unstamp_time();

    spi = solo_return_winfo_ptr();
    lm = spi->list_sweeps;
    wwptr = solo_return_wwptr(frme);

    n = mddir_gen_swp_list_v3(frme, wwptr->sweep->radar_num, lm);
}
/* c------------------------------------------------------------------------ */

int solo_get_files(dir, lm)
  char *dir;
  struct solo_list_mgmt *lm;
{
    /* tries to create a list of files in a directory
     */
    DIR *dir_ptr;
    struct dirent *dp;
    char mess[256];

    lm->num_entries = 0;

    if(!(dir_ptr = opendir(dir))) {
	sprintf(mess, "Cannot open directory %s\n", dir);
	solo_message(mess);
	return(-1);
    }

    for(;;) {
	dp=readdir(dir_ptr);
	if(dp == NULL ) {
	    break;
	}
	if(isalnum(*dp->d_name)) {
	    solo_add_list_entry(lm, dp->d_name, strlen(dp->d_name));
	}
    }
    closedir(dir_ptr);
    if(lm->num_entries > 1)
	  solo_sort_strings(lm->list, lm->num_entries);

    return(lm->num_entries);
}
/* c------------------------------------------------------------------------ */

struct solo_list_mgmt *solo_return_radar_list()
{
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();

    spi = solo_return_winfo_ptr();
    return(spi->list_radars);
}
/* c------------------------------------------------------------------------ */

struct solo_list_mgmt *solo_return_sweep_list()
{
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();

    spi = solo_return_winfo_ptr();
    return(spi->list_sweeps);
}
/* c------------------------------------------------------------------------ */

