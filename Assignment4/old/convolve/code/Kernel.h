/*
   Standard definitions for the Kernel class

   D. H. House  10/12/94
*/

#ifndef _KERNEL
#define _KERNEL

#include <cstdio>

class Kernel{
public:
  int size;
  int width, height;
  float offset, range;
  float scale;
  float **weights;
  
  Kernel();
  
  ~Kernel();
  
  int Read(FILE *filtfile);
  
  void SetScale(float s);
  
  void SetSize(int s);
  
  void SetOffsetRange();
  
  void SetSize(int w, int h);
};

#endif
