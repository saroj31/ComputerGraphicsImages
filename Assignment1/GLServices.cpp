#include "GLServices.h"
#include <iostream>
using namespace std;



int GLServices::_iw = 0;
int GLServices::_ih = 0;
int GLServices::_ich = 0;
unsigned char* GLServices::_pixels=NULL;

//Initializes the OpenGL Window with initial values.
void GLServices::Init()
{
	//Initialization of all services of openGL is done hereS
	
	unsigned char iPixels[WIDTH][HEIGHT][CHANNELS];
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("ImageWindow");
	
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glRasterPos2i(0,0);
	glDrawPixels(WIDTH,HEIGHT,GL_RGBA,GL_UNSIGNED_BYTE,iPixels);
	glFlush();
	
	return;
}

//Callback when the widnowo reshapes. This method basically redraws the image again at the center of the Window.
void GLServices::reshape(int iw,int ih)
{
	
	DisplayImage(_pixels,_iw,_ih,_ich);
	glutPostRedisplay();  
}



//This image displays the image with a viewport at the center of the Window.
int GLServices::DisplayImage(unsigned char *iPixels/*=NULL*/,int iw/*=WIDTH*/,int ih/*=HEIGHT*/,int ich/*=CHANNELS*/)
{
	//To Dispay the image in the OpenGL  Window
	if(NULL == iPixels)    
	{
		cout<<"empty Pixels recieved in Display image"<<endl;
		
	}
	//cout<<"Display image"<<endl;
	
	//extracts the Widnow width and height and makes thte viewport at the center of the window
	//such that the image appears in the middle.
	int WinWidth = glutGet(GLUT_WINDOW_WIDTH);
	int WinHeight = glutGet(GLUT_WINDOW_HEIGHT);
	_iw = iw;
	_ih = ih;
	_ich = ich;
	_pixels = iPixels;
	int x = (WinWidth-iw);
	int y = (WinHeight-ih);
	
	glViewport(x/2,y/2,iw,ih);
	glMatrixMode(GL_PROJECTION);
 	glLoadIdentity();
 	gluOrtho2D(0, iw, 0, ih);

	// specify window clear (background) color to be opaque white
	glClearColor(1, 1, 1, 1);
	glMatrixMode (GL_MODELVIEW);
	glRasterPos2i(0,0);
	

	if(3==_ich)
		glDrawPixels(iw,ih,GL_RGB,GL_UNSIGNED_BYTE,iPixels);
	else if(4 == _ich)
		glDrawPixels(iw,ih,GL_RGBA,GL_UNSIGNED_BYTE,iPixels);

	//cout<<"Image displayed"<<endl;
	glFlush();
	return 0;
}


//This method reads the pixels from the window.
unsigned char *GLServices::ReadPixels(int inbw,int inbh,int inbch)
{
	unsigned char *pixmap = new unsigned char[inbw*inbh*inbch];
	
	int WinWidth = glutGet(GLUT_WINDOW_WIDTH);
	int WinHeight = glutGet(GLUT_WINDOW_HEIGHT);
	
	int x = WinWidth-inbw;
	int y = WinHeight-inbh;
	
	if(3==inbch)	
		glReadPixels(x/2, y/2, inbw, inbh, GL_RGB, GL_UNSIGNED_BYTE, pixmap);
	else if(4==inbch)
		glReadPixels(x/2, y/2, inbw, inbh, GL_RGBA, GL_UNSIGNED_BYTE, pixmap);
	
	return pixmap;
}






