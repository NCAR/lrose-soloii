/*
 *	$Id$
 *
 *	Module:		 DrawWidget.h
 *	Original Author: Richard E. Neitzel
 *      Copywrited by the National Center for Atmospheric Research
 *	Date:		 $Date$
 *
 * revision history
 * ----------------
 * $Log$
 * Revision 1.1  2002/07/02 14:57:33  oye
 * Initial revision
 *
 * Revision 1.2  1994/03/11 17:24:04  sherrie
 * *** empty log message ***
 *
 * Revision 1.2  1992/12/08  15:55:58  thor
 * Went to XImage only drawing.
 *
 * Revision 1.1  1992/12/03  20:01:52  thor
 * Initial revision
 *
 *
 *
 * description:
 *        
 */
#ifndef INCDrawWidget_h
#define INCDrawWidget_h

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/XawInit.h>

#include <X11/Xaw/Simple.h>
#include <X11/Xmu/Converters.h>

extern WidgetClass drawWidgetClass;

typedef struct _DrawClassRec *DrawWidgetClass;
typedef struct _DrawRec      *DrawWidget;

/* Global convience routines */
extern void DrawWidgetGc(DrawWidget w, GC gc);
extern void DrawSetImage(DrawWidget w, XImage *image);

#define XtNcallback2 "callback2"
#define XtNcallback3 "callback3"

#endif /* INCDrawWidget_h */
