/*      $Id$         */


# ifndef NEX_TABLES_H
# define NEX_TABLES_H



static const char * nex_sites_ptrs[] = {
"# radar sites database- {number,sitename,city,state,lat(ddmmss),lon(ddmmss),",
"# antenna height(msl),frequency(mhz),short pulse(ns),long pulse(ns)}",
"#",
"# CAVEAT EMPTOR!    These numbers should be checked before doing any",
"#   	    	    serious analysis with any data.",
"#",
"# scan with \"%d%s%s%s%d%d%d%d%d%d%f%f%f%f\"",
"#",
"     0  KOUN  Norman                   OK      35 14 10   -97 27 48   381.3  -999     0     0",
"     1  RCWF  ROC                      TW      25 04 22   121 46 21   757.0  -999     0     0",
"     1  TJUA  San_Juan                 PR      18 06 56   -66 04 41   851.8  -999     0     0",
"# Last updated 23 July 97",
"#",
" 14929  KABR  Aberdeen                 SD      45 27 21   -98 24 47   416.9  -999     0     0",
" 54766  KENX  Albany                   NY      42 35 11   -74  3 50   576.6  -999     0     0",
" 03019  KABX  Albuquerque              NM      35  8 59  -106 49 26  1809.2  -999     0     0",
" 03981  KFDR  Altus_AFB                OK      34 21 44   -98 58 35   396.2  -999     0     0",
" 23047  KAMA  Amarillo                 TX      35 14  0  -101 42 33  1113.4  -999     0     0",
" 26548  PAHG  Anchorage                AK      60 43 33  -151 21  5   103.8  -999     0     0",
" 41417  PGUA  Anderson_AFB             GU      13 27  9   144 48 41   110.5  -999     0     0",
" 53819  KFFC  Atlanta                  GA      33 21 49   -84 33 57   291.6  -999     0     0",
" 12971  KEWX  Austin/San_Antonio       TX      29 42 14   -98  1 42   213.0  -999     0     0",
" 93240  KBBX  Beale_AFB                CA      39 29 46  -121 37 54    61.0  -999     0     0",
" 94096  KBLX  Billings                 MT      45 51 14  -108 36 24  1106.7  -999     0     0",
" 04725  KBGM  Binghamton               NY      42 11 59   -75 59  5   509.6  -999     0     0",
" 53823  KBMX  Birmingham               AL      33 10 20   -86 46 11   226.6  -999     0     0",
" 24011  KBIS  Bismarck                 ND      46 46 15  -100 45 38   525.4  -999     0     0",
" 04101  KCBX  Boise                    ID      43 29 26  -116 14  8   953.0  -999     0     0",
" 54765  KBOX  Boston                   MA      41 57 21   -71  8 13    66.0  -999     0     0",
" 12919  KBRO  Brownsville              TX      25 54 58   -97 25  8    17.1  -999     0     0",
" 14733  KBUF  Buffalo                  NY      42 56 56   -78 44 12   231.3  -999     0     0",
" 54774  KCXX  Burlington               VT      44 30 40   -73 10  1   126.7  -999     0     0",
" 43216  RKSG  Camp_Humphreys           Korea   36 57 21   127  1 16    35.9  -999     0     0",
" 03022  KFDX  Cannon_AFB               NM      34 38  7  -103 37 48  1427.4  -999     0     0",
" 53118  KICX  Cedar_City               UT      37 35 27  -112 51 44  3260.9  -999     0     0",
" 53845  KCLX  Charleston               SC      32 39 20   -81  2 32    59.6  -999     0     0",
" 53834  KRLX  Charleston               WV      38 18 40   -81 43 23   359.2  -999     0     0",
" 24018  KCYS  Cheyenne                 WY      41  9  7  -104 48 22  1877.9  -999     0     0",
" 04831  KLOT  Chicago                  IL      41 36 17   -88  5  5   222.1  -999     0     0",
" 13841  KILN  Cincinnati               OH      39 25 13   -83 49 18   351.9  -999     0     0",
" 14820  KCLE  Cleveland                OH      41 24 47   -81 51 35   252.6  -999     0     0",
" 13883  KCAE  Columbia                 SC      33 56 55   -81  7  6   100.5  -999     0     0",
" 53837  KGWX  Columbus_AFB             MS      33 53 48   -88 19 44   165.1  -999     0     0",
" 12924  KCRP  Corpus_Christi           TX      27 47  3   -97 30 40    33.8  -999     0     0",
" 03985  KFWS  Dallas/Ft._Worth         TX      32 34 23   -97 18 11   228.2  -999     0     0",
" 94982  KDVN  Davenport                IA      41 36 42   -90 34 51   249.9  -999     0     0",
" 03018  KFTG  Denver                   CO      39 47 12  -104 32 45  1705.5  -999     0     0",
" 94984  KDMX  Des_Moines               IA      41 43 52   -93 43 22   329.1  -999     0     0",
" 04830  KDTX  Detroit                  MI      42 41 59   -83 28 18   356.8  -999     0     0",
" 13985  KDDC  Dodge_City               KS      37 45 39   -99 58  8   809.5  -999     0     0",
" 93770  KDOX  Dover_AFB                DE      38 49 32   -75 26 23    45.3  -999     0     0",
" 14913  KDLH  Duluth                   MN      46 50 13   -92 12 35   465.3  -999     0     0",
" 03987  KDYX  Dyess_AFB                TX      32 32 18   -99 15 16   472.4  -999     0     0",
" 53114  KEYX  Edwards_AFB              CA      35  5 52  -117 33 39   860.4  -999     0     0",
" 53825  KEVX  Eglin_AFB                FL      30 33 52   -85 55 17    62.7  -999     0     0",
" 03020  KEPZ  El_Paso                  TX      31 52 23  -106 41 53  1280.9  -999     0     0",
" 04108  KLRX  Elko                     NV      40 44 23  -116 48 10  2085.6  -999     0     0",
" 94289  KBHX  Eureka                   CA      40 29 54  -124 17 31   762.2  -999     0     0",
" 24690  PAPD  Fairbanks                AK      65  2  6  -147 30  6   820.4  -999     0     0",
" 53113  KFSX  Flagstaff                AZ      34 34 28  -111 11 52  2280.8  -999     0     0",
" 53839  KHPX  Fort_Campbell            KY      36 44 12   -87 17  6   185.6  -999     0     0",
" 03992  KGRK  Fort_Hood                TX      30 43 19   -97 22 59   174.0  -999     0     0",
" 03993  KPOE  Fort_Polk                LA      31  9 20   -92 58 33   134.4  -999     0     0",
" 53851  KEOX  Fort_Rucker              AL      31 27 38   -85 27 34   142.3  -999     0     0",
" 53906  KSRX  Fort_Smith               AR      35 17 26   -94 21 42   215.1  -999     0     0",
" 04844  KIWX  Fort_Wayne               IN      41 21 32   -85 42  0   317.7  -999     0     0",
" 04837  KAPX  Gaylord                  MI      44 54 26   -84 43 11   466.3  -999     0     0",
" 94008  KGGW  Glasgow                  MT      48 12 23  -106 37 30   713.8  -999     0     0",
" 23065  KGLD  Goodland                 KS      39 22  1  -101 42  1  1122.9  -999     0     0",
" 94986  KMVX  Grand_Forks              ND      47 31 40   -97 19 32   320.6  -999     0     0",
" 03025  KGJX  Grand_Junction           Co      39  3 44  -108 12 50  3055.3  -999     0     0",
" 94860  KGRR  Grand_Rapids             MI      42 53 38   -85 32 41   257.2  -999     0     0",
" 04102  KTFX  Great_Falls              MT      47 27 35  -111 23  7  1142.1  -999     0     0",
" 14898  KGRB  Green_Bay                WI      44 29 54   -88  6 41   237.9  -999     0     0",
" 03870  KGSP  Greer                    SC      34 53  0   -82 13 12   316.6  -999     0     0",
" 54763  KRMX  Griffiss_AFB             NY      43 28  4   -75 27 28   492.1  -999     0     0",
" 94981  KUEX  Hastings                 NE      40 19 15   -98 26 31   622.3  -999     0     0",
" 03023  KHDX  Holloman_AFB             NM      33  4 35  -106  7 22  1296.9  -999     0     0",
" 94625  KCBW  Houlton                  ME      46  2 21   -67 48 23   257.4  -999     0     0",
" 03980  KHGX  Houston/Galveston        TX      29 28 19   -95  4 45    25.5  -999     0     0",
" 53857  KHTX  Huntsville               AL      34 55 50   -86  5  0   551.5  -999     0     0",
" 93819  KIND  Indianapolis             IN      39 42 27   -86 16 49   260.8  -999     0     0",
" 03889  KJKL  Jackson                  KY      37 35 27   -83 18 47   440.8  -999     0     0",
" 03940  KJAN  Jackson                  MS      32 19  4   -90  4 48   110.6  -999     0     0",
" 13889  KJAX  Jacksonville             FL      30 29  5   -81 42  7    30.1  -999     0     0",
" 42219  RODN  Kadena                   Okinawa  26 18  7   127 54 35    96.5  -999     0     0",
" 03983  KEAX  Kansas_City              MO      38 48 37   -94 15 52   323.3  -999     0     0",
" 92804  KBYX  Key_West                 FL      24 35 51   -81 42 11    22.5  -999     0     0",
" 53832  KMRX  Knoxville/Tri-Cities     TN      36 10  7   -83 24  6   427.6  -999     0     0",
" 94987  KARX  La_Crosse                WI      43 49 22   -91 11 28   409.0  -999     0     0",
" 03937  KLCH  Lake_Charles             LA      30  7 31   -93 12 57    24.0  -999     0     0",
" 53110  KESX  Las_Vegas                NV      35 42  4  -114 53 29  1503.5  -999     0     0",
" 22015  KDFX  Laughlin_AFB             TX      29 16 22  -100 16 50   354.8  -999     0     0",
" 04833  KILX  Lincoln                  IL      40  9  2   -89 20 13   207.4  -999     0     0",
" 03952  KLZK  Little_Rock              AR      34 50 11   -92 15 44   193.2  -999     0     0",
" 53101  KVTX  Los_Angeles              CA      34 24 42  -119 10 46   850.9  -999     0     0",
" 53827  KLVX  Louisville               KY      37 58 31   -85 56 38   249.2  -999     0     0",
" 23042  KLBB  Lubbock                  TX      33 39 15  -101 48 51  1013.4  -999     0     0",
" 94850  KMQT  Marquette                MI      46 31 52   -87 32 54   460.1  -999     0     0",
" 53826  KMXX  Maxwell_AFB              AL      32 32 12   -85 47 23   152.0  -999     0     0",
" 94296  KMAX  Medford                  OR      42  4 52  -122 43  2  2300.0  -999     0     0",
" 12838  KMLB  Melbourne                FL      28  6 48   -80 39 15    30.7  -999     0     0",
" 93839  KNQA  Memphis                  TN      35 20 41   -89 52 24   106.0  -999     0     0",
" 12899  KAMX  Miami                    FL      25 36 40   -80 24 46    24.3  -999     0     0",
" 23023  KMAF  Midland/Odessa           TX      31 56 36  -102 11 21   884.2  -999     0     0",
" 04834  KMKX  Milwaukee                WI      42 58  4   -88 33  2   302.0  -999     0     0",
" 94983  KMPX  Minneapolis/St._Paul     MN      44 50 56   -93 33 56   318.4  -999     0     0",
" 94048  KMBX  Minot_AFB                ND      48 23 33  -100 51 54   475.1  -999     0     0",
" 04103  KMSX  Missoula                 MT      47  2 28  -113 59 10  2404.3  -999     0     0",
" 13894  KMOB  Mobile                   AL      30 40 46   -88 14 23    83.4  -999     0     0",
" 22547  PHMO  Molokai                  HI      21  7 58  -157 10 48   435.5  -999     0     0",
" 53856  KVAX  Moody_AFB                GA      30 23 25   -83  0  6    82.8  -999     0     0",
" 93768  KMHX  Morehead_City            NC      34 46 34   -76 52 34    39.5  -999     0     0",
" 53833  KOHX  Nashville                TN      36 14 50   -86 33 45   196.5  -999     0     0",
" 53813  KLIX  New_Orleans              LA      30 20 12   -89 49 32    37.4  -999     0     0",
" 94703  KOKX  New_York_City            NY      40 51 56   -72 51 50    56.0  -999     0     0",
" 93773  KAKQ  Norfolk/Richmond         VA      36 59  2   -77  0 26    54.2  -999     0     0",
" 94049  KLNX  North_Platte             NE      41 57 28  -100 34 35   925.3  -999     0     0",
" 03979  KTLX  Oklahoma_City            OK      35 19 59   -97 16 40   384.8  -999     0     0",
" 94980  KOAX  Omaha                    NE      41 19 13   -96 22  0   380.0  -999     0     0",
" 03816  KPAH  Paducah                  KY      37  4  6   -88 46 19   149.5  -999     0     0",
" 24155  KPDT  Pendleton                OR      45 41 26  -118 51 10   471.8  -999     0     0",
" 93771  KDIX  Philadelphia             PA      39 56 49   -74 24 39    65.5  -999     0     0",
" 23104  KIWA  Phoenix                  AZ      33 17 21  -111 40 12   422.4  -999     0     0",
" 04832  KPBZ  Pittsburgh               PA      40 31 54   -80 13  5   381.2  -999     0     0",
" 04107  KSFX  Pocatello/Idaho_Falls    ID      43  6 21  -112 41 10  1373.7  -999     0     0",
" 54762  KGYX  Portland                 ME      43 53 29   -70 15 23   134.7  -999     0     0",
" 94288  KRTX  Portland                 OR      45 42 53  -122 57 55   509.2  -999     0     0",
" 03021  KPUX  Pueblo                   CO      38 27 34  -104 10 53  1609.9  -999     0     0",
" 93772  KRAX  Raleigh/Durham           NC      35 39 56   -78 29 23   136.1  -999     0     0",
" 94047  KUDX  Rapid_City               SD      44  7 30  -102 49 47   949.3  -999     0     0",
" 53104  KRGX  Reno                     NV      39 45 15  -119 27 44  2549.6  -999     0     0",
" 24061  KRIW  Riverton                 WY      43  3 58  -108 28 38  1707.2  -999     0     0",
" 53831  KFCX  Roanoke                  VA      37  1 28   -80 16 26   894.2  -999     0     0",
" 53824  KJGX  Robins_AFB               GA      32 40 31   -83 21  4   178.9  -999     0     0",
" 93235  KDAX  Sacramento               CA      38 30  4  -121 40 40    39.2  -999     0     0",
" 03982  KLSX  Saint_Louis              MO      38 41 56   -90 40 58   215.4  -999     0     0",
" 04104  KMTX  Salt_Lake_City           UT      41 15 46  -112 26 52  1999.1  -999     0     0",
" 23034  KSJT  San_Angelo               TX      31 22 17  -100 29 33   606.1  -999     0     0",
" 53115  KNKX  San_Diego                CA      32 55  8  -117  2 31   311.1  -999     0     0",
" 93236  KMUX  San_Francisco            CA      37  9 19  -121 53 54  1077.4  -999     0     0",
" 53111  KHNX  San_Joaquin_Valley       CA      36 18 51  -119 37 56    94.1  -999     0     0",
" 53117  KSOX  Santa_Ana_Mountains      CA      33 49  4  -117 38  9   937.7  -999     0     0",
" 94287  KATX  Seattle/Tacoma           WA      48 11 40  -122 29 45   180.6  -999     0     0",
" 13957  KSHV  Shreveport               LA      32 27  3   -93 50 29   113.3  -999     0     0",
" 14944  KFSD  Sioux_Falls              SD      43 35 16   -96 43 46   445.9  -999     0     0",
" 22548  PHKI  South_Kauai              HI      21 53 39  -159 33  8    84.6  -999     0     0",
" 04106  KOTX  Spokane                  WA      47 40 49  -117 37 36   736.7  -999     0     0",
" 13995  KSGF  Springfield              MO      37 14  7   -93 24  2   409.6  -999     0     0",
" 54764  KCCX  State_College            PA      40 55 23   -78  0 13   753.1  -999     0     0",
" 93767  KLWX  Sterling                 VA      38 58 31   -77 28 40   108.0  -999     0     0",
" 93805  KTLH  Tallahassee              FL      30 23 51   -84 19 44    49.3  -999     0     0",
" 92801  KTBW  Tampa                    FL      27 42 20   -82 24  6    32.5  -999     0     0",
" 03986  KTWX  Topeka                   KS      38 59 49   -96 13 57   426.7  -999     0     0",
" 53112  KEMX  Tucson                   AZ      31 53 37  -110 37 49  1616.5  -999     0     0",
" 03984  KINX  Tulsa                    OK      36 10 30   -95 33 53   223.7  -999     0     0",
" 03995  KVNX  Vance_AFB                OK      36 44 27   -98  7 40   378.9  -999     0     0",
" 93234  KVBX  Vandenberg_AFB           CA      34 50 17  -120 23 49   392.8  -999     0     0",
" 03928  KICT  Wichita                  KS      37 39 17   -97 26 34   417.0  -999     0     0",
" 93774  KLTX  Wilmington               NC      33 59 22   -78 25 44    39.6  -999     0     0",
" 53116  KYUX  Yuma                     AZ      32 29 43  -114 39 24    63.1  -999     0     0",
};



static const char * nex_vcps_ptrs[] = {
"#",
"#   	    VCP (volume coverage pattern)",
"#   	    there appear to be 6 basic items defining the scan pattern",
"#   	    scan with \"%s%d%d%d%d%d%x\"",
"# item  1",
"# item  2   is the pattern type",
"# item  3   is the VCP number (this number is on the tape)",
"# item  4   is the number of sweeps",
"# item  5",
"# item  6   is the pulse width flag",
"#",
"#   	FACTOR=180./4096.",
"#   	AZ_FACTOR=22.5/2048.",
"#",
"#   	    there are 17 items in each line for one elevation of a vcp",
"#   	    	the elevation number is also on the tape",
"#   	    scan with \"%x%d%d%d%x%x%d%d%x%x%d%d%x%d%d%d%d\"",
"#",
"# item  1   (fixed_angle*8)/FACTOR",
"# item  2   wave type",
"# item  3   IS PRF number",
"# item  4   IS pulse count",
"# item  5   (az_rate*8)/AZ_FACTOR",
"# item  6",
"# item  7   ID PRF number",
"# item	8   ID pulse count",
"# item  9",
"# item 10",
"# item 11",
"# item 12",
"# item 13",
"# item 14",
"# item 15",
"# item 16",
"# item 17",
"#",
"VCPAT11  346  2   11  16  1  0202 ",
" 0058  1  1   17  3520     0  0    0  0010     0  0   0     0  0 0 0 0",
" 0058  2  0    0  36B0     0  5   52  0030     0  0   0     0  0 0 0 0",
" 0108  1  1   16  3870     0  0    0  0010     0  0   0     0  0 0 0 0",
" 0108  2  0    0  36B0     0  5   52  0030     0  0   0     0  0 0 0 0",
" 01B8  4  1    6  2DD8     0  5   41  0030     0  0   0     0  0 0 0 0",
" 0260  4  2    6  32E8     0  5   41  0030     0  0   0     0  0 0 0 0",
" 0310  4  2    6  32E8     0  5   41  0030     0  0   0     0  0 0 0 0",
" 03B8  4  3   10  31A8     0  5   41  0030     0  0   0     0  0 0 0 0",
" 0468  4  3   10  31B0     0  5   41  0030     0  0   0     0  0 0 0 0",
" 0558  3  0    0  4798     0  6   43  0030     0  0   0     0  0 0 0 0",
" 0630  3  0    0  4840     0  7   46  0030     0  0   0     0  0 0 0 0",
" 0720  3  0    0  4850     0  7   46  0030     0  0   0     0  0 0 0 0",
" 0888  3  0    0  4870     0  7   46  0030     0  0   0     0  0 0 0 0",
" 09F8  3  0    0  4890     0  7   46  0030     0  0   0     0  0 0 0 0",
" 0BE0  3  0    0  48D0     0  7   46  0030     0  0   0     0  0 0 0 0",
" 0DE0  3  0    0  4918     0  7   46  0030     0  0   0     0  0 0 0 0",
"#",
"VCPAT21  346  2   21  11  1  0202 ",
" 0058  1  1   28  2040     0  0    0  0010     0  0   0     0  0 0 0 0",
" 0058  2  0    0  2050     0  5   88  0030     0  0   0     0  0 0 0 0",
" 0108  1  1   28  2040     0  0    0  0010     0  0   0     0  0 0 0 0",
" 0108  2  0    0  2050     0  5   88  0030     0  0   0     0  0 0 0 0",
" 01B8  4  2    8  1ED0     0  5   70  0030     0  0   0     0  0 0 0 0",
" 0260  4  2    8  1ED0     0  5   70  0030     0  0   0     0  0 0 0 0",
" 0310  4  2    8  1FD0     0  5   70  0030     0  0   0     0  0 0 0 0",
" 0448  4  3   12  1FD0     0  5   70  0030     0  0   0     0  0 0 0 0",
" 0708  3  0    0  2990     0  7   82  0030     0  0   0     0  0 0 0 0",
" 0A60  3  0    0  28C0     0  7   82  0030     0  0   0     0  0 0 0 0",
" 0DE0  3  0    0  2900     0  7   82  0030     0  0   0     0  0 0 0 0",
"#",
"VCPAT31  346  2   31   8  1  0204 ",
" 0058  1  1   63  0E58     0  0    0  0000     0  0   0     0  0 0 0 0",
" 0058  2  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0",
" 0110  1  1   63  0E58     0  0    0  0000     0  0   0     0  0 0 0 0",
" 0110  2  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0",
" 01C8  1  1   63  0E58     0  0    0  0000     0  0   0     0  0 0 0 0",
" 01C8  2  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0",
" 0280  3  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0",
" 0330  3  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0",
"#",
"VCPAT32  346  2   32   7  1  0202 ",
" 0058  1  1   64  0E20     0  0    0  0004     0  0   0     0  0 0 0 0",
" 0058  2  0    0  0CF0     0  5  220  0004     0  0   0     0  0 0 0 0",
" 0110  1  1   64  0E20     0  0    0  0004     0  0   0     0  0 0 0 0",
" 0110  2  0    0  0CF0     0  5  220  0004     0  0   0     0  0 0 0 0",
" 01C8  4  2   11  0B90     0  5  220  0008     0  0   0     0  0 0 0 0",
" 0280  4  2   11  0B90     0  5  220  0008     0  0   0     0  0 0 0 0",
" 0330  4  2   11  0B90     0  5  220  0008     0  0   0     0  0 0 0 0",
"#",
"VCPAT300 346  2  300   4  1  0202 ",
" 0058  1  1   28  2040     0  0    0  FFF4     0  0   0     0  0 0 0 0",
" 0058  2  0    0  2050  9558  5   88  0021  EE38  5  88  0015  0 0 0 0 ",
" 01B8  4  2    8  1FD0  9558  5   70  0019  EE38  5  70  0015  0 0 0 0 ",
" 0708  3  0    0  2890     0  7   82  0019     0  0   0     0  0 0 0 0",
"# ",
};




static const char * nex_unamb_ranges_ptrs[] = {
"#",
"# unambigous range values for each prf within a delta",
"# there are 8 ranges and 5 deltas",
"#",
" 460  332  230  173  146  135  125  115",
" 463  334  232  174  147  136  126  116",
" 466  336  233  175  148  137  127  117",
" 468  338  234  176  149  138  128  118",
" 471  340  236  177  150  139  129  119",
};


# endif

# ifdef obsolete

static const char * nex_sites =
"# radar sites database- {number,sitename,city,state,lat(ddmmss),lon(ddmmss),
# antenna height(msl),frequency(mhz),short pulse(ns),long pulse(ns)}
#
# CAVEAT EMPTOR!    These numbers should be checked before doing any
#   	    	    serious analysis with any data.
#
# scan with \"%d%s%s%s%d%d%d%d%d%d%f%f%f%f\"
#
     0  KOUN  Norman                   OK      35 14 10   -97 27 48   381.3  -999     0     0
     1  RCWF  ROC                      TW      25 04 22   121 46 21   757.0  -999     0     0
# Last updated 23 July 97
#
 14929  KABR  Aberdeen                 SD      45 27 21   -98 24 47   416.9  -999     0     0
 54766  KENX  Albany                   NY      42 35 11   -74  3 50   576.6  -999     0     0
 03019  KABX  Albuquerque              NM      35  8 59  -106 49 26  1809.2  -999     0     0
 03981  KFDR  Altus_AFB                OK      34 21 44   -98 58 35   396.2  -999     0     0
 23047  KAMA  Amarillo                 TX      35 14  0  -101 42 33  1113.4  -999     0     0
 26548  PAHG  Anchorage                AK      60 43 33  -151 21  5   103.8  -999     0     0
 41417  PGUA  Anderson_AFB             GU      13 27  9   144 48 41   110.5  -999     0     0
 53819  KFFC  Atlanta                  GA      33 21 49   -84 33 57   291.6  -999     0     0
 12971  KEWX  Austin/San_Antonio       TX      29 42 14   -98  1 42   213.0  -999     0     0
 93240  KBBX  Beale_AFB                CA      39 29 46  -121 37 54    61.0  -999     0     0
 94096  KBLX  Billings                 MT      45 51 14  -108 36 24  1106.7  -999     0     0
 04725  KBGM  Binghamton               NY      42 11 59   -75 59  5   509.6  -999     0     0
 53823  KBMX  Birmingham               AL      33 10 20   -86 46 11   226.6  -999     0     0
 24011  KBIS  Bismarck                 ND      46 46 15  -100 45 38   525.4  -999     0     0
 04101  KCBX  Boise                    ID      43 29 26  -116 14  8   953.0  -999     0     0
 54765  KBOX  Boston                   MA      41 57 21   -71  8 13    66.0  -999     0     0
 12919  KBRO  Brownsville              TX      25 54 58   -97 25  8    17.1  -999     0     0
 14733  KBUF  Buffalo                  NY      42 56 56   -78 44 12   231.3  -999     0     0
 54774  KCXX  Burlington               VT      44 30 40   -73 10  1   126.7  -999     0     0
 43216  RKSG  Camp_Humphreys           Korea   36 57 21   127  1 16    35.9  -999     0     0
 03022  KFDX  Cannon_AFB               NM      34 38  7  -103 37 48  1427.4  -999     0     0
 53118  KICX  Cedar_City               UT      37 35 27  -112 51 44  3260.9  -999     0     0
 53845  KCLX  Charleston               SC      32 39 20   -81  2 32    59.6  -999     0     0
 53834  KRLX  Charleston               WV      38 18 40   -81 43 23   359.2  -999     0     0
 24018  KCYS  Cheyenne                 WY      41  9  7  -104 48 22  1877.9  -999     0     0
 04831  KLOT  Chicago                  IL      41 36 17   -88  5  5   222.1  -999     0     0
 13841  KILN  Cincinnati               OH      39 25 13   -83 49 18   351.9  -999     0     0
 14820  KCLE  Cleveland                OH      41 24 47   -81 51 35   252.6  -999     0     0
 13883  KCAE  Columbia                 SC      33 56 55   -81  7  6   100.5  -999     0     0
 53837  KGWX  Columbus_AFB             MS      33 53 48   -88 19 44   165.1  -999     0     0
 12924  KCRP  Corpus_Christi           TX      27 47  3   -97 30 40    33.8  -999     0     0
 03985  KFWS  Dallas/Ft._Worth         TX      32 34 23   -97 18 11   228.2  -999     0     0
 94982  KDVN  Davenport                IA      41 36 42   -90 34 51   249.9  -999     0     0
 03018  KFTG  Denver                   CO      39 47 12  -104 32 45  1705.5  -999     0     0
 94984  KDMX  Des_Moines               IA      41 43 52   -93 43 22   329.1  -999     0     0
 04830  KDTX  Detroit                  MI      42 41 59   -83 28 18   356.8  -999     0     0
 13985  KDDC  Dodge_City               KS      37 45 39   -99 58  8   809.5  -999     0     0
 93770  KDOX  Dover_AFB                DE      38 49 32   -75 26 23    45.3  -999     0     0
 14913  KDLH  Duluth                   MN      46 50 13   -92 12 35   465.3  -999     0     0
 03987  KDYX  Dyess_AFB                TX      32 32 18   -99 15 16   472.4  -999     0     0
 53114  KEYX  Edwards_AFB              CA      35  5 52  -117 33 39   860.4  -999     0     0
 53825  KEVX  Eglin_AFB                FL      30 33 52   -85 55 17    62.7  -999     0     0
 03020  KEPZ  El_Paso                  TX      31 52 23  -106 41 53  1280.9  -999     0     0
 04108  KLRX  Elko                     NV      40 44 23  -116 48 10  2085.6  -999     0     0
 94289  KBHX  Eureka                   CA      40 29 54  -124 17 31   762.2  -999     0     0
 24690  PAPD  Fairbanks                AK      65  2  6  -147 30  6   820.4  -999     0     0
 53113  KFSX  Flagstaff                AZ      34 34 28  -111 11 52  2280.8  -999     0     0
 53839  KHPX  Fort_Campbell            KY      36 44 12   -87 17  6   185.6  -999     0     0
 03992  KGRK  Fort_Hood                TX      30 43 19   -97 22 59   174.0  -999     0     0
 03993  KPOE  Fort_Polk                LA      31  9 20   -92 58 33   134.4  -999     0     0
 53851  KEOX  Fort_Rucker              AL      31 27 38   -85 27 34   142.3  -999     0     0
 53906  KSRX  Fort_Smith               AR      35 17 26   -94 21 42   215.1  -999     0     0
 04844  KIWX  Fort_Wayne               IN      41 21 32   -85 42  0   317.7  -999     0     0
 04837  KAPX  Gaylord                  MI      44 54 26   -84 43 11   466.3  -999     0     0
 94008  KGGW  Glasgow                  MT      48 12 23  -106 37 30   713.8  -999     0     0
 23065  KGLD  Goodland                 KS      39 22  1  -101 42  1  1122.9  -999     0     0
 94986  KMVX  Grand_Forks              ND      47 31 40   -97 19 32   320.6  -999     0     0
 03025  KGJX  Grand_Junction           Co      39  3 44  -108 12 50  3055.3  -999     0     0
 94860  KGRR  Grand_Rapids             MI      42 53 38   -85 32 41   257.2  -999     0     0
 04102  KTFX  Great_Falls              MT      47 27 35  -111 23  7  1142.1  -999     0     0
 14898  KGRB  Green_Bay                WI      44 29 54   -88  6 41   237.9  -999     0     0
 03870  KGSP  Greer                    SC      34 53  0   -82 13 12   316.6  -999     0     0
 54763  KRMX  Griffiss_AFB             NY      43 28  4   -75 27 28   492.1  -999     0     0
 94981  KUEX  Hastings                 NE      40 19 15   -98 26 31   622.3  -999     0     0
 03023  KHDX  Holloman_AFB             NM      33  4 35  -106  7 22  1296.9  -999     0     0
 94625  KCBW  Houlton                  ME      46  2 21   -67 48 23   257.4  -999     0     0
 03980  KHGX  Houston/Galveston        TX      29 28 19   -95  4 45    25.5  -999     0     0
 53857  KHTX  Huntsville               AL      34 55 50   -86  5  0   551.5  -999     0     0
 93819  KIND  Indianapolis             IN      39 42 27   -86 16 49   260.8  -999     0     0
 03889  KJKL  Jackson                  KY      37 35 27   -83 18 47   440.8  -999     0     0
 03940  KJAN  Jackson                  MS      32 19  4   -90  4 48   110.6  -999     0     0
 13889  KJAX  Jacksonville             FL      30 29  5   -81 42  7    30.1  -999     0     0
 42219  RODN  Kadena                   Okinawa  26 18  7   127 54 35    96.5  -999     0     0
 03983  KEAX  Kansas_City              MO      38 48 37   -94 15 52   323.3  -999     0     0
 92804  KBYX  Key_West                 FL      24 35 51   -81 42 11    22.5  -999     0     0
 53832  KMRX  Knoxville/Tri-Cities     TN      36 10  7   -83 24  6   427.6  -999     0     0
 94987  KARX  La_Crosse                WI      43 49 22   -91 11 28   409.0  -999     0     0
 03937  KLCH  Lake_Charles             LA      30  7 31   -93 12 57    24.0  -999     0     0
 53110  KESX  Las_Vegas                NV      35 42  4  -114 53 29  1503.5  -999     0     0
 22015  KDFX  Laughlin_AFB             TX      29 16 22  -100 16 50   354.8  -999     0     0
 04833  KILX  Lincoln                  IL      40  9  2   -89 20 13   207.4  -999     0     0
 03952  KLZK  Little_Rock              AR      34 50 11   -92 15 44   193.2  -999     0     0
 53101  KVTX  Los_Angeles              CA      34 24 42  -119 10 46   850.9  -999     0     0
 53827  KLVX  Louisville               KY      37 58 31   -85 56 38   249.2  -999     0     0
 23042  KLBB  Lubbock                  TX      33 39 15  -101 48 51  1013.4  -999     0     0
 94850  KMQT  Marquette                MI      46 31 52   -87 32 54   460.1  -999     0     0
 53826  KMXX  Maxwell_AFB              AL      32 32 12   -85 47 23   152.0  -999     0     0
 94296  KMAX  Medford                  OR      42  4 52  -122 43  2  2300.0  -999     0     0
 12838  KMLB  Melbourne                FL      28  6 48   -80 39 15    30.7  -999     0     0
 93839  KNQA  Memphis                  TN      35 20 41   -89 52 24   106.0  -999     0     0
 12899  KAMX  Miami                    FL      25 36 40   -80 24 46    24.3  -999     0     0
 23023  KMAF  Midland/Odessa           TX      31 56 36  -102 11 21   884.2  -999     0     0
 04834  KMKX  Milwaukee                WI      42 58  4   -88 33  2   302.0  -999     0     0
 94983  KMPX  Minneapolis/St._Paul     MN      44 50 56   -93 33 56   318.4  -999     0     0
 94048  KMBX  Minot_AFB                ND      48 23 33  -100 51 54   475.1  -999     0     0
 04103  KMSX  Missoula                 MT      47  2 28  -113 59 10  2404.3  -999     0     0
 13894  KMOB  Mobile                   AL      30 40 46   -88 14 23    83.4  -999     0     0
 22547  PHMO  Molokai                  HI      21  7 58  -157 10 48   435.5  -999     0     0
 53856  KVAX  Moody_AFB                GA      30 23 25   -83  0  6    82.8  -999     0     0
 93768  KMHX  Morehead_City            NC      34 46 34   -76 52 34    39.5  -999     0     0
 53833  KOHX  Nashville                TN      36 14 50   -86 33 45   196.5  -999     0     0
 53813  KLIX  New_Orleans              LA      30 20 12   -89 49 32    37.4  -999     0     0
 94703  KOKX  New_York_City            NY      40 51 56   -72 51 50    56.0  -999     0     0
 93773  KAKQ  Norfolk/Richmond         VA      36 59  2   -77  0 26    54.2  -999     0     0
 94049  KLNX  North_Platte             NE      41 57 28  -100 34 35   925.3  -999     0     0
 03979  KTLX  Oklahoma_City            OK      35 19 59   -97 16 40   384.8  -999     0     0
 94980  KOAX  Omaha                    NE      41 19 13   -96 22  0   380.0  -999     0     0
 03816  KPAH  Paducah                  KY      37  4  6   -88 46 19   149.5  -999     0     0
 24155  KPDT  Pendleton                OR      45 41 26  -118 51 10   471.8  -999     0     0
 93771  KDIX  Philadelphia             PA      39 56 49   -74 24 39    65.5  -999     0     0
 23104  KIWA  Phoenix                  AZ      33 17 21  -111 40 12   422.4  -999     0     0
 04832  KPBZ  Pittsburgh               PA      40 31 54   -80 13  5   381.2  -999     0     0
 04107  KSFX  Pocatello/Idaho_Falls    ID      43  6 21  -112 41 10  1373.7  -999     0     0
 54762  KGYX  Portland                 ME      43 53 29   -70 15 23   134.7  -999     0     0
 94288  KRTX  Portland                 OR      45 42 53  -122 57 55   509.2  -999     0     0
 03021  KPUX  Pueblo                   CO      38 27 34  -104 10 53  1609.9  -999     0     0
 93772  KRAX  Raleigh/Durham           NC      35 39 56   -78 29 23   136.1  -999     0     0
 94047  KUDX  Rapid_City               SD      44  7 30  -102 49 47   949.3  -999     0     0
 53104  KRGX  Reno                     NV      39 45 15  -119 27 44  2549.6  -999     0     0
 24061  KRIW  Riverton                 WY      43  3 58  -108 28 38  1707.2  -999     0     0
 53831  KFCX  Roanoke                  VA      37  1 28   -80 16 26   894.2  -999     0     0
 53824  KJGX  Robins_AFB               GA      32 40 31   -83 21  4   178.9  -999     0     0
 93235  KDAX  Sacramento               CA      38 30  4  -121 40 40    39.2  -999     0     0
 03982  KLSX  Saint_Louis              MO      38 41 56   -90 40 58   215.4  -999     0     0
 04104  KMTX  Salt_Lake_City           UT      41 15 46  -112 26 52  1999.1  -999     0     0
 23034  KSJT  San_Angelo               TX      31 22 17  -100 29 33   606.1  -999     0     0
 53115  KNKX  San_Diego                CA      32 55  8  -117  2 31   311.1  -999     0     0
 93236  KMUX  San_Francisco            CA      37  9 19  -121 53 54  1077.4  -999     0     0
 53111  KHNX  San_Joaquin_Valley       CA      36 18 51  -119 37 56    94.1  -999     0     0
 53117  KSOX  Santa_Ana_Mountains      CA      33 49  4  -117 38  9   937.7  -999     0     0
 94287  KATX  Seattle/Tacoma           WA      48 11 40  -122 29 45   180.6  -999     0     0
 13957  KSHV  Shreveport               LA      32 27  3   -93 50 29   113.3  -999     0     0
 14944  KFSD  Sioux_Falls              SD      43 35 16   -96 43 46   445.9  -999     0     0
 22548  PHKI  South_Kauai              HI      21 53 39  -159 33  8    84.6  -999     0     0
 04106  KOTX  Spokane                  WA      47 40 49  -117 37 36   736.7  -999     0     0
 13995  KSGF  Springfield              MO      37 14  7   -93 24  2   409.6  -999     0     0
 54764  KCCX  State_College            PA      40 55 23   -78  0 13   753.1  -999     0     0
 93767  KLWX  Sterling                 VA      38 58 31   -77 28 40   108.0  -999     0     0
 93805  KTLH  Tallahassee              FL      30 23 51   -84 19 44    49.3  -999     0     0
 92801  KTBW  Tampa                    FL      27 42 20   -82 24  6    32.5  -999     0     0
 03986  KTWX  Topeka                   KS      38 59 49   -96 13 57   426.7  -999     0     0
 53112  KEMX  Tucson                   AZ      31 53 37  -110 37 49  1616.5  -999     0     0
 03984  KINX  Tulsa                    OK      36 10 30   -95 33 53   223.7  -999     0     0
 03995  KVNX  Vance_AFB                OK      36 44 27   -98  7 40   378.9  -999     0     0
 93234  KVBX  Vandenberg_AFB           CA      34 50 17  -120 23 49   392.8  -999     0     0
 03928  KICT  Wichita                  KS      37 39 17   -97 26 34   417.0  -999     0     0
 93774  KLTX  Wilmington               NC      33 59 22   -78 25 44    39.6  -999     0     0
 53116  KYUX  Yuma                     AZ      32 29 43  -114 39 24    63.1  -999     0     0
";

static const char * nex_vcps =
"#
#   	    VCP (volume coverage pattern)
#   	    there appear to be 6 basic items defining the scan pattern
#   	    scan with \"%s%d%d%d%d%d%x\"
# item  1
# item  2   is the pattern type
# item  3   is the VCP number (this number is on the tape)
# item  4   is the number of sweeps
# item  5
# item  6   is the pulse width flag
#
#   	FACTOR=180./4096.
#   	AZ_FACTOR=22.5/2048.
#
#   	    there are 17 items in each line for one elevation of a vcp
#   	    	the elevation number is also on the tape
#   	    scan with \"%x%d%d%d%x%x%d%d%x%x%d%d%x%d%d%d%d\"
#
# item  1   (fixed_angle*8)/FACTOR
# item  2   wave type
# item  3   IS PRF number
# item  4   IS pulse count
# item  5   (az_rate*8)/AZ_FACTOR
# item  6
# item  7   ID PRF number
# item	8   ID pulse count
# item  9
# item 10
# item 11
# item 12
# item 13
# item 14
# item 15
# item 16
# item 17
#
VCPAT11  346  2   11  16  1  0202 
 0058  1  1   17  3520     0  0    0  0010     0  0   0     0  0 0 0 0
 0058  2  0    0  36B0     0  5   52  0030     0  0   0     0  0 0 0 0
 0108  1  1   16  3870     0  0    0  0010     0  0   0     0  0 0 0 0
 0108  2  0    0  36B0     0  5   52  0030     0  0   0     0  0 0 0 0
 01B8  4  1    6  2DD8     0  5   41  0030     0  0   0     0  0 0 0 0
 0260  4  2    6  32E8     0  5   41  0030     0  0   0     0  0 0 0 0
 0310  4  2    6  32E8     0  5   41  0030     0  0   0     0  0 0 0 0
 03B8  4  3   10  31A8     0  5   41  0030     0  0   0     0  0 0 0 0
 0468  4  3   10  31B0     0  5   41  0030     0  0   0     0  0 0 0 0
 0558  3  0    0  4798     0  6   43  0030     0  0   0     0  0 0 0 0
 0630  3  0    0  4840     0  7   46  0030     0  0   0     0  0 0 0 0
 0720  3  0    0  4850     0  7   46  0030     0  0   0     0  0 0 0 0
 0888  3  0    0  4870     0  7   46  0030     0  0   0     0  0 0 0 0
 09F8  3  0    0  4890     0  7   46  0030     0  0   0     0  0 0 0 0
 0BE0  3  0    0  48D0     0  7   46  0030     0  0   0     0  0 0 0 0
 0DE0  3  0    0  4918     0  7   46  0030     0  0   0     0  0 0 0 0
#
VCPAT21  346  2   21  11  1  0202 
 0058  1  1   28  2040     0  0    0  0010     0  0   0     0  0 0 0 0
 0058  2  0    0  2050     0  5   88  0030     0  0   0     0  0 0 0 0
 0108  1  1   28  2040     0  0    0  0010     0  0   0     0  0 0 0 0
 0108  2  0    0  2050     0  5   88  0030     0  0   0     0  0 0 0 0
 01B8  4  2    8  1ED0     0  5   70  0030     0  0   0     0  0 0 0 0
 0260  4  2    8  1ED0     0  5   70  0030     0  0   0     0  0 0 0 0
 0310  4  2    8  1FD0     0  5   70  0030     0  0   0     0  0 0 0 0
 0448  4  3   12  1FD0     0  5   70  0030     0  0   0     0  0 0 0 0
 0708  3  0    0  2990     0  7   82  0030     0  0   0     0  0 0 0 0
 0A60  3  0    0  28C0     0  7   82  0030     0  0   0     0  0 0 0 0
 0DE0  3  0    0  2900     0  7   82  0030     0  0   0     0  0 0 0 0
#
VCPAT31  346  2   31   8  1  0204 
 0058  1  1   63  0E58     0  0    0  0000     0  0   0     0  0 0 0 0
 0058  2  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0
 0110  1  1   63  0E58     0  0    0  0000     0  0   0     0  0 0 0 0
 0110  2  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0
 01C8  1  1   63  0E58     0  0    0  0000     0  0   0     0  0 0 0 0
 01C8  2  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0
 0280  3  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0
 0330  3  0    0  0E68     0  2   87  0000     0  0   0     0  0 0 0 0
#
VCPAT32  346  2   32   7  1  0202 
 0058  1  1   64  0E20     0  0    0  0004     0  0   0     0  0 0 0 0
 0058  2  0    0  0CF0     0  5  220  0004     0  0   0     0  0 0 0 0
 0110  1  1   64  0E20     0  0    0  0004     0  0   0     0  0 0 0 0
 0110  2  0    0  0CF0     0  5  220  0004     0  0   0     0  0 0 0 0
 01C8  4  2   11  0B90     0  5  220  0008     0  0   0     0  0 0 0 0
 0280  4  2   11  0B90     0  5  220  0008     0  0   0     0  0 0 0 0
 0330  4  2   11  0B90     0  5  220  0008     0  0   0     0  0 0 0 0
#
VCPAT300 346  2  300   4  1  0202 
 0058  1  1   28  2040     0  0    0  FFF4     0  0   0     0  0 0 0 0
 0058  2  0    0  2050  9558  5   88  0021  EE38  5  88  0015  0 0 0 0 
 01B8  4  2    8  1FD0  9558  5   70  0019  EE38  5  70  0015  0 0 0 0 
 0708  3  0    0  2890     0  7   82  0019     0  0   0     0  0 0 0 0
#
";

static const char * nex_unamb_ranges =
"#
# unambigous range values for each prf within a delta
# there are 8 ranges and 5 deltas
#
 460  332  230  173  146  135  125  115
 463  334  232  174  147  136  126  116
 466  336  233  175  148  137  127  117
 468  338  234  176  149  138  128  118
 471  340  236  177  150  139  129  119
";

# endif
