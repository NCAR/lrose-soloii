/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */

/*
 * This file contains the following routines:
 * 
 * frme_intxns
 * init_xyraster
 * ray_raster_setup
 * return_xyras
 * sp_meters_per_pixel
 * swap_edges
 * swap_ends
 * x_inner_loop
 * xx_inner_loop
 * y_inner_loop
 * yy_inner_loop
 * 
 * 
 */

# include <dorade_headers.h>
# include <solo_window_structs.h>
# include "soloii.h"
# include "sii_externals.h"
# include <solo_editor_structs.h>
# include "xyraster.h"
# include <function_decl.h>
# include <dgi_func_decl.h>
# include <dd_math.h>

static int Num_xyras=0;
static struct xyras *xyraz[16];
static double tickle=0.0;
extern int LittleEndian;
static double db_ang0=13, db_ang1=21;


/* external routines */
void se_clr_bnd();		/* se_bnd.c */
void xse_add_bnd_pt();		/* se_bnd.c */
int xse_find_intxns();		/* se_bnd.c */
int d_inSector( double ang, double ang1, double ang2);
gboolean sii_debug_flag();

/* internal routines */
int frme_intxns();
void init_xyraster();
int ray_raster_setup();
struct xyras *return_xyras();
double sp_meters_per_pixel();
void swap_edges ();
void swap_ends ();
void x_inner_loop();
void xx_inner_loop();
void y_inner_loop();
void yy_inner_loop();

/* c------------------------------------------------------------------------ */

int frme_intxns(rxy, edge)
  struct xyras *rxy;
  struct line_segment *edge;
{
    int ii, nn;
    struct bnd_point_mgmt *bpmx;
    double d;

    bpmx = rxy->screen_boundary->first_intxn;

    if(rxy->radar_outside) {
	if((edge->num_intxns = rxy->screen_boundary->num_intxns) > 1) {
	    edge->p0.x = (int)((double)bpmx->rx * edge->cos);
	    edge->p0.y = (int)((double)bpmx->rx * edge->sin);
	    edge->p0.gate_num =
		  GATE_NUM(bpmx->rx, rxy->r0, rxy->rcp_gs);

	    bpmx = bpmx->next_intxn;
	    edge->p1.x = (int)((double)bpmx->rx * edge->cos);
	    edge->p1.y = (int)((double)bpmx->rx * edge->sin);
	    edge->p1.gate_num =
		  GATE_NUM(bpmx->rx, rxy->r0, rxy->rcp_gs);
	}
	else if(edge->num_intxns == 1) {
	    /* ray ends before intersecting
	     * another boundary
	     */
	    edge->p0.x = (int)((double)bpmx->rx * edge->cos);
	    edge->p0.y = (int)((double)bpmx->rx * edge->sin);
	    edge->p0.gate_num =
		  GATE_NUM(bpmx->rx, rxy->r0, rxy->rcp_gs);

	    bpmx = bpmx->next_intxn;
	    edge->p1.x = (int)((double)rxy->r1 * edge->cos);
	    edge->p1.y = (int)((double)rxy->r1 * edge->sin);
	    edge->p1.gate_num =
		  GATE_NUM(rxy->r1, rxy->r0, rxy->rcp_gs);
	}
	else {
	    return(-1);
	}
    }
    else {			/* the radar is inside */
	edge->p0.x = 0;
	edge->p0.y = 0;
	edge->p0.gate_num = GATE_NUM(0, rxy->r0, rxy->rcp_gs);

	if((edge->num_intxns = rxy->screen_boundary->num_intxns) > 0) {
	    edge->p1.x = (int)((double)bpmx->rx * edge->cos);
	    edge->p1.y = (int)((double)bpmx->rx * edge->sin);
	    edge->p1.gate_num =
		  GATE_NUM(bpmx->rx, rxy->r0, rxy->rcp_gs);
	}
	else {			/* if the radar is inside and no intersections
				 * then the beam ends inside the frame
				 */
	    edge->p1.x = (int)((double)rxy->r1 * edge->cos);
	    edge->p1.y = (int)((double)rxy->r1 * edge->sin);
	    edge->p1.gate_num =
		  GATE_NUM(rxy->r1, rxy->r0, rxy->rcp_gs);
	}
    }
    return(1);
}
/* c------------------------------------------------------------------------ */

void init_xyraster(frme)
  int frme;
{
    /* routine to initialize rasterization parameters at the beginning
     * of each sweep
     * for the sanity of the author all calculations are in cartesian
     * coordinates and the switch to screen coordinates is only at the
     * very last minute.
     * all coordinates are relative to the radar which is (0,0)
     */
    int ii, jj, nn, mark, *iptr, scan_mode=0;
    struct solo_click_info *sci, *clear_click_info();
    struct one_boundary *ob, *se_malloc_one_bnd();
    double d, sin(), cos(), theta, range, a0, a1, fmod(), fill=1.1;
    double fabs(), atan2(), trip=44.5, dinc=1., sp_meters_per_pixel();
    struct xyras *rxy, *return_xyras();
    WW_PTR wwptr, solo_return_wwptr();
    struct boundary_stuff *sebs, *return_se_bnd_struct();
    struct point_in_space *pisp, *solo_return_radar_pisp();
    struct bnd_point_mgmt *bpm;


    wwptr = solo_return_wwptr(frme);
    rxy = return_xyras(frme);

    /*
     * nab the plot parameters
     * coordinates to the center of the image and the corners are
     * in meters relative to the radar.
     */
    rxy->angle_ctr = wwptr->center_of_view->rotation_angle;
    rxy->range_ctr = wwptr->center_of_view->range*1000.; /* meters! */

    if((d = wwptr->view->magnification) < .005) d = .005;
    rxy->meters_per_pixel = sp_meters_per_pixel()/d;
    rxy->pixels_per_meter = 1./rxy->meters_per_pixel;

    theta = RADIANS(CART_ANGLE(rxy->angle_ctr));
    rxy->center.x = rxy->range_ctr * cos(theta);
    rxy->center.y = rxy->range_ctr * sin(theta);

    rxy->image_width = wwptr->view->width_in_pix;
    rxy->image_height = wwptr->view->height_in_pix;
    rxy->image = (char *)wwptr->image->data;
# ifdef obsolete
    d = atan2((double)rxy->image_height, (double)rxy->image_width);
    rxy->sin_corner = sin(d);
# else
    rxy->sin_corner = SIN45;
# endif
    rxy->image_l_left.x = rxy->center.x -(.5*(rxy->image_width-1.) *
	  rxy->meters_per_pixel);
    rxy->image_l_left.y = rxy->center.y -(.5*(rxy->image_height-1.) *
	  rxy->meters_per_pixel);

    if(wwptr->constrained_rasterz) {
	rxy->view_width = IFIX(.5 * rxy->image_width) + RXY_FUDGE;
	rxy->view_height = IFIX(.5 * rxy->image_height) + RXY_FUDGE;
    }
    else {
	rxy->view_width = rxy->image_width;
	rxy->view_height = rxy->image_height;
    }
    /*
     * view width may be smaller than the image width
     * but the rasterization is defined by the view size
     */
    rxy->l_left.x = rxy->center.x -(.5*(rxy->view_width-1.) *
	  rxy->meters_per_pixel);
    rxy->l_left.y = rxy->center.y -(.5*(rxy->view_height-1.) *
	  rxy->meters_per_pixel);

    rxy->u_right.x = rxy->l_left.x +(rxy->view_width-1.) *
	  rxy->meters_per_pixel;
    rxy->u_right.y = rxy->l_left.y +(rxy->view_height-1.) *
	  rxy->meters_per_pixel;

    rxy->radar_outside = (rxy->l_left.x > 0 || rxy->u_right.x < 0 ||
			  rxy->l_left.y > 0 || rxy->u_right.y < 0) ? YES : NO;
    /*
     * generate screen boundary
     * this algorithm uses the editor boundary routines to
     * calculate intersections of the ray and the bounds
     * of the viewing area
     */
    if(!rxy->screen_boundary) {
	rxy->screen_boundary = se_malloc_one_bnd();
    }
    rxy->screen_boundary->radar_inside_boundary = !rxy->radar_outside;
    se_clr_bnd(rxy->screen_boundary);
    sebs = return_se_bnd_struct();
    sebs->view_bounds = YES;
    
    sci = clear_click_info();

    sci->x = IFIX(rxy->l_left.x);
    sci->y = IFIX(rxy->l_left.y);
    xse_add_bnd_pt(sci, rxy->screen_boundary);

    sci->y = IFIX(rxy->u_right.y);
    xse_add_bnd_pt(sci, rxy->screen_boundary);

    sci->x = IFIX(rxy->u_right.x);
    xse_add_bnd_pt(sci, rxy->screen_boundary);

    sci->y = IFIX(rxy->l_left.y);
    xse_add_bnd_pt(sci, rxy->screen_boundary);

    bpm = rxy->screen_boundary->top_bpm;
    /* set shifted x,y,z...no shift here!
     */
    for(ii=0; ii < 4; ii++, bpm = bpm->next) {
	bpm->_x = bpm->x;
	bpm->_y = bpm->y;
    }
    sebs->view_bounds = NO;
}
/* c------------------------------------------------------------------------ */

int ray_raster_setup(frme, angle0, angle1, rxy)
  int frme;
  double angle0, angle1;
  struct xyras *rxy;
{

    /* this routine does calculation of the intersections for both
     * edgets of the ray to be rasterized
     * and then keeps ajusting to loop constraints such that
     * no if tests are necessary in the innermost rasterization loops
     */
    char str[256];
    int sp_diag_flag(), fd;
    int ii, jj, nn, max, min, nn0, nn1, mark=0;
    register double d, d0, d1, dx0, dy0, dx1, dy1;
    double fabs(), sin(), cos(), sqrt();
    WW_PTR wwptr, solo_return_wwptr();



# ifdef obsolete
    if (d_inSector (angle0, db_ang0, db_ang1)) {
       sii_set_debug_flag(YES);
    }
    else {
       sii_set_debug_flag(NO);
    }
# endif

    wwptr = solo_return_wwptr(frme);
    d = RADIANS(CART_ANGLE(angle0));
    rxy->edge0.sin = sin(d);
    rxy->edge0.cos = cos(d);
    d = RADIANS(CART_ANGLE(angle1));
    rxy->edge1.sin = sin(d);
    rxy->edge1.cos = cos(d);
    rxy->ignore_this_ray = YES;

    rxy->gs = wwptr->uniform_cell_spacing;
    rxy->rcp_gs = 1./rxy->gs;
    rxy->r0 = wwptr->uniform_cell_one;
    rxy->max_gate = wwptr->number_cells-1;
    rxy->r1 = rxy->r0 +rxy->max_gate *rxy->gs;
    rxy->colors = wwptr->cell_colors;
    /*
     * calculate the intersections of the two edges with
     * the screen boundary
     */
    xse_find_intxns(angle0, rxy->r1, rxy->screen_boundary);
    if((mark = frme_intxns(rxy, &rxy->edge0)) < 1) {
       return(-1);
    }
    xse_find_intxns(angle1, rxy->r1, rxy->screen_boundary);
    if((mark = frme_intxns(rxy, &rxy->edge1)) < 1) {
       return(-1);
    }

    if(rxy->radar_outside && rxy->edge1.num_intxns != rxy->edge0.num_intxns) {
# ifdef notyet
       return(-1);
# endif
    }

    if(fabs(rxy->edge0.sin) > rxy->sin_corner) {
	/*
	 * inner loop in x
	 */
	rxy->x_inner_loop = YES;

	if(rxy->edge0.p1.y < rxy->edge0.p0.y) { /* flip points so we can
						 * iterate in positive y */
	    swap_ends(&rxy->edge0);
	    swap_ends(&rxy->edge1);
	}
	/*
	 * intersections may not line up perfectly in double precision
	 * since the boundary arithmetic
	 * and values start out as integer values
	 * so make them line up
	 */
	dy0 = rxy->edge1.p0.y -rxy->edge0.p0.y;
	dy1 = rxy->edge1.p1.y -rxy->edge0.p1.y;

	if(fabs(dy0) < rxy->meters_per_pixel) {
	    rxy->edge1.p0.y = rxy->edge0.p0.y = rxy->edge0.p0.y + .5*dy0;
	}
	if(fabs(dy1) < rxy->meters_per_pixel) {
	    rxy->edge1.p1.y = rxy->edge0.p1.y = rxy->edge0.p1.y + .5*dy1;
	}

	/* first calculate the change in x as you move up one
	 * pixel in y (the change in y is rxy->meters_per_pixel).
	 * and the change in gate number as you move up in y
	 */
	rxy->edge0.x_inc = rxy->meters_per_pixel*rxy->edge0.cos/rxy->edge0.sin;

	rxy->edge0.major_gate_inc =
	      rxy->meters_per_pixel/rxy->edge0.sin * rxy->rcp_gs;
	rxy->edge0.minor_gate_inc = rxy->edge1.minor_gate_inc = .5 *
 	      (rxy->edge0.cos + rxy->edge1.cos) *
		    rxy->meters_per_pixel * rxy->rcp_gs;
	rxy->edge0.num_iters = ((rxy->edge0.p1.y -
				rxy->edge0.p0.y)*rxy->pixels_per_meter);

	rxy->edge1.x_inc = rxy->meters_per_pixel*rxy->edge1.cos/rxy->edge1.sin;

	rxy->edge1.major_gate_inc =
	      rxy->meters_per_pixel/rxy->edge1.sin * rxy->rcp_gs;
	rxy->edge1.num_iters = ((rxy->edge1.p1.y -
				rxy->edge1.p0.y)*rxy->pixels_per_meter);
	/*
	 * first line up the y-values
	 */
	d = (rxy->edge1.p0.y -rxy->edge0.p0.y) * rxy->pixels_per_meter;
	if(d >= .5) {
	    nn = d +.5;
	    /*
	     * edge 1 starts higher up than edge 0
	     * make edge 0 line up in y with edge 1
	     */
	    rxy->edge0.p0.gate_num += nn*rxy->edge0.major_gate_inc;
	    rxy->edge0.p0.x += nn*rxy->edge0.x_inc;
	    rxy->edge0.p0.y += nn*rxy->meters_per_pixel;
	    rxy->edge0.num_iters -= nn;
	}
	else if( d <= -.5) {	/* edge0 starts higher up than edge1 */
	    nn = -d -.5;
	    rxy->edge1.p0.gate_num += nn*rxy->edge1.major_gate_inc;
	    rxy->edge1.p0.x += nn*rxy->edge1.x_inc;
	    rxy->edge1.p0.y += nn*rxy->meters_per_pixel;
	    rxy->edge1.num_iters -= nn;
	}
	d = (rxy->edge1.p1.y -rxy->edge0.p1.y) * rxy->pixels_per_meter;

	if(d >= .5) {		/* edge1 ends higher than edge0
				 * so bring it down */
	    nn = d +.5;
	    rxy->edge1.p1.gate_num -= nn*rxy->edge1.major_gate_inc;
	    rxy->edge1.p1.x -= nn*rxy->edge1.x_inc;
	    rxy->edge1.p1.y -= nn*rxy->meters_per_pixel;
	    rxy->edge1.num_iters -= nn;
	}
	else if( d <= -.5) {	/* edge0 ends higher than edge1 */
	    nn = -d -.5;
	    rxy->edge0.p1.gate_num -= nn*rxy->edge0.major_gate_inc;
	    rxy->edge0.p1.x -= nn*rxy->edge0.x_inc;
	    rxy->edge0.p1.y -= nn*rxy->meters_per_pixel;
	    rxy->edge0.num_iters -= nn;
	}

	if(rxy->edge0.p0.x > rxy->edge1.p0.x ||
	   rxy->edge0.p1.x > rxy->edge1.p1.x) { /* swap edges so we can iterate
						 * in positive x */
	    swap_edges(&rxy->edge0, &rxy->edge1);
	}
	/*
	 * we will be referenceing on edge0
	 *
	 * now adjust things so the gate numbers are reasonable
	 */
	if(rxy->edge0.major_gate_inc < 0) {
	    if(rxy->edge0.p0.gate_num > rxy->max_gate ||
	       rxy->edge1.p0.gate_num > rxy->max_gate) {
		d0 = rxy->edge0.p0.gate_num -rxy->max_gate;
		d1 = rxy->edge1.p0.gate_num -rxy->max_gate;
		d = d0 > d1 ? d0 : d1;
		nn = -d/rxy->edge0.major_gate_inc +1.;
		/*
		 * adjusting the max gate number
		 */
		rxy->edge0.p0.gate_num += nn*rxy->edge0.major_gate_inc;
		rxy->edge0.p0.x += nn*rxy->edge0.x_inc;
		rxy->edge1.p0.x += nn*rxy->edge1.x_inc;
		rxy->edge0.p0.y += nn*rxy->meters_per_pixel;
		rxy->edge0.num_iters -= nn;
	    }
	    if(rxy->edge0.p1.gate_num < 0 || rxy->edge1.p1.gate_num < 0) {
		d0 = -rxy->edge0.p1.gate_num;
		d1 = -rxy->edge1.p1.gate_num;
		d = d0 > d1 ? d0 : d1;
		nn = -d/rxy->edge0.major_gate_inc +1.;
		/*
		 * adjusting the min gate number
		 */
		rxy->edge0.num_iters -= nn;
	    }
	}
	else {			/* points up */
	    if(rxy->edge0.p0.gate_num < 0 || rxy->edge1.p0.gate_num < 0) {
		d0 = -rxy->edge0.p0.gate_num;
		d1 = -rxy->edge1.p0.gate_num;
		d = d0 > d1 ? d0 : d1;
		nn = d/rxy->edge0.major_gate_inc +1.;
		/*
		 * adjusting the min gate number
		 */
		rxy->edge0.p0.gate_num += nn*rxy->edge0.major_gate_inc;
		rxy->edge0.p0.x += nn*rxy->edge0.x_inc;
		rxy->edge1.p0.x += nn*rxy->edge1.x_inc;
		rxy->edge0.p0.y += nn*rxy->meters_per_pixel;
		rxy->edge0.num_iters -= nn;
		
	    }
	    if(rxy->edge0.p1.gate_num > rxy->max_gate ||
	       rxy->edge1.p1.gate_num > rxy->max_gate) {
		d0 = rxy->edge0.p1.gate_num -rxy->max_gate;
		d1 = rxy->edge1.p1.gate_num -rxy->max_gate;
		d = d0 > d1 ? d0 : d1;
		nn = d/rxy->edge0.major_gate_inc +1.;
		/*
		 * adjusting the max gate number
		 */
		rxy->edge0.num_iters -= nn;
	    }
	}
	/* now convert location coordinates to screen coordinates
	 */
	rxy->edge0.p0.x = (rxy->edge0.p0.x -rxy->image_l_left.x) *
		  rxy->pixels_per_meter;
	rxy->edge1.p0.x = (rxy->edge1.p0.x -rxy->image_l_left.x) *
	      rxy->pixels_per_meter;
	rxy->edge0.p0.y = (rxy->edge0.p0.y -rxy->image_l_left.y) *
	      rxy->pixels_per_meter;
	rxy->edge0.x_inc *= rxy->pixels_per_meter;
	rxy->edge1.x_inc *= rxy->pixels_per_meter;
	/*
	 * just fudge the indexing into the image to reflect that
	 * the screen coordinates are upside down
	 */
    }

    /*
     * the inner loop is in y
     */

    else {
	rxy->x_inner_loop = NO;

	if(rxy->edge0.p0.x > rxy->edge0.p1.x) { /* flip points to iterate
						 in positive x */
	    swap_ends(&rxy->edge0);
	    swap_ends(&rxy->edge1);
	}
	dx0 = rxy->edge1.p0.x -rxy->edge0.p0.x;
	dx1 = rxy->edge1.p1.x -rxy->edge0.p1.x;

	if(fabs(dx0) < rxy->meters_per_pixel) {
	    rxy->edge1.p0.x = rxy->edge0.p0.x = rxy->edge0.p0.x + .5*dx0;
	}
	if(fabs(dx1) < rxy->meters_per_pixel) {
	    rxy->edge1.p1.x = rxy->edge0.p1.x = rxy->edge0.p1.x + .5*dx1;
	}

	rxy->edge0.y_inc = rxy->meters_per_pixel*rxy->edge0.sin/rxy->edge0.cos;

	rxy->edge0.major_gate_inc =
	      rxy->meters_per_pixel/rxy->edge0.cos * rxy->rcp_gs;
	rxy->edge1.minor_gate_inc = 
	      rxy->edge0.minor_gate_inc = .5 *
		    (rxy->edge0.sin + rxy->edge1.sin) *
			  rxy->meters_per_pixel * rxy->rcp_gs;
	rxy->edge0.num_iters = ((rxy->edge0.p1.x - rxy->edge0.p0.x) *
			       rxy->pixels_per_meter);

	rxy->edge1.y_inc = rxy->meters_per_pixel*rxy->edge1.sin/rxy->edge1.cos;

	rxy->edge1.major_gate_inc =
	      rxy->meters_per_pixel/rxy->edge1.cos * rxy->rcp_gs;
	rxy->edge1.num_iters = ((rxy->edge1.p1.x - rxy->edge1.p0.x) *
			       rxy->pixels_per_meter);

	d = (rxy->edge1.p0.x -rxy->edge0.p0.x)*rxy->pixels_per_meter;
	if(d >= .5) {
	    /* edge1 starts beyond edge0
	     */
	    nn = d +.5;
	    rxy->edge0.p0.gate_num += nn*rxy->edge0.major_gate_inc;
	    rxy->edge0.p0.y += nn*rxy->edge0.y_inc;
	    rxy->edge0.p0.x += nn*rxy->meters_per_pixel;
	    rxy->edge0.num_iters -= nn;
	}
	else if(d <= -.5) {	/* edge0 starts before edge1 */
	    nn = -d -.5;
	    rxy->edge1.p0.gate_num += nn*rxy->edge1.major_gate_inc;
	    rxy->edge1.p0.y += nn*rxy->edge1.y_inc;
	    rxy->edge1.p0.x += nn*rxy->meters_per_pixel;
	    rxy->edge1.num_iters -= nn;
	}
	d = (rxy->edge1.p1.x -rxy->edge0.p1.x)*rxy->pixels_per_meter;

	if(d >= .5) {	    /* edge1 ends beyond edge0 */
	    nn = d +.5;
	    rxy->edge1.p1.gate_num -= nn*rxy->edge1.major_gate_inc;
	    rxy->edge1.p1.y -= nn*rxy->edge1.y_inc;
	    rxy->edge1.p1.x -= nn*rxy->meters_per_pixel;
	    rxy->edge1.num_iters -= nn;
	}
	else if(d <= -.5) {	/* edge0 ends beyond edge1 */
	    nn = -d -.5;
	    rxy->edge0.p1.gate_num -= nn*rxy->edge0.major_gate_inc;
	    rxy->edge0.p1.y -= nn*rxy->edge0.y_inc;
	    rxy->edge0.p1.x -= nn*rxy->meters_per_pixel;
	    rxy->edge0.num_iters -= nn;
	}
	if(rxy->edge0.p0.y > rxy->edge1.p0.y || rxy->edge0.p1.y > rxy->edge1.p1.y) {
	    /* swap edges so we can interate in positive y
	     */
	    swap_edges(&rxy->edge0, &rxy->edge1);
	}
	/*
	 * now adjust things so the min and max gate numbers are reasonable
	 */
	if(rxy->edge0.major_gate_inc < 0) {
	    if(rxy->edge0.p0.gate_num > rxy->max_gate ||
	       rxy->edge1.p0.gate_num > rxy->max_gate) {
		d0 = rxy->edge0.p0.gate_num -rxy->max_gate;
		d1 = rxy->edge1.p0.gate_num -rxy->max_gate;
		d = d0 > d1 ? d0 : d1;
		nn = -d/rxy->edge0.major_gate_inc +1.;
		rxy->edge0.p0.gate_num += nn*rxy->edge0.major_gate_inc;
		rxy->edge0.p0.y += nn*rxy->edge0.y_inc;
		rxy->edge1.p0.y += nn*rxy->edge1.y_inc;
		rxy->edge0.p0.x += nn*rxy->meters_per_pixel;
		rxy->edge0.num_iters -= nn;
	    }
	    if(rxy->edge0.p1.gate_num < 0 || rxy->edge1.p1.gate_num < 0) {
		d0 = -rxy->edge0.p1.gate_num;
		d1 = -rxy->edge1.p1.gate_num;
		d = d0 > d1 ? d0 : d1;
		nn = -d/rxy->edge0.major_gate_inc +1.;
		rxy->edge0.num_iters -= nn;
	    }
	}
	else {			
	    if(rxy->edge0.p0.gate_num < 0 || rxy->edge1.p0.gate_num < 0) {
		d0 = -rxy->edge0.p0.gate_num;
		d1 = -rxy->edge1.p0.gate_num;
		d = d0 > d1 ? d0 : d1;
		nn = d/rxy->edge0.major_gate_inc +1.;
		rxy->edge0.p0.gate_num += nn*rxy->edge0.major_gate_inc;
		rxy->edge0.p0.y += nn*rxy->edge0.y_inc;
		rxy->edge1.p0.y += nn*rxy->edge1.y_inc;
		rxy->edge0.p0.x += nn*rxy->meters_per_pixel;
		rxy->edge0.num_iters -= nn;
	    }
	    if(rxy->edge0.p1.gate_num > rxy->max_gate ||
	       rxy->edge1.p1.gate_num > rxy->max_gate) {
		d0 = rxy->edge0.p1.gate_num -rxy->max_gate;
		d1 = rxy->edge1.p1.gate_num -rxy->max_gate;
		d = d0 > d1 ? d0 : d1;
		nn = d/rxy->edge0.major_gate_inc +1.;
		rxy->edge0.num_iters -= nn;
	    }
	}
	rxy->edge0.p0.y = (rxy->edge0.p0.y -rxy->image_l_left.y) *
	      rxy->pixels_per_meter;
	rxy->edge1.p0.y = (rxy->edge1.p0.y -rxy->image_l_left.y) *
	      rxy->pixels_per_meter;
	rxy->edge0.p0.x = (rxy->edge0.p0.x -rxy->image_l_left.x) *
	      rxy->pixels_per_meter;
	rxy->edge0.y_inc *= rxy->pixels_per_meter;
	rxy->edge1.y_inc *= rxy->pixels_per_meter;
    }
    if (sii_debug_flag()) {
       g_string_sprintfa (gs_complaints, "a:(%.2f,%.2f)it:(%d,%d)"
			 , angle0
			 , angle1
			 , rxy->edge0.num_iters
			 , rxy->edge1.num_iters
			 );
    }
    
    if(rxy->edge0.num_iters < 1) {
	return(-1);
    }
    else {
	rxy->ignore_this_ray = NO;
	return(1);
    }
}
/* c------------------------------------------------------------------------ */

struct xyras *return_xyras(frme)
  int frme;
{
    int ii=0;

    if(!Num_xyras) {
	for(ii=0; ii < 16; ii++) {
	    xyraz[ii] = NULL;
	}
    }
    if(xyraz[frme])
	  return(xyraz[frme]);

    Num_xyras++;
    xyraz[frme] = (struct xyras *)malloc(sizeof(struct xyras));
    memset(xyraz[frme], 0, sizeof(struct xyras));
    return(xyraz[frme]);
}
/* c------------------------------------------------------------------------ */

double sp_meters_per_pixel()
{
    return((double)300.);
}
/* c------------------------------------------------------------------------ */

void swap_edges(edge0, edge1)
  struct line_segment *edge0, *edge1;
{
    int sl=sizeof(struct line_segment);
    struct line_segment edge_tmp;

    memcpy(&edge_tmp, edge0, sl);
    memcpy(edge0, edge1, sl);
    memcpy(edge1, &edge_tmp, sl);
}
/* c------------------------------------------------------------------------ */

void swap_ends(edge)
  struct line_segment *edge;
{
    int sxy=sizeof(struct rxy_coords);
    struct rxy_coords xy_tmp;

    memcpy(&xy_tmp, &edge->p0, sxy);
    memcpy(&edge->p0, &edge->p1, sxy);
    memcpy(&edge->p1, &xy_tmp, sxy);
}
/* c------------------------------------------------------------------------ */

void xx_inner_loop(rxy)
  struct xyras *rxy;
{
    /* c...mark */
    int ii, jj, nn,  x_col, y_row, width, mark, max;
    int ndx0, ndx;
    char *deep6=0, a;
    char *image=(char*)rxy->image;
    unsigned long *colors=rxy->colors;
    double d;


    int gate_num_scaled;
    int start_gate_num_scaled = RXY_SCALE(rxy->edge0.p0.gate_num);
    int y_gate_inc_scaled = RXY_SCALE(rxy->edge0.major_gate_inc);
    int x0_pxl_inc_scaled = RXY_SCALE(rxy->edge0.x_inc);
    int x1_pxl_inc_scaled = RXY_SCALE(rxy->edge1.x_inc);
    int x_gate_inc_scaled = RXY_SCALE(rxy->edge0.minor_gate_inc);
    int x0_pxl_scaled = RXY_SCALE(rxy->edge0.p0.x);
    int x1_pxl_scaled = RXY_SCALE(rxy->edge1.p0.x);
    int hits = 0;

    register short *gate_num;
    register short *x_gate_inc;
    register short *x0_pxl;
    register short *x1_pxl;


    if((jj = rxy->edge0.num_iters) < 1)
	  return;

    if(x0_pxl_scaled < 0) x0_pxl_scaled = 0;

    gate_num = LittleEndian
      ? (short *)&gate_num_scaled +1 : (short *)&gate_num_scaled;
    x_gate_inc = LittleEndian
      ? (short *)&x_gate_inc_scaled +1 : (short *)&x_gate_inc_scaled;
    x0_pxl = LittleEndian
      ? (short *)&x0_pxl_scaled +1 : (short *)&x0_pxl_scaled;
    x1_pxl = LittleEndian
      ? (short *)&x1_pxl_scaled +1 : (short *)&x1_pxl_scaled;

    if (sii_debug_flag()) {
       ii = rxy->edge0.num_iters;
       g_string_sprintfa (gs_complaints, " xx i:%d %d %d %d %d"
			  , ii
			  , *gate_num
			  , *x_gate_inc
			  , *x0_pxl
			  , *x1_pxl

			  );
    }
    max = rxy->image_width * rxy->image_height -1;
    width = rxy->image_width;
    /*
     * remember...image y coords are reversed
     */
    y_row = (rxy->image_height-1) -IFIX(rxy->edge0.p0.y);
    if(y_row < 0) y_row = 0;
    ndx0 = y_row * width;


    for(; jj--;) {
	ndx = ndx0 +(*x0_pxl);
	ii = *x1_pxl -(*x0_pxl) +1;
	gate_num_scaled = start_gate_num_scaled;

	for(; ii-- > 0; ndx++) {
# ifdef obsolete
	    if(ndx > max || ndx < 0) {
		mark = 0;
		*deep6 = 1;
	    }
# endif
	    *(image +ndx) = *(colors +(*gate_num));
	    gate_num_scaled += x_gate_inc_scaled;
	    hits++;
	}

	start_gate_num_scaled += y_gate_inc_scaled;
	x0_pxl_scaled += x0_pxl_inc_scaled;
	x1_pxl_scaled += x1_pxl_inc_scaled;
	ndx0 -= width;		/* remember image y coords are reversed */
    }
    if (sii_debug_flag()) {
       g_string_sprintfa (gs_complaints, " hits:%d %d %d"
			  , hits
			  , x0_pxl_inc_scaled
			  , x1_pxl_inc_scaled
			  );
       g_message (gs_complaints->str);
       g_string_truncate (gs_complaints, 0);
    }
}
/* c------------------------------------------------------------------------ */

void yy_inner_loop(rxy)
  struct xyras *rxy;
{
    /* c...mark */
    int ii, jj, nn,  x_col, y_row, width, flipit=rxy->image_height-1;
    int ndx0, ndx, mark, max;
    char *deep6=0, a;
    char *image=(char *)rxy->image;
    unsigned long *colors=rxy->colors;
    double d;

    int gate_num_scaled;
    int start_gate_num_scaled = RXY_SCALE(rxy->edge0.p0.gate_num);
    int x_gate_inc_scaled = RXY_SCALE(rxy->edge0.major_gate_inc);
    int y0_pxl_inc_scaled = RXY_SCALE(rxy->edge0.y_inc);
    int y1_pxl_inc_scaled = RXY_SCALE(rxy->edge1.y_inc);
    int y_gate_inc_scaled = RXY_SCALE(rxy->edge0.minor_gate_inc);
    int y0_pxl_scaled = RXY_SCALE(rxy->edge0.p0.y);
    int y1_pxl_scaled = RXY_SCALE(rxy->edge1.p0.y);
    int hits = 0;

    register short *gate_num;
    register short *y_gate_inc;
    register short *y0_pxl;
    register short *y1_pxl;

    ii = rxy->edge0.num_iters;

    if((ii = rxy->edge0.num_iters) < 1) {
       return;
    }
    if(y0_pxl_scaled < 0) y0_pxl_scaled = 0;

    gate_num = LittleEndian
      ? (short *)&gate_num_scaled +1 : (short *)&gate_num_scaled;
    y_gate_inc = LittleEndian
      ? (short *)&y_gate_inc_scaled +1 : (short *)&y_gate_inc_scaled;
    y0_pxl = LittleEndian
      ? (short *)&y0_pxl_scaled +1 : (short *)&y0_pxl_scaled;
    y1_pxl = LittleEndian
      ? (short *)&y1_pxl_scaled +1 : (short *)&y1_pxl_scaled;

    max = rxy->image_width * rxy->image_height -1;
    width = rxy->image_width;
    ndx0 = IFIX(rxy->edge0.p0.x);
    if(ndx0 < 0) ndx0 = 0;
    /*
     * remember...image y coords are reversed!
     */
    for(; ii--; ndx0++) {
	ndx = ndx0 +(flipit -(*y0_pxl)) * width;
	jj = *y1_pxl -(*y0_pxl) +1;
	gate_num_scaled = start_gate_num_scaled;

	for(; jj-- > 0; ndx-=width) {
# ifdef obsolete
	    if(ndx > max || ndx < 0) {
		mark = 0;
		*deep6 = 1;
	    }
# endif
	    *(image +ndx) = *(colors +(*gate_num));
	    gate_num_scaled += y_gate_inc_scaled;
	    hits++;
	}

	start_gate_num_scaled += x_gate_inc_scaled;
	y0_pxl_scaled += y0_pxl_inc_scaled;
	y1_pxl_scaled += y1_pxl_inc_scaled;
    }
    if (sii_debug_flag()) {
       g_string_sprintfa (gs_complaints, " yy loop ii:%d h:%d", ii, hits);
       g_message (gs_complaints->str);
       g_string_truncate (gs_complaints, 0);
    }
}
/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

/* c------------------------------------------------------------------------ */

