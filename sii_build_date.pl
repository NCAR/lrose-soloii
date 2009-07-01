#!/bin/env perl

($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst)
    = localtime (time);
#print "$sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst\n";

$tstamp = sprintf 'static char *sii_date_stamp = "%02d/%02d/%02d  %02d%02d";'
    , ($mon+1,$mday,$year%100,$hour,$min);

print "$tstamp\n";

$ii = `echo '$tstamp' > ../include/date_stamp.h`;
