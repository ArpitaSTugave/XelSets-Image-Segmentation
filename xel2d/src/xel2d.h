#ifndef __XEL2D_H__
#define __XEL2D_H__
#include "param2d.h"
#include "point2d.h"
#include "xeltree2d.h"

////////////////////////////////////////////////////////////////////////
//naming criteria
// 'i' for linear Index
// 'I' for 2D grid index of type TXY<int>
// 'p' for point of type TXY<float>
// 'P' for mid point of a xel
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
//global variables and macros

//counter of TXel2D class instance initialized
//static long xelCtr;		
//neighbour reference 
	//first 6 for xels in even rows
	//next 6 for xel in odd rows.
extern TXY<int> nbrI[12]; 

#define INRANGE(I) (I.x>=0 && I.x<Nx && I.y>=0 && I.y<Ny)
#define ISNIL(k) k>0.00001 || k<-0.00001 
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//stores input image
extern float  *imgY;
extern float** blurimg1;
extern int imageWidth, imageHeight;
////////////////////////////////////////////////////////////////////////
	


////////////////////////////////////////////////////////////////////////
class TXel2D
{
public:
	const TXY<int> I;		//Index of xel in grid
	const TXY<float> P;	//mid point of xel in grid
	
	TXY<float> p;			//floating point position
	int xelType;			//0 = empty, 1 = static, 2 = mobile	
	int networkType;		//which network type: -1 = un-accountable, 0 = orphan, 1 = end point, 2 = chained point, 3 = triple point
	
	float Y;				//sampled grey value from the image
	
	TXY<float> departingPoint;	//the point after movement
	TXY<int> departingXel;		//the xel to which the present point departs to after a step of motion
	TXY<float> enteringPoint;	//the point which is supposed to enter into the xel
	int enteringPointCount;		//count of the points which are supposed to enter
		
	TXel2D();				//constructor
	~TXel2D();				//destructor
};
////////////////////////////////////////////////////////////////////////







////////////////////////////////////////////////////////////////////////
//global function prototypes
//converts linear index to 2D grid index 
TXY<int> to_iI(int i);					    
//converts 2D grid index to linear index
int		to_Ii(TXY<int> I);					
//returns the mid point of a xel at index i (Linear index)
TXY<float> getXelMidPoint(long int I);		
//returns the mid point of a xel at Index I
TXY<float> getXelMidPoint(TXY<int> I);		
//return index of xel from cartesian coordinates
TXY<int> getXelIndex(TXY<float> p);			
//adds a given point in xel sets if it lies in the xel
void addPointToXelSet(TXel2D *xel, TXY<float> p, int xelType);		
//adds a mid point in the xel containing the given point
void addMidPointToXelSet(TXel2D *xel, TXY<float> p, int xelType);	
//adds a mid point in the xel at a given index
void addMidPointToXelSet(TXel2D *xel, TXY<int> I, int xelType);	
void cleanXel(TXel2D *xel, TXY<int> I);
void fillXelSet(TXel2D *xel);

//iteration steps
//runs the iteration on CPU
void cpuRun(TXel2D *xel);				
//buffer new point position
void bufferNewPoints(TXel2D *xel);		
//interpolate missed points
void interpolatePoints(TXel2D *xel);	
//does the copying, resetting and finishing of iteration
void finishIteration(TXel2D *xel);	
//extract the topological network
void extractNetwork(TXel2D *xel);
void removeCrossLinks(TXel2D *xel);	
////////////////////////////////////////////////////////////////////////


#endif
