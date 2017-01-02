/*
   
   all OpenGL Services are kept here. This should be improved later to better services which gives correct results.
   CPSC 4040/6040            Saroj Kumar Dash         09/07/2016

*/

#include "GL/glut.h"

//the specs defined for window setup in init
#define WIDTH 600
#define HEIGHT 600
#define CHANNELS 4

class GLServices
{
	
	
public:
	static int _iw;
	static int _ih;
	static int _ich;
	static unsigned char* _pixels;

	static void Init(/*int *ipnbargc,char **ippcargv*/);
	static void reshape(int iw,int ih);
	static int DisplayImage(unsigned char *iPixels=NULL,int iw=WIDTH,int ih=HEIGHT,int ich=CHANNELS);
	static unsigned char *ReadPixels(int inbw,int inbh,int inbch);

};
