//abstracts parameters used by the program
#ifndef __PARAM2D_H__
#define __PARAM2D_H__

//frequently used headers
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "iostream"
using namespace std;

//program parameters
extern int Nx, Ny;	//dimensions of xel grid
extern long Np;		//Nx * Ny
extern int scale;	
extern string inputFile;
extern int blurCount;
extern float imageThreshold;
extern const int xelSize;
extern const float minorRadius, majorRadius;
extern const float pitchX, pitchY;
extern float imageForceScale;
extern float crossLinkThreshold;

//generic macro for FOR loops
#define FOR(i,N)  for(int i=0;i<N;i++)

//Pie value
#define PI 3.14159265358979323F	

#endif
