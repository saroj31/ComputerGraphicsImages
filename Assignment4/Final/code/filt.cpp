#include <iostream>
using namespace std;
#include "string.h"
#include <math.h>

#include "OIIOServices.h"
#include "GLView.h"
#include "KernalData.h"


#define PI 3.14159265

ImageData *g_pInImage = NULL, *g_pOutImage = NULL;
KernelData *g_pKernel = NULL;


//Theta is in degrees
KernelData *BuildGaborKernel(float iTheta, int iSigma, float iPeriod) {

  //Todo: Build a kernel of Ksize, Kscale = 1, Kfloat * contains an array of all float values in the Kernel

  int Ksize = (4*iSigma + 1);
  //if(iTheta != 0)
    //iTheta = (180 / PI)*(iTheta);


  float *KWtValues = new float[Ksize*Ksize];

  float x1, y1;    //the xprime and yprime variables in the function
  float value = 0.0f;
  for (int i = 0; i < Ksize; i++)
  {
    for (int j = 0; j < Ksize; j++) {

      float x = i - (2*iSigma);
      float y = j - (2*iSigma);
      x1 = (x*cos(iTheta) + (y*sin(iTheta)));
      y1 = -(x*sin(iTheta)) + (y*cos(iTheta));


      float PowOfExp = -( ((x1*x1) + (y1*y1))/(2 * iSigma*iSigma) );
      float CosVar = (2 * PI*x1) / iPeriod;

      value = exp(PowOfExp)*cos(CosVar);
      
      KWtValues[i*Ksize + j] = value;
    }
  }

  KernelData  *pGaborKernel = new KernelData(KWtValues,Ksize,1);

  return pGaborKernel;

}

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
		cout << " ./filt Imagename.png  filtername.filt " << endl;
		return 0;
	}

  string pInImagename, pFiltname, pOutImagename;
  float pTheta, pSigma, pPeriod;
  bool bGaborMode = false;

  switch (argc)
  {
    case 3: //./filt In.png filter.filt
      cout << "argument read as 3" << endl;
            pInImagename = argv[1];
            pFiltname = argv[2];
            break;
    
    case 4:  //./filt In.png filter.filt Out.png
            pInImagename = argv[1];
            pFiltname = argv[2];
            pOutImagename = argv[3];
            break;

    case 6:  // ./filt In.png -g theta sigma period
            cout << argv[2] << endl;
            cout << strcmp(argv[2], "-g") << endl;
            if (0 == strcmp(argv[2],"-g")) {
              
              bGaborMode = true;
              pInImagename = argv[1];
              pTheta = atof(argv[3]);
              pSigma = atof(argv[4]);
              pPeriod = atof(argv[5]);
            }
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

  if (pFiltname.size()) {

    g_pKernel = new KernelData();
    g_pKernel->ReadFile(pFiltname.c_str());
    g_pKernel->Display();
  }
  else if(false == bGaborMode)
  {
    cout << "enter the name of Kernel file" << endl;
    cin >> pFiltname;
    g_pKernel = new KernelData();
    g_pKernel->ReadFile(pFiltname.c_str());
    g_pKernel->Display();
  }
  
  //if -g option in e.g. ./filt in.png filt.filt [out.png]
  if(bGaborMode){
    if(g_pKernel) delete g_pKernel;
    g_pKernel = NULL;
    g_pKernel = BuildGaborKernel(pTheta,pSigma,pPeriod);  //theta,sigma,T
  }
  

  
	//convolve function
  g_pOutImage = g_pInImage->ConvolveWith(g_pKernel);
 
	
	GLView::SetImageToDisplay(*g_pInImage);
	GLView::Init(pInImagename.c_str());
	glutDisplayFunc(DisplayInput);
	
	GLView::SetImageToDisplay(*g_pOutImage);
	char pOUTFilename[] = "OUTPUT Result";
	GLView::Init(pOUTFilename);
	glutDisplayFunc(DisplayOutput);
	
	glutMainLoop();

	delete g_pInImage;
	delete g_pOutImage;
	delete g_pKernel;

	return 0;
}