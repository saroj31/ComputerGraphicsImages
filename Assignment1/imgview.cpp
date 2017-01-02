/*
   
   Cient program to use OpenGL ad OpneImageIO services to display an image file in a window using opengl.
 
   By default program opens a window and draws a blank window of black background.
   if you press r then the command prompt asks for the name of the image file that needs to be shown in the window.
   Press w to write the image in a different format.

   CPSC 4040/6040            Saroj Kumar Dash         09/07/2016

*/


#include "GLServices.h"
#include "OIIOServices.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int w,h,ch;   //Right now using the variables to use the dimensions of the image we read.


//This method displays a blank image with a black background
void DisplayBlankImage()
{
	GLServices::DisplayImage();
	return;
}


//This method helps in keeping the image in the middle of the window.
//BUG to fx: when the window is squeezed smaller than the image dimension then the image does not adjust itselfs
void reshape(int iw,int ih)
{
	GLServices::reshape(iw,ih);
	return;
}

//The method Read an image shows it  in Window using GL Services
void ReadAndDIsplayImage(const char* ipFilename=NULL)
{
	unsigned char *ipixels = NULL;
	char InputName[30];
	
	cout<<"filename = "<<ipFilename;
	if(NULL == ipFilename) 
	{
 		cout<<"Name of the File to be read: ";
		cin>>InputName;
		ipFilename = (const char*)InputName;
	}
	
	
	ipixels = OIIOServices::ReadImage((const char*)ipFilename,w,h,ch);
	
	if(NULL==ipixels)	cout<<"ipixels is empty"<<endl;

	GLServices::DisplayImage(ipixels,w,h,ch);
	
	return;

}


//Callback method to respond the user responses to read and write image.
void handleKey(unsigned char key, int x, int y){
  
  //int w,h,ch;
  unsigned char *pixels = NULL;
  char opFilename[30];
  
  switch(key){
    case 'r':		// 'r' - read and display the image
    case 'R':
		ReadAndDIsplayImage();
      		break;

    case 'q':		// q - quit
    case 'Q':
    case 27:		// esc - quit
      		exit(0);

   case 'w':
   case 'W':           //TO write the image in the same folder
		
		cout<<"Give the name of the image to write: ";
		cin>>opFilename;
		cout<<"w= "<<w<<"h= "<<h<<"ch= "<<ch<<endl;
		pixels = GLServices::ReadPixels(w,h,ch);
		
		if(NULL != pixels)
			OIIOServices::WriteImage(opFilename,pixels,w,h,ch);
		break;
      
    default:		// not a valid key -- just ignore it
      		return;
  }

  delete pixels;
  return;
}


int main(int argc, char* argv[])
{
	
	glutInit(&argc,argv);
	GLServices::Init();
	

	if(NULL != argv[1])
		ReadAndDIsplayImage(argv[1]);
	else
		glutDisplayFunc(DisplayBlankImage);


	glutKeyboardFunc(handleKey);
	glutReshapeFunc(reshape);
	
	glutMainLoop();
	
	return 0;
}



