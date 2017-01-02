/*
   
   This static class takes care of all OPenGl View related functions.

   This should hiave very less inteligence on the data that comes but it should be just a 
   system which only operates on pixels.
   CPSC 4040/6040            Saroj Kumar Dash         09/07/2016

*/

#ifdef __APPLE__
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "ImageData.h"

//the specs defined for window setup in init
#define WIDTH 600
#define HEIGHT 600
#define CHANNELS 4

class GLView{

public:
	static int _iw;
	static int _ih;
	static int _ich;
	static unsigned char* _pixels;

	static void Init(const char *ipViewName=NULL);
	static void reshape(int iw,int ih);
	static int SetImageToDisplay(ImageData &iDataImage);
	static void SetDataToDisplay(unsigned char *ipPixels,int iw,int ih,int ich);
	static void DisplayImage();
	static unsigned char *ReadPixels();
	static void CleanCache();

};
