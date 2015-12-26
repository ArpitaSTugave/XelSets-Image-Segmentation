
//handles all the openGL business
#ifndef __UIHANDLER2D_H__
#define __UIHANDLER2D_H__

#include "param2d.h"
#include "xel2d.h"
#include "input2d.h"
#include<iostream>
#include "opencv.hpp"
//inluding opengl libraries
#ifdef Linux
	#include "../../opengl/linux/GL/freeglut.h"
	#include "../../opengl/linux/GL/freeglut.h"
#endif
#ifdef Darwin
	#include "../../opengl/osx/GL/freeglut.h"
	#include "../../opengl/osx/GL/freeglut.h"
#endif


////////////////////////////////////////////////////////////////////////
//global function prototypes

void initUI(TXel2D *xel);							//initialize GL
void drawHandler();									//draw event handler
void keyHandler(unsigned char key, int x, int y);	//key event handler
void mouseHandler(int button, int state, int x, int y);
void reshape(int w, int h);							//reshape handler
void process();
void drawGrid();								//draw the primitive grid
void fillGrid();								//draw primitive grid with grey values of xels filled. 
void drawPoints();								//draw the points (p) in the grid
////////////////////////////////////////////////////////////////////////

#endif
