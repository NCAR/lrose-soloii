/* 	$Id$	 */

# ifndef HELP_PARAM_H
# define HELP_PARAM_H

static const gchar * hlp_param_overview[] = {
"The colors that are used to depict a data field are controlled by a",
"palette. This widget enables adjusting the palette for each field.",
" ",
"The exceeded color is assigned to data values beyond the limits of",
"the color bar. The missing data color is assigned to data values",
"flagged as missing or bad.",
" ",
"Entries containing two values must have the values seperated by a",
"comma and/or a space.",
" ",
"When the \"Emphasis Min/Max\" values are not equal, the emphasis",
"color is assigned to data values within the emphasis limits.",
};

static const gchar * hlp_param_file[] = {
"The \"File\" menu enables closing or cancelling the Parameter widget",
"and displaying lists of palettes, color tables, and colors. Clicking",
"in any of these lists selects an entry. With the exception of colors",
"this causes an immediate change in the entries in this widget.",
" ",
"A clicked color name is placed in the clipboard and can be used to",
"replace any color entry in the palette by clicking in the entry and",
"typing Ctrl+V.",
" ",
"Under \"Broadcast...\" there are options to broadcast the grid color,",
"the boundary color and color bar orientation to all the other windows.",
"Before broadcasting make sure the source frame for broadcasting has",
"been replotted with any new settings since hardware colors are ",
"established only during plotting.",
};

static const gchar * hlp_param_options[] = {
"The \"Options\" menu enables choosing the location of the color bar,",
"assigning a set of symbols to the colors of the color bar, removing",
"highlighting from the title and colorbar labels, and immediately",
"plotting the clicked field in the \"Parameters\" list.",
" ",
"There are two sets of symbols currently in place. One for the \"PD\"",
"and \"WPD\" fields and another for the fields \"RR_DXD,RNX,RZD,RKD\"",
"which are log10 of the rain rate. The symbols are the associated linear",
"values which give the appearance of a log scale.",
" ",
"The particle fields symbol set assumes a 17 color table and the",
"rain rate fields an 11 color table. The particle symbols have the",
"following meanings:",
" ",
"cld  = Cloud                          ",
"drz  = Drizzle                         ",
"lrn  = Light Rain                      ",
"mrn  = Moderate Rain                   ",
"hrn  = Heavy Rain                      ",
"hail = Hail                            ",
"rhm  = Rain Hail Mixture               ",
"gsh  = Graupel Small Hail              ",
"grn  = Graupel Rain                    ",
"dsn  = Dry Snow                        ",
"wsn  = Wet Snow                        ",
"icr  = Ice Crystals                    ",
"iicr = Irreg Ice Crystals              ",
"sld  = Super Cooled Liquid Droplets    ",
"bgs  = Insects                         ",
"2tr  = 2nd Trip Echoes (positive Ldr only)                ",
"gclt = Ground Clutter                  ",
};

static const gchar * hlp_param_links[] = {
"As a default the parameter information is not linked to any other",
"frame.  Links are useful when you are plotting the same field from",
"different sweepfiles and want to be able to change the field in",
"several frames at once.",
};

static const gchar * hlp_param_palettes[] = {
"A palette contains most of the information you see in this widget",
"with the exception of the parameters list and the parameter name.",
"In addition it contains a list of fields that are likely to use",
"this palette. This list appears next to the palette name in the",
"palettes list.",
" ",
"If you wish to define a new palette for a parameter, change all",
"items you wish to be different, type a new name in the",
"\"Color Palette\" entry and hit <return>.",
" ",
"The last palette selected or modified is always at the top of the",
"list of palettes. Since a field can exist is more than one palette,",
"the search for a palette to be associated with a field is always",
"based on the first encounter of a field.",
};

static const gchar * hlp_param_minmax[] = {
"Soloii always uses the center, increment and the number of colors",
"to define the limits and resolution of the data displayed. Typing",
"a min. and max. values in the \"Min & Max\" entry and hitting ",
"<return> causes the center and increment to be adjusted to fit",
"the min. and max. The center and increment can then be adjusted",
"if necessary to provide more desirable labeling.",
};



# endif

# ifdef obsolete
static const gchar * hlp_param_overview =
"
The colors that are used to depict a data field are controlled by a
palette. This widget enables adjusting the palette for each field.

The exceeded color is assigned to data values beyond the limits of
the color bar. The missing data color is assigned to data values
flagged as missing or bad.

Entries containing two values must have the values seperated by a
comma and/or a space.

When the \"Emphasis Min/Max\" values are not equal, the emphasis
color is assigned to data values within the emphasis limits.
";

static const gchar * hlp_param_file =
"
The \"File\" menu enables closing or cancelling the Parameter widget
and displaying lists of palettes, color tables, and colors. Clicking
in any of these lists selects an entry. With the exception of colors
this causes an immediate change in the entries in this widget.

A clicked color name is placed in the clipboard and can be used to
replace any color entry in the palette by clicking in the entry and
typing Ctrl+V.

Under \"Broadcast...\" there are options to broadcast the grid color,
the boundary color and color bar orientation to all the other windows.
Before broadcasting make sure the source frame for broadcasting has
been replotted with any new settings since hardware colors are 
established only during plotting.
";

static const gchar * hlp_param_options =
"
The \"Options\" menu enables choosing the location of the color bar,
assigning a set of symbols to the colors of the color bar, removing
highlighting from the title and colorbar labels, and immediately
plotting the clicked field in the \"Parameters\" list.

There are two sets of symbols currently in place. One for the \"PD\"
and \"WPD\" fields and another for the fields \"RR_DXD,RNX,RZD,RKD\"
which are log10 of the rain rate. The symbols are the associated linear
values which give the appearance of a log scale.

The particle fields symbol set assumes a 17 color table and the
rain rate fields an 11 color table. The particle symbols have the
following meanings:

cld  = Cloud                          
drz  = Drizzle                         
lrn  = Light Rain                      
mrn  = Moderate Rain                   
hrn  = Heavy Rain                      
hail = Hail                            
rhm  = Rain Hail Mixture               
gsh  = Graupel Small Hail              
grn  = Graupel Rain                    
dsn  = Dry Snow                        
wsn  = Wet Snow                        
icr  = Ice Crystals                    
iicr = Irreg Ice Crystals              
sld  = Super Cooled Liquid Droplets    
bgs  = Insects                         
2tr  = 2nd Trip Echoes (positive Ldr only)                
gclt = Ground Clutter                  
";

static const gchar * hlp_param_links =
"
As a default the parameter information is not linked to any other
frame.  Links are useful when you are plotting the same field from
different sweepfiles and want to be able to change the field in
several frames at once.
";

static const gchar * hlp_param_palettes =
"
A palette contains most of the information you see in this widget
with the exception of the parameters list and the parameter name.
In addition it contains a list of fields that are likely to use
this palette. This list appears next to the palette name in the
palettes list.

If you wish to define a new palette for a parameter, change all
items you wish to be different, type a new name in the
\"Color Palette\" entry and hit <return>.

The last palette selected or modified is always at the top of the
list of palettes. Since a field can exist is more than one palette,
the search for a palette to be associated with a field is always
based on the first encounter of a field.
";

static const gchar * hlp_param_minmax =
"
Soloii always uses the center, increment and the number of colors
to define the limits and resolution of the data displayed. Typing
a min. and max. values in the \"Min & Max\" entry and hitting 
<return> causes the center and increment to be adjusted to fit
the min. and max. The center and increment can then be adjusted
if necessary to provide more desirable labeling.
";

static const gchar * hlp_param_emphasis =
"
";

static const gchar * hlp_param_ =
"
";

# endif







