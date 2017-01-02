/*

   This class is the model of the system. 
   This contains all the data and data related information.

*/


//Header Declaration
#include <iostream>
using namespace std;

#include "ImageData.h"
#include "OIIOServices.h"
//Header Declaration



//Default Constructor of ImageData or model
ImageData::ImageData():_pPixels(NULL),_sfilename(""), _iwidth(0),_iheight(0), _inbChannels(0){

	//cout << "Default ImageData constructed" << endl;
	_pPixels=NULL;
	_sfilename = "";
	_iwidth = 0;
	_iheight = 0;
	_inbChannels = 0;

}

ImageData::ImageData(const char *isFileName):_pPixels(NULL),_sfilename(""),_iwidth(0),_iheight(0),_inbChannels(0){

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
	_inbChannels = 4;

  _sfilename = ipfilename;

}

void ImageData::WriteImage(char *ipfilename) {

	OIIOServices::WriteImage(ipfilename,_pPixels,_iwidth,_iheight,_inbChannels);
	return;
}



