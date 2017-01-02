#include "OIIOServices.h"
#include <iostream>
using namespace std;



//Method that reads the image with a particular file name. Have to improve this later.
unsigned char *OIIOServices::ReadImage(const char *filename,int &inbw,int &inbh,int &inbch)
{

	//Extract pixels from the image
	unsigned char *pixels;
	
	ImageInput *in = ImageInput::create (filename);
	ImageSpec spec;
	in->open (filename, spec);
	inbw = spec.width;
	inbh = spec.height;
	inbch = spec.nchannels;

	if((0==inbw) && (0==inbh) && (0==inbch) )
		cout<<"the given file could not be read properly"<<endl;

	pixels = new unsigned char [inbw*inbh*inbch];
	int scanlinesize = inbw * inbch * sizeof(pixels[0]);
	
	in->read_image(TypeDesc::UINT8,
			(char *)pixels + (inbh - 1)*scanlinesize,
			AutoStride,-scanlinesize,AutoStride);  //Scansline from below such that we do not get a flipped image

	if (3 == inbch)
	{
		unsigned char *pixmap = new unsigned char[inbw*inbh*4];
		int i, j, k;
		for(i=0;i<=inbw;i++)
		{
			for (j = 0;j<=inbh;j++)
			{
				pixmap[(i*j + 1)] = *(pixels + (i*j + 1));
				pixmap + (i*j + 2) = *(pixels + (i*j + 2));
				pixmap + (i*j + 3) = *(pixels + (i*j + 3));
				pixmap + (i*j + 4) = 1;   //Opaque
			}
		}
		delete pixels;
		pixels = NULL;
		pixels = pixmap;
	}

	in->close();
	delete in;
	return pixels;

}


//Uses OIIO apis to read the image from the file. have to improve it further.
void OIIOServices::WriteImage(char *filename,unsigned char *pixels,int inbw,int inbh,int inbch)
{
	
	//write the pixels to an image file.

	if(NULL == pixels)	
	{
		cout<<"Invalid pixels input to write Image"<<endl;
		return;
	}

	ImageOutput *out = ImageOutput::create (filename);
	ImageSpec spec (inbw, inbh, inbch, TypeDesc::UINT8);
	out->open(filename, spec);

	int scanlinesize = inbw * inbch * sizeof(pixels[0]);
	if(pixels)
	{
		out->write_image(TypeDesc::UINT8,(char *)pixels + (inbh - 1)*scanlinesize,
			AutoStride,-scanlinesize,AutoStride);
	}

	out->close();
	delete out;
	return;

}



