#include <iostream>
using namespace std;

#include "OIIOServices.h"
#include "RGBtoHSV.h"

unsigned char *OIIOServices::RGBtoRGBA(unsigned char *ipixels,int iw,int ih,int ich)
{
	//cout << "RGBToRGBA method calling" << endl;
	//Checking for Bad input
	if ((NULL == ipixels) && (3 != ich)) {
		cout << "Its not a rgb format, channels = " <<ich<<endl;
		return ipixels;
	}

	unsigned char *pixmap = new unsigned char[iw*ih*4];

	int i, j = 0;
	for (i = 0; i <= (iw*ih*4); i = i + 4)
	{
		int ir, ig, ib,ia;
		ir = *(ipixels + j); j++;
		ig = *(ipixels + j); j++;
		ib = *(ipixels + j); j++;
		ia = 1;
			
		pixmap[i] = ir;
		pixmap[i+1] = ig;
		pixmap[i + 2] = ib;
		pixmap[i + 3] = ia;
		
	}
	
	delete ipixels;
	ipixels = NULL;

	return pixmap;

}

//Method that reads the image with a particular file name.
unsigned char *OIIOServices::ReadImage(const char *ifilename,int &ionbw,int &ionbh,int &ionbch)
{
	//To Extract pixels from the image
	unsigned char *opixels;
	
	ImageInput *in = ImageInput::create (ifilename);
	if (NULL == in)
	{
		cout << "File not read, Probably not at current location" << endl;
		return NULL;
	}

	ImageSpec spec;
	in->open (ifilename, spec);
	ionbw = spec.width;
	ionbh = spec.height;
	ionbch = spec.nchannels;
	
	if((0==ionbw) && (0==ionbh) && (0==ionbch) )
		cout<<"the given file could not be read properly"<<endl;

	opixels = new unsigned char [ionbw*ionbh*ionbch];
	int scanlinesize = ionbw * ionbch * sizeof(opixels[0]);
	
	in->read_image(TypeDesc::UINT8,
			(char *)opixels + (ionbh - 1)*scanlinesize,
			AutoStride,-scanlinesize,AutoStride);  //Scansline from below such that we do not get a flipped image

	in->close();
	delete in;
	return opixels;

}


//Take care of the number of channels when yo use it. the nbchannels is 4 in this RGBA case
unsigned char *OIIOServices::ReadImageInRGBA(const char *ipfilename, int &ionbw, int &ionbh)
{
	unsigned char *oPixels = NULL;

	int nbch = 0;
	oPixels = ReadImage(ipfilename, ionbw, ionbh, nbch);

	if (3 == nbch)
		oPixels = RGBtoRGBA(oPixels, ionbw, ionbh, nbch);

	return oPixels;
}

//Take care of the number of channels when yo use it. the nbchannels is 3 in RGB case
unsigned char *OIIOServices::ReadImageInRGB(const char *ipfilename, int &ionbw, int &ionbh)
{
	unsigned char *oPixels = NULL;
	int nbch = 0;

	oPixels = ReadImage(ipfilename, ionbw, ionbh, nbch);

	if (3 != nbch)
	{
		delete oPixels;
		oPixels = NULL;
	}

	return oPixels;
}


//Uses OIIO apis to read the image from the file. have to improve it further.
void OIIOServices::WriteImage(char *ipfilename,unsigned char *ippixels,int inbw,int inbh,int inbch)
{
	//write the pixels to an image file.
	if(NULL == ippixels){
		cout<<"Invalid pixels input to write Image"<<endl;
		return;
	}

	ImageOutput *out = ImageOutput::create (ipfilename);
	ImageSpec spec (inbw, inbh, inbch, TypeDesc::UINT8);
	out->open(ipfilename, spec);

	int scanlinesize = inbw * inbch * sizeof(ippixels[0]);
	if(ippixels)
	{
		out->write_image(TypeDesc::UINT8,(char *)ippixels + (inbh - 1)*scanlinesize,
			AutoStride,-scanlinesize,AutoStride);
	}

	out->close();
	delete out;
	return;

}






