/* 	$Id$	 */

# ifndef SII_IMAGE_H
# define SII_IMAGE_H


typedef struct {
  int bits_per_pixel;
  int bytes_per_line;
  int byte_order;
  int width;
  int height;
  void *data;

} SiiImage;


# endif 
