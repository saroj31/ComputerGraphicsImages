/*
   Program to make several images for use in filter tests

   Command line parameters are as follows:

   makeimages

   VIZA 654            Donald H. House         10/14/98
*/

#include <stdio.h>	/* definitions for standard I/O routines */
#include <ctype.h>	/* character processing routines */
#include <stdlib.h>	/* standard C library */
#include <math.h>
#include <gl/gl.h>	/* definitions for GL graphics routines */
#include <gl/device.h>	/* definitions for GL input device handling */

#include "floatimage.h" /* definitions for floating point image routines */
#include "ppmrw.h"	/* definitions for ppm read/write routines */

#define R	0	/* color primary indices for Channel arrays */
#define G	1
#define B	2

/*
   Make sure that we have no file parameters Abort otherwise.
*/
#define INF	 0
#define FILTF	 0
#define OUTF	 0
#define MINARGS	 0
#define MAXARGS	 0
void process_args(int argc, char *argv[],
		  FloatImage *sqrimage,
		  FloatImage *lineimage,
		  FloatImage *sineimage){

  if(argc < MINARGS + 1 || argc > MAXARGS + 1)
    goto error;

  if((sqrimage->fptr = fopen("square.ppm", "w")) == NULL){
    fprintf(stderr, 
	    "makeimages - output PPM file square.ppm cannot be opened\n");
    goto error;
  }
  if((lineimage->fptr = fopen("lines.ppm", "w")) == NULL){
    fprintf(stderr, 
	    "makeimages - output PPM file lines.ppm cannot be opened\n");
    goto error;
  }
  if((sineimage->fptr = fopen("sines.ppm", "w")) == NULL){
    fprintf(stderr, 
	    "makeimages - output PPM file sines.ppm cannot be opened\n");
    goto error;
  }
  return;

 error:
  fprintf(stderr,
	  "makeimages - usage: makeimages\n");
  exit(1);
}

/*
   Open the GL Window and switch to full color RGB mode
*/
void open_window(int width, int height, char *title){
  long backcolor[] = {0, 0, 0};  /* black */

  /* open window */
  prefsize(width, height);
  winopen(title);

  RGBmode();		/* we are going to do colors as RGB triples */
  gconfig();
}

/*
   open a GL window and display the image pixmap
*/
void display_image(FloatImage *image){
  
  open_window(image->width, image->height, image->filename);

  lrectwrite(0, 0, image->width - 1, image->height - 1, image->pixmap);
}

/*
   Wait for any mouse button to be pressed.  Also, catch any REDRAW
   events from the window manager, and redraw the window.
*/
void wait_for_click(){
  long device;
  short status;

  qdevice(LEFTMOUSE);
  qdevice(MIDDLEMOUSE);
  qdevice(RIGHTMOUSE);
  do{
    device = qread(&status);
  }while(!(device == LEFTMOUSE || device == MIDDLEMOUSE ||
	   device == RIGHTMOUSE));
}

void buildsquare(FloatImage *sqrimage){
  unsigned long *rastloc;
  int row, col;
  int red, green, blue;

  sqrimage->width = sqrimage->height = 256;
  sqrimage->rgb = 3;
  makearrays(sqrimage);

  rastloc = sqrimage->pixmap;
  for(row = 0; row < sqrimage->height; row++)
    for(col = 0; col < sqrimage->width; col++, rastloc++){
      if(row > 64 && row < 256 - 64 && col > 64 && col < 256 - 64)
	red = green = blue = 255;
      else
	red = green = blue = 0;
      *rastloc = blue << 16 | green << 8 | red;      
    }
}

void buildlines(FloatImage *lineimage){
  unsigned long *rastloc;
  int row, col;
  int red, green, blue;

  lineimage->width = lineimage->height = 256;
  lineimage->rgb = 3;
  makearrays(lineimage);

  rastloc = lineimage->pixmap;
  for(row = 0; row < lineimage->height; row++)
    for(col = 0; col < lineimage->width; col++, rastloc++){
      if((col / 4) % 2 == 0)
	red = green = blue = 255;
      else
	red = green = blue = 0;
      *rastloc = blue << 16 | green << 8 | red;      
    }
}

void buildsine(FloatImage *sineimage){
  unsigned long *rastloc;
  int row, col;
  int red, green, blue;
  double r;
  int grey;
  static double pi = 3.1415926536;

  sineimage->width = sineimage->height = 256;
  sineimage->rgb = 3;
  makearrays(sineimage);

  rastloc = sineimage->pixmap;
  for(row = 0; row < sineimage->height; row++)
    for(col = 0; col < sineimage->width; col++, rastloc++){
      r = sqrt(sqr((row - 128) / 127.0) + sqr((col - 128) / 127.0));
      grey = round(255 * ((1 + sin(2 * pi * 12 * sqr(r))) / 2));
      red = green = blue = grey;
      *rastloc = blue << 16 | green << 8 | red;      
    }
}

/*
   Main program to read a PPM image file, take its DFT, an optional 
   PPM filter file (taken to already be in the Frequency Domain),
   compute their product and display.
*/
int main(int argc, char *argv[]){
  FloatImage sqrimage, lineimage, sineimage;

  /*
     make sure command line is valid, open input files as necessary
  */
  process_args(argc, argv, &sqrimage, &lineimage, &sineimage);
  
  foreground();		/* when we start GL, stay connected to winterm
			   so that printf() and scanf() operate normally */

  /* build the square image, and display it */
  buildsquare(&sqrimage);
  display_image(&sqrimage);
  /* build the lines image, and display it */
  buildlines(&lineimage);
  display_image(&lineimage);
  /* build the sine image, and display it */
  buildsine(&sineimage);
  display_image(&sineimage);

  /* write the images to files */
  write_ppm(&sqrimage);
  write_ppm(&lineimage);
  write_ppm(&sineimage);

  /* wait for a mouse button to be pressed, then quit */
  wait_for_click();
}
