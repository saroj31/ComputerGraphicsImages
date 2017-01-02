/*
   Program to apply a median filter to an input image and
   display the result as an image.  The filtered image is written
   to an output file, if a filename is provided.

   Command line parameters are as follows:

   median inimage.ppm [outimage.ppm]

   VIZA 654            Donald H. House         10/1/96
               modified to use OpenGL 10/8/99

   To compile:
    cc -I /usr/local/include -o median median.c -L /usr/local/lib -lglut -lGLU -lGL -lXmu -lX11 -lm

*/

#include <stdio.h>	/* definitions for standard I/O routines */
#include <ctype.h>	/* character processing routines */
#include <math.h>
#include <GL/gl.h>	/* definitions for GL graphics routines */
#include <GL/glu.h>	/* definitions for GL input device handling */
#include <glut.h>

#include "floatimage.h" /* definitions for floating point image routines */
#include "ppmrw.h"	/* definitions for ppm read/write routines */

#define max(x, y, z)	((x) > (y)? ((x) > (z)? (x): (z)): ((y) > (z)? (y): (z)))
#define avg(x, y, z)	(((x) + (y) + (z)) / 3.0)

#define R	0	/* color primary indices for Channel arrays */
#define G	1
#define B	2

#define THRESHOLD	0.2

/*
  Global variables required by the GLUT interface 
*/
int inwindow, outwindow;	/* input/output window ids */
FloatImage inimage, outimage;

/*
   Make sure that we have at least an input file parameter and a
   single possible output file parameter.  Abort otherwise.
   Open input and output files if specified
*/
#define INF	 0
#define OUTF	 1
#define MINARGS	 2
#define MAXARGS	 3
void process_args(int argc, char *argv[],
		  int *kernelsize,
		  FloatImage *inimage,
		  FloatImage *outimage){
  int i;
  int whichfile;

  if(argc < MINARGS + 1 || argc > MAXARGS + 1)
    goto error;

  *kernelsize = 0;
  *kernelsize = atoi(argv[1]);
  if(*kernelsize % 2 != 1){
    fprintf(stderr, "median -- kernel size of %s must be an odd integer\n",
	    argv[1]);
    goto error;    
  }

  outimage->fptr = NULL;

  for(i = 2, whichfile = INF; i < argc; i++){
    switch(whichfile++){
    case INF:
      strcpy(inimage->filename, argv[i]);
      if((inimage->fptr = fopen(argv[i], "r")) == NULL){
	fprintf(stderr, "median -- input PPM file %s cannot be opened\n",
		argv[i]);
	goto error;
      }
      break;
    case OUTF:
      strcpy(outimage->filename, argv[i]);
      if((outimage->fptr = fopen(argv[i], "w")) == NULL){
	fprintf(stderr, "median -- output PPM file %s cannot be opened\n",
		argv[i]);
	goto error;
      }
      break;
    }
  }

  return;

 error:
  fprintf(stderr,
	  "usage: median kernelsize inimage.ppm [outimage.ppm]\n");
  exit(1);
}

/*
  Median filter inimage and store result in outimage
*/
void domedian(FloatImage *inimage, FloatImage *outimage, int kernelsize){

  int indent;
  int row, col;
  int krow, kcol;
  int i, k, kk, imindx, midx, imedian;
  double v, vmedian;
  double *values;
  int *idx;
  int jj;

  values = (double *)malloc(kernelsize * kernelsize * sizeof(double));
  idx = (int *)malloc(kernelsize * kernelsize * sizeof(int));

  indent = kernelsize / 2;
  for(row = indent, i = indent * inimage->width + indent;
      row < inimage->height - indent; row++, i += 2 * indent)
    for(col = indent; col < inimage->width - indent; col++, i++){
      for(krow = k = 0, imindx = i - indent * inimage->width - indent;
	  krow < kernelsize;
	  krow++, imindx += inimage->width - kernelsize)
	for(kcol = 0; kcol < kernelsize; kcol++, imindx++, k++){
	  v = avg(inimage->Channel[R][imindx], 
		  inimage->Channel[G][imindx],
		  inimage->Channel[B][imindx]);
	  for(kk = k; kk > 0 && values[kk - 1] > v; kk--){
	    values[kk] = values[kk - 1];
	    idx[kk] = idx[kk - 1];
	  }
	  values[kk] = v;
	  idx[kk] = imindx;
	}

      midx = (kernelsize * kernelsize) / 2;
      vmedian = values[midx];
      imedian = idx[midx];
      v = avg(inimage->Channel[R][i], 
	      inimage->Channel[G][i], 
	      inimage->Channel[B][i]);
      if(fabs(vmedian - v) < THRESHOLD){
	outimage->Channel[R][i] = inimage->Channel[R][i];
	outimage->Channel[G][i] = inimage->Channel[G][i];
	outimage->Channel[B][i] = inimage->Channel[B][i];
      }
      else{
	/* 
	  printf("i = %d, v = %lf\n", i, v);
	  for(jj = 0; jj < kernelsize * kernelsize; jj ++){
	    printf("(%d, %lf) ", idx[jj], values[jj]);
	  }
	  printf("\n");
	*/
	outimage->Channel[R][i] = inimage->Channel[R][imedian];
	outimage->Channel[G][i] = inimage->Channel[G][imedian];
	outimage->Channel[B][i] = inimage->Channel[B][imedian];
      }
    }

  topixmap(outimage, 1);

  free(values);
  free(idx);
}

/*
   Wait for any mouse button to be pressed, and exit the program
*/
void handleButton(int button, int state, int x, int y){

  if(state != GLUT_UP)
    return;
  else{
    /* if an output file is provided, write image to the file */
    if(outimage.fptr != NULL)
      write_ppm(&outimage);
    exit(0);
  }
}

void displayInput(){
  glRasterPos2i(0, 0);
  glDrawPixels(inimage.width, inimage.height, GL_RGBA, GL_UNSIGNED_BYTE, 
	       inimage.pixmap);
  glFlush();
}

void displayOutput(){
  glRasterPos2i(0, 0);
  glDrawPixels(outimage.width, outimage.height, GL_RGBA, GL_UNSIGNED_BYTE,
	       outimage.pixmap);
  glFlush();
}

/*
   Main program to read a PPM image file, apply the median filter
   and display.
*/
int main(int argc, char *argv[]){

  int kernelsize;

  /*
     make sure command line is valid, open input files as necessary,
     and set rgb flags per command line switches
  */
  process_args(argc, argv, &kernelsize, &inimage, &outimage);
  
  /* read the PPM input file */
  if(!read_ppm(&inimage)){
    fprintf(stderr, "median -- error reading input PPM file %s\n",
	    inimage.filename);
    exit(1);
  }
  
  tochannels(&inimage);	/* copy the pixmap to channel arrays */

  /* make the output image the same size and type as the input image */
  outimage.rgb = inimage.rgb;
  outimage.width = inimage.width;
  outimage.height = inimage.height;
  makearrays(&outimage);

  /* median filter the input image */
  domedian(&inimage, &outimage, kernelsize);

  /* start up the glut utilities, using RGBA single buffered */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

  /*
    create the RGBA graphics window for the input image
  */
  glutInitWindowSize(inimage.width, inimage.height);
  inwindow = glutCreateWindow(inimage.filename);

  /* input window callbacks */
  glutDisplayFunc(displayInput);
  glutMouseFunc(handleButton);

  /* window coordinates = pixel coordinates */
  gluOrtho2D(0, inimage.width, 0, inimage.height);

  /*
    create the RGBA graphics window for the output image
  */
  glutInitWindowSize(outimage.width, inimage.height);
  if(outimage.fptr != NULL)
    outwindow = glutCreateWindow(outimage.filename);
  else
    outwindow = glutCreateWindow("Median Filtered Image");

  /* input window callbacks */
  glutDisplayFunc(displayOutput);
  glutMouseFunc(handleButton);

  /* window coordinates = pixel coordinates */
  gluOrtho2D(0, inimage.width, 0, inimage.height);

  glutMainLoop();
}
