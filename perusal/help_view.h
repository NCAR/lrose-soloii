/* 	$Id$	 */

# ifndef HELP_VIEW_H
# define HELP_VIEW_H

static const gchar * hlp_view_overview[] = {
"The View widget enables adjustments to the zoom, shape and location",
"of the plot plus how, where and if range overlays are drawn.",
" ",
"A zoom of 1.0 implies 300 meters/pixel (roughly 10 km. per cm. of screen).",
" ",
"The location of the landmark determines the origin of range overlay",
"information.",
};

static const gchar * hlp_view_options[] = {
"The \"Options\" submenu contains two sets of radio buttons and two",
"toggles. The first set is a choice amoung options for range rings and",
"azimuth spokes followed by a toggle to enable or disable labeling.",
" ",
"The second set of radio buttons affects tic mark options.",
};

static const gchar * hlp_view_viewlinks[] = {
"The \"ViewLinks\" submenu brings up a links widget which identifies",
"which frames are to share this view information.",
};

static const gchar * hlp_view_center[] = {
"The \"Center\" submenu enables recentering based on previous clicks",
"in the data plus options to define how centering is done in this and",
"other frames.",
" ",
"The default is to center on the local radar and be independent of what",
"is happening in other frames. It is possible to force all linked",
"frames to center on a fixed latitude and longitude or to force all",
"linked frames to center on a reference frame. Otherwise the reference",
"frame has no effect.",
" ",
"The \"Set Links\" item brings up a links widget to define which frames",
"share this centering information.",
};

static const gchar * hlp_view_landmark[] = {
"The \"Landmark\" submenu enables referencing the landmark to locations",
"other than the radar. Referencing the radar is the default.",
" ",
"It is possible to force all linked frames to reference a fixed",
"latitude and longitude or to force all linked frames to share the",
"landmark in a reference frame. Otherwise the reference frame has no",
"effect.",
" ",
"The \"Set Links\" item brings up a links widget to define which frames",
"information.",
};

static const gchar * hlp_view_timeseries[] = {
" ",
"The \"TimeSeries\" submenu enables color bscan plots based on",
"time. Each pixel along the horizontal is assigned a constant interval",
"of time which enables compression or expansion of time plus revealing",
"gaps in the data.",
" ",
"You have to click \"OK\" or one of the Replot options to get the ",
"time series parameters activated.",
" ",
"Start and stop times are set in the \"Sweepfiles\" widget. A relative ",
"time in the \"Start Time\" entry shifts the start of the plot. A ",
"relative time in the \"Stop Time\" entry can be used to define the",
"time span.",
" ",
"The default is to plot increasing time from left to right relative to",
"the indicated times with the range increasing upward.",
" ",
"For aircraft data it is possible to fix the center of the plot on an",
"msl altitude and shift the data such the data bin closest to this ",
"altitude is at the center of the screen. ",
" ",
"In flights where the instrument's fixed angle is changing between nadir",
"and zenith, the \"Automatic\" option changes increasing range to be",
"either up or down depending on the fixed angle of the instrument.",
};



# endif

# ifdef obsolete
static const gchar * hlp_view_overview =
"
The View widget enables adjustments to the zoom, shape and location
of the plot plus how, where and if range overlays are drawn.

A zoom of 1.0 implies 300 meters/pixel (roughly 10 km. per cm. of screen).

The location of the landmark determines the origin of range overlay
information.
";

static const gchar * hlp_view_options =
"
The \"Options\" submenu contains two sets of radio buttons and two
toggles. The first set is a choice amoung options for range rings and
azimuth spokes followed by a toggle to enable or disable labeling.

The second set of radio buttons affects tic mark options.
";

static const gchar * hlp_view_viewlinks =
"
The \"ViewLinks\" submenu brings up a links widget which identifies
which frames are to share this view information.
";

static const gchar * hlp_view_center =
"
The \"Center\" submenu enables recentering based on previous clicks
in the data plus options to define how centering is done in this and
other frames.

The default is to center on the local radar and be independent of what
is happening in other frames. It is possible to force all linked
frames to center on a fixed latitude and longitude or to force all
linked frames to center on a reference frame. Otherwise the reference
frame has no effect.

The \"Set Links\" item brings up a links widget to define which frames
share this centering information.
";


static const gchar * hlp_view_landmark =
"
The \"Landmark\" submenu enables referencing the landmark to locations
other than the radar. Referencing the radar is the default.

It is possible to force all linked frames to reference a fixed
latitude and longitude or to force all linked frames to share the
landmark in a reference frame. Otherwise the reference frame has no
effect.

The \"Set Links\" item brings up a links widget to define which frames
share this landmark information.";


static const gchar * hlp_view_timeseries =
"

The \"TimeSeries\" submenu enables color bscan plots based on
time. Each pixel along the horizontal is assigned a constant interval
of time which enables compression or expansion of time plus revealing
gaps in the data.

You have to click \"OK\" or one of the Replot options to get the 
time series parameters activated.

Start and stop times are set in the \"Sweepfiles\" widget. A relative 
time in the \"Start Time\" entry shifts the start of the plot. A 
relative time in the \"Stop Time\" entry can be used to define the
time span.

The default is to plot increasing time from left to right relative to
the indicated times with the range increasing upward.

For aircraft data it is possible to fix the center of the plot on an
msl altitude and shift the data such the data bin closest to this 
altitude is at the center of the screen. 

In flights where the instrument's fixed angle is changing between nadir
and zenith, the \"Automatic\" option changes increasing range to be
either up or down depending on the fixed angle of the instrument.
";

# endif
