/*

   This class is the model of the system. 
   This contains all the data and data related information.

*/


//Header Declaration
#include <iostream>
using namespace std;

#include "ImageData.h"
#include "OIIOServices.h"
#include "KernalData.h"
//Header Declaration



//Default Constructor of ImageData or model
ImageData::ImageData():_pPixels(NULL), _pRPixels(NULL), _pGPixels(NULL),
	_pBPixels(NULL),_sfilename(""), _iwidth(0),_iheight(0), _inbChannels(0){

	//cout << "Default ImageData constructed" << endl;
	_pPixels=NULL;
	_sfilename = "";
	_iwidth = 0;
	_iheight = 0;
	_inbChannels = 0;

}

ImageData::ImageData(const char *isFileName):_pPixels(NULL), _pRPixels(NULL), _pGPixels(NULL), 
	_pBPixels(NULL),_sfilename(""),_iwidth(0),_iheight(0),_inbChannels(0){

	//cout << "valued constructor is called "<<isFileName<<endl;
	_sfilename = isFileName;

	//Read the image by the OIIOServices to get other data.
	//_sfilename.insert(0,"images\\");
	char *pFileName = (char*)_sfilename.data();

	cout << "pFileName = " << pFileName << endl;
	//Fills up the values in _pPixels, _pRPixels, _pGPixels, _pBPixels
	ReadImage(pFileName);


}

ImageData::ImageData(ImageData &iCopy):_sfilename(iCopy._sfilename),_iwidth(iCopy._iwidth),
	_iheight(iCopy._iheight),_inbChannels(iCopy._inbChannels){

	_pPixels = new unsigned char[_iwidth*_iheight*_inbChannels];
	for (int i = 0; i < (_iwidth*_iheight*_inbChannels); i++) {

		_pPixels[i] = iCopy._pPixels[i];
	}

	_pRPixels = new float[_iwidth*_iheight*_inbChannels];
	for (int i = 0; i < (_iwidth*_iheight*_inbChannels); i++) {

		_pRPixels[i] = iCopy._pRPixels[i];
	}

	_pGPixels = new float[_iwidth*_iheight*_inbChannels];
	for (int i = 0; i < (_iwidth*_iheight*_inbChannels); i++) {

		_pGPixels[i] = iCopy._pGPixels[i];
	}

	_pBPixels = new float[_iwidth*_iheight*_inbChannels];
	for (int i = 0; i < (_iwidth*_iheight*_inbChannels); i++) {

		_pBPixels[i] = iCopy._pBPixels[i];
	}
}


ImageData::~ImageData(){

	//cout << "ImageData Destructor is called"<<endl;
	_sfilename = "";
	_iwidth = 0;
	_iheight = 0;
	_inbChannels = 0;

	if (_pPixels){

		delete _pPixels;
		_pPixels = NULL;
	}
	
	if (_pRPixels) {

		delete _pRPixels;
		_pRPixels = NULL;
	}

	if (_pGPixels) {

		delete _pGPixels;
		_pGPixels = NULL;
	}

	if (_pBPixels) {

		delete _pBPixels;
		_pBPixels = NULL;
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


unsigned char* ImageData::GetPixels(){
	return _pPixels;
}


float* ImageData::GetRedPixels()
{
	return _pRPixels;
}

float* ImageData::GetGreenPixels()
{
	return _pGPixels;
}

float* ImageData::GetBluePixels(){
	
	return _pBPixels;
}

unsigned char* ImageData::UpdatePixels() {

	if (_pPixels) {

		delete _pPixels;
		_pPixels = NULL;
	}

	_pPixels = new unsigned char[_iwidth*_iheight*_inbChannels];

	for (int ithCol = 0; ithCol < _iheight; ithCol++) {
		for (int ithRow = 0; ithRow < _iwidth; ithRow++){
		
			
			_pPixels[ithCol*_iwidth*_inbChannels + ithRow*_inbChannels + 0] = static_cast<int>(_pRPixels[ithCol*_iwidth + ithRow] * 255);
			_pPixels[ithCol*_iwidth*_inbChannels + ithRow*_inbChannels + 1] = static_cast<int>(_pGPixels[ithCol*_iwidth + ithRow] * 255);
			_pPixels[ithCol*_iwidth*_inbChannels + ithRow*_inbChannels + 2] = static_cast<int>(_pBPixels[ithCol*_iwidth + ithRow] * 255);
						
		}

	}

	cout << "Pixel updated" << endl;
	return _pPixels;
}


string ImageData::GetFileName() {

	return _sfilename;
}

int ImageData::GetNbOfChannels() {

	return _inbChannels;
}

void ImageData::ReadImage(char *ipfilename) {

	if (NULL != _pPixels) {
		delete _pPixels;
		_pPixels = NULL;
	}

  if (NULL == ipfilename)
  {
    cout << " enter the filename of thee image" << endl;
    cin >> ipfilename;
  }
  
  _sfilename.assign(ipfilename);
	char *pFileName = (char*)_sfilename.data();

	cout << "Image File Read is " << ipfilename << endl;

	//_pPixels is memory allocated has to be deleted in Coonstructor
	_pPixels = OIIOServices::ReadImageInRGBA(ipfilename, _iwidth, _iheight);
	//_pPixels = OIIOServices::ReadImage(ipfilename, _iwidth, _iheight,_inbChannels);
	_inbChannels = 4;
	//cout << "Number of Channels = " << _inbChannels << endl;
	_sfilename = ipfilename;

	//Each channel will be on width*height times of the image spec
	int chSize = (_iwidth*_iheight);

	_pRPixels = new float[chSize];
	_pGPixels = new float[chSize];
	_pBPixels = new float[chSize];


	for (int ithCol = 0; ithCol < _iheight; ithCol++) {
		for (int ithRow = 0; ithRow < _iwidth; ithRow++) {
				
			_pRPixels[ithCol*_iwidth + ithRow] = (float)(_pPixels[ithCol*_iwidth*_inbChannels + ithRow*_inbChannels+0])/255;
			_pGPixels[ithCol*_iwidth + ithRow] = (float)(_pPixels[ithCol*_iwidth*_inbChannels + ithRow*_inbChannels + 1])/255;
			_pBPixels[ithCol*_iwidth + ithRow] = (float)(_pPixels[ithCol*_iwidth*_inbChannels + ithRow*_inbChannels + 2])/255;
				
		}
	}

}

void ImageData::WriteImage(char *ipfilename) {

	OIIOServices::WriteImage(ipfilename,_pPixels,_iwidth,_iheight,_inbChannels);
	return;
}


ImageData *ImageData::ConvolveWith(KernelData *ipKernel) {

	//Todo: Do the Convolution operation here

	//Todo: Run a loop on the r,g,b pixels of the input image from KernelHalfSize to (rPixelsRowSize-KernelHalfSize)
	int blocksize = (_iwidth * _iheight * 4);
  int maxOutImage = ((_iwidth + ipKernel->GetSize())*(_iheight + ipKernel->GetSize()) * 4);
	unsigned char *pOutPixels = new unsigned char[maxOutImage];
	unsigned char *pInPixels = _pPixels;

	int iw = _iwidth, ih = _iheight, ich = _inbChannels;
	float RChannel[iw][ih];
	float GChannel[iw][ih];
	float BChannel[iw][ih];

  cout << "iw = " << iw << endl;
  cout << "ih = " << ih << endl;

	for (int i = 0; i < ih; i++) {
		for (int j = 0; j < iw; j++) {

			RChannel[i][j] = (float)pInPixels[i*iw*ich + j*ich + 0] / 255;
			GChannel[i][j] = (float)pInPixels[i*iw*ich + j*ich + 1] / 255;
			BChannel[i][j] = (float)pInPixels[i*iw*ich + j*ich + 2] / 255;

		}
	}

	float *pArr = ipKernel->GetWtValue();

	int kHalfSize = ipKernel->GetHalfSize();
	int ksize = ipKernel->GetSize();
	for (int ic = 0; ic < (ih - kHalfSize); ic++)
	{
		for (int ir = 0; ir < (iw - kHalfSize); ir++)
		{
			float sumr = 0.0f, sumg = 0.0f, sumb = 0.0f;

			for (int kc = 0; kc < ksize; kc++)
			{
				for (int kr = 0; kr < ksize; kr++)
				{
          //if (((ic + kc)*ih + (ir + kr)) > blocksize)		break;
          if ((ic + kc) > ih && (ir + kr) > iw) break;
					sumr += (pArr[kc*ksize + kr] * RChannel[ic + kc][ir + kr]);
					sumg += (pArr[kc*ksize + kr] * GChannel[ic + kc][ir + kr]);
					sumb += (pArr[kc*ksize + kr] * BChannel[ic + kc][ir + kr]);
				}

			}
			float rchval = 0, gchval = 0, bchval = 0;
			int scale = ipKernel->GetScale();
			rchval = ((float)abs(sumr) / scale) * 255;
			gchval = ((float)abs(sumg) / scale) * 255;
			bchval = ((float)abs(sumb) / scale) * 255;

			pOutPixels[(ic* iw * 4) + (ir * 4) + 0] = rchval;
			pOutPixels[(ic* iw * 4) + (ir * 4) + 1] = gchval;
			pOutPixels[(ic* iw * 4) + (ir * 4) + 2] = bchval;
			pOutPixels[(ic* iw * 4) + (ir * 4) + 3] = 255;

		}

	}

	int newwidth = _iwidth;
	int newheight = _iheight;

	
	char OutFile[] = "Output.png";
	OIIOServices::WriteImage(OutFile, pOutPixels, newwidth, newheight, 4);

	ImageData *pOutImage = new ImageData(OutFile);

	return pOutImage;


}
