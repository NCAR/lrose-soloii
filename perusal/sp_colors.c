/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

# include <ctype.h>
# include <string.h>
# include <sys/types.h>
# include <errno.h>
# include <dirent.h>
# include <solo_window_structs.h>
# include <solo_defs.h>
# include <dd_files.h>
# include <dd_defines.h>
# include <dorade_headers.h>
# include <dd_general_info.h>
# include <sed_shared_structs.h>
# include <function_decl.h>
# include <dgi_func_decl.h>

/*
 * 
 * solo_absorb_color_table
 * solo_color_bar_pixels
 * solo_color_check
 * solo_data_color_lut
 * solo_hardware_color_table
 * solo_pixelate_color
 * 
 */

/* external functions */
void solo_add_list_entry();	/* dorade_share.c */
void solo_message();		/* solo.c */

/* internal functions */
int solo_absorb_color_table();
void solo_color_bar_pixels();
int solo_color_check();
void solo_data_color_lut();
int solo_hardware_color_table();
int solo_pixelate_color();


/* c------------------------------------------------------------------------ */

int solo_absorb_color_table(frme, dir, file)
  int frme;
  char *dir, *file;
{
    /* the purpose of this routine is to read in a color either of
     * rgb values normalized to one or xcolor names
     */
    int i, j, n;
    FILE *stream;
    WW_PTR wwptr, solo_return_wwptr();
    struct solo_list_mgmt *lm;
    float red, green, blue, rnum, rden, gnum, gden, bnum, bden;
    char *aa, *a, *b, *c, str[256], *dd_whiteout();
    char mess[256];
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_list_mgmt *slm;

    slash_path(str, dir);
    strcat(str, file);

    wwptr = solo_return_wwptr(frme);
    lm = wwptr->list_colors;
    wwptr->num_colors_in_table = lm->num_entries = 0;
    
    if(!(stream = fopen(str, "r"))) {
       slm = solo_return_winfo_ptr()->list_a_message;
       solo_reset_list_entries(slm);
       sprintf(mess, "Unable to open color table %s", str);
       solo_add_list_entry(slm, mess, strlen(mess));
       slm_list_ui(slm);
       popup_message(slm);
       solo_reset_list_entries(slm);
       return(0);
    }
    for(;;) {
	if(!(fgets(str, 128, stream)))
	      break;
	/*
	 * advance to first non-whiteout character
	 */
	aa = dd_whiteout(str);

	if(!strlen(aa) || strstr(aa, "colortable") ||
	   strstr(aa, "endtable") || strstr(aa, "!")) {
	    continue;
	}

	if(a=strstr(aa, "xcolor")) { /* X color name */
	    n = strlen(a);
	    b = a+n;
	    a += sizeof("xcolor");
	    /* move to start of name
	     */
	    for(i=0; i < 128 && !isalnum(*a); i++, a++);
	    /* backup to end of name (there can be blanks in the name)
	     */
	    for(; a < b && !isalnum(*(--b)); *b ='\0');

	    solo_add_list_entry(lm, a, strlen(a));
	}
	else if(strstr(aa, "/")) { /* funkier zeb color files */
	    sscanf(aa, "%f/%f %f/%f %f/%f"
		   , &rnum, &rden
		   , &gnum, &gden
		   , &bnum, &bden);
	    i = wwptr->num_colors_in_table++;
	    (wwptr->xcolors+i)->red =     (rnum/rden)*(K64 - 1);
	    (wwptr->xcolors+i)->green = (gnum/gden)*(K64 - 1);
	    (wwptr->xcolors+i)->blue =   (bnum/bden)*(K64 - 1);
	}
	else {			/* assume rgb vals between 0 and 1 */
	    sscanf(aa, "%f %f %f", &red, &green, &blue);
	    i = wwptr->num_colors_in_table++;
	    (wwptr->xcolors+i)->red =     red*(K64 - 1);
	    (wwptr->xcolors+i)->green = green*(K64 - 1);
	    (wwptr->xcolors+i)->blue =   blue*(K64 - 1);
	}		   
    }
    fclose(stream);
    return(wwptr->num_colors_in_table
	   ? wwptr->num_colors_in_table : lm->num_entries);
}
/* c------------------------------------------------------------------------ */

void solo_color_bar_pixels(frme, xlen, ylen, buf)
  int frme, xlen, ylen;
  char *buf;
{
    /* generate pixel values in the color bar image
     */
    int i, j, k, n;
    WW_PTR wwptr, solo_return_wwptr();
    double f_inc, f_min, f_max, f_val;
    char *b=buf;

    if(xlen < 1 || ylen < 1 || !buf)
          return;

    wwptr = solo_return_wwptr(frme);

    f_val = f_min = wwptr->palette->center
          -.5*wwptr->num_colors_in_table*wwptr->palette->increment;
    f_max = wwptr->palette->center
          +.5*wwptr->num_colors_in_table*wwptr->palette->increment;

    f_inc = (f_max -f_min)/(float)xlen;

    /* do the first line
     */
    for(i=0; i++ < xlen; f_val += f_inc) {
        k = DD_SCALE(f_val, wwptr->parameter_scale, wwptr->parameter_bias);
        if(k < -K32) k = -K32;
        if(k >= K32) k = K32-1;
        *b++ = *(wwptr->data_color_lut+k);
    }
}
/* c------------------------------------------------------------------------ */

void xsolo_color_bar_pixels(XImage *cb_image, int height, int frme)
{
    /* generate pixel values in the color bar image
     */
    int i, j, k;
    int width = cb_image->width;
    WW_PTR wwptr, solo_return_wwptr();
    double f_inc, f_min, f_max, f_val;

    wwptr = solo_return_wwptr(frme);

    f_min = wwptr->palette->center
	  -.5*wwptr->num_colors_in_table*wwptr->palette->increment;
    f_max = wwptr->palette->center
	  +.5*wwptr->num_colors_in_table*wwptr->palette->increment;

    f_inc = (f_max -f_min)/(float)width;

    /* do the first line
     */
    for (j = 0; j < height; j++)
    {
	for(i = 0; i < width; i++) 
	{
	    f_val = f_min + i * f_inc;
	    k = DD_SCALE(f_val, wwptr->parameter_scale, wwptr->parameter_bias);
	    if(k < -K32) k = -K32;
	    if(k >= K32) k = K32-1;
	    XPutPixel (cb_image, i, j, wwptr->data_color_lut[k]);
	}
    }
}
/* c------------------------------------------------------------------------ */

int solo_color_check(theColorName, theHardwareColor)
  char *theColorName;
  XColor *theHardwareColor;
{
    char mess[256];
    XColor theRGBColor;
    int theStatus;
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_list_mgmt *slm;

    spi = solo_return_winfo_ptr();
    slm = spi->list_a_message;
# ifdef FIXME
    theStatus = XLookupColor( spi->display
			     , spi->theColormap
			     , theColorName
			     , &theRGBColor
			     , theHardwareColor);
# endif 
    if(theStatus == 0) {
	sprintf(mess, "Unable to use color: %s\n"
		, theColorName);
	solo_add_list_entry(slm, mess, strlen(mess));
    }
    return(theStatus);
}
/* c------------------------------------------------------------------------ */

int xsolo_hardware_color_table(frme, directory, filename)
  int frme;
  char *directory, *filename;
{
    char mess[256];
    int ii, jj, nc, ok=YES;
    struct solo_list_mgmt *lm;
    XColor xc;
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_list_mgmt *slm;
    WW_PTR wwptr, solo_return_wwptr();

    wwptr = solo_return_wwptr(frme);
    spi = solo_return_winfo_ptr();
    slm = spi->list_a_message;
    solo_reset_list_entries(slm);

    /* try absorbing the color table
     */
    if((nc = solo_absorb_color_table(frme, directory, filename)) < 1) {
	sprintf(mess, "Not a useful color table: %s %s"
		, directory, filename);
	solo_add_list_entry(slm, mess, strlen(mess));
	slm_list_ui(slm);
	popup_message(slm);
	solo_reset_list_entries(slm);
	return(nc);
    }
    ok = YES;
    /*
     * check the conversions for bad apples
     */
    if(!wwptr->num_colors_in_table) {
	lm = wwptr->list_colors;
	for(ii=0; ii < lm->num_entries; ii++) {
	    /* see if the name can be converted to rgb values
	     * and then if the rgb vals can be converted to a pixel value
	     */
	    if(!(jj=solo_color_check(*(lm->list+ii), &xc))) {
		ok = NO;
	    }
	    else if(!(jj=solo_pixelate_color(&xc, *(lm->list+ii)))) {
		ok = NO;
	    }
	}
	if(ok) {		/* go ahead and use it */
	    for(ii=0; ii < lm->num_entries; ii++) {
		jj = solo_color_check(*(lm->list+ii), wwptr->xcolors+ii);
	    }
	    wwptr->num_colors_in_table = lm->num_entries;
	}
    }
    else {
	/* this was a color table with only rgb values
	 * see if they can be converted to pixel values
	 */
	for(ii=0; ii < wwptr->num_colors_in_table; ii++) {
	    if(!(jj=solo_pixelate_color(wwptr->xcolors+ii, ""))) {
		ok = NO;
	    }
	}
    }
    if(!ok) {
	sprintf(mess, "Not a useful color table: %s %s"
		, directory, filename);
	solo_add_list_entry(slm, mess, strlen(mess));
	slm_list_ui(slm);
	popup_message(slm);
	solo_reset_list_entries(slm);
	return(0);
    }
    else {
	for(ii=0; ii < wwptr->num_colors_in_table; ii++) {
	    jj = solo_pixelate_color(wwptr->xcolors+ii, "");
	}
	wwptr->palette->num_colors = wwptr->num_colors_in_table;
	return(wwptr->num_colors_in_table);
    }
}
/* c------------------------------------------------------------------------ */

int solo_pixelate_color(theHardwareColor, theColorName)
  XColor *theHardwareColor;
  char *theColorName;
{
    char mess[256];
    int theStatus, mark;
    struct solo_perusal_info *spi, *solo_return_winfo_ptr();
    struct solo_list_mgmt *slm;

    spi = solo_return_winfo_ptr();
    slm = spi->list_a_message;
# ifdef FIXME
    theStatus = XAllocColor( spi->display, spi->theColormap
			     , theHardwareColor);
# endif
    if(theStatus == 0) {
	sprintf(mess, "Unable to pixelate color: %s %d %d %d"
		, theColorName, theHardwareColor->red
		, theHardwareColor->green, theHardwareColor->blue);
	solo_add_list_entry(slm, mess, strlen(mess));
    }
    if(theHardwareColor->pixel > 255) {
	sprintf(mess, "Pixel value %d exceeds 255 for %s %d %d %d"
		, theHardwareColor->pixel
		, theColorName, theHardwareColor->red
		, theHardwareColor->green, theHardwareColor->blue);
	solo_add_list_entry(slm, mess, strlen(mess));
    }
    return(theStatus);
}
/* c------------------------------------------------------------------------ */

