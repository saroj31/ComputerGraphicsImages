#include <iostream>
using namespace std;

#include "OIIOServices.h"


unsigned char *OIIOServices::RGBtoRGBA(unsigned char *ipixels,int iw,int ih,int ich)
{
	//Checking for Bad input
	if (NULL == ipixels) {
		cout << "bad pixels entered" <<endl;
		return ipixels;
	}

	if ((ich <= 0) && (ich >= 4))
	{
		cout << "Cannot handle this number of channels " << ich << endl;
		delete ipixels;
		ipixels = NULL;
		return ipixels;
	}

	unsigned char *pixmap = new unsigned char[iw*ih*4];

	int i, j = 0;
	for (i = 0; i <= (iw*ih*4); i = i + 4)
	{
		
		for (int k = 0; k < ich; k++)
		{
			pixmap[i + k] = ipixels[j + k];
		}
		j = j + ich;
	
		for (int rk = ich; rk < 4; rk++)
		{
			pixmap[i + rk] = ipixels[j-1];
		}
		pixmap[i + 3] = 255;
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

	oPixels = RGBtoRGBA(oPixels, ionbw, ionbh, nbch);
	
	if (NULL == oPixels)
	{
		cout << "ReadImage failed. Program terminated" << endl;
		exit(0);
	}


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






