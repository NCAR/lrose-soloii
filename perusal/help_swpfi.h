/* 	$Id$	 */

# ifndef HELP_SWPFI_H
# define HELP_SWPFI_H

static const gchar * hlp_swpfi_overview[] = {
"The \"File\" menu provides the option to \"Close\" or remove the",
"Sweepfiles widget. This is the same as clicking the \"Cancel\"",
"button. No information from the widget is saved.",
" ",
"Selecting the \"List Radars\" File menu item will generate a list of",
"the radars for which there are sweepfiles in the current directory.",
"If there are more than one, edit the \"Radar\" entry to select a radar.",
" ",
"The \"Electric Swpfis\" File menu toggle item causes an immediate plot",
"of the clicked sweepfile in the Sweepfiles List widget. If the toggle",
"button appears raised, it is inactive. If it appears pushed in or",
"depressed it is active.",
" ",
"The \"Sweeps\" button shows a list of sweepfiles for the radar.",
"Clicking the button again will remove the list widget. Clicking an",
"item in the list selects that sweepfile for plotting.",
" ",
"The \"Replot\" button saves the current entries and causes an",
"immediate replot of the frame. The \"Replot \" menu bar item",
"enables replotting all the frames in the current lockstep",
"or all locksteps.",
" ",
"The \"OK\" button saves the current entries and removes the",
"widget.",
};

static const gchar * hlp_swpfi_links[] = {
"The SwpfiLinks submenu enables the selection of a set frames that",
"display data from the same sweepfile and to enable the display of data",
"from more than one radar or directory in the same set of frames.",
" ",
"Buttons that appear sunken or pushed in indicate frames that are part",
"of the link set. Buttons that appear to stand out or raised indicate",
"frames that are not part of the link set.",
" ",
"Clicking the \"OK\" button will save information from this widget.",
};

static const gchar * hlp_swpfi_lockstep[] = {
"The Lockstep submenu enables the selection of a set of frames",
"that will plot in response to an action that plots a lockstep",
"such as the <return> key or left or right arrows on the keyboard.",
" ",
"Buttons that appear sunken or pushed in indicate frames that are part",
"of the link set. Buttons that appear to stand out or raised indicate",
"frames that are not part of the link set.",
" ",
"Clicking the \"OK\" button will save information from this widget.",
};

static const gchar * hlp_swpfi_times[] = {
"The start and stop times are mainly for controlling time series (color",
"bscan) plots. Times can be absolute or relative. Relative times are",
"preceeded by either a \"+\" or \"-\" with no space between the sign",
"and the following number. Relative times such as \"+5m\" imply a 5",
"minute shift forward or \"-1h\" a backward shift of one hour. The",
"absence of an \"m\" or \"h\" implies seconds.",
" ",
"Absolute time should follow the example in the entry window",
"but can be truncated to the nearest hour.",
" ",
"If you're not in time series mode and you type \"+5m\" in the",
"start time entry followed by a <return>. The next sweepfile",
"closest to 5 minutes ahead of the current time will be plotted.",
};

static const gchar * hlp_swpfi_filter[] = {
"The \"Filter\" requires later versions of sweepfile names that",
"contain the fixed angle, scan mode and volume number. Pressing",
"one of the arrow keys with the filter on causes soloii to look",
"for the next volume where the criteria are satisfied.",
};

static const gchar * hlp_swpfi_tsync[] = {
"The \"Link Time\" toggle button enables sweep sets to be synchronized",
"by the start time of the sweepfile displayed in frame 1. e.g. the",
"display of two or more radars could advance together in time. Also",
"lower time resolution datasets such as the rainfall accumulations",
"could move (stay synchronized) with the data in frame 1.",
" ",
"A sweep set is set of sweeps distinguished by the radar name or a set",
"of sweeps residing in a different directory from the the set of sweeps",
"selected in frame 1. \"SwpfiLinks\" settings are used to differentiate",
"sweep sets.",
" ",
"The Link Time button must be toggled in one frame of each sweep set",
"where synchronization is desired.",
};


# endif






