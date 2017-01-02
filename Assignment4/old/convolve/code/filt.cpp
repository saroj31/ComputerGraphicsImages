/*
 Program to convolve an image with a convolution kernel and
 display the result as an image.  The filtered image is written
 to an output file, if a filename is provided.
 
 Command line parameters are as follows:
 
 filt [-n[orm]] inimage.png kernel.filt [outimage.png]
 
 CPSC 404/604        Donald H. House         10/11/11
 modified to use OpenGL 10/8/99
 modified to use ImageMagick 10/6/11
 */

#include <cstdlib>
#include <cstdio>	/* definitions for standard I/O routines */
#include <cstring>
#include <cctype>	/* character processing routines */
#include <cmath>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "FloatImage.h" /* definitions for floating point image routines */
#include "Kernel.h"

using namespace std;

#define ESC	27	// ASCII esc code

#define R	0	/* color primary indices for Channel arrays */
#define G	1
#define B	2

/*
 Global variables required by the GLUT interface 
 */
FloatImage inimage, outimage;
FILE *filtfile;
int donorm;
Kernel kernel;

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
				  int &donorm,
				  FloatImage &inimage,
				  FILE **filtfile, 
				  FloatImage &outimage){
  int i;
  int whichfile;
  char filtfilename[256];
  FILE *fptr;
  
  donorm = false;
  
  if(argc < MINARGS + 1 || argc > MAXARGS + 1)
    goto error;
  
  for(i = 1, whichfile = INF; i < argc; i++){
    if(argv[i][0] == '-'){
      if(strcmp(argv[i], "-n") == 0 ||
		 strcmp(argv[i], "-no") == 0 ||
		 strcmp(argv[i], "-nor") == 0 ||
		 strcmp(argv[i], "-norm") == 0)
		donorm = true;
    }
    else{
      switch(whichfile++){
		case INF:
		  strcpy(inimage.filename, argv[i]);
		  if((fptr = fopen(argv[i], "r")) == NULL){
			fprintf(stderr, "filt -- input image file %s cannot be opened\n",
					argv[i]);
			goto error;
		  }
		  fclose(fptr);
		  break;
		case FILTF:
		  strcpy(filtfilename, argv[i]);
		  if((*filtfile = fopen(argv[i], "r")) == NULL){
			fprintf(stderr,"filt -- filter kernel file %s cannot be opened\n",
					argv[i]);
			goto error;
		  }
		  break;
		case OUTF:
		  strcpy(outimage.filename, argv[i]);
		  if((fptr = fopen(argv[i], "w")) == NULL){
			fprintf(stderr, "filt -- output image file %s cannot be opened\n",
					argv[i]);
			goto error;
			fclose(fptr);
		  }
		  break;
      }
    }
  }
  return;
  
error:
  fprintf(stderr,
		  "filt -- usage: filt [-n] inimage.png kernel.filt [outimage.png]\n");
  exit(1);
}

/*
	Convolve an inimage with convolution kernel, and store result in outimage
*/
void doconvolve(const FloatImage &inimage, FloatImage &outimage,
				const Kernel &kernel, int normalize){
  int rindent, cindent;
  int row, col;
  int krow, kcol;
  int ch;
  int rowoffset, coloffset;
  double sum[3];
  
  rindent = kernel.height / 2;
  cindent = kernel.width / 2;
  for(row = rindent; row < inimage.height - rindent; row++)
    for(col = cindent; col < inimage.width - cindent; col++){
	  for(ch = 0; ch < 3; ch++)
		sum[ch] = 0;
      for(krow = 0; krow < kernel.height; krow++){
		rowoffset = kernel.height - krow - 1 - kernel.height / 2;
		for(kcol = 0; kcol < kernel.width; kcol++){
		  coloffset = kernel.width - kcol - 1 - kernel.width / 2;
		  for(int ch = 0; ch < 3; ch++)
			sum[ch] += kernel.weights[krow][kcol] * 
						inimage.Channel[ch][row + rowoffset][col + coloffset];
		}
	  }
	  
      if(normalize)
		for(ch = 0; ch < 3; ch++)
		  sum[ch] = 255 * (sum[ch] + kernel.offset) / kernel.range;
      else
		for(ch = 0; ch < 3; ch++)
		  sum[ch] = 255 * abs(sum[ch]) / kernel.scale;
	  
	  for(ch = 0; ch < 3; ch++){
		sum[ch] = (sum[ch] > 255)? 255: (sum[ch] < 0? 0: sum[ch]);
		outimage.pixmap[row][col][ch] = int(sum[ch]);
	  }
	  outimage.pixmap[row][col][3] = 255;
    }
}

/*
 Wait for any mouse button to be pressed, and exit the program
 */
void handleButton(int button, int state, int x, int y){
  
  if(state != GLUT_UP)
    return;
  else{
    /* if an output filename is provided, write image to the file */
    if(outimage.filename != NULL)
      outimage.write_image();
    exit(0);
  }
}

void displayInput(){
  glRasterPos2i(0, 0);
  glDrawPixels(inimage.width, inimage.height, GL_RGBA, GL_UNSIGNED_BYTE, 
			   inimage.pixmap[0]);
  glutSwapBuffers();
}

void displayOutput(){
  glRasterPos2i(0, 0);
  glDrawPixels(outimage.width, outimage.height, GL_RGBA, GL_UNSIGNED_BYTE,
			   outimage.pixmap[0]);
  glutSwapBuffers();
}

void handleKeys(unsigned char key, int x, int y){
  switch(key){
	case 'q':
	case 'Q':
	case ESC:
	  exit(0);
  }
}

/*
 Main program to read a PPM image file, and do a spatial convolution
 to filter it.
 */
int main(int argc, char *argv[]){
  
  /*
   make sure command line is valid, open input files as necessary,
   and set rgb flags per command line switches
   */
  process_args(argc, argv, donorm, inimage, &filtfile, outimage);
  
  /* read the input image file */
  if(!inimage.read_image()){
    fprintf(stderr, "filt -- error reading input image file %s\n",
			inimage.filename);
    exit(1);
  }
  inimage.tochannels();	/* copy the pixmap to channel arrays */
  
  /* read the filter kernel file, and print it */
  if(!kernel.Read(filtfile)){
    fprintf(stderr, "filt -- error reading filter kernel data %s\n",
			inimage.filename);
    exit(1);
  }

  /* make the output image the same size and type as the input image */
  outimage.width = inimage.width;
  outimage.height = inimage.height;
  outimage.makearrays();
  
  /* convolve input image with the kernel */
  doconvolve(inimage, outimage, kernel, donorm);
  outimage.tochannels();
  
  /* start up the glut utilities, using RGBA single buffered */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  
  /*
   create the RGBA graphics window for the input image
   */
  glutInitWindowSize(inimage.width, inimage.height);
  inimage.window = glutCreateWindow(inimage.filename);
  
  /* window coordinates = pixel coordinates */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, inimage.width, 0, inimage.height);
  
  /* input window callbacks */
  glutDisplayFunc(displayInput);
  glutMouseFunc(handleButton);
  glutKeyboardFunc(handleKeys);
  
  /*
   create the RGBA graphics window for the output image
  */
  glutInitWindowSize(outimage.width, inimage.height);
  if(outimage.filename[0] != '\0')
    outimage.window = glutCreateWindow(outimage.filename);
  else
    outimage.window = glutCreateWindow("result window");

  /* window coordinates = pixel coordinates */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, inimage.width, 0, inimage.height);
  glMatrixMode(GL_MODELVIEW);
  
  /* output window callbacks */
  glutDisplayFunc(displayOutput);
  glutMouseFunc(handleButton);
  glutKeyboardFunc(handleKeys);
  
  glutMainLoop();
}
