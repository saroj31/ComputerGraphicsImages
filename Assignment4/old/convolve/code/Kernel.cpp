/*-------------------------------------
 |    Kernel methods
 *--------------------------------------
 |
 |  Programmer: Donald House
 |  Date: Fri Oct 31 1996
 |
 |
 */

#include "Kernel.h"

#include <cstdlib>
#include <cmath>

using namespace std;

Kernel::Kernel(){
  width = height = size = 0;
  offset = range = 0.0;
  scale = 1.0;
  weights = NULL;
};

Kernel::~Kernel(){
  if(weights != NULL){
	delete weights[0];
	delete weights;
  }
}

/*
 Load the convolution kernel information from the kernel file
 */
int Kernel::Read(FILE *filtfile){
  int row, col;
  int ksize;
  float kscale;
  float weight;

  if(fscanf(filtfile, "%d %f", &ksize, &kscale) != 2){
    fprintf(stderr,"kernel size or scale invalid\n");
    return 0;
  }

  kscale = (kscale == 0)? 1: kscale;
  SetScale(kscale);
  printf("filter kernel size = %d X %d, scale factor = 1/%f\n\n",
		 ksize, ksize, kscale);
  
  if(ksize < 1 || ksize > 99 || ksize % 2 == 0){
    fprintf(stderr,"kernel size invalid, must be odd and between 1 and 99\n");
    return 0;
  }
  SetSize(ksize);
  
  for(row = 0; row < ksize; row++){
    for(col = 0; col < ksize; col++){
      if(fscanf(filtfile, "%f", &weight) != 1){
		fprintf(stderr,"kernel weight invalid or missing\n");
		return 0;
	  }
	  weights[row][col] = weight;
      printf("%5.1f ", weight);
    }
    printf("\n");
  }
  printf("\n");
  
  SetOffsetRange();
  
  return 1;
}

void Kernel::SetScale(float s){
  scale = s;
}

void Kernel::SetSize(int s){
  SetSize(s, s);
}

void Kernel::SetOffsetRange(){
  float negsum, possum;
  
  negsum = possum = 0;
  for(int row = 0; row < height; row++)
	for(int col = 0; col < width; col++)
	  if(weights[row][col] < 0)
		negsum += weights[row][col];
	  else
		possum += weights[row][col];
  
  offset = -negsum;
  range = possum - negsum;
}

void Kernel::SetSize(int w, int h){
  width = w;
  height = h;
  size = w * h;
  weights = new float*[height];
  weights[0] = new float[size];
  for(int i = 1; i < height; i++)
	weights[i] = weights[i - 1] + width;
  for(int row = 0; row < height; row++)
	for(int col = 0; col < width; col++)
	  weights[row][col] = 0;
}

