/*
   
  API service to access the OpenImage IO APIs to access the image file and all.
  CPSC 4040/6040            Saroj Kumar Dash         09/07/2016

*/

#include "OpenImageIO/imageio.h"

using namespace OpenImageIO;

class OIIOServices
{

public:
	static unsigned char *ReadImageInRGBA(const char *ipfilename,int &ionbw,int &ionbh);
	static unsigned char *ReadImageInRGB(const char *ipfilename, int &ionbw, int &ionbh);
	static unsigned char *ReadImage(const char *ipfilename, int &ionbw, int &ionbh,int &ionbch);
	static void WriteImage(char *filename,unsigned char *pixels,int inbw,int inbh,int inbch);
	static unsigned char *RGBtoRGBA(unsigned char *ipixels,int iw,int ih,int ich);
	
};
