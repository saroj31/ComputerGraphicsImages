#include "GLView.h"
#include <iostream>
using namespace std;


int GLView::_iw = 0;
int GLView::_ih = 0;
int GLView::_ich = 0;
unsigned char* GLView::_pixels=NULL;

int GLView::SetImageToDisplay(ImageData &iDataImage) {
	
	_iw = iDataImage.GetWidth();
	_ih = iDataImage.GetHeight();
	_ich = iDataImage.GetNbOfChannels();
	_pixels = iDataImage.GetPixelsinRGBA();

	//cout << "Image is set for view." << endl;

	return 0;
}


void GLView::SetDataToDisplay(unsigned char *ipPixels, int iw, int ih, int ich) {

	_pixels = ipPixels;
	_iw = iw;
	_ih = ih;
	_ich = ich;

}

//Initializes the OpenGL Window with initial values.
void GLView::Init()
{
	
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	if((0==_iw) && (0 == _ih))
		glutInitWindowSize(WIDTH, HEIGHT);
	else
		glutInitWindowSize(_iw,_ih);

	glutCreateWindow("OpenGL View ");
	
	//Initializing the Window with emty color and the initial raster position
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0,0,10,10);
	glRasterPos2i(0,0);
	
	return;
}

//Callback when the widnowo reshapes. This method basically redraws the image again at the center of the Window.
void GLView::reshape(int iw,int ih)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, iw, ih);

	DisplayImage();
	glutPostRedisplay();
	//cout << "Image reshaped" << endl;
}

//This image displays the image with a viewport at the center of the Window.
void GLView::DisplayImage()
{
	//cout << "Image Display " << endl;
	//To Dispay the image in the OpenGL  Window
	if (NULL == _pixels) {
		cout << "Display Pixels empty" << endl;
		return;
	}
	
	glutReshapeWindow(_iw, _ih);
	glMatrixMode(GL_PROJECTION);
 	glLoadIdentity();
 	gluOrtho2D(0, _iw, 0, _ih);

	// specify window clear (background) color to be opaque white
	glClearColor(1, 1, 1, 1);
	glMatrixMode (GL_MODELVIEW);
	glRasterPos2i(0,0);
	

	if(3==_ich)
		glDrawPixels(_iw,_ih,GL_RGB,GL_UNSIGNED_BYTE,_pixels);
	else if(4 == _ich)
		glDrawPixels(_iw,_ih,GL_RGBA,GL_UNSIGNED_BYTE,_pixels);

	glFlush();
	return;
}


//This method reads the pixels from the window.
unsigned char *GLView::ReadPixels()
{
	unsigned char *pixmap = new unsigned char[_iw*_ih*_ich];
	
	if(3==_ich)	
		glReadPixels(0,0,_iw,_ih, GL_RGB, GL_UNSIGNED_BYTE, pixmap);
	else if(4==_ich)
		glReadPixels(0, 0, _iw, _ih, GL_RGBA, GL_UNSIGNED_BYTE, pixmap);
	
	return pixmap;
}

void GLView::CleanCache(){

	if (NULL == _pixels) return;
	_pixels = NULL;
	_iw = 0;
	_ih = 0;
	_ich = 0;
}


                  