/* 	$Id$	 */

# include "soloii.h"
# include "sii_externals.h"
# include <gdk/gdkx.h>


/* c---------------------------------------------------------------------- */
/* c---------------------------------------------------------------------- */

int sii_png_image (char *fname)
{
  static int err_count = 0;
  char mess[256], command[512], *aa;
  int ii;
  Window windowID=0;

  windowID = GDK_WINDOW_XWINDOW (main_event_box->window);

  sprintf (command, "xwd -id %d -nobdrs | convert xwd:- ", windowID);
  aa = command +strlen(command);

  strcpy (aa, fname);
  strcat (aa, ".png");
  ii = system(command);
  if (ii && ++err_count == 1) {
    sprintf (mess, "Unable to write %s", fname);
    sii_message (mess);
    perror ("Unable to write PNG image");
  }
  else
    { err_count = 0; }
  return ii;
}

/* c---------------------------------------------------------------------- */


/* c---------------------------------------------------------------------- */

