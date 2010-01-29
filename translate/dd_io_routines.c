/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */
/* c------------------------------------------------------------------------ */
# include <sys/types.h>
# include <sys/file.h>

# ifdef HAVE_SYS_MTIO_H
#   include <sys/mtio.h>
# endif /* ifdef HAVE_SYS_MTIO_H */
# include <stdio.h>
# include <unistd.h>
# include <ctype.h>
# ifdef sun
# include <sys/ioccom.h>
# endif

# ifdef SVR4
# include <fcntl.h>
# endif



/*
# define MTIOCTOP 0
 * from <unistd.h> just to remember what the symbols mean
 */
/* #define	SEEK_SET		0	absolute offset */
/* #define	SEEK_CUR		1	relative to current offset */
/* #define	SEEK_END		2	relative to end of file */



# include "dorade_headers.h"
# include "dd_io_structs.h"
# ifndef K64
# define K64 65536
# endif


static int num_io_structs=0;
static struct input_read_info *read_io_list[MAX_OPENED_FILES];

/* c------------------------------------------------------------------------ */

dd_input_read_open(iri, dev_name)
  struct input_read_info *iri;
  char *dev_name;
{
    int ii;
    long *lp, keep;
    char *b;

    dd_reset_ios(iri, 1);
    iri->sizeof_file = 0;

    strcpy(iri->dev_name, dev_name);
    printf( "Input file name: %s\n", dev_name);
    if((iri->fid = open(dev_name, 0 )) < 0) { 
	printf( "Could not open input file %s  error=%d\n"
	       , dev_name, iri->fid);
	exit(1);
    }
    if(strstr(dev_name, "/dev/")) {
	iri->io_type = PHYSICAL_TAPE;
    }
    else {
	iri->sizeof_file = lseek(iri->fid, 0L, SEEK_END);
	iri->current_offset = lseek(iri->fid, 0L, SEEK_SET); /* back to zero */
	iri->io_type = BINARY_IO;

	dd_phys_read(iri);
	lp = (long *)iri->top->buf;
	if(*lp <= K64-8 && *lp <= iri->sizeof_file) { /* this might be FB_IO */
    	    b = iri->top->buf + *lp +sizeof(long);
	    memcpy(&keep, b, 4);
	    if(keep == *lp) {
		iri->io_type = FB_IO;
	    }
	}
	dd_rewind_dev(iri);
    }
    return(iri->fid);
}
/* c------------------------------------------------------------------------ */

dd_phys_read(iri)
  struct input_read_info *iri;
{
    int ii, mark;

    iri->seq_num++;
    iri->top->seq_num = iri->seq_num;

    if(iri->io_type == PHYSICAL_TAPE) {
	iri->top->read_state = read(iri->fid, iri->top->buf, iri->sizeof_bufs);
	if(iri->top->read_state > 0)
	      iri->f_bytes += iri->top->read_state;
	if(iri->f_bytes > 2.e9) {
	    printf("Reset input dev: %d at %.3f MB\n"
		   , iri->fid, iri->f_bytes*1.e-6);
	    mark = lseek(iri->fid, 0L, 0L); /* reset klooge for 2GB limit */
	    iri->f_bytes = 0;
	}
    }
    else if(iri->io_type == FB_IO) {
	iri->top->offset = lseek(iri->fid, 0L, SEEK_CUR);
	if(iri->current_offset >= iri->sizeof_file) 
	      iri->top->read_state = -1;
	else 
	      iri->top->read_state = fb_read(iri->fid, iri->top->buf
					     , iri->sizeof_bufs);
	iri->current_offset = lseek(iri->fid, 0L, SEEK_CUR);
    }
    else if(iri->io_type == BINARY_IO) {
	iri->top->offset = lseek(iri->fid, 0L, SEEK_CUR);
	if(iri->current_offset >= iri->sizeof_file) 
	      iri->top->read_state = -1;
	else 
	      iri->top->read_state = read(iri->fid, iri->top->buf
				    , iri->sizeof_bufs);
	iri->current_offset = lseek(iri->fid, 0L, SEEK_CUR);
    }    
    else {
	printf("iri->io_type: %d has gone awry\n", iri->io_type);
	exit(0);
    }
    if(iri->read_count < MAX_BUFS && iri->top->read_state >= 0) {
	iri->read_count++;
    }
    iri->read_ahead_count = 0;
    iri->top->eof_flag = iri->top->read_state == 0;
    iri->top->sizeof_read = iri->top->read_state > 0 ?
	  iri->top->read_state : 0;
    return(iri->top->read_state);
}
/* c------------------------------------------------------------------------ */

dd_logical_read(iri, direction)
  struct input_read_info *iri;
  int direction;
{
    int ii;

    if(direction == FORWARD) {
	if(iri->read_ahead_count > 0) {
	    iri->read_ahead_count--;
	    iri->top = iri->top->next;
	}
	else {			/* we have to do it physically */
	    iri->top = iri->top->next;
	    return(dd_phys_read(iri));
	}
    }
    else if(direction == BACKWARD) {
	if(iri->read_count > 1 &&
	   iri->read_ahead_count < iri->read_count-1) {
	    /* we can go back to the previous record
	     * without any physical io
	     */
	    iri->read_ahead_count++;
	    iri->top = iri->top->last;
	}
	else {			/* we have to do it physically */
	    dd_really_skip_recs(iri, direction, 2);
	    return(dd_phys_read(iri));
	}
    }
    return(iri->top->read_state);
}
/* c------------------------------------------------------------------------ */

dd_skip_recs(iri, direction, skip_count)
  struct input_read_info *iri;
  int direction, skip_count;
{
    /* move in the direction indicated and read the next record
     */
    int ii, jj, nn, mark;
    long offset;
    char mess[256];

    if(!skip_count && !iri->read_ahead_count) {
	/* read in a record */
	iri->top = iri->top->next;
	dd_phys_read(iri);
	return(iri->top->read_state);
    }
    if(skip_count <= 0)
	  return(iri->top->read_state);


    if(direction == FORWARD &&
       skip_count < iri->read_ahead_count) {
	for(ii=0; ii <= skip_count; ii++) {
	    iri->top = iri->top->next;
	    iri->read_ahead_count--;
	}
	return(iri->top->read_state);
    }
    if(direction == BACKWARD &&
       skip_count <= (iri->read_count - iri->read_ahead_count)) {
	for(ii=0; ii < skip_count-1; ii++) {
	    iri->top = iri->top->last;
	    iri->read_ahead_count++;
	}
	return(iri->top->read_state);
    }
    /* to get where we want to go there must be some physical io
     */
    if(direction == FORWARD)
	skip_count -= iri->read_ahead_count;
    else if(iri->read_count > 0)
	skip_count += iri->read_ahead_count;

    if(direction == FORWARD && skip_count < 2*iri->que_count) {
	/* just read all the records into the que so they'll be there
	 * if we want to back up a few
	 */
	for(ii=skip_count; ii > 0; ii--) {
	    iri->top = iri->top->next;
	    if(dd_phys_read(iri) < 0)
		  break;
	}
	if(ii) {
	    sprintf(mess,
		    "Trouble skipping %d blocks on %s\n fid: %d state: %d\n"
		    , skip_count, iri->dev_name
		    , iri->fid, iri->top->read_state);
	    perror(mess);
	}
	return(iri->top->read_state);
    }
    dd_really_skip_recs(iri, direction, skip_count);
    return(iri->top->skip_state);
}
/* c------------------------------------------------------------------------ */

dd_really_skip_recs(iri, direction, skip_count)
  struct input_read_info *iri;
  int direction, skip_count;
{
    int ii, jj, nn, mark, kount=0;
    long ll, rlen, keep_offset;
#ifdef HAVE_SYS_MTIO_H
    struct mtop op;
#endif /* ifdef HAVE_SYS_MTIO_H */
    char mess[256];

    dd_reset_ios(iri, 0);
    iri->top->skip_state = skip_count;

#ifdef HAVE_SYS_MTIO_H
    if(iri->io_type == PHYSICAL_TAPE) {
	op.mt_count = skip_count;
	op.mt_op = direction == FORWARD ? MTFSR : MTBSR;
	if((ii = ioctl(iri->fid, MTIOCTOP, &op)) < 0 ) {
	    sprintf(mess,
"Skip records error on %s\n count: %d dir: %d fid: %d state: %d\n"
		    , iri->dev_name, skip_count, direction
		    , iri->fid, iri->top->skip_state);
	    perror(mess);
	    iri->top->skip_state = ii;
	}
    }
    else
#endif /* ifdef HAVE_SYS_MTIO_H */
    if(iri->io_type == FB_IO) {
	/* record headers and trailers should be 4 bytes
	 */
	if(direction == FORWARD) {
	    for(; skip_count > 0; skip_count--) {
		/* read the next record header */
		if((nn=read(iri->fid, &rlen, sizeof(rlen))) < sizeof(rlen)) {
		    break;
		}
		/* skip over the current record plus trailer
		 */
		if((iri->current_offset =
		    lseek(iri->fid, rlen+sizeof(rlen), SEEK_CUR)) <= 0) {
		    break;
		}
	    }
	}
	else if((iri->current_offset =
		 lseek(iri->fid, 0, SEEK_CUR)) < 8) { /* at BOF */
	    mark = 0;
	}
	else {
	    for(; skip_count > 0; skip_count--) {
		/* back over the trailing record length indicator
		 */
		iri->current_offset =
		      lseek(iri->fid, -sizeof(rlen), SEEK_CUR);
		/* read the trailer (tells record length)
		 */
		nn = read(iri->fid, &rlen, sizeof(rlen));
		/* now back over the whole record plus header and trailer
		 */
		if((iri->current_offset =
		    lseek(iri->fid, -(rlen+2*sizeof(rlen)), SEEK_CUR)) < 8){
		    break;
		}
	    }
	}
	if(skip_count > 0) {
	    mark = 0;
	}
    }
    else {
	/* assume binary io */
	iri->current_offset = lseek(iri->fid, 0L, SEEK_CUR);
	if(direction == FORWARD) {
	    iri->sizeof_file = lseek(iri->fid, 0L, SEEK_END);

	    if(iri->current_offset + skip_count*iri->sizeof_bufs <
	       iri->sizeof_file) {
		iri->current_offset = lseek
		      (iri->fid
		       , iri->current_offset+skip_count*iri->sizeof_bufs
		       , SEEK_SET);
		skip_count = 0;
	    }
	    else {
		iri->current_offset = lseek(iri->fid, 0L, SEEK_END);
	    }
	}
	else {
	    if(iri->current_offset -skip_count*iri->sizeof_bufs >= 0) {
		iri->current_offset = lseek
		      (iri->fid
		       , iri->current_offset -skip_count*iri->sizeof_bufs
		       , SEEK_SET);
		skip_count = 0;
	    }
	    else {
		iri->current_offset = lseek(iri->fid, 0L, SEEK_SET);
	    }
	}
    }
    return(iri->top->skip_state);
}
/* c------------------------------------------------------------------------ */

dd_rewind_dev(iri)
  struct input_read_info *iri;
{
    int ii, jj, mark, ok=YES;
    long ll, rlen;
#ifdef HAVE_SYS_MTIO_H
    struct mtop op;
#endif /* ifdef HAVE_SYS_MTIO_H */
    char mess[256];


    dd_reset_ios(iri, 1);

#ifdef HAVE_SYS_MTIO_H
    if(iri->io_type == PHYSICAL_TAPE) {
	op.mt_op = MTREW;
	op.mt_count = 1;	/* Only rewind once */
	if((iri->top->skip_state = ioctl(iri->fid, MTIOCTOP, &op)) < 0 ) {
	    sprintf(mess, "Rewind error on %s fid: %d state: %d\n"
		    , iri->dev_name, iri->fid, iri->top->skip_state);
	    perror(mess);
	}
    }
    else
#endif /* ifdef HAVE_SYS_MTIO_H */
    {
	iri->current_offset = lseek(iri->fid, 0L, SEEK_SET);
    }
}
/* c------------------------------------------------------------------------ */

dd_unwind_dev(iri)
  struct input_read_info *iri;
{
    int ii, jj, mark, ok=YES;
    long ll, rlen;
#ifdef HAVE_SYS_MTIO_H
    struct mtop op;
#endif /* ifdef HAVE_SYS_MTIO_H */
    char mess[256];

    dd_reset_ios(iri, 0);

    if(iri->io_type == PHYSICAL_TAPE) {
	    sprintf(mess, "Unwind undefined for PHYSICAL TAPE\n");
	    perror(mess);
    }
    else {
	iri->current_offset = iri->sizeof_file = 
	      lseek(iri->fid, 0L, SEEK_END);
    }
}
/* c------------------------------------------------------------------------ */

dd_skip_files(iri, direction, skip_count)
  struct input_read_info *iri;
  int direction, skip_count;
{
    int ii, jj, mark, ok=YES;
    long ll, rlen;
#ifdef HAVE_SYS_MTIO_H
    struct mtop op;
#endif /* ifdef HAVE_SYS_MTIO_H */
    char mess[256];

    dd_reset_ios(iri, 0);
    iri->top->skip_state = skip_count;

#ifdef HAVE_SYS_MTIO_H
    if(iri->io_type == PHYSICAL_TAPE) {
	op.mt_count = skip_count;
	op.mt_op = direction == FORWARD ? MTFSF : MTBSF;
	if((iri->top->skip_state = ioctl(iri->fid, MTIOCTOP, &op)) < 0 ) {
	    sprintf(mess,
"Skip files error on %s\n count: %d dir: %d fid: %d state: %d\n"
		    , iri->dev_name, skip_count, direction
		    , iri->fid, iri->top->skip_state);
	    perror(mess);
	}
    }
    else 
#endif /* ifdef HAVE_SYS_MTIO_H */
    if(iri->io_type == FB_IO) {
	/* record headers and trailers should be 4 bytes
	 */
	if(direction == FORWARD) {
	    for(; skip_count > 0; skip_count--) {
		for(;;) {
		    /* read the next record header */
		    if((ll=read(iri->fid, &rlen, sizeof(rlen))) < sizeof(rlen)) {
			ok = NO;
			break;
		    }
		    /* skip over the current record plus trailer
		     */
		    if((ll = lseek(iri->fid, rlen+sizeof(rlen), SEEK_CUR)) <= 0) {
			ok = NO;
			break;
		    }
		    if(rlen == 0) /* this is a logical EOF */
			  break; 
		}
		if(!ok) {
		    break;
		}
	    }
	}
	else if((ll=lseek(iri->fid, 0, SEEK_CUR)) < 8) { /* at BOF */
	    mark = 0;
	}
	else {
	    for(; skip_count > 0; skip_count--) {
		for(;;) {
		    /* back over the trailing record length indicator
		     */
		    ll = lseek(iri->fid, -sizeof(rlen), SEEK_CUR);
		    /* read in the trailer (tells record length)
		     */
		    ll = read(iri->fid, &rlen, sizeof(rlen));
		    /* now back over the whole record plus header and trailer
		     */
		    if((ll=lseek(iri->fid, -(rlen+2*sizeof(rlen))
				 , SEEK_CUR)) < 8) {
			ok = NO;
			break;
		    }
		    if(rlen == 0) /* this is a logical EOF */
			  break; 
		}
		if(!ok)
		      break;
	    }
	}
	if(skip_count > 0) {
	    mark = 0;
	}
    }
    else {
	/* assume binary io */
	sprintf(mess, "NO file skipping capability in binary io\n");
	perror(mess);
    }
}
/* c------------------------------------------------------------------------ */

struct input_read_info *
dd_return_ios(index)
  int index;
{
    int ii;
    struct input_read_info *dd_init_io_structs();

    if(index < 0 || index >= MAX_OPENED_FILES) {
	return NULL;
    }

    if(!num_io_structs) {
	for(ii=0; ii < MAX_OPENED_FILES; ii++) {
	    read_io_list[ii] = NULL;
	}
    }
    if(read_io_list[index])
	  return(read_io_list[index]);

    return(dd_init_io_structs(index));
}
/* c------------------------------------------------------------------------ */

struct input_read_info *
dd_init_io_structs(index)
  int index;
{
    int ii, jj;
    struct input_read_info *iri;
    struct input_buf *next, *last;
    struct ddex_block *db, *db_last;

    num_io_structs++;
    iri = (struct input_read_info *)malloc(sizeof(struct input_read_info));
    memset(iri, 0, sizeof(struct input_read_info));
    /* implicit here is that io type starts out as PHYSICAL_TAPE
     */
    iri->que_count = MAX_BUFS;
    iri->block_count = MAX_LOG_BLOCKS;
    iri->sizeof_bufs = K64;
    read_io_list[index] = iri;

    for(ii=0; ii < iri->que_count; ii++) {
	next = (struct input_buf *)malloc(sizeof(struct input_buf));
	memset(next, 0, sizeof(struct input_buf));
	next->id_num = ii;
	iri->ibs[ii] = next;

	next->buf = (char *)malloc(iri->sizeof_bufs);
	memset(next->buf, 0, iri->sizeof_bufs);

	if(ii) {
	    next->last = last;
	    last->next = next;
	}
	else {			/* first time through */
	    iri->top = next;
	}
	iri->top->last = next;
	next->next = iri->top;
	last = next;
    }

    for(ii=0; ii < iri->block_count; ii++) {
	db = (struct ddex_block *)malloc(sizeof(struct ddex_block));
	memset(db, 0, sizeof(struct ddex_block));

	if(ii) {
	    db->last = db_last;
	    db_last->next = db;
	}
	else {
	    iri->block_que = db;
	}
	iri->block_que->last = db;
	db->next = iri->block_que;
	db_last = db;
    }
    return(iri);
}
/* c------------------------------------------------------------------------ */

dd_reset_ios(iri, level)
  struct input_read_info *iri;
  int level;
{
    int ii;
    struct input_buf *ib;

    iri->read_ahead_count = iri->read_count = 0;

    if(level == 1) {
	iri->f_bytes =
	      iri->current_offset =
		    iri->top->skip_state = 0;
    }

    for(ii=0; ii < iri->que_count; ii++) {
	ib = iri->top = iri->top->next;
	ib->seq_num = 0;
    }
}
/* c------------------------------------------------------------------------ */

struct ddex_block*
dd_return_current_block(iri)
  struct input_read_info *iri;
{
    return(iri->block_que);
}
/* c------------------------------------------------------------------------ */

struct ddex_block*
dd_return_next_block(iri)
  struct input_read_info *iri;
{
    struct ddex_block *db;

    db = iri->block_que = iri->block_que->next;
    db->ib_id = iri->top->id_num;
    db->seq_num = iri->top->seq_num;
    db->buf = iri->top->buf;
}
/* c------------------------------------------------------------------------ */

