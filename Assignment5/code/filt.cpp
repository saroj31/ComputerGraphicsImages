#include <iostream>
using namespace std;
#include "string.h"
#include <math.h>

#include "OIIOServices.h"
#include "GLView.h"


#define PI 3.14159265

ImageData *g_pInImage = NULL, *g_pOutImage = NULL;




void DisplayInput() {

	GLView::SetImageToDisplay(*g_pInImage);
	glutReshapeWindow(g_pInImage->GetWidth(), g_pInImage->GetHeight());
	GLView::DisplayImage();
}


void DisplayOutput() {

	GLView::SetImageToDisplay(*g_pOutImage);
	glutReshapeWindow(g_pOutImage->GetWidth(), g_pOutImage->GetHeight());
	GLView::DisplayImage();

}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	if( 3 > argc){
		cout << " filename not entered" << endl;
		cout << " please enter the in below format:" << endl;
		cout << " ./warp Imagename.png  filtername.filt " << endl;
		return 0;
	}

  string pInImagename, pOutImagename;
  
  switch (argc)
  {
    case 3: //./filt In.png filter.filt
            pInImagename = argv[1];
            pOutImagename = argv[2];
            break;

    default:
            cout << "Wrong input given" << endl;
            break;
  }

  if(pInImagename.size())
	  g_pInImage = new ImageData(pInImagename.c_str());
  else
  {
    return 0;
  }

 
  //g_pOutImage=g_pInImage->ForwardMap();
	
	GLView::SetImageToDisplay(*g_pInImage);
	GLView::Init(pInImagename.c_str());
	glutDisplayFunc(DisplayInput);
	
	GLView::SetImageToDisplay(*g_pOutImage);
	char pOUTFilename[] = "Warp";
	GLView::Init(pOUTFilename);
	glutDisplayFunc(DisplayOutput);
	
	glutMainLoop();

	delete g_pInImage;
	delete g_pOutImage;
	
	return 0;
}