#ifndef __INPUT2D_H__
#define __INPUT2D_H__

#include "param2d.h"
#include "xel2d.h"
#include "opencv.hpp"
#include<iostream>
#include <vector>
////////////////////////////////////////////////////////////////////////
//global functions
//loads image and re-samples it on xelular grid
void loadImage1();
void varyblur1(float  *imgY,TXel2D *xel,int blurCount );	
void resampleImage1(TXel2D *xel);
//blur grey values n times
float* blurY1(float *Y, int W, int H, int n = 1);	
////////////////////////////////////////////////////////////////////////


#endif
