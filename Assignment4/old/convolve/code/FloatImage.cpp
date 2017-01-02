/*-------------------------------------
 |    FloatImage Methods
 *--------------------------------------
 |
 |  Programmer: Donald House
 |  Date: Fri Oct 31 1996
 |
 |
 */

#include <Magick++.h>

#include "floatimage.h"

#include <cstdlib>
#include <cmath>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;
using namespace Magick;

void FloatImage::clone(const FloatImage &im){
  strcpy(filename, im.filename);
  window = im.window;
  width = im.width;
  height = im.height;
  pixmap = NULL;
  for(int ch = 0; ch < 4; ch++)
	Channel[ch] = NULL;
  
  if(width * height != 0){
	makearrays();
	for(int i = 0; i < width * height; i++){
	  for(int ch = 0; ch < 4; ch++)
		pixmap[0][i][ch] = im.pixmap[0][i][ch];
	}
	for(int ch = 0; ch < 4; ch++)
	  for(int i = 0; i < width * height; i++)
		Channel[ch][0][i] = im.Channel[ch][0][i];
  }  
}

FloatImage::FloatImage(){
  filename[0] = '\0';
  width = height = 0;
  pixmap = NULL;
  for(int i = 0; i < 4; i++)
	Channel[i] = NULL;
}

/*
  Copy constructor requires deep copy
*/
FloatImage::FloatImage(const FloatImage &im){
  clone(im);
}

FloatImage::~FloatImage(){
  if(pixmap != NULL){
	delete pixmap[0];
	delete pixmap;
  }
  
  for(int i = 0; i < 4; i++)
	if(Channel[i] != NULL){
	  delete Channel[i][0];
	  delete Channel[i];
	}
}

int FloatImage::read_image(){
  Image image;
  
  // use Magick++ to load the image based on the filename and extension
  try{
    image.read(filename);
  }
  catch(Error& error){	  // bomb out if error reading image
    fprintf(stderr, "Error reading image file: %s\n", error.what());
    return 0;
  }
  
  // Magick++ treats upper left corner of image as its origin, while
  // OpenGL treats the lower left corner as its origin.
  image.flip();
  
  // Record image width and height in global variables for easy access
  width = image.columns();
  height = image.rows();
  
  // allocate the pixmap and channel arrays for the image
  makearrays();
  
  // fetch the pixmap from the Magick++ Image
  image.write(0, 0, width, height, "RGBA", CharPixel, (void *)pixmap[0]);
  
  // returns image size in pixels
  return width * height;
}

void FloatImage::write_image(){
  
  // Create a Magick++ image and pass it the pixmap
  Image view_image(width, height, "RGBA", CharPixel, (void *)pixmap[0]);
  
  // Magick++ treats upper left corner of image as its origin, while
  // OpenGL treats the lower left corner as its origin.
  view_image.flip();
  
  // Write the image out to the named image file
  try{
    view_image.write(filename);
  }
  catch(Error& error){
    fprintf(stderr, "Error writing image file: %s\n", error.what());
  }  
}

/*
 Convert image pixmap data into floating point channel data
 scaled to range [0, 1].
 */
void FloatImage::tochannels(){
  int row, col;
  int ch;
  
  for(ch = 0; ch < 4; ch++)
    for(row = 0; row < height; row++)
      for(col = 0; col < width; col++)
		Channel[ch][row][col] = float(pixmap[row][col][ch]) / 255.0;
}

/*
   Converts Image Channel data in floating point form back to
   a pixmap.  This routine assumes that all Channel values are on the
   range [0, 1].  If channel values are outside of the [0, 1] range,
   they are truncated.  If gamma value for image is not 1.0 (or 0),
   Channel values are raised to the 1/gamma power before being scaled and
   placed in the pixmap.
 */
void FloatImage::topixmap(float gamma){
  int row, col;
  int ch;
  float gammainv;
  bool havegamma = false;
  unsigned int val;
  
  /* zero out the pixmap to begin with */
  for(row = 0; row < height; row++)
    for(col = 0; col < width; col++)
	  for(int i = 0; i < 4; i++)
		pixmap[row][col][i] = 0;

  /* copy channel data to pixmap, doing gamma correction */
  if(gamma != 0.0 && gamma != 1.0){
    gammainv = 1.0 / gamma;
    havegamma = true;
  }

  for(ch = 0; ch < 3; ch++)
	for(row = 0; row < height; row++)
	  for(col = 0; col < width; col++){
		if(havegamma)
		  val = Round(pow(Channel[ch][row][col], gammainv) * 255.0);
		else
		  val = Round(Channel[ch][row][col] * 255.0);
		val = (val < 0? 0: (val > 255? 255: val));
		pixmap[row][col][ch] = val;
	  }

  // make alpha 1.0
  for(row = 0; row < height; row++)
	for(col = 0; col < width; col++)
	  pixmap[row][col][3] = 255;
}

/*
 Given a FloatImage whose width, height, rgb, and nchannels fields are
 already set, allocate the pixmap and channel data arrays.
 */
void FloatImage::makearrays(){
  int size;
  int ch;
  
  size = width * height;
  
  pixmap = new Pixel*[height];
  if(pixmap != NULL)
	pixmap[0] = new Pixel[size];
  if(pixmap == NULL || pixmap[0] == NULL){
    fprintf(stderr, "ERROR: makearrays can't allocate memory for pixmap\n");
    exit(-1);
  }
  for(int i = 1; i < height; i++)
	pixmap[i] = pixmap[i - 1] + width;

  /* make enough space for all the arrays */
  for(ch = 0; ch < 4; ch++){
	Channel[ch] = new float*[height];
	if(Channel[ch] != NULL)
	  Channel[ch][0] = new float[size];
	if(Channel[ch] == NULL || Channel[ch][0] == NULL){
	  fprintf(stderr, "ERROR: makearrays can't allocate memory for Channel\n");
	  exit(-1);
	}
	for(int i = 1; i < height; i++)
	  Channel[ch][i] = Channel[ch][i - 1] + width;
  }
}

/*
  Assignment requires deep copy
*/
const FloatImage& FloatImage::operator=(const FloatImage& im){
  clone(im);
  return *this;
}

