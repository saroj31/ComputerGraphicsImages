/*
   Sample Solution to Quiz 5
   CP SC 404/604            Donald H. House         10/4/2011
 
   Program to read, display and write an image in original, normalized, and 
   histogram equalized forms. 
 
   The program responds to the following keyboard commands:
    r or R: prompt for an input image file name, read the image into 
	    an appropriately sized pixmap, resize the window, and display
    w or W: prompt for an output image file name, read the display into a pixmap,
	    and write from the pixmap to the file.
    h or H: toggle display between original, normalized, and equalized images
    q, Q or ESC: quit.
 
   When the window is resized by the user: If the size of the window becomes bigger than
   the image, the image is centered in the window. If the size of the window becomes 
   smaller than the image, the image is uniformly scaled down to the largest size that
   fits in the window.
 
   usage: histeq [filename.ext]
     filename is an optional initial input image file. ext is the filename extension,
     determining the image file format.
*/

#include <Magick++.h>
#include <cstdlib>
#include <cmath>
#include <iostream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;
using namespace Magick;

#define max(a, b) ((a) > (b)? (a): (b))
#define min(a, b) ((a) < (b)? (a): (b))

//
// Global variables and constants
//
const int DEFAULTWIDTH = 600;	// default window dimensions if no image
const int DEFAULTHEIGHT = 600;

int WinWidth, WinHeight;	// window width and height
int ImWidth, ImHeight;		// image width and height
int VpWidth, VpHeight;		// viewport width and height
int Xoffset, Yoffset;		// viewport offset from lower left corner of window

Image image;			// the image in Magick++ form
unsigned char *Pixmap = NULL;	  // the image pixmap used for OpenGL
unsigned char *EqPixmap = NULL;	  // the histogram equalized image pixmap
unsigned char *NrmPixmap = NULL;  // the normalized image pixmap

int WhichImage = 0;		// 0 = original, 1 = normalized, 2 = equalized

//
// create the normalized image
//
void normalize(int w, int h){
  int roffset, pix;
  int cmin, cmax;
  
  // get rid of the old pixmap and make a new one of the new size
  delete NrmPixmap;
  NrmPixmap = new unsigned char[w * h * 4];
  
  // find the minimum and maximum channel values
  cmin = 255;
  cmax = 0;
  for(int row = 0; row < h; row++){
	roffset = row * w * 4;
	for(int col = 0; col < w; col++){
	  pix = roffset + col * 4;
	  for(int channel = 0; channel < 3; channel++){
		if(Pixmap[pix + channel] < cmin)
		  cmin = Pixmap[pix + channel];
		if(Pixmap[pix + channel] > cmax)
		  cmax = Pixmap[pix + channel];
	  }
	}
  }
  
  for(int row = 0; row < h; row++){
	roffset = row * w * 4;
	for(int col = 0; col < w; col++){
	  pix = roffset + col * 4;
	  
	  NrmPixmap[pix] = int(255 * float(Pixmap[pix] - cmin) / (cmax - cmin));
	  NrmPixmap[pix + 1] = int(255 * float(Pixmap[pix + 1] - cmin) / (cmax - cmin));
	  NrmPixmap[pix + 2] = int(255 * float(Pixmap[pix + 2] - cmin) / (cmax - cmin));
	  NrmPixmap[pix + 3] = 255;
	}
  }
}

//
// create the histogram equalized image
//
void equalize(int w, int h){
  int hist[256];
  int i, count;
  int roffset, pix;
  int y;
  float f;
  int cmax;
  
  // get rid of the old pixmap and make a new one of the new size
  delete EqPixmap;
  EqPixmap = new unsigned char[w * h * 4];
  
  // zero out the histogram cells
  for(y = 0; y < 256; y++)
	hist[y] = 0;
  
  // build the luminance histogram
  for(int row = 0; row < h; row++){
	roffset = row * w * 4;
	for(int col = 0; col < w; col++){
	  pix = roffset + col * 4;
	  y = int(0.30 * Pixmap[pix] + 0.59 * Pixmap[pix + 1] + 0.11 * Pixmap[pix + 2] + 0.5);
	  hist[y]++;
	}
  }
  
  // build the histogram equalized image
  for(int row = 0; row < h; row++){
	roffset = row * w * 4;
	for(int col = 0; col < w; col++){
	  pix = roffset + col * 4;
	  
	  y = int(0.30 * Pixmap[pix] + 0.59 * Pixmap[pix + 1] + 0.11 * Pixmap[pix + 2] + 0.5);
	  count = 0;
	  for(i = 0; i <= y; i++)		// count pixels with luminance <= this pixel
		count += hist[i];
	  f = float(count) / (w * h);	// fraction of pixels with luminance <= this pixel

	  cmax = max(Pixmap[pix], max(Pixmap[pix + 1], Pixmap[pix + 2])); // max of RGB for pixel
	  
	  EqPixmap[pix] = int(255 * (f * Pixmap[pix]) / cmax);			// scale luminance
	  EqPixmap[pix + 1] = int(255 * (f * Pixmap[pix + 1]) / cmax);
	  EqPixmap[pix + 2] = int(255 * (f * Pixmap[pix + 2]) / cmax);
	  EqPixmap[pix + 3] = 255;
	}
  }
}

//
//  Routine to read an image file and store in a pixmap
//  returns the size of the image in pixels if correctly read, or 0 if failure
//
int readimage(string infilename){
  
  // use Magick++ to load the image based on the filename and extension
  try{
    image.read(infilename);
  }
  catch(Error& error){	  // bomb out if error reading image
    cout << "Error reading image file: " << error.what() << endl;
    return 0;
  }

  // Magick++ treats upper left corner of image as its origin, while
  // OpenGL treats the lower left corner as its origin.
  image.flip();

  // Record image width and height in global variables for easy access
  ImWidth = image.columns();
  ImHeight = image.rows();

  // get rid of the old OpenGL pixmap and make a new one of the new size
  delete Pixmap;
  Pixmap = new unsigned char[ImWidth * ImHeight * 4];

  // fetch the pixmap from the Magick++ Image
  image.write(0, 0, ImWidth, ImHeight, "RGBA", CharPixel, (void *)Pixmap);
  
  normalize(ImWidth, ImHeight);	  // make the normalized image
  equalize(ImWidth, ImHeight);	  // make the histogram equalized image

  // returns image size in pixels
  return ImWidth * ImHeight;
}

//
// Routine to display a pixmap in the current window
//
void displayimage(){
  // if the window is smaller than the image, scale it down, otherwise do not scale
  if(WinWidth < ImWidth  || WinHeight < ImHeight)
    glPixelZoom(float(VpWidth) / ImWidth, float(VpHeight) / ImHeight);
  else
    glPixelZoom(1.0, 1.0);

  // display starting at the lower lefthand corner of the viewport
  glRasterPos2i(0, 0);
  
  switch(WhichImage){
	case 0:
	  glDrawPixels(ImWidth, ImHeight, GL_RGBA, GL_UNSIGNED_BYTE, Pixmap);
	  break;
	case 1:
	  glDrawPixels(ImWidth, ImHeight, GL_RGBA, GL_UNSIGNED_BYTE, NrmPixmap);
	  break;
	case 2:
	  glDrawPixels(ImWidth, ImHeight, GL_RGBA, GL_UNSIGNED_BYTE, EqPixmap);
  }
}

//
// Routine to write the current framebuffer to an image file
//
void writeimage(string outfilename){
  // make a pixmap that is the size of the window and grab framebuffer into it
  unsigned char local_pixmap[WinWidth * WinHeight * 4];
  glReadPixels(0, 0, WinWidth, WinHeight, GL_RGBA, GL_UNSIGNED_BYTE, local_pixmap);

  // Create a Magick++ image and pass it the pixmap
  Image view_image(WinWidth, WinHeight, "RGBA", CharPixel, (void *)local_pixmap);
  
  // Magick++ treats upper left corner of image as its origin, while
  // OpenGL treats the lower left corner as its origin.
  view_image.flip();
  
  // Write the image out to the named image file
  try{
    view_image.write(outfilename);
  }
  catch(Error& error){
    cout << "Error writing image file: " << error.what() << endl;
  }
}

//
//   Display Callback Routine: clear the screen and draw the current image
//
void handleDisplay(){
  glClear(GL_COLOR_BUFFER_BIT);  // clear window to background color
  
  // only draw the image if it is of a valid size
  if(ImWidth > 0 && ImHeight > 0)
    displayimage();
  
  // flush the OpenGL pipeline to the viewport
  glFlush();
}

//
//  Keyboard Callback Routine: 'r' - read and display a new image, 
//  'w' - write the current window to an image file, 'q' or ESC - quit
//
void handleKey(unsigned char key, int x, int y){
  string infilename, outfilename;
  int ok;

  switch(key){
    case 'r':		// 'r' - read an image from a file
    case 'R':
      cout << "Input image filename? ";	  // prompt user for input filename
      cin >> infilename;
      ok = readimage(infilename);
      if(ok)
		glutReshapeWindow(ImWidth, ImHeight); // OpenGL window should match new image
	  WhichImage = 0;		  // start by displaying original image
      glutPostRedisplay();
      break;
      
    case 'w':		// 'w' - write the image to a file
    case 'W':
      cout << "Output image filename? ";  // prompt user for output filename
      cin >> outfilename;
      writeimage(outfilename);
      break;
      
	case 'h':		// 'h' - toggle between original, normalized, and equalized images
	case 'H':
	  WhichImage = (WhichImage + 1) % 3;
	  glutPostRedisplay();
	  break;
	  
    case 'q':		// q or ESC - quit
    case 'Q':
    case 27:
      exit(0);
      
    default:		// not a valid key -- just ignore it
      return;
  }
}

//
//  Reshape Callback Routine: If the window is too small to fit the image, 
//  make a viewport of the maximum size that maintains the image proportions. 
//  Otherwise, size the viewport to match the image size. In either case, the 
//  viewport is centered in the window.
//
void handleReshape(int w, int h){
  float imageaspect = (float)ImWidth / (float)ImHeight;	// aspect ratio of image
  float newaspect = (float)w / (float)h;		// new aspect ratio of window
  
  // record the new window size in global variables for easy access
  WinWidth = w;
  WinHeight = h;
  
  // if the image fits in the window viewport is the same size as the image
  if(w >= ImWidth && h >= ImHeight){
    Xoffset = (w - ImWidth) / 2;
    Yoffset = (h - ImHeight) / 2;
    VpWidth = ImWidth;
    VpHeight = ImHeight;
  }
  // if the window shape is wider than the image shape use the full window height
  // and size the width to match the image aspect ratio
  else if(newaspect > imageaspect){
    VpHeight = h;
    VpWidth = int(imageaspect * VpHeight);
    Xoffset = int((w - VpWidth) / 2);
    Yoffset = 0;
  }
  // if the window shape is narrower than the image shape use the full window width
  // and size the height to match the image aspect ratio
  else{
    VpWidth = w;
    VpHeight = int(VpWidth / imageaspect);      
    Yoffset = int((h - VpHeight) / 2);
    Xoffset = 0;
  }
  
  // center the viewport in the window
  glViewport(Xoffset, Yoffset, VpWidth, VpHeight); 
  
  // viewport coordinates are simply pixel coordinates 
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, VpWidth, 0, VpHeight);
  glMatrixMode(GL_MODELVIEW);
}

//
//  Scan command line for an initial image filename, and load it if present
//  Set initial window and image width and height, and the OpenGL pixmap based 
//  on the loaded image. Or use the default window width and height and a NULL
//  pixmap if no image, or image failed to load.
//
void commandline(int argc, char* argv[]){
  int ok;
  
  // only one parameter allowed, an optional image filename and extension
  if(argc > 2){
    cout << "usage: histeq [filename.ext]" << endl;
    exit(1);
  }
  
  // set up the default window and empty pixmap if no image or image fails to load
  WinWidth = DEFAULTWIDTH;
  WinHeight = DEFAULTHEIGHT;
  ImWidth = 0;
  ImHeight = 0;
  Pixmap = NULL;
  
  // load the image if present, and size the window to match
  if(argc == 2){
    ok = readimage(argv[1]);
    if(ok){
      WinWidth = ImWidth;
      WinHeight = ImHeight;
    }
  }
}

//
// Main program to scan the commandline, set up GLUT and OpenGL, and start Main Loop
//
int main(int argc, char* argv[]){
  // scan command line and process
  commandline(argc, argv);
  
  // start up GLUT
  glutInit(&argc, argv);

  // create the graphics window, giving width, height, and title text
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowSize(WinWidth, WinHeight);
  glutCreateWindow("Histogram Equalizer");

  // set up the callback routines
  glutDisplayFunc(handleDisplay); // display update callback
  glutKeyboardFunc(handleKey);	  // keyboard key press callback
  glutReshapeFunc(handleReshape); // window resize callback

  // specify window clear (background) color to be opaque black
  glClearColor(0, 0, 0, 1);

  // Enter GLUT's event loop
  glutMainLoop();
  return 0;
}
