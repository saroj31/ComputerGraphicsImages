/*
   Standard definitions for the floatimage data object

   D. H. House  10/12/94
*/

#ifndef _FLOATIMAGE
#define _FLOATIMAGE

#include <cstdio>

/* #define DEBUG 1 */	/* define DEBUG for debug printouts */

#define PI		3.1415926536

#define Round(x)	((x) >= 0? (int)((x) + 0.5): (int)((x) - 0.5))
#define Sqr(x)		((x) * (x))
#define Abs(x)		((x) >= 0? (x): -(x))

#define MAXCHARS	255

/*
   Definitions handy for building floating point
   version of image, for purposes of image calculations
*/

/* channel array indices */
enum channels {Red, Green, Blue, Alpha}; 

typedef unsigned char Pixel[4];

/* floating point image structure */
struct FloatImage{
private:
  void clone(const FloatImage& im);
  
public:
  char filename[MAXCHARS];
  int window;
  int width, height;
  Pixel **pixmap;
  float **Channel[4];

  FloatImage();
  FloatImage(const FloatImage& im);
  ~FloatImage();
  
  int read_image();
  void write_image();
  
  void tochannels();
  void topixmap(float gamma);
  void makearrays();
  
  const FloatImage& operator=(const FloatImage& im);
};

#endif
