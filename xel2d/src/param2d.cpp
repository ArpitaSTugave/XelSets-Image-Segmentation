#include "param2d.h"
//programmer's (user's) parameters
int Nx = 20;
int Ny = 20;
int scale=1;
const int xelSize = 20;
string inputFile = "opencvtest.bmp";
int blurCount = 10;		//number of times the image is blured
float imageThreshold =0.001;
float imageForceScale = xelSize * 10;
float crossLinkThreshold = 0.1;

//derived parameters: do not change directly
const float minorRadius = xelSize * sqrt(3.0) / 2.0;
const float majorRadius = xelSize;
const float pitchX = minorRadius * 2;
const float pitchY = majorRadius * 1.5;
long Np = Nx*Ny;
