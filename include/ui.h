/* $Id$ */

# ifndef UII_H
# define UII_H

struct date_st
{
	long	ds_yymmdd;	/* Day portion	*/
	long	ds_hhmmss;	/* time portion */
};
typedef struct date_st date;		/* Date in date/time format */

/*
 * this union type is used to pass symbol values around.
 */
typedef union usy_value
{
	long us_v_int;		/* Integer symbol value		*/
	float	us_v_float;	/* Floating point value		*/
	date	us_v_date;	/* Date value			*/
	char	*us_v_ptr;	/* Everything else		*/
} SValue;

/*
 * This is the structure filled in by the command parser.  One of these
 * structures will appear for each non-ignored token in the input
 * stream.
 */
struct ui_command
{
	int	uc_ctype;	/* The token type -- see below.	*/
	int	uc_vptype;	/* Value parameter type		*/
	union usy_value uc_v;	/* Value parameter value.	*/
	int	uc_col;		/* Column number of the token	*/
	char 	*uc_text;	/* The actual token text	*/
};
/*
 * Token types.

 */

typedef union usyii_value
{
	long     us_v_int;	/* Integer symbol value		*/
	float	us_v_float;	/* Floating point value		*/
	date	us_v_date;	/* Date value			*/
	char	*us_v_ptr;	/* Everything else		*/
} SValueii;


/*
 * This is the structure filled in by the command parser.  One of these
 * structures will appear for each non-ignored token in the input
 * stream.
 */
struct uii_command
{
	int	uc_ctype;	/* The token type -- see below.	*/
	int	uc_vptype;	/* Value parameter type		*/
	union usyii_value uc_v;	/* Value parameter value.	*/
	int	uc_col;		/* Column number of the token	*/
	char 	*uc_text;	/* The actual token text	*/
};

/* c------------------------------------------------------------------------ */

struct ui_cmd_mgmt
{
  int max_cmds;
  int num_cmds;

  int max_tokens;
  int max_token_size;

  int num_tokens;
  int (*cmd_proc)();
  int one_time_only;
  int xtra_tokens;

  char * error;
  char * keyword;
  char * cmd_line;
  char * tmplt;

  struct ui_command *cmds;
  struct ui_cmd_mgmt *next;

};

/*
 * Token types.
 */

# define UTT_END	0	/* End of the token list	*/
# define UTT_VALUE	1	/* Value parameter		*/
# define UTT_OTHER	2	/* Something else.		*/
# define UTT_KW		3	/* Keyword token type.		*/
# define UTT_PARTIAL	4	/* End of a partial command	*/
# define UTT_SYM	5	/* Non-subst sym		*/

# define UTT_INT	6	/* <integer> flag		*/
# define UTT_REAL	7	/* <float> flag 		*/
# define UTT_WHERE      8	/* <where> flag                 */
# define UTT_FIELD      9	/* <field> flag                 */

/*
 * Macros to ease dealing with cmds stuff.
 */
# define UINT(cmd) 	(cmd).uc_v.us_v_int
# define UKEY(cmd) 	(cmd).uc_v.us_v_int
# define UBOOL(cmd)	(cmd).uc_v.us_v_int
# define UPTR(cmd)	(cmd).uc_v.us_v_ptr
# define UFLOAT(cmd)	(cmd).uc_v.us_v_float
# define UDATE(cmd)	(cmd).uc_v.us_v_date
# define UCOL(cmd)	(cmd).uc_col

# endif



