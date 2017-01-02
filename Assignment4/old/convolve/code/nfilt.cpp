/*
   Program to take convolve an image with a convolution kernel and
   display the result as an image.  The filtered image is written
   to an output file, if a filename is provided.

   Version modified to do image normalization as a post-process to use
   full dynamic range of the display.

   Command line parameters are as follows:

   nfilt [-n[orm]] inimage.ppm kernel.filt [outimage.ppm]

   VIZA 654            Donald H. House         10/1/96
               modified to use OpenGL 10/8/99

   To compile:
    cc -I /usr/local/include -o nfilt nfilt.c -L /usr/local/lib -lglut -lGLU -lGL -lXmu -lX11 -lm

*/

#include <stdio.h>	/* definitions for standard I/O routines */
#include <ctype.h>	/* character processing routines */
#include <math.h>
#include <GL/gl.h>	/* definitions for GL graphics routines */
#include <GL/glu.h>	/* definitions for GL input device handling */
#include <glut.h>

#include "floatimage.h" /* definitions for floating point image routines */
#include "ppmrw.h"	/* definitions for ppm read/write routines */

#define R	0	/* color primary indices for Channel arrays */
#define G	1
#define B	2

/*
  data structure to hold filter convolution kernel
*/
typedef struct _kernel{
  int size;
  float offset, range;
  float scale;
  float *weights;
}Kernel;

/*
  Global variables required by the GLUT interface 
*/
int inwindow, outwindow;	/* input/output window ids */
FloatImage inimage, outimage;
FILE *filtfile;
int donorm;
Kernel *kernel;

/*
   Make sure that we have at least two input file parameters, and a
   single possible output file parameter.  Abort otherwise.
   Open input, output, and filter files if specified
*/
#define INF	 0
#define FILTF	 1
#define OUTF	 2
#define MINARGS	 2
#define MAXARGS	 4
void process_args(int argc, char *argv[],
		  int *donorm,
		  FloatImage *inimage,
		  FILE **filtfile, char *filtfilename,
		  FloatImage *outimage){
  int i;
  int whichfile;

  *donorm = FALSE;

  if(argc < MINARGS + 1 || argc > MAXARGS + 1)
    goto error;

  outimage->fptr = NULL;

  for(i = 1, whichfile = INF; i < argc; i++){
    if(argv[i][0] == '-'){
      if(strcmp(argv[i], "-n") == 0 ||
	 strcmp(argv[i], "-no") == 0 ||
	 strcmp(argv[i], "-nor") == 0 ||
	 strcmp(argv[i], "-norm") == 0)
	*donorm = TRUE;
    }
    else{
      switch(whichfile++){
      case INF:
	strcpy(inimage->filename, argv[i]);
	if((inimage->fptr = fopen(argv[i], "r")) == NULL){
	  fprintf(stderr, "nfilt -- input PPM file %s cannot be opened\n",
		  argv[i]);
	  goto error;
	}
	break;
      case FILTF:
	strcpy(filtfilename, argv[i]);
	if((*filtfile = fopen(argv[i], "r")) == NULL){
	  fprintf(stderr,"nfilt -- filter kernel file %s cannot be opened\n",
		  argv[i]);
	  goto error;
	}
	break;
      case OUTF:
	strcpy(outimage->filename, argv[i]);
	if((outimage->fptr = fopen(argv[i], "w")) == NULL){
	  fprintf(stderr, "nfilt -- output PPM file %s cannot be opened\n",
		  argv[i]);
	  goto error;
	}
	break;
      }
    }
  }
  return;

 error:
  fprintf(stderr,
	  "filt -- usage: nfilt [-n] inimage.ppm kernel.filt [outimage.ppm]\n");
  exit(1);
}

/*
  Load the convolution kernel information from the kernel file
*/
Kernel *read_kernel(FILE *filtfile){

  Kernel *kernel;
  int row, col, i;
  double negsum, possum;

  if((kernel = (Kernel *)malloc(sizeof(Kernel))) == NULL)
    return NULL;

  if(fscanf(filtfile, "%d %f", &(kernel->size), &(kernel->scale)) != 2){
    free(kernel);
    return NULL;
  }

  kernel->scale = (kernel->scale == 0)? 1: kernel->scale;
  printf("filter kernel size = %d X %d, scale factor = 1/%f\n\n",
	 kernel->size, kernel->size, kernel->scale);

  if(kernel->size < 1 || kernel->size > 99 || kernel->size % 2 == 0){
    fprintf(stderr,"kernel size invalid, must be odd and between 1 and 99\n");
    free(kernel);
    return NULL;
  }
  
  kernel->weights = (float *)calloc(kernel->size*kernel->size, sizeof(float));

  for(row = 0, i = 0; row < kernel->size; row++){
    for(col = 0; col < kernel->size; col++, i++){
      if(fscanf(filtfile, "%f", &(kernel->weights[i])) != 1){
	free(kernel);
	return NULL;
      }
      printf("%5.1f ", kernel->weights[i]);
    }
    printf("\n");
  }
  printf("\n");

  negsum = possum = 0;
  for(i = 0; i < kernel->size * kernel->size; i++){
    if(kernel->weights[i] < 0)
      negsum += kernel->weights[i];
    else
      possum += kernel->weights[i];
  }
  kernel->offset = -negsum;
  kernel->range = possum - negsum;

  return kernel;
}

/*
  Convolve an inimage with convolution kernel, and store result in outimage
*/
void doconvolve(FloatImage *inimage, FloatImage *outimage,
		Kernel *kernel, int normalize){

  int indent;
  int row, col;
  int krow, kcol;
  int i, k, imindx;
  double rsum, gsum, bsum;
  double maxch, minch;

  maxch = 0;
  minch = 1;
  indent = kernel->size / 2;
  for(row = indent, i = indent * inimage->width + indent;
      row < inimage->height - indent; row++, i += 2 * indent)
    for(col = indent; col < inimage->width - indent; col++, i++){
      rsum = gsum = bsum = 0;
      for(krow = k = 0, imindx = i - indent * inimage->width - indent;
	  krow < kernel->size;
	  krow++, imindx += inimage->width - kernel->size)
	for(kcol = 0; kcol < kernel->size; kcol++, imindx++, k++){
	  rsum += kernel->weights[k] * inimage->Channel[R][imindx];
	  gsum += kernel->weights[k] * inimage->Channel[G][imindx];
	  bsum += kernel->weights[k] * inimage->Channel[B][imindx];
	}

      if(normalize){
	rsum = (rsum + kernel->offset) / kernel->range;
	gsum = (gsum + kernel->offset) / kernel->range;
	bsum = (bsum + kernel->offset) / kernel->range;
	/*printf("%d %d %d ", (int)rsum, (int)gsum, (int)bsum);*/
      }
      else{
	rsum = abs(rsum) / kernel->scale;
	gsum = abs(gsum) / kernel->scale;
	bsum = abs(bsum) / kernel->scale;
      }
      if(rsum > maxch)
	maxch = rsum;
      if(gsum > maxch)
	maxch = gsum;
      if(bsum > maxch)
	maxch = bsum;
      if(rsum < minch)
	minch = rsum;
      if(gsum < minch)
	minch = gsum;
      if(bsum < minch)
	minch = bsum;

      imindx = row * inimage->width + col;
      outimage->Channel[R][imindx] = rsum;
      outimage->Channel[G][imindx] = gsum;
      outimage->Channel[B][imindx] = bsum;
    }

  for(row = 0; row < inimage->height; row++)
    for(col = 0; col < inimage->width; col++){
      imindx = row * inimage->width + col;
      rsum = (outimage->Channel[R][imindx] - minch) / (maxch - minch);
      gsum = (outimage->Channel[G][imindx] - minch) / (maxch - minch);
      bsum = (outimage->Channel[B][imindx] - minch) / (maxch - minch);

      outimage->Channel[R][imindx] = rsum;
      outimage->Channel[G][imindx] = gsum;
      outimage->Channel[B][imindx] = bsum;      
    }
  topixmap(outimage, 1);
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
   Main program to read a PPM image file, take its DFT, an optional 
   PPM filter file (taken to already be in the Frequency Domain),
   compute their product and display.
*/
int main(int argc, char *argv[]){

  char filtfilename[MAXCHARS];

  /*
     make sure command line is valid, open input files as necessary,
     and set rgb flags per command line switches
  */
  process_args(argc, argv, &donorm, &inimage, &filtfile, filtfilename,
	       &outimage);
  
  /* read the PPM input file */
  if(!read_ppm(&inimage)){
    fprintf(stderr, "nfilt -- error reading input PPM file %s\n",
	    inimage.filename);
    exit(1);
  }

  /* read the filter kernel file, and print it */
  if((kernel = read_kernel(filtfile)) == NULL){
    fprintf(stderr, "nfilt -- error reading filter kernel file %s\n",
	    filtfilename);
    exit(1);
  }
  
  tochannels(&inimage);	/* copy the pixmap to channel arrays */

  /* make the output image the same size and type as the input image */
  outimage.rgb = inimage.rgb;
  outimage.width = inimage.width;
  outimage.height = inimage.height;
  makearrays(&outimage);

  /* convolve input image with the kernel */
  doconvolve(&inimage, &outimage, kernel, donorm);
  tochannels(&outimage);

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
    outwindow = glutCreateWindow("result window");

  /* input window callbacks */
  glutDisplayFunc(displayOutput);
  glutMouseFunc(handleButton);

  /* window coordinates = pixel coordinates */
  gluOrtho2D(0, inimage.width, 0, inimage.height);

  glutMainLoop();
}
