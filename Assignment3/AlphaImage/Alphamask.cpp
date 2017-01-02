#include <iostream>
using namespace std;
#include "string.h"

#include "OIIOServices.h"
#include "GLView.h"

ImageData *g_pImage = NULL;
int g_isRead = 0;

void ShowOptions()
{
	cout << "******************" << endl;
	cout << "press a/A to display the alpha image" << endl;
	cout << "press r/R to read another image" << endl;
	cout << "press d/D to display the image" << endl;
	cout << "press w/W to write the image in view" << endl;
	cout << "press q/Q to quit" << endl;
	cout << "******************" << endl<<endl;
	cout << "Enter your Option::";
	

}

void ReadImage() {

	if (1 == g_isRead) {
		delete g_pImage;
		g_pImage = NULL;
	}

	char *pFilename = new char[25];
	cout << "Enter the filename of the image to read::";
	cin >> pFilename;

	g_pImage = new ImageData(pFilename);
	
	cout << "Name of the Image that is read " << pFilename << endl;
	delete pFilename;
	pFilename = NULL;
	
	g_isRead = 1;

	return;
}

void writeimage()
{
	char *pFilename = new char[25];
	cout << "Enter the filename to write:";
	cin >> pFilename;

	if (g_pImage)
	{
		unsigned char *pCurrentPixels = GLView::ReadPixels();

		OIIOServices::WriteImage(pFilename,pCurrentPixels,g_pImage->GetWidth(),g_pImage->GetHeight(),g_pImage->GetNbOfChannels());

		delete pCurrentPixels;
		pCurrentPixels = NULL;
	}

	delete(pFilename);
	pFilename = NULL;

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

	//Display part
	GLView::SetDataToDisplay(pAlphaImage,g_pImage->GetWidth(),g_pImage->GetHeight(),g_pImage->GetNbOfChannels());
	glutPostRedisplay();
	return;
}

void DisplayImage() {

	if (NULL != g_pImage)
	{
		GLView::SetImageToDisplay(*g_pImage);
		glutPostRedisplay();
	}
}


void handleKey(unsigned char key, int x, int y) {

	switch (key) {
	case 'a':		// 'a' - to get the alpha image
	case 'A':
		AlphaImage();
		break;

	case 'r':		// 'r' - read the image with the given name
	case 'R':
		ReadImage();
		break;
	
	case 'd':		// 'd' - Display the currentImage
	case 'D':
		DisplayImage();
		break;

	case 'w':		// 'w' - TO read the imag
	case 'W':
		writeimage();
		break;

	case 'q':		// q - quit
	case 'Q':
	case 27:		// esc - quit
		GLView::CleanCache();
		if (g_pImage)	delete g_pImage;
		exit(0);

	default:		// not a valid key -- just ignore it
		return;
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	//Reading the image to work upon at first.
	ReadImage();
	
	ShowOptions();

	GLView::Init();
	glutDisplayFunc(GLView::DisplayImage);
	glutKeyboardFunc(handleKey);
	glutReshapeFunc(GLView::reshape);

	glutMainLoop();

	return 0;
}