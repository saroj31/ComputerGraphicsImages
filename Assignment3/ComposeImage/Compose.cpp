/*

   The compose program takes two input images

   the first iamge is assumed to be taken from the output of Alphamask program.

   The program does the compose on the two images and gives output image as a compositing.
*/

#include <iostream>
using namespace std;
#include "string.h"

#include "OIIOServices.h"
#include "GLView.h"

ImageData *g_pImage = NULL,*g_BGImage = NULL;


void ReadImage() {

	if (g_pImage) {
		delete g_pImage;
		g_pImage = NULL;
	}

	if (g_BGImage){
		delete g_BGImage;
		g_pImage = NULL;
	}


	char *pFilename = new char[25];
	cout << "Enter the filename of the foreground image to read::";
	cin >> pFilename;

	g_pImage = new ImageData(pFilename);
	
	strcpy(pFilename, "");
	cout << "Enter the filename of the Background image: ";
	cin >> pFilename;

	g_BGImage = new ImageData(pFilename);

	delete pFilename;
	pFilename = NULL;
	
	return;
}

void AlphaImage() {

	//Gets the Alpha Image and displays it
	unsigned char *pAlphaImage = NULL;
	if (g_pImage)
		pAlphaImage = g_pImage->r_GetAssociatedImage();

	//Write Image
	char *pFilename = new char[25];
	cout << "Enter the filename to write:";
	cin >> pFilename;

	OIIOServices::WriteImage(pFilename, pAlphaImage, g_pImage->GetWidth(), g_pImage->GetHeight(), g_pImage->GetNbOfChannels());

	return;
}

void DisplayImage() {

	//Was using this for debugging purpose.
	if (NULL != g_pImage)
	{
		GLView::SetImageToDisplay(*g_pImage);
		glutPostRedisplay();
	}
}


int main(int argc, char* argv[])
{
	//glutInit(&argc, argv);

	//Reading the image to work upon at first.
	ReadImage();
	
	int iw, ih;
	unsigned char* pComposePixels = g_pImage->r_Compose(g_BGImage,iw,ih);


	char *pFilename = new char[25];
	cout << "Enter the name of the Composite image to save::";
	cin >> pFilename;

	//I am assuming that the output image is of 4 channel
	OIIOServices::WriteImage(pFilename, pComposePixels, iw, ih, 4);

	delete pFilename;
	pFilename = NULL;

	delete pComposePixels;
	pComposePixels = NULL;

	return 0;
}