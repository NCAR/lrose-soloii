/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

# include <errno.h>
# include <dirent.h>

# include <dorade_headers.h>
# include <solo_window_structs.h>
# include <seds.h>
# include <solo_list_widget_ids.h>
# include <solo_editor_structs.h>
# include <dd_files.h>
# include <sed_shared_structs.h>
# include <dd_math.h>
# include <function_decl.h>
# include <dgi_func_decl.h>

/*
 * solo_free_palette
 * solo_gen_palette_list
 * solo_get_palette
 * solo_init_palette
 * solo_malloc_palette
 * solo_merge_usual_parm
 * solo_palette_color_numbers
 * solo_palette_message
 * solo_palette_remove
 * solo_push_palette
 * solo_same_palette
 * solo_this_palette
 * solo_usual_parms
 * solo_subscribe_palette
 * solo_topof_palettes
 * solo_zap_usual_parm
 */

static struct solo_palette_ptr *top_palette=NULL;
static char *parm_delim=",";

/* external routines */
void solo_message();		/* solo.c */
int solo_color_check();		/* sp_colors.c */
int solo_pixelate_color();	/* sp_colors.c */
void yes_exit();		/* solo.c */


/* internal routines */
struct solo_palette_ptr *solo_palette_insert();
void solo_free_palette();
void solo_gen_palette_list();
struct solo_palette_ptr *solo_get_palette();
void solo_init_palette();
struct solo_palette_ptr *solo_malloc_palette();
void solo_merge_usual_parm();
int solo_palette_color_numbers();
void solo_palette_message();
struct solo_palette_ptr *solo_palette_remove();
struct solo_palette_ptr *solo_push_palette();
int solo_same_palette();
void solo_subscribe_palette();
void solo_check_zap_parms();
struct solo_palette_ptr *solo_this_palette();
struct solo_palette_ptr *solo_usual_parms();
struct solo_palette_ptr *solo_topof_palettes();
void solo_zap_usual_parm();



# ifndef obsolete

/* c------------------------------------------------------------------------ */

struct solo_palette_ptr *
solo_palette_insert(this, here, before)
  struct solo_palette_ptr *this, *here;
  int before;
{
    if(!this)
	  return(this);

    this->next = this->last = NULL;

    if(before) {
	if(here == top_palette) {
	    if(top_palette)
		  top_palette->last = this;
	    this->next = top_palette;
	    top_palette = this;
	    return(this);
	}
	else if(!here) {
	    return(this);
	}
	this->next = here;
	this->last = here->last;
	here->last = here->last->next = this;
    }
    else {
	if(!here)
	      return(this);

	if(here->next) {	/* this is not the last entry */
	    here->next->last = this;
	    this->next = here->next;
	}
	this->last = here;
	here->next = this;
    }
    return(this);
}
/* c------------------------------------------------------------------------ */

void solo_free_palette(this)
  struct solo_palette_ptr *this;
{
    struct solo_palette_ptr *next;

    if(!this) {
	return;
    }
    free(this->at);
    free(this);
}
/* c------------------------------------------------------------------------ */

void solo_gen_palette_list()
{
    struct solo_list_mgmt *lm;
    int i, j, n;
    char str[256];
    struct solo_palette_ptr *pal, *solo_topof_palettes();
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();

    spi = solo_return_winfo_ptr();

    lm = spi->list_palettes;
    pal = solo_topof_palettes();

    lm->num_entries = 0;

    for(;;) {
	strcpy(str, " ");
	strcat(str, pal->at->palette_name);
	strcat(str, " <- ");
	strcat(str, pal->at->usual_parms);
	solo_add_list_entry(lm, str, strlen(str));
	if(!(pal = pal->next)) {
	    break;
	}
    }
}
/* c------------------------------------------------------------------------ */


struct solo_palette_ptr *
xsolo_get_palette(name, frme)
  char *name;
  int frme;
{
    int i, j, n, mark;
    struct solo_palette_ptr        *pal=NULL, *top
	  , *solo_topof_palettes()
		, *solo_this_palette()
		      , *solo_usual_parms()
			    , *solo_malloc_palette()
				  , *solo_push_palette();
    char mess[256], str[88];

/*
 * forward declarations
 */
    void solo_merge_usual_parm ();
    
    top = solo_topof_palettes();

    if(!(pal = solo_usual_parms(name, top))) {
	/* this name was not found in the usual parms list in this palette */
	strcpy(str, "p_");
	strcat(str, name);
	pal = solo_malloc_palette(str);

	strcpy(pal->at->palette_name, "p_");
	strcat(pal->at->palette_name, name);
	solo_merge_usual_parm(pal, name);
# ifdef obsolete
	strcpy(pal->at->usual_parms, name);
	strcat(pal->at->usual_parms, ",");
# endif
	pal->user_ww[frme] = YES;
	pal = solo_push_palette(pal);
	sprintf(mess, "Generated palette for %s\n", name);
	solo_message(mess);
    }
    return(pal);
}
/* c------------------------------------------------------------------------ */

void solo_init_palette()
{
    struct solo_palette_ptr *next, *last, *solo_malloc_palette();
    struct solo_palette_ptr *solo_push_palette();

    if(top_palette)
	  return;

    next = solo_malloc_palette("p_ahav");
    next->at->center = 0.;
    next->at->increment = 25.;
    strcpy(next->at->usual_parms, "AH,AV,");
    strcpy(next->at->units, "deg");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_chcv");
    next->at->center = .5;
    next->at->increment = .1;
    strcpy(next->at->usual_parms, "CH,CV,");
    strcpy(next->at->units, "???");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_pdesc");
    next->at->center = 9.;
    next->at->increment = 1.;
    strcpy(next->at->usual_parms, "PD,WPD,HYDID,");
    strcpy(next->at->units, "???");
    strcpy(next->at->color_table_name, "pd17.colors");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_raccum");
    next->at->center = 50.;
    next->at->increment = 6.;
    strcpy(next->at->usual_parms, "KAC,ZAC,DAC,HAC,NAC,");
    strcpy(next->at->units, "mm.");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_rho");
    next->at->center = .5;
    next->at->increment = .1;
    strcpy(next->at->usual_parms, "RHOHV,RHO,RH,RX,");
    strcpy(next->at->units, "deg");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_kdp");
    next->at->center = .7;
    next->at->increment = .12;
    strcpy(next->at->usual_parms, "KDP,CKDP,NKDP,MKDP,DKD_DSD,");
    strcpy(next->at->units, "deg");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_rrate");
    next->at->center = 8.;
    next->at->increment = 1.;
    strcpy(next->at->usual_parms, "RR_DSD,RNX,RZD,RKD,");
    strcpy(next->at->units, "mm/hr");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_nt");
    next->at->center = 2.;
    next->at->increment = .25;
    strcpy(next->at->usual_parms, "NT_DSD");
    strcpy(next->at->units, "N/mm3");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_res");
    next->at->center = 5.;
    next->at->increment = .6;
    strcpy(next->at->usual_parms, "RES_DSD");
    strcpy(next->at->units, "mm");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_d0");
    next->at->center = 2;
    next->at->increment = .25;
    strcpy(next->at->usual_parms, "D0_DSD");
    strcpy(next->at->units, "mm");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_lam");
    next->at->center = 5.;
    next->at->increment = .6;
    strcpy(next->at->usual_parms, "LAM_DSD");
    strcpy(next->at->units, "1/mm");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_mu");
    next->at->center = 5.;
    next->at->increment = .6;
    strcpy(next->at->usual_parms, "MU_DSD");
    strcpy(next->at->units, "???");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_n0");
    next->at->center = 4.;
    next->at->increment = .5;
    strcpy(next->at->usual_parms, "N0_DSD");
    strcpy(next->at->units, "???");
    last = solo_push_palette(next);

# ifdef notyet

    next = solo_malloc_palette("p_");
    next->at->center = 0.;
    next->at->increment = 20.;
    strcpy(next->at->usual_parms, "");
    strcpy(next->at->units, "deg");
    last = solo_push_palette(next);
# endif

    next = solo_malloc_palette("p_phi");
    next->at->center = 0.;
    next->at->increment = 20.;
    strcpy(next->at->usual_parms, "PHIDP,PHI,PH,DP");
    strcpy(next->at->units, "deg");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_zdr");
    next->at->center = 0.;
    next->at->increment = .5;
    strcpy(next->at->usual_parms, "ZDR,ZD,DR,UZDR,");
    strcpy(next->at->units, "db.");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_ldr");
    next->at->center = -6.;
    next->at->increment = 3.;
    strcpy(next->at->usual_parms, "LDR,TLDR,ULDR,");
    strcpy(next->at->units, "db.");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_dBm");
    next->at->center = -80.;
    next->at->increment = 5.;
    strcpy(next->at->usual_parms, "DM,LM,XM,XL,DL,DX,");
    strcpy(next->at->units, "dBm");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_sdev");
    next->at->center = 8.;
    next->at->increment = 1.;
    strcpy(next->at->usual_parms, "SD,");
    strcpy(next->at->units, "???");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_dBz");
    next->at->center = 15.;
    next->at->increment = 5.;
    strcpy(next->at->usual_parms, "DBZ,DZ,XZ,DB,Z,UDBZ,CDZ,DCZ");
    strcpy(next->at->units, "dBz");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_ncp");
    next = solo_malloc_palette("p_spectral");
    next->at->center = 8.;
    next->at->increment = 1.;
    strcpy(next->at->usual_parms, "SR,SW,S1,S2");
    strcpy(next->at->units, "m/s");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_ncp");
    next->at->center = 0.5;
    next->at->increment = 0.1;
    strcpy(next->at->usual_parms, "NCP,NC,");
    strcpy(next->at->units, "ncp");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_fore");
    next->at->center = -40.;
    next->at->increment = 7.;
    strcpy(next->at->usual_parms, "VR,");
    strcpy(next->at->units, "m/s");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_aft");
    next->at->center = 40.;
    next->at->increment = 7.;
    strcpy(next->at->usual_parms, "VR,");
    strcpy(next->at->units, "m/s");
    last = solo_push_palette(next);

    next = solo_malloc_palette("p_vel");
    next->at->center = 0.;
    next->at->increment = 3.;
    strcpy(next->at->usual_parms, "VR,VF,VG,VH,VN,VE,VU,VT,V1,V2,VELOCITY,");
    strcpy(next->at->units, "m/s");
    last = solo_push_palette(next);

}
/* c------------------------------------------------------------------------ */

struct solo_palette_ptr *
solo_malloc_palette(name)
  char *name;
{
    /* this routine generates a set of palette pointers and
     * a generic palette
     */
    struct solo_palette_ptr *next;
    char *color_dir, *getenv();
    long time_now();

    next = (struct solo_palette_ptr *)
	  malloc(sizeof(struct solo_palette_ptr));
    memset((char *)next, 0, sizeof(struct solo_palette_ptr));

    next->at = (struct solo_palette *)
	  malloc(sizeof(struct solo_palette));
    memset(next->at, 0, sizeof(struct solo_palette));

    strcpy(next->at->name_struct, "SPAL");
    next->at->sizeof_struct = sizeof(struct solo_palette);
    next->at->time_modified = time_now();
    next->at->center = 0;
    next->at->increment = 5.;
    next->at->num_colors = 17;
# ifdef obsolete
    strcpy(next->at->usual_parms, "NONE,");
    strcpy(next->at->palette_name, "GENERIC");
# endif
    strcpy(next->at->palette_name, name);

    /* use environment variable for Colors directory if it exists;
     * otherwise, use the one created during Solo build
     */
    if (color_dir = getenv ("SOLO_COLORS_DIR"))
        strcpy (next->at->color_table_dir, color_dir);
    else
# ifdef fixme
        strcpy(next->at->color_table_dir, SOLO_COLORS_DIR);
# endif

    strcpy(next->at->color_table_name, "carbone17.colors");
    strcpy(next->at->units, "???");
    strcpy(next->at->missing_data_color, "darkslateblue");
    strcpy(next->at->emphasis_color, "thistle");
    strcpy(next->at->exceeded_color, "grey72");
    strcpy(next->at->annotation_color, "grey87");
    strcpy(next->at->background_color, "midnightblue");
    strcpy(next->at->bnd_color, "cyan");
    strcpy(next->at->bnd_alert, "yellow");
    strcpy(next->at->bnd_last_line, "magenta");
    strcpy(next->at->grid_color, "cyan2");
    return(next);
}

/* c------------------------------------------------------------------------ */

struct solo_palette *
solo_malloc_old_palette()
{
    /* this routine generates a set of palette pointers and
     * a generic palette
     */
    struct solo_palette *pal;

    pal = (struct solo_palette *)
	  malloc(sizeof(struct solo_palette));
    memset(pal, 0, sizeof(struct solo_palette));

    strcpy(pal->name_struct, "SPAL");
    pal->sizeof_struct = sizeof(struct solo_palette);
    return(pal);
}

/* c------------------------------------------------------------------------ */

void solo_merge_usual_parm(this, name)
  struct solo_palette_ptr *this;
  char *name;
{
    char *a, *aa, *b, *c, *e, *f, str[256], *strstr(), *strchr();
    int i, j, n, nn, mark, prepend=NO;
    /*
     * put "name" at the beginning of the list
     */
    if(!(nn = strlen(name)))
	  return;

    a = this->at->usual_parms;

    strcpy(str, name);
    strcat(str, parm_delim);

    if(!strlen(a)) {
	strcpy(a, str);
	return;
    }

    for(b = a; *b ;) {		/* try to find a match */
       aa = b;
       for(f = name, n = 0;;) {
	  n++;
	  if(*b++ != *f++) {
	     aa = NULL;
	  }
	  if(*b == *parm_delim) {
	     b++;
	     break;
	  }
	  if(!(*b)) {
	     break;
	  }
       }
       if(aa) {
	  break;
       }
    }
    if(!aa || nn != n) {	/* we don't have a match */
	strcat(str, a);
	strcpy(this->at->usual_parms, str);
	return;
    }
    /* we really have a match
     */
    for(c=str+strlen(str); a < aa; *c++ = *a++); /* copy up to this name */
    for(; *b; *c++ = *b++); /* copy everything after it */
    *c = '\0';
    strcpy(this->at->usual_parms, str);
    return;
}
/* c------------------------------------------------------------------------ */

int xsolo_palette_color_numbers(frme)
  int frme;
{
    /* this routine assumes that all the color names have been checked
     */
    int ii, ok=YES;
    XColor xcolor;
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);

    if(!(ii = solo_color_check(wwptr->palette->emphasis_color, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->emphasis_color)
	 )) ok = NO;
    wwptr->emphasis_color_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->exceeded_color, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->exceeded_color)
	 )) ok = NO;
    wwptr->exceeded_color_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->missing_data_color, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->missing_data_color)
	 )) ok = NO;
    wwptr->missing_data_color_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->background_color, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->background_color)
	 )) ok = NO;
    wwptr->background_color_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->annotation_color, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->annotation_color)
	 )) ok = NO;
    wwptr->annotation_color_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->grid_color, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->grid_color)
	 )) ok = NO;
    wwptr->grid_color_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->bnd_color, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->bnd_color)
	 )) ok = NO;
    wwptr->bnd_color_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->bnd_alert, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->bnd_alert)
	 )) ok = NO;
    wwptr->bnd_alert_num = xcolor.pixel;

    if(!(ii = solo_color_check(wwptr->palette->bnd_last_line, &xcolor)
	 )) ok = NO;
    if(!(ii = solo_pixelate_color(&xcolor, wwptr->palette->bnd_last_line)
	 )) ok = NO;
    wwptr->bnd_last_num = xcolor.pixel;

    if(!ok) {
	uii_printf("Unable to allocate these colors\n");
# ifdef fixme
	yes_exit();
# endif
    }

    return(ok);
}
/* c------------------------------------------------------------------------ */

void solo_palette_message(frme)
  int frme;
{
    WW_PTR wwptr, solo_return_wwptr();
    float half;

    wwptr = solo_return_wwptr(frme);
    half = (float)wwptr->palette->num_colors*.5*wwptr->palette->increment;
    sprintf(wwptr->palette->message, "%d Colors  %.2f < %.2f     "
	    , wwptr->palette->num_colors
	    , wwptr->palette->center-half+.001
	    , wwptr->palette->center+half+.001
	    );
}
/* c------------------------------------------------------------------------ */

struct solo_palette_ptr *
solo_palette_remove(this)
  struct solo_palette_ptr *this;
{
    struct solo_palette_ptr *next;

    if(!this) {
	return(this);
    }
    next = this->next;

    if(this->last) {
	this->last->next = next;
    }
    if(next) {
	next->last = this->last;
    }
    if(this == top_palette)
	  top_palette = next;

    return(this);
}
/* c------------------------------------------------------------------------ */

struct solo_palette_ptr *
solo_push_palette(this)
  struct solo_palette_ptr *this;
{
    struct solo_palette_ptr *solo_palette_insert(), *next, *that;
    struct solo_palette_ptr *solo_palette_remove();
    int before=YES, mark;


    if(!top_palette) {
	this = solo_palette_insert(this, top_palette, before);
	return(this);
    }
# ifdef obsolete
    else if(!strcmp(top_palette->at->palette_name
		   , this->at->palette_name)) {
# endif
    else if(top_palette->at == this->at) {
	return(this);		/* this palette is already on top */
    }
    /* 
     * if this exists elsewhere on the stack remove it
     */
    for(that=top_palette->next; that; that=that->next) {

# ifdef obsolete
	if(!strcmp(this->at->palette_name, that->at->palette_name)) {
# endif
	if(this->at == that->at) {
	    next = solo_palette_remove(that);
	    break;
	}
    }
    this = solo_palette_insert(this, top_palette, before);
    return (this);
}
/* c------------------------------------------------------------------------ */

int solo_same_palette(this, that)
  struct solo_palette *this, *that;
{
    if(strcmp(this->palette_name, that->palette_name))
	  return(NO);
    if(this->center != that->center || this->increment != that->increment ||
       this->num_colors != that->num_colors)
	  return(NO);
    if(strcmp(this->color_table_name, that->color_table_name))
	  return(NO);

    return(YES);
}
/* c------------------------------------------------------------------------ */

void solo_subscribe_palette(frme, this, parameter_name)
  int frme;
  struct solo_palette_ptr *this;
  char *parameter_name;
{
    char str[256];
    int i, j, n, mark;
    struct solo_palette_ptr *top, *pal
	  , *solo_topof_palettes();

    char mess[256];
    
    pal = solo_topof_palettes();

    for(; pal; pal = pal->next) { /* loop through the palettes */
	/* unsubscribe this window
	 */
	pal->user_ww[frme] = NO;
    }
    /* prepend name at beginning of usual parms
     */
    solo_merge_usual_parm(this, parameter_name);
    /* register this window with the palette
     */
    this->user_ww[frme] = YES;
}
/* c------------------------------------------------------------------------ */

void solo_check_zap_parms(name, except)
  char *name;
  struct solo_palette_ptr *except;
{
    struct solo_palette_ptr *top, *pal
	  , *solo_topof_palettes();

/*
 * forward declarations
 */
   void solo_zap_usual_parm ();
    
    pal = solo_topof_palettes();

    for(; pal; pal = pal->next) {
	if(pal == except)
	      continue;
	/* loop through the palettes and
	 * remove this name if found
	 */
# ifdef obsolete
	solo_zap_usual_parm(pal, name);
# endif
    }
}
/* c------------------------------------------------------------------------ */

struct solo_palette_ptr *
solo_this_palette(name, next)
  char *name;
  struct solo_palette_ptr *next;
{
    struct solo_palette_ptr *last;

    for(; next; next=next->next) {
	if(strcmp(next->at->palette_name, name)) 
	      continue;		/* no match! */
	return(next);
    }
    return(NULL);
}
/* c------------------------------------------------------------------------ */

struct solo_palette_ptr *
solo_usual_parms(name, next)
  char *name;
  struct solo_palette_ptr *next;
{
    struct solo_palette_ptr *last;

    for(; next; next=next->next) {
	if(strstr(next->at->usual_parms, name)) 
	      return(next);
    }
    return(NULL);
}
/* c------------------------------------------------------------------------ */

struct solo_palette_ptr *
solo_topof_palettes()
{
    return(top_palette);
}
/* c------------------------------------------------------------------------ */

void solo_zap_usual_parm(this, name)
  struct solo_palette_ptr *this;
  char *name;
{
    char *a, *b, *c, *e, str[256], *strstr(), *strchr();
    int i, j, n, mark;
    struct solo_palette_ptr *solo_palette_remove();
    /*
     * remove name from list
     */
    a = this->at->usual_parms;

    if(!strlen(name))
	  return;
    if(!(b=strstr(a, name)))
	  return;
    if(!(b == a || *(b-1) == ',')) {
	return;			/* not at the beginning of a name */
    }
    for(n=0,e=b; *e && *e++ != ','; n++); /* count the characters */

    if(strncmp(b, name, strlen(name)))
	  return;
    /* we really have a match
     */
    for(c=str; a < b; *c++ = *a++); /* copy up to this name */
    for(; *e; *c++ = *e++); /* copy everything after it */
    *c = '\0';

    if(!strlen(str)) {		/* there are no parameters in this palette
				 * zap it */
	this = solo_palette_remove(this);
    }
    else
	  strcpy(this->at->usual_parms, str);
}
/* c------------------------------------------------------------------------ */

# endif
