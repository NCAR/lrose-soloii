/*
 *	$Id$
 *
 *	Module:		 MiniRIMS.h
 *	Original Author: Richard E. Neitzel
 *      Copywrited by the National Center for Atmospheric Research
 *	Date:		 $Date$
 *
 * revision history
 * ----------------
 * $Log$
 * Revision 1.1  2002/07/02 14:57:34  oye
 * Initial revision
 *
 * Revision 1.3  1994/04/26 22:01:37  case
 * added an #endif.
 *
 * Revision 1.2  1994/04/05  15:31:08  case
 * moved an ifdef RPC and changed an else if to make HP compiler happy.
 *
 * Revision 1.1  1992/07/28  17:23:46  thor
 * Initial revision
 *
 *
 *
 * description:
 *        
 */
#ifndef INCMiniRIMSh
#define INCMiniRIMSh

#ifdef OK_RPC

#if defined(UNIX) && defined(sun)
#include <rpc/rpc.h>
#else
# if defined(WRS)
#include "rpc/rpc.h"
#endif /* WRS */

#endif /* UNIX */

#endif /* OK_RPC */

struct minirims_data {
    char  minirims_data_id[4];	/* Identifier = MINI. */
    long  minirims_data_len;	/* Block size in bytes. */
    short command;		/* Current command latch setting. */
    short status;		/* Current status. */
    float temperature;		/* Degrees C. */
    float x_axis_gyro[128];	/* Roll axis gyro position. */
    float y_axis_gyro[128];	/* Pitch axis gyro position. */
    float z_axis_gyro[128];	/* Yaw axis gyro position. */
    float xr_axis_gyro[128];	/* Roll axis redundate gyro position. */
    float x_axis_vel[128];	/* Longitudinal axis velocity. */
    float y_axis_vel[128];	/* Lateral axis velocity. */
    float z_axis_vel[128];	/* Vertical axis velocity. */
    float x_axis_pos[128];	/* Roll axis gimbal. */
};


typedef struct minirims_data minirims_data;
typedef struct minirims_data MINIRIMS_DATA;

#ifdef OK_RPC
#if defined(sun) || defined(WRS)
extern bool_t xdr_minirims_data(XDR *, minirims_data *);
#endif

#endif /* OK_RPC */
#endif /* INCMiniRIMSh */

