/*
 *	$Id$
 *
 *	Module:		 DrawWidgetP.h
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
 * Revision 1.1  1993/11/17 21:51:02  oye
 * Initial revision
 *
 * Revision 1.2  1992/12/08  15:55:58  thor
 * Went to XImage only drawing.
 *
 * Revision 1.1  1992/12/03  20:01:53  thor
 * Initial revision
 *
 *
 *
 * description:
 *        
 */
#ifndef INCDrawWidgetP_h
#define INCDrawWidgetP_h

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/XawInit.h>

#include "DrawWidget.h"
#include <X11/Xaw/SimpleP.h>

/* New fields for the Draw widget class record */

typedef struct {int foo;} DrawClassPart;

/* Full class record declaration */
typedef struct _DrawClassRec {
    CoreClassPart core_class;
    SimpleClassPart simple_class;
    DrawClassPart draw_class;
} DrawClassRec;

extern DrawClassRec drawClassRec;

/* New fields for the Draw widget record */
typedef struct {
    /* resources */
    XtCallbackList callback1;   /* User callbacks. */
    XtCallbackList callback2;   /* User callbacks for button2. */
    XtCallbackList callback3;   /* User callbacks for button3. */
    void (*userRedraw)(DrawWidget, XtPointer); /* User defined redraw code. */
    XtPointer userData;		/* Used in above. */
    GC gc;			/* GC for everything */
    XImage *image;		/* Ditto. */
    int depth;			/* Likewise for screen depth */
    Display *display;		/* and the display. */
} DrawPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _DrawRec {
    CorePart core;
    SimplePart simple;
    DrawPart draw;
} DrawRec;

/* Local routines */
static void ClassInitialize(void);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redraw(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Destroy(Widget);

/* Action routines */
static void ButtonOne(Widget, XEvent *, String *, Cardinal *);
static void ButtonTwo(Widget, XEvent *, String *, Cardinal *);
static void ButtonThree(Widget, XEvent *, String *, Cardinal *);

#endif /* INCDrawWidgetP_h */
