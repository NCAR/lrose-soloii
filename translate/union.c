# define X2(x) (swap2((char *)&(x)))
# define X4(x) (swap4((char *)&(x)))

/*
 * many of the parameters in the field specific parameters need
 * to be divide by the scale to get scientific units.
 * 
 */
struct fsrpg {
    short word_20;
    short word_21;
    short word_22;
    short word_23;
    short word_24;
    short word_25;
};
/*
 * for velocity data:
 * word 20: short nyq_vel;
 * word 21: char  flagged[2];
 *   contains "FL" if flagged velocities
 *   i.e if the low order bit of this field is
 *     0 than the velocity is unreliable and if it is
 *     1 than the velocity is considered ok
 */
struct fsrpv {
    short nyq_vel;		/* scaled */
    char flagged[2];		/* could contain an "FL" */
    short vword_22;
    short vword_23;
    short vword_24;
    short vword_25;
};
 
/*
 * for power fields:
 * word 20: short rad_const;
 * word 21: short noise_power;
 * word 22: short rec_gain;
 * word 23: short peak_power;
 * word 24: short ant_gain;
 * word 25: short pulse_dur_x64e6;
 */
struct fsrpp {
    short rad_const;		/* scaled */
    short noise_power;		/* scaled */
    short rec_gain;		/* scaled */
    short peak_power;		/* scaled */
    short ant_gain;		/* scaled */
    short pulse_dur_x64e6;
};

union abc { /* the next six words are field type specdific */
    struct fsrpg fsrpg;
    struct fsrpv fsrpv;
    struct fsrpp fsrpp;
} fsrp;


/* c------------------------------------------------------------------------ */

main() {
    union abc *def;
    int i=8;
    short swap2();
    long swap4();

    i = swap2((char *)&i);
    i = X2(i);

    def = &fsrp;
    def->fsrpp.rad_const = 1.;
    fsrp.fsrpp.rad_const = 1.;
}
/* c------------------------------------------------------------------------ */

short swap2( ov )		/* swap integer*2 */
  char *ov;
{
    union {
	short newval;
	char nv[2];
    }u;
    u.nv[1] = *ov++; u.nv[0] = *ov++;
    return(u.newval);
}
/* c------------------------------------------------------------------------ */

long swap4( ov )		/* swap integer*4 */
  char *ov;
{
    union {
	long newval;
	char nv[4];
    }u;
    u.nv[3] = *ov++; u.nv[2] = *ov++; u.nv[1] = *ov++; u.nv[0] = *ov++;
    return(u.newval);
}
/* c------------------------------------------------------------------------ */
