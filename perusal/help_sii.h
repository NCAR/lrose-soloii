/* 	$Id$	 */

# ifndef HELP_SII_H
# define HELP_SII_H

static const gchar * hlp_sii_basics[] = {
"Clicking the right mouse button brings up the frame menu",
"which enables changing sweepfile and lockstep settings;",
"changing parameter and color settings;",
"changing viewing, centering and landmark settings;",
"accessing the data editing widget;",
"accessing the data examine and point by point modification widget;",
"and popping up an individual small clicked data display widget.",
" ",
"If you click the \"Data Widget\" button and the data widgit is",
"showing, it will be hidden.",
" ",
"All clicks in menus and widgets should be done with the left mouse",
"button.",
" ",
"Clicking the center button in a data frame causes the lockstep to",
"recenter on the clicked location. There is a crosshair in the center",
"of the frame to enable minor adjustments to the center.",
" ",
"A lockstep is a set of frames that move forward and backward together",
"in response to the arrow keys.",
" ",
"Clicking the left button in the data causes location information to be",
"sent to the small clicked data display, the examine widget, and the",
"edit widget.",
" ",
"The keyboard right arrow triggers the display of the next sweepfile",
"for the lockstep. The left arrow moves backwards. You can trigger",
"continuous plotting by holding down the shift key while you press an",
"arrow key, but BEWARE, the loop can only be stopped by typing",
"Control-C with the cursor in the window where you started soloii.",
" ",
"Hitting the return/enter will cause the lockstep associated with the",
"frame in which the cursor currently resides to replot. If the cursor",
"is in the menubar, all locksteps will replot.",
" ",
"If you have plotted more than one field in the same frame, the \"t\"",
"key will toggle the plot between the last two fields plotted.",
" ",
"For most widgets clicking either \"OK\" or one of the \"Replot\"",
"buttons will cause the changes to be accepted. Clicking the \"Close\"",
"or \"Cancel\" buttons will leave plot parameters unchanged.",
};

static const gchar * hlp_sii_file[] = {
"If you have not set any of the environment variables \"DORADE_DIR\",",
"\"SOLO_WINDOW_INFO\" or \"swi\" (swi and SOLO_WINDOW_INFO are",
"synonomous), the \"Soloii Initialization\" widget will appear allowing",
"you to set the directory containing the source sweepfiles or the",
"configuration directory. This same widget will appear when you click",
"\"Set Source Dir\", \"Config Files\", or \"Set Image Dir\".",
" ",
"Subsequent changes to the source directory can and should be done",
"in the \"Sweepfiles Widget\"",
" ",
"To save a configuration, you can modify the \"Comment\" and then click",
"\"Save Config\". Because there are more frames possible in soloii,",
"configuration files from the old solo will not work in soloii.",
" ",
"To make a PNG image of the soloii display click the button labeled",
"\"Make PNG Image\" in the Initialization widget. Be sure the data",
"display window is completely visible and that no other widgets are",
"encroaching on the display You can also set the destination directory",
"for PNG images. Setting the environment variable \"WINDOW_DUMP_DIR\"",
"also sets the directory for PNG images.",
" ",
"If you have set the \"BATCH_MODE\" environment variable to blast out",
"images of all the sweepfiles in the source directory, you will likely",
"want to set the SOLO_WINDOW_INFO environment variable and the",
"WINDOW_DUMP_DIR variable.",
};

static const gchar * hlp_sii_zoom[] = {
"With this menu you can zoom or unzoom the data or the size of the data",
"widgets (i.e. the whole configuration).",
" ",
"The data zoom applies to all frames. The default zoom/magnification",
"value is 1.0 which implies 300 meters per pixel (roughly 10 km. per",
"cm. of screen).",
" ",
"There is a minimum height and width for a frame. The frame may change",
"proportions as these minimums are reached. The minimum width is based",
"on the worst case scenario for the title line for the smallest font",
"used.",
" ",
"At this point the only way to shrink the configuration is to use the",
"negative zooms under \"Config\".",
};

static const gchar * hlp_sii_ctr[] = {
"For centering on two or four clicks, the new center is based on the",
"center of the extremes in the horizontal and vertical. The zoom is",
"adjusted such the all the clicks fit in the current frame shape.",
" ",
"The \"Electric\" toggle button enables or disables replotting when the",
"center mouse button is clicked in a frame.",
};

static const gchar * hlp_sii_config[] = {
"The configuration of the frames of data is based on a 4x4 matrix.  All",
"data frames must be rectangular but they can be of different sizes as",
"long as the fit the matrix and the final overal configuration is",
"rectangluar. There can never be more than 12 frames.",
" ",
"The top part of the menu consists of an exhaustive list of the",
"possible combinations uniformly shaped frames.",
" ",
"Next are options to force the frames to specified proportions.",
" ",
"Finally a set of unusual configurations just for grins.",
};

static const gchar * hlp_sii_shortcuts[] = {
"Text Editing Shortcuts",
" ",
"Alt may not work on some systems. Try the diamond key instead.",
" ",
"Ctrl+A -- Beginning of Line",
"Ctrl+E -- End of Line",
"Ctrl+N -- Next Line",
"Ctrl+P -- Previous Line",
"Ctrl+B -- Backward one character",
"Ctrl+F -- Forward one character",
" Alt+B -- Backward one word",
" Alt+f -- Forward one word",
" ",
"Ctrl+H -- Delete backward character (backspace)",
"Ctrl+D -- Delete forward character (delete)",
"Ctrl+W -- Delete backward word",
" Alt+D -- Delete forward word",
"Ctrl+K -- Delete to end of line",
"Ctrl+U -- Delete line",
" ",
"Ctrl+X -- Cut to clipboard",
"Ctrl+C -- Copy to clipboard",
"clipboard",
};

static const gchar * hlp_sii_comparisons[] = {
"The procedure for aligning two or more radars to do comparisons",
"involves resetting the \"SwpfiLinks\" and most likely the \"Lockstep\"",
"links that reside in the \"Sweepfiles\" widget.",
" ",
"This should be done in several steps. First starting with the",
"Sweepfiles widget for frame 1, click the SwpfiLinks->Set_Links",
"button. Clear all links and then click the frames that share the same",
"set of sweepfiles with frame 1 including frame 1. You will most likely",
"want to control the display of each radar seperately rather than",
"having them advance together; therefore, repeat this procedure for the",
"\"Lockstep\". Be sure to click \"OK\" in each when you are finished",
"setting the links.",
" ",
"Now click \"OK\" in the Sweepfiles widget and replot all frames by",
"moving the cursor up to main menubar and pressing \"Return\". It is",
"important to do this for each new radar because the sorting out and",
"finalizing of the links for all frames happens during the plot phase.",
" ",
"Repeat the above steps in the first frame in which the next radar will",
"be plotted but also type the directory containing the sweepfiles of",
"the next radar before you replot all frames.",
" ",
"You will then select the fields plotted for each radar in each frame",
"with the \"Parameters\" widget.",
" ",
"Comparisons usually involves referencing the centers of frames",
"containing the other radars to the center of Frame 1 or fixing the",
"centers of all frames at a specified lat/lon. The widget that enables",
"this is the \"Center\" button in the \"View\" widget.",
};

static const gchar * hlp_sii_about[] = {
"SoloII was specifically targeted to work using the GTK+ graphics",
"libraries and the GNU compilers that come with RedHat Linux operating",
"system and in ATD/RTF systems environment in the SPOL radar and at the",
"ATD Boulder Foothills Lab.",
" ",
"Resonable attempts will be made to help users with different",
"environments and OSs get Soloii installed, compiled and running but",
"Soloii comes with no guarentees of working in any other environments.",
" ",
"date:",
};

static const gchar * hlp_sii_[] = {
};



# endif

# ifdef obsolete
static const gchar * hlp_sii_basics =
"
Clicking the right mouse button brings up the frame menu
which enables changing sweepfile and lockstep settings;
changing parameter and color settings;
changing viewing, centering and landmark settings;
accessing the data editing widget;
accessing the data examine and point by point modification widget;
and popping up an individual small clicked data display widget.

If you click the \"Data Widget\" button and the data widgit is
showing, it will be hidden.

All clicks in menus and widgets should be done with the left mouse
button.

Clicking the center button in a data frame causes the lockstep to
recenter on the clicked location. There is a crosshair in the center
of the frame to enable minor adjustments to the center.

A lockstep is a set of frames that move forward and backward together
in response to the arrow keys.

Clicking the left button in the data causes location information to be
sent to the small clicked data display, the examine widget, and the
edit widget.

The keyboard right arrow triggers the display of the next sweepfile
for the lockstep. The left arrow moves backwards. You can trigger
continuous plotting by holding down the shift key while you press an
arrow key, but BEWARE, the loop can only be stopped by typing
Control-C with the cursor in the window where you started soloii.

Hitting the return/enter will cause the lockstep associated with the
frame in which the cursor currently resides to replot. If the cursor
is in the menubar, all locksteps will replot.

If you have plotted more than one field in the same frame, the \"t\"
key will toggle the plot between the last two fields plotted.

For most widgets clicking either \"OK\" or one of the \"Replot\"
buttons will cause the changes to be accepted. Clicking the \"Close\"
or \"Cancel\" buttons will leave plot parameters unchanged.
";


gchar * hlp_sii_file =
"
If you have not set any of the environment variables \"DORADE_DIR\",
\"SOLO_WINDOW_INFO\" or \"swi\" (swi and SOLO_WINDOW_INFO are
synonomous), the \"Soloii Initialization\" widget will appear allowing
you to set the directory containing the source sweepfiles or the
configuration directory. This same widget will appear when you click
\"Set Source Dir\", \"Config Files\", or \"Set Image Dir\".

Subsequent changes to the source directory can and should be done
in the \"Sweepfiles Widget\"

To save a configuration, you can modify the \"Comment\" and then click
\"Save Config\". Because there are more frames possible in soloii,
configuration files from the old solo will not work in soloii.

To make a PNG image of the soloii display click the button labeled
\"Make PNG Image\" in the Initialization widget. Be sure the data
display window is completely visible and that no other widgets are
encroaching on the display You can also set the destination directory
for PNG images. Setting the environment variable \"WINDOW_DUMP_DIR\"
also sets the directory for PNG images.

If you have set the \"BATCH_MODE\" environment variable to blast out
images of all the sweepfiles in the source directory, you will likely
want to set the SOLO_WINDOW_INFO environment variable and the
WINDOW_DUMP_DIR variable.
";


gchar * hlp_sii_zoom = 
"
With this menu you can zoom or unzoom the data or the size of the data
widgets (i.e. the whole configuration).

The data zoom applies to all frames. The default zoom/magnification
value is 1.0 which implies 300 meters per pixel (roughly 10 km. per
cm. of screen).

There is a minimum height and width for a frame. The frame may change
proportions as these minimums are reached. The minimum width is based
on the worst case scenario for the title line for the smallest font
used.

At this point the only way to shrink the configuration is to use the
negative zooms under \"Config\".
";


gchar * hlp_sii_ctr = 
"
For centering on two or four clicks, the new center is based on the
center of the extremes in the horizontal and vertical. The zoom is
adjusted such the all the clicks fit in the current frame shape.

The \"Electric\" toggle button enables or disables replotting when the
center mouse button is clicked in a frame.
";


gchar * hlp_sii_config = 
"
The configuration of the frames of data is based on a 4x4 matrix.  All
data frames must be rectangular but they can be of different sizes as
long as the fit the matrix and the final overal configuration is
rectangluar. There can never be more than 12 frames.

The top part of the menu consists of an exhaustive list of the
possible combinations uniformly shaped frames.

Next are options to force the frames to specified proportions.

Finally a set of unusual configurations just for grins.
";


static gchar *hlp_sii_shortcuts =
"
Text Editing Shortcuts

Alt may not work on some systems. Try the diamond key instead.

Ctrl+A -- Beginning of Line
Ctrl+E -- End of Line
Ctrl+N -- Next Line
Ctrl+P -- Previous Line
Ctrl+B -- Backward one character
Ctrl+F -- Forward one character
 Alt+B -- Backward one word
 Alt+f -- Forward one word

Ctrl+H -- Delete backward character (backspace)
Ctrl+D -- Delete forward character (delete)
Ctrl+W -- Delete backward word
 Alt+D -- Delete forward word
Ctrl+K -- Delete to end of line
Ctrl+U -- Delete line

Ctrl+X -- Cut to clipboard
Ctrl+C -- Copy to clipboard
Ctrl+V -- Paste from clipboard";


gchar * hlp_sii_comparisons = 
"The procedure for aligning two or more radars to do comparisons
involves resetting the \"SwpfiLinks\" and most likely the \"Lockstep\"
links that reside in the \"Sweepfiles\" widget.

This should be done in several steps. First starting with the
Sweepfiles widget for frame 1, click the SwpfiLinks->Set_Links
button. Clear all links and then click the frames that share the same
set of sweepfiles with frame 1 including frame 1. You will most likely
want to control the display of each radar seperately rather than
having them advance together; therefore, repeat this procedure for the
\"Lockstep\". Be sure to click \"OK\" in each when you are finished
setting the links.

Now click \"OK\" in the Sweepfiles widget and replot all frames by
moving the cursor up to main menubar and pressing \"Return\". It is
important to do this for each new radar because the sorting out and
finalizing of the links for all frames happens during the plot phase.

Repeat the above steps in the first frame in which the next radar will
be plotted but also type the directory containing the sweepfiles of
the next radar before you replot all frames.

You will then select the fields plotted for each radar in each frame
with the \"Parameters\" widget.

Comparisons usually involves referencing the centers of frames
containing the other radars to the center of Frame 1 or fixing the
centers of all frames at a specified lat/lon. The widget that enables
this is the \"Center\" button in the \"View\" widget.
";

gchar * hlp_sii_about = 
"SoloII was specifically targeted to work using the GTK+ graphics
libraries and the GNU compilers that come with RedHat Linux operating
system and in ATD/RTF systems environment in the SPOL radar and at the
ATD Boulder Foothills Lab.

Resonable attempts will be made to help users with different
environments and OSs get Soloii installed, compiled and running but
Soloii comes with no guarentees of working in any other environments.

Build date: ";

gchar * hlp_sii_ = 
"
";


# endif

