/*
 *	$Id$
 *
 *	Module:		 IndFreq.h
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
 * Revision 1.2  1994/04/26 22:05:15  case
 * added an #endif and moved the #ifdef OK_RPC.
 *
 * Revision 1.1  1994/04/26  21:55:05  case
 * Initial revision
 *
 * Revision 1.1  1992/07/28  17:23:46  thor
 * Initial revision
 *
 *
 *
 * description:
 *        
 */
#ifndef INCIndFreqh
#define INCIndFreqh

#ifdef OK_RPC

#if defined(UNIX) && defined(sun)
#include <rpc/rpc.h>
#else
#if defined(WRS)
#include "rpc/rpc.h"
#endif /* WRS */

#endif /* UNIX */

#endif /* OK_RPC */

struct indep_freq {
    char  indep_freq_id[4];	/* Identifier = INDF. */
    long  indep_freq_len;	/* Block size in bytes. */
};


typedef struct indep_freq indep_freq;
typedef struct indep_freq INDEP_FREQ;

#ifdef OK_RPC
#if defined(sun) || defined(WRS)
extern bool_t xdr_indep_freq(XDR *, indep_freq *);
#endif

#endif /* OK_RPC */
#endif /* INCIndFreqh */

