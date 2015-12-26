#ifndef __XELTREE2D_H__
#define __XELTREE2D_H__

#include "param2d.h"
#include "point2d.h"
#include <vector>
#include <fstream>

////////////////////////////////////////////////////////////////////////
//class for storing xel branches in xel tree
class TXelBranch
{
public:
	TXY<int> startI, endI;
	int startNodeIndex, endNodeIndex;
	vector<TXY<int> > pointList;
};
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//class for storing xel tree
class TXelTree
{
public:
	vector<TXY<int> > nodeList;
	vector<TXelBranch> branchList;
	vector<int> pendingBranchIndexList;
	void print();
};
////////////////////////////////////////////////////////////////////////


#endif
