/* 	$Id$	 */

# ifndef HELP_EXAM_H
# define HELP_EXAM_H

static const gchar * hlp_exam_overview[] = {
"The Examine widget enables the \"Display\" of cell values for several",
"variables and several rays for the sweepfile displayed in the frame",
"plus an inventory of all the rays in the sweepfile, the contents of",
"the DORADE sweepfile headers \"Metadata\" and a list of the editing",
"commands applied to the sweepfile.",
" ",
"It is also possible to edit individual cells by applying selected",
"operations under the \"Edit\" menu. ",
" ",
"When displaying cell values, hitting the left arrow key changes",
"the display to start one ray earlier and right arrow moves every",
"colummn to one ray later. The up arrow scrolls the display to",
"previous lines and the down arrow moves to the next set of lines.",
" ",
"All the cells in each the rays selected are accessible",
"for display and editing.",
" ",
"Hitting Return/Enter in the entry fields of the \"Examine Menu\"",
"widget causes a redisplay of the data based on the new contents",
"of the entry. ",
" ",
"The \"Ray\" entry selects the first ray of the display.  Typing a",
"\"Range\" value will display the cell nearest the range to be",
"displayed at the top of the \"Examine Display Widget\".  Likewise",
"typing a \"Cell\" number shifts the display be begin at the cell",
"entered.",
};

static const gchar * hlp_exam_operations[] = {
"The \"Edit\" submenu enables flagging individual cells as bad",
"or subtracting or adding the Nyquist interval to clicked cells or",
"removing the aircraft motion from a velocity cell of aircraft data.",
" ",
"One can also cause a ray to be removed from the sweepfile by clicking",
"the \"Delete Ray\" radio button and clicking in one of the data",
"columns for that ray.",
" ",
"Subtracting or adding the Nyquist Interval to the whole ray",
"is also an option.",
" ",
"If it is desireable to unfold some contiguous range of cells rather",
"than the whole ray and the user notes the cell at which unfolding",
"should end and then clicks the cell where the unfolding should start,",
"the whole ray will initially be modified, but if the user subsequently",
"clicks on the noted final cell the unfolding will be only within the",
"range defined by the first and second cells clicked as long as the two",
"cells clicked are in the same column.",
" ",
"Folding operations on a ray from the clicked cell to the end are",
"accomplished with the \"- Fold Ray >\" and \"+ Fold Ray >\" options.",
" ",
};

static const gchar * hlp_exam_options[] = {
"The \"Options\" submenu provides for switching the display reference",
"information between (rotation angle, range) and (ray number, cell number).",
" ",
"Also the user can toggle logging updates of the Examine Display Widget",
"on and off plus closing the current log file to begin a new one or",
"flushing the current logging information to the file in order to be",
"visable to other processes.",
};

static const gchar * hlp_exam_nyqvel[] = {
"When \"Nyq Vel\" is set to zero the Nyquist Velocity from the data is",
"used. To change that, enter the temporary Nyquist Velocity and hit",
"Return/Enter. To return to using the default data Nyquist Velocity",
"type \"0\" and hit Return.",
" ",
"The default data Nyquest Velocity can be found by clicking \"Metadata\"",
"int the Display menu and looking for \"eff_unamb_vel\" in the radar",
"(\"RADD\") descriptor.",
};

static const gchar * hlp_exam_logdir[] = {
"The \"Log Dir\" entry defines the directory into which log files will",
"be placed. Hitting Return/Enter in the Log Dir entry activates logging",
"of the contents of the \"Examine Display Widget\".",
" ",
"Changing the directory name and hitting Return closes the current file",
"if there is one and opens a new file in the new directory.",
};


# endif

# ifdef obsolete
static const gchar * hlp_exam_overview =
"
The Examine widget enables the \"Display\" of cell values for several
variables and several rays for the sweepfile displayed in the frame
plus an inventory of all the rays in the sweepfile, the contents of
the DORADE sweepfile headers \"Metadata\" and a list of the editing
commands applied to the sweepfile.

It is also possible to edit individual cells by applying selected
\"Operations\". 

When displaying cell values, hitting the left arrow key changes
the display to start one ray earlier and right arrow moves every
colummn to one ray later. The up arrow scrolls the display to
previous lines and the down arrow moves to the next set of lines.

All the cells in each the rays selected are accessible
for display and editing.

Hitting Return/Enter in the entry fields of the \"Examine Menu\"
widget causes a redisplay of the data based on the new contents
of the entry. 

The \"Ray\" entry selects the first ray of the display.  Typing a
\"Range\" value will display the cell nearest the range to be
displayed at the top of the \"Examine Display Widget\".  Likewise
typing a \"Cell\" number shifts the display be begin at the cell
entered.

CAUTION!
If the width of the data display is shorted to less than two variables
and five rays (11 columns), the current display does not right justfy
properly and clicking in the data causes unpredictable results.
Resizing the widget slightly causes the columns to right justify again.
There can be on the order of 30 columns of data.
";

static const gchar * hlp_exam_operations =
"
The \"Operations\" submenu enables flagging individual cells as bad
or subtracting or adding the Nyquist interval to clicked cells or
removing the aircraft motion from a velocity cell of aircraft data.

One can also cause a ray to be removed from the sweepfile by clicking
the \"Delete Ray\" radio button and clicking in one of the data
columns for that ray.

Subtracting or adding the Nyquist Interval to the whole ray
is also an option.

If it is desireable to unfold some contiguous range of cells rather
than the whole ray and the user notes the cell at which unfolding
should end and then clicks the cell where the unfolding should start,
the whole ray will initially be modified, but if the user subsequently
clicks on the noted final cell the unfolding will be only within the
range defined by the first and second cells clicked as long as the two
cells clicked are in the same column.

";


static const gchar * hlp_exam_options =
"
The \"Options\" submenu provides for switching the display reference
information between (rotation angle, range) and (ray number, cell number).

Also the user can toggle logging updates of the Examine Display Widget
on and off plus closing the current log file to begin a new one or
flushing the current logging information to the file in order to be
visable to other processes.
";


static const gchar * hlp_exam_nyqvel =
"
When \"Nyq Vel\" is set to zero the Nyquist Velocity from the data is
used. To change that, enter the temporary Nyquist Velocity and hit
Return/Enter. To return to using the default data Nyquist Velocity
type \"0\" and hit Return.

The default data Nyquest Velocity can be found by clicking \"Metadata\"
int the Display menu and looking for \"eff_unamb_vel\" in the radar
(\"RADD\") descriptor.
";

static const gchar * hlp_exam_logdir =
"
The \"Log Dir\" entry defines the directory into which log files will
be placed. Hitting Return/Enter in the Log Dir entry activates logging
of the contents of the \"Examine Display Widget\".

Changing the directory name and hitting Return closes the current file
if there is one and opens a new file in the new directory.
";

# ifdef notyet

static const gchar * hlp_exam_ =
"
";

static const gchar * hlp_exam_ =
"
";

# endif


# endif




