#include <iostream>
using namespace std;

#include "ImageData.h"
#include "OIIOServices.h"
#include "RGBtoHSV.h"

ImageData::ImageData() {
	//cout << "Default ImageData constructed" << endl;
	_prgbapixels=NULL;
	_sfilename = "";
	_iwidth = 0;
	_iheight = 0;
	_inbChannels = 0;

}

ImageData::ImageData(const char *isFileName):
	_prgbapixels(NULL),_sfilename(""),_iwidth(0),
	_iheight(0),_inbChannels(0){

	//cout << "valued constructor is called "<<isFileName<<endl;
	_sfilename = isFileName;

	//Read the image by the OIIOServices to get other data.
	const char *pFileName = (const char*)_sfilename.data();

	_prgbapixels = OIIOServices::ReadImage(isFileName, _iwidth, _iheight,_inbChannels);

	if (3 == _inbChannels)
	{
		_prgbapixels = OIIOServices::RGBtoRGBA(_prgbapixels, _iwidth, _iheight,_inbChannels);
		_inbChannels = 4;
	}
	
}

ImageData::~ImageData(){

	//cout << "ImageData Destructor is called"<<endl;
	_sfilename = "";
	_iwidth = 0;
	_iheight = 0;
	_inbChannels = 0;

	if (_prgbapixels){

		delete _prgbapixels;
		_prgbapixels = NULL;
	}
	
}

int ImageData::GetWidth()
{
	return _iwidth;
}

int ImageData::GetHeight()
{
	return _iheight;
}


unsigned char* ImageData::GetPixelsinRGBA()
{
	return _prgbapixels;
}


string ImageData::GetFileName() {

	return _sfilename;

}

int ImageData::GetNbOfChannels() {

	return _inbChannels;
}

unsigned char *ImageData::r_GetAlphaMask()
{
	
	unsigned char *pFGPixels = NULL;
	if ((NULL == _prgbapixels) && (4 != _inbChannels)) return NULL;

	//The user of this method should release  this memory block on its side.
	int nbBlockSize = _iwidth*_iheight*_inbChannels;
	pFGPixels = new unsigned char[nbBlockSize];
	for (int i = 0; i < (nbBlockSize); i = i + 4)
	{
		int ir, ig, ib, ia;
		ir = *(_prgbapixels + i);
		ig = *(_prgbapixels + i + 1);
		ib = *(_prgbapixels + i + 2);
		ia = *(_prgbapixels + i + 3);

		//We have to make sure that the ia is 1 if its noot green.
		if (1 != ia) ia = 1;

		hsvColor hsv = RGBtoHSV::rgbtohsvcolor(ir, ig, ib);
		if ((150 > hsv.h) && (80 < hsv.h)) {
			//cout << "hsv hue is "<<hsv.h<< endl;
			ia = 0;
		}
		else {
			ia = 1;
		}

		pFGPixels[i] = ir;
		pFGPixels[i+1] = ig;
		pFGPixels[i + 2] = ib;
		pFGPixels[i + 3] = ia;

	}


	return pFGPixels;
}


unsigned char*ImageData::r_GetAssociatedImage() {

	unsigned char *ipPixels;
	
	unsigned char *pFGPixels = NULL;

	//Getting the Matt image
	pFGPixels = this->r_GetAlphaMask();
	if (NULL == pFGPixels) cout << "Foreground image not constructed" << endl;

	//Size of block to allocate
	int nbBlockSize = (_iwidth*_iheight*4);
	//So we calculate the pixels of the matt image
	unsigned char *pAssociatedImage = new unsigned char[nbBlockSize];
	for (int i = 0; i < (nbBlockSize); i = i + 4) {
		int ir, ig, ib, ia;

		//doing this extra step for good readability
		ir = pFGPixels[i];
		ig = pFGPixels[i + 1];
		ib = pFGPixels[i + 2];
		ia = pFGPixels[i + 3];
		//ia = 1;

		pAssociatedImage[i] = ia*ir;
		pAssociatedImage[i + 1] = ia*ig;
		pAssociatedImage[i + 2] = ia*ib;
		if(ia == 1)		
			pAssociatedImage[i + 3] = 255;
		else if(ia == 0)
			pAssociatedImage[i + 3] = 0;
	}

	delete pFGPixels;
	pFGPixels = NULL;

	return pAssociatedImage;

}

void getCompositeColor(int fg, int  bg, int alphaF, int &compositeChannel)
{
	//we know that composition Ch = CA + (1-alphaA)Cb
	compositeChannel = static_cast<int>(((float)fg / 255 + (float)(1 - (float)alphaF / 255) * (float)bg / 255) * 255);
}

unsigned char*ImageData::r_Compose(ImageData *pBGImage, int &oFinalWidth, int &oFinalheight) {

	unsigned char *pOverredPixels = NULL;

	//Check input parameters
	if (NULL == pBGImage) cout << "INVALID_INPUT: NULL backgrund image pointer" << endl;

	//I take the shortest possible dimensions from the two input images.
	oFinalheight = (this->GetHeight() <= pBGImage->GetHeight()) ? this->_iheight : pBGImage->GetHeight();
	oFinalWidth = (this->GetWidth() <= pBGImage->GetWidth()) ? this->_iwidth : pBGImage->GetWidth();
	
	int nbBLockSize = (oFinalheight*oFinalWidth * 4);

	pOverredPixels = new unsigned char[nbBLockSize];
	//Release this pointer  later
	//unsigned char *pFGAlphaPixels = this->r_GetAlphaMask();
	unsigned char *pFGAlphaPixels = _prgbapixels;

	//Release this pointer 
	unsigned char *pBGPixels = pBGImage->GetPixelsinRGBA();
	
	for (int ithPix = 0; ithPix < (nbBLockSize); ithPix = ithPix + 4) {

		float nbFGr,nbFGg,nbFGb,nbFGa;
		float nbBGr, nbBGg, nbBGb, nbBGa;

		
		nbFGr = (float)pFGAlphaPixels[ithPix]/255;
		nbFGg = (float)pFGAlphaPixels[ithPix + 1]/255;
		nbFGb = (float)pFGAlphaPixels[ithPix + 2]/255;
		nbFGa = (float)pFGAlphaPixels[ithPix + 3]/255;

		nbBGr = (float)pBGPixels[ithPix]/255;
		nbBGg = (float)pBGPixels[ithPix + 1]/255;
		nbBGb = (float)pBGPixels[ithPix + 2]/255;
		nbBGa = (float)pBGPixels[ithPix + 3]/255;

		int temp = 0;
		getCompositeColor(nbFGr, nbBGr, nbFGa,temp);
		pOverredPixels[ithPix] = (int)((nbFGr + ((float)1-nbFGa)*nbBGr)*255);
		getCompositeColor(nbFGg, nbBGg, nbFGa, temp);
		pOverredPixels[ithPix + 1] = (int)((nbFGg + ((float)1 - nbFGa)*nbBGg) * 255);
		getCompositeColor(nbFGb, nbBGb, nbFGa, temp);
		pOverredPixels[ithPix + 2] = (int)((nbFGb + ((float)1 - nbFGa)*nbBGb) * 255);
		pOverredPixels[ithPix + 3] = 255;
		
	}

	return pOverredPixels;
}




void ImageData::WriteImage(char *ipfilename) {

	OIIOServices::WriteImage(ipfilename,_prgbapixels,_iwidth,_iheight,_inbChannels);
	return;
}
