/* 	$Id$	 */

#ifndef lint
static char vcid[] = "$Id$";
#endif /* lint */
# include <dorade_headers.h>

void dd_derived_fields();
void dd_pct_stats();
void dd_cappiz();
void produce_uf();
void dd_gecho();
void dd_product_x();
void produce_nssl_mrd();
void produce_shanes_data();

/* c------------------------------------------------------------------------ */

void by_products(dgi, ztime)
  struct dd_general_info *dgi;
  time_t ztime;
{
    dd_derived_fields(dgi);
    dd_pct_stats(dgi);		/* this routine is in the same file with
				 * dd_derived_fields
				 */
    produce_uf(dgi);
    dd_gecho(dgi);
    dd_product_x(dgi);
    produce_nssl_mrd(dgi, NO);
    produce_shanes_data(dgi);
    produce_ncdf( dgi, NO );

}
/* c------------------------------------------------------------------------ */
