#include "xel2d.h"

////////////////////////////////////////////////////////////////////////
//global variables
TXY<int> nbrI[12]; 	//first 14 for points in even z row and remaining for off z row.
TXY<float> hexNorm[3];
float  *imgY;
float** blurimg1;
int imageWidth, imageHeight;
TXelTree tree;
static long xelCtr;
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
//class definitions TXel2D
TXel2D::TXel2D():I(to_iI( (xelCtr<Np)?(xelCtr):0 )), P(getXelMidPoint( (xelCtr<Np)?(xelCtr):0 ))
{
	xelCtr++;
	xelType = 0;
	networkType = -1;
	p.set(0.0);
	Y = 0.0;
	departingPoint.set(0);
	departingXel.set(0);
	enteringPoint.set(0.0);
	enteringPointCount = 0;
		
	//////////////// initialization of neighbor index
	if(xelCtr==1)
	{

		//edge-normal initialization
		hexNorm[0].set(1.0, 0.0);
		hexNorm[1].set(0.5, sqrt(3.0)/2.0);
		hexNorm[2].set(-0.5, sqrt(3.0)/2.0);
		
		//assigning neighbours
		nbrI[ 0].set( 0, 1);
		nbrI[ 1].set( 1, 0);
		nbrI[ 2].set( 0,-1);
		nbrI[ 3].set(-1,-1);
		nbrI[ 4].set(-1, 0);
		nbrI[ 5].set(-1, 1);
		
		nbrI[ 6].set( 1, 1);
		nbrI[ 7].set( 1, 0);
		nbrI[ 8].set( 1,-1);
		nbrI[ 9].set( 0,-1);
		nbrI[10].set(-1, 0);
		nbrI[11].set( 0, 1);
	}
	else
	if(xelCtr == Np)
	{
		//do-nothing
	}
	
}

TXel2D::~TXel2D()
{
	//nothing yet
}
////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////
//global function definitions

TXY<int> to_iI(int i)
{
	TXY<int> I;
	
	I.set(i%Nx, i/Nx);
	
	return I;
}

int to_Ii(TXY<int> I)
{
	return I.y*Nx + I.x;
}

TXY<float> getXelMidPoint(long int i)
{
	TXY<float> P;
	
	TXY<int> I = to_iI(i);
	
	P.set(I.x * pitchX, I.y * pitchY);
	P.x += minorRadius*(I.y%2);
	
	return P;
}

TXY<float> getXelMidPoint(TXY<int> I)
{
	TXY<float> P;
	
	P.set(I.x * pitchX, I.y * pitchY);
	P.x += minorRadius*(I.y%2);
	
	//to-do
	return P;
}

TXY<int> getXelIndex(TXY<float> p)
{
	TXY<int> i;
	
	int I = floor( (p.x*hexNorm[0].x + p.y*hexNorm[0].y)/minorRadius );
	int J = floor( (p.x*hexNorm[1].x + p.y*hexNorm[1].y)/minorRadius );
	int K = floor( (p.x*hexNorm[2].x + p.y*hexNorm[2].y)/minorRadius );
	
	i.y = floor((J+K+2)/3);	
	if( (i.y % 2) == 0)
		i.x = floor( (I + 1.0)/2.0 );
	else
		i.x = floor( I/2.0 );
		
	return i;
}

void addPointToXelSet(TXel2D *xel, TXY<float> p, int xelType)
{
	TXY<int> I = getXelIndex(p);
	if(INRANGE(I))
	{
		int i = to_Ii(I);
		if(xel[i].xelType==0)	//adds point only if the xel was previously empty
		{
			xel[i].p = p;
			xel[i].xelType = xelType;
		}
	}
}

void addMidPointToXelSet(TXel2D *xel, TXY<float> p, int xelType)
{
	TXY<int> I = getXelIndex(p);
	if(INRANGE(I))
	{
		int i = to_Ii(I);
		if(xel[i].xelType==0)	//adds point only if the xel was previously empty
		{
			xel[i].p = xel[i].P;
			xel[i].xelType = xelType;
		}
	}
}

void addMidPointToXelSet(TXel2D *xel, TXY<int> I, int xelType)
{
	if(INRANGE(I))
	{
		int i = to_Ii(I);
		if(xel[i].xelType==0)	//adds point only if the xel was previously empty
		{
			xel[i].p = xel[i].P;
			xel[i].xelType = xelType;
		}
	}
}

void cleanXel(TXel2D *xel, TXY<int> I)
{
	if(INRANGE(I))
	{
		int i = to_Ii(I);
		xel[i].xelType = 0;
		xel[i].p.set(0.0);
		xel[i].networkType = -1;
		xel[i].departingPoint.set(0.0);
		xel[i].departingXel.set(0);
		xel[i].enteringPoint.set(0.0);
		xel[i].enteringPointCount = 0;
	}
}

void fillXelSet(TXel2D *xel)
{
	FOR(i, Np)
	{
		if(xel[i].xelType==0)
			addMidPointToXelSet(xel, to_iI(i), 2);
	}
}

void cpuRun(TXel2D *xel)
{
	bufferNewPoints(xel);	//calculate movement, apply and buffer new positions
	interpolatePoints(xel);	//interpolate points between new buffered positions
	finishIteration(xel);	//write new points, delete old
}

void bufferNewPoints(TXel2D *xel)
{
	FOR(i, Np)
	{
		//calculed the new moved point if the xel is movable type
		if(xel[i].xelType==2)
		{
			//xel force
			TXY<float> xelF(0.0, 0.0);
			//image force
			TXY<float> imgF(0.0, 0.0);
			
			//nbr count and mean point calculation
			TXY<int> I(0,0);
			int zMod2 = xel[i].I.y % 2;
			int dj = zMod2*6;
			
			TXY<float> mean(0, 0);
			int nbrCount = 0;
			
			//calculating mean of neighbors
			FOR(j,6)
			{
				I = nbrI[dj+j] + xel[i].I;
				if(INRANGE(I))
				{
					int in = to_Ii(I);
					if( xel[in].xelType==2)
					{
						mean = mean + xel[in].p;
						nbrCount++;
					}
				}
			}
			mean = mean/nbrCount;
			
			//calculating image force
			if(nbrCount < 6)
			{
				TXY<float> _p = xel[i].p/pitchX;
                                _p=_p *scale;
				int X1 = floor(_p.x);
				int Y1 = floor(_p.y);
				if(X1>=0 && X1<imageWidth-1 && Y1>=0 && Y1<=imageHeight-1)
				{
					float dy = _p.y - Y1;
					float dx = _p.x - X1;
					imgF.x = (1-dy)*(imgY[Y1*imageWidth+X1+1] - imgY[Y1*imageWidth+X1]) + dy*(imgY[(Y1+1)*imageWidth+X1+1] - imgY[(Y1+1)*imageWidth+X1]);
					imgF.y = (1-dx)*(imgY[(Y1+1)*imageWidth+X1] - imgY[Y1*imageWidth+X1]) + dx*(imgY[(Y1+1)*imageWidth+X1+1] - imgY[Y1*imageWidth+X1+1]);
					imgF = imgF * imageForceScale;
				}
			}
			
			//calculaing xelular force
			if(nbrCount>1 && nbrCount<6)
			{
				
				//generating intertial tensor
				float xx=0, xy=0, yy=0;
				TXY<float> dp;
					
				FOR(j,6)
				{
					I = nbrI[dj+j] + xel[i].I;
					if(INRANGE(I))
					{
						
						int in = to_Ii(I);
						if(xel[in].xelType==1 || xel[in].xelType==2)
						{
							dp = xel[in].p - mean;
							xx += dp.x*dp.x;
							xy += dp.x*dp.y;
							yy += dp.y*dp.y;
						}
					}
				}
				
				//eigen value and eigen vector
				float eVal = (xx+yy - sqrt((xx-yy)*(xx-yy) + 4*xy*xy))/2.0;
				float eVal2 = (xx+yy + sqrt((xx-yy)*(xx-yy) + 4*xy*xy))/2.0;
				
				dp = mean - xel[i].p;
				
				xelF.set( (xx - eVal2)*dp.x + xy*dp.y, xy*dp.x + (yy - eVal2)*dp.y);
				xelF = xelF/(2*(eVal - eVal2));
				
				imgF.set( (xx - eVal2)*imgF.x + xy*imgF.y, xy*imgF.x + (yy - eVal2)*imgF.y);
				imgF = imgF/(eVal - eVal2);
				
			}
			
			TXY<float> newPoint = xel[i].p + xelF + imgF;
			xel[i].departingPoint = newPoint;
			xel[i].departingXel = getXelIndex(newPoint);	//the xel in which the point is departing
			
			
			if(INRANGE(xel[i].departingXel))
			{
				int i2 = to_Ii(xel[i].departingXel);
				
				//if(xel[i2].xelType==0 || xel[i2].xelType==2)	//point enters into xel only if the xel is empty or mobile
				//{
					xel[i2].enteringPoint = xel[i2].enteringPoint + newPoint;
					xel[i2].enteringPointCount++;
				//}
			}
		}
	}
}


void interpolatePoints(TXel2D *xel)
{
	FOR(i, Np)
	{
		
		TXY<float> p, p1, p2, dp;
		float dsMin = 0.02;
		int div;
		bool ifXelMoved1, ifXelMoved2;	//flags if the neighbors have moved out of their xels or not
		
		if(xel[i].xelType==0)
		{
			continue;	
		}
		else
		if(xel[i].xelType==2)
		{
			p1 = xel[i].departingPoint;
			ifXelMoved1 = true;
		}
		
		TXY<int> I2(0, 0);
		int zMod2 = xel[i].I.y % 2;
		int dj = zMod2*6;
			
		//iterating for each neighbour
		FOR(j, 6)
		{
			I2 = nbrI[j+dj] + xel[i].I;
			
			if(INRANGE(I2))
			{
				int i2 = to_Ii(I2);
				
				if(xel[i2].xelType==0)
				{
					continue;
				}
				else
				if(xel[i2].xelType==2)
				{
					p2 = xel[i2].departingPoint;
					ifXelMoved2 = true;
				}
				
				if(ifXelMoved1 || ifXelMoved2)
				{
					dp = p2 - p1;
					float ds = dp.length();
					div = floor(ds/dsMin);
					dp = dp/div;
					p = p1 + dp;
					
					FOR(k,div-1)
					{
						TXY<int> I3 = getXelIndex(p);
						
						if(INRANGE(I3))
						{
							int i3 = to_Ii(I3);
							if(xel[i3].xelType!=1 && xel[i3].enteringPointCount==0)
							{
								xel[i3].enteringPoint = p;
								xel[i3].enteringPointCount++;
							}
						}
						
						p = p + dp;
					}
					
				}
			}
		}
	}
}

void finishIteration(TXel2D *xel)
{
	FOR(i, Np)
	{
		if(xel[i].xelType == 0)	//to empty xels
		{
			if(xel[i].enteringPointCount > 0)
			{
				xel[i].p = xel[i].enteringPoint / xel[i].enteringPointCount;
				xel[i].xelType = 2;
				xel[i].departingPoint.set(0.0);
				xel[i].departingXel.set(0);
				xel[i].enteringPoint.set(0.0);
				xel[i].enteringPointCount = 0;
				
			}
			else
			{
				//simply, do nothing
			}
		}
		else
		if(xel[i].xelType == 2)		//to mobile xels
		{
			if(xel[i].enteringPointCount > 0)
			{
				xel[i].p = xel[i].enteringPoint / xel[i].enteringPointCount;
				xel[i].xelType = 2;
				xel[i].departingPoint.set(0.0);
				xel[i].departingXel.set(0);
				xel[i].enteringPoint.set(0.0);
				xel[i].enteringPointCount = 0;
				
			}
			else
			{
				xel[i].p.set(0.0);
				xel[i].xelType = 0;
				xel[i].departingPoint.set(0.0);
				xel[i].departingXel.set(0);
				xel[i].enteringPoint.set(0.0);
				xel[i].enteringPointCount = 0;
			}
		}
	}
}

//extract network information from xel
void extractNetwork(TXel2D *xel)
{
	
	//cleaning unwanted xels
	FOR(i, Np)
	{
		if(xel[i].xelType!=0) //if the xel is non-empty
		{
			
			int dj = (xel[i].I.y % 2)*6;
			
			TXY<int> nbrList0[6];		//nbr list of xel itself
			TXY<int> nbrList1[6];		//nbr list of 1st nbr
			TXY<int> nbrList2[6];		//nbr list of 2nd nbr
			int nbrCount0 = 0;			//nbr count of xel itself
			int nbrCount1 = 0;			//nbr count of 1st nbr
			int nbrCount2 = 0;			//nbr count of 2nd nbr
			
			
			//calculating nbr list of xel
			nbrCount0 = 0;
			FOR(j, 6)
			{
				//if a nbr is non-empty, it adds in nbrList0
				TXY<int> I2 = nbrI[j+dj] + xel[i].I;
				if(INRANGE(I2))
					if(xel[to_Ii(I2)].xelType != 0)
					{
						nbrList0[nbrCount0++] = I2;					
					}
			}
			
			//spanning accross the 6 triad formed
			FOR(j, 6)
			{
				//1st nbr of triad
				TXY<int> nbrI1 = nbrI[j+dj] + xel[i].I;
				int nbri1 = to_Ii(nbrI1);
				
				//2nd nbr of triad
				TXY<int> nbrI2 = nbrI[(j+1)%6+dj] + xel[i].I;
				int nbri2 = to_Ii(nbrI2);
				
				if(INRANGE(nbrI1) && INRANGE(nbrI2))
					if(xel[nbri1].xelType != 0 && xel[nbri2].xelType != 0 )
					{
						//we have detected three nbr-ing points forming a trait
						
						//calculating nbr list of nbr 1
						int dk1 = (xel[nbri1].I.y % 2)*6;
						int dk2 = (xel[nbri2].I.y % 2)*6;
						nbrCount1 = 0;
						nbrCount2 = 0;
						
						FOR(k, 6)
						{
							TXY<int> I2;
							I2 = nbrI[k+dk1] + xel[nbri1].I;
							
							if(INRANGE(I2))
								if(xel[to_Ii(I2)].xelType != 0)
								{
									nbrList1[nbrCount1++] = I2;					
								}
							
							if(INRANGE(I2))
								I2 = nbrI[k+dk2] + xel[nbri2].I;
								if(xel[to_Ii(I2)].xelType != 0)
								{
									nbrList2[nbrCount2++] = I2;					
								}
							
						}
						
						//a xel to be accountable should: 
							// have nbr other than the triad
							// and other than the nbrs of the triad
						bool triadPassed = false;
						FOR(l, nbrCount0)
						{
							
							if(nbrList0[l] == nbrI1)
								continue;
								
							if(nbrList0[l] == nbrI2)
								continue;
							
							bool nbrSharedByTraidMember1 = false;
							bool nbrSharedByTraidMember2 = false;
							
							FOR(l1, nbrCount1)
							if(nbrList0[l] == nbrList1[l1])
							{
								nbrSharedByTraidMember1 = true;
								break;
							}
							if(nbrSharedByTraidMember1)
								continue;
							
							FOR(l2, nbrCount2)
							if(nbrList0[l] == nbrList2[l2])
							{
								nbrSharedByTraidMember2 = true;
								break;
							}
							if(nbrSharedByTraidMember2)
								continue;
								
							triadPassed = true;
								
						} 
						
						if(!triadPassed)
						{
							cleanXel(xel, xel[i].I);
						}
					}
			}
		}
	}
	//cleanig unwanted xel completed
	
	
	//assigning network type number
	FOR(i, Np)
	{
		if(xel[i].xelType!=0) //if the xel is non-empty
		{
			
			int dj = (xel[i].I.y % 2)*6;
			
			//calculating nbr list of xel
			int nbrCount = 0;
			FOR(j, 6)
			{
				TXY<int> I2 = nbrI[j+dj] + xel[i].I;
				if(INRANGE(I2))
					if(xel[to_Ii(I2)].xelType != 0)
						nbrCount++;
			}
			
			if(nbrCount==0)
				xel[i].networkType = 0;
			else
			if(nbrCount==1)
				xel[i].networkType = 1;
			else
			if(nbrCount==2)
				xel[i].networkType = 2;
			else
			if(nbrCount > 2)
				xel[i].networkType = 3;
			
				
		}
		else
			xel[i].networkType = -1;
	}
	
	
	/*
	//removing triad of all 3-triple points
	FOR(i, Np)
	{
		if(xel[i].networkType==3) //if the xel is non-empty
		{
			
			int dj = (xel[i].I.y % 2)*6;
			
			//spanning accross the 6 triad formed
			FOR(j, 6)
			{
				//1st nbr of triad
				TXY<int> nbrI1 = nbrI[j+dj] + xel[i].I;
				int nbri1 = to_Ii(nbrI1);
				
				//2nd nbr of triad
				TXY<int> nbrI2 = nbrI[(j+1)%6+dj] + xel[i].I;
				int nbri2 = to_Ii(nbrI2);
				
				if(INRANGE(nbrI1) && INRANGE(nbrI2))
					if(xel[nbri1].networkType==3 && xel[nbri2].networkType==3 )
					{
						xel[nbri1].networkType = 2;
						xel[nbri2].networkType = 2;
					}
			}
		}
	}
	*/
	
	
	//extracting branch tree out of xel network
	
	//step-0 clear off the old network
	tree.nodeList.clear();
	tree.branchList.clear();
	tree.pendingBranchIndexList.clear();
	
	
	//step-1 finding out the first node
	FOR(i, Np)
	{
		if(xel[i].networkType == 1)
		{
			//adding first branch to tree
			tree.nodeList.push_back(xel[i].I);
						
			int dj = (xel[i].I.y % 2)*6;
			FOR(j, 6)
			{
				TXY<int> I2 = nbrI[j+dj] + xel[i].I;
				if(INRANGE(I2))
					if(xel[to_Ii(I2)].xelType != 0)
					{
						TXelBranch branch;
						branch.startI = xel[i].I;
						branch.startNodeIndex = 0;
						branch.pointList.push_back(xel[i].I);
						branch.pointList.push_back(I2);
						tree.branchList.push_back(branch);
						tree.pendingBranchIndexList.push_back(0);
						break;
					}
			}
			break;
		}
	}
	
	//step 2 finding branches
	int ctr = 0;
	while(tree.pendingBranchIndexList.size() > 0)
	{
		int bI = tree.pendingBranchIndexList[0];	//branch index
		TXY<int> I = tree.branchList[bI].pointList.back();
		
		int branchPointCount = tree.branchList[bI].pointList.size();
		
		TXY<int> prevI = tree.branchList[bI].pointList[branchPointCount-2];			
		
		int i = to_Ii(I);
		int dj = (xel[i].I.y % 2)*6;
		
		if(xel[i].networkType == 1)
		{
			tree.nodeList.push_back(I);
			tree.branchList[bI].endI = I;
			tree.branchList[bI].endNodeIndex = tree.nodeList.size()-1;
			tree.pendingBranchIndexList.erase(tree.pendingBranchIndexList.begin());
			continue;
		}
		else
		if(xel[i].networkType == 3)
		{
			
			int nodeCount = tree.nodeList.size();
			bool oldNodeMatched = false;
			FOR(k, nodeCount)
			{
				if(I == tree.nodeList[k])
				{
					oldNodeMatched = true;
					
					tree.branchList[bI].endI = I;
					tree.branchList[bI].endNodeIndex = k;
					tree.branchList[bI].pointList.push_back(I);
					tree.pendingBranchIndexList.erase(tree.pendingBranchIndexList.begin());
				}
			}
			
			if(!oldNodeMatched)
			{
				tree.nodeList.push_back(I);
				tree.branchList[bI].endI = I;
				tree.branchList[bI].endNodeIndex = tree.nodeList.size()-1;
				tree.pendingBranchIndexList.erase(tree.pendingBranchIndexList.begin());
				
				FOR(j, 6)
				{
					TXY<int> I2 = nbrI[j+dj] + xel[i].I;
					
					int i2 = to_Ii(I2);
					
					if(I2 == prevI)
						continue;
					
					if(INRANGE(I2))
						if(xel[i2].xelType != 0)
						{
							TXelBranch branch;
							branch.startI = I;
							branch.startNodeIndex = tree.nodeList.size()-1;
							branch.pointList.push_back(I);
							branch.pointList.push_back(I2);
							tree.branchList.push_back(branch);
							tree.pendingBranchIndexList.push_back(tree.branchList.size()-1);
						}
				}			
			}
			
			continue;
		}
		else
		if(xel[i].networkType == 2)
		{
			FOR(j, 6)
			{
				TXY<int> I2 = nbrI[j+dj] + xel[i].I;
				int i2 = to_Ii(I2);
				if(INRANGE(I2))
					if(xel[i2].xelType != 0)
					{
						//situations when we don't need to consider a nbr as a advancing chain
						if(branchPointCount == 1)
						{
							int dk = (tree.branchList[bI].startI.y % 2)%6;
							bool nbrMakesDifferentBranch = false;
							FOR(k, 6)
							{
								TXY<int> nodeNbrI = nbrI[k+dk] + tree.branchList[bI].startI;
								if(I2 == nodeNbrI)
								{
									nbrMakesDifferentBranch = true;
									break;
								}
							}
							if(nbrMakesDifferentBranch)	
							{					
								continue;	//we don't need to consider such point
							}
							
						}
						
						if(I2 == prevI)
							continue;
						
						//////////// now the point is to be added
						if(xel[i2].networkType == 2)
						{
							//I2.print();
							tree.branchList[bI].pointList.push_back(I2);
						}
						else
						if(xel[i2].networkType == 1)
						{
							//I2.print();
							tree.nodeList.push_back(I2);
							tree.branchList[bI].endI = I2;
							tree.branchList[bI].endNodeIndex = tree.nodeList.size()-1;
							tree.branchList[bI].pointList.push_back(I2);
							tree.pendingBranchIndexList.erase(tree.pendingBranchIndexList.begin());
						}
						else
						if(xel[i2].networkType == 3)
						{
							int nodeCount = tree.nodeList.size();
							bool oldNodeMatched = false;
							FOR(k, nodeCount)
							{
								if(I2 == tree.nodeList[k])
								{
									oldNodeMatched = true;
									
									tree.branchList[bI].endI = I2;
									tree.branchList[bI].endNodeIndex = k;
									tree.branchList[bI].pointList.push_back(I2);
									tree.pendingBranchIndexList.erase(tree.pendingBranchIndexList.begin());
									
								}
							}
							
							if(!oldNodeMatched)
							{
								tree.nodeList.push_back(I2);
								tree.branchList[bI].endI = I2;
								tree.branchList[bI].endNodeIndex = tree.nodeList.size()-1;
								tree.branchList[bI].pointList.push_back(I2);
								tree.pendingBranchIndexList.erase(tree.pendingBranchIndexList.begin());
								
								int dk = (I2.y % 2)*6;
								FOR(k, 6)
								{
									TXY<int> I3 = nbrI[k+dk] + I2;
									
									if(I3 == I)
										continue;
									
									int i3 = to_Ii(I3);
									
									if(INRANGE(I3))
										if(xel[i3].xelType != 0)
										{
											TXelBranch branch;
											branch.startI = I2;
											branch.startNodeIndex = tree.nodeList.size()-1;
											branch.pointList.push_back(I2);
											branch.pointList.push_back(I3);
											tree.branchList.push_back(branch);
											tree.pendingBranchIndexList.push_back(tree.branchList.size()-1);
										}
								}
							}
						}
					}
			}
			
		}
	}
	
	//cutting off repeated branches
	int branchCount = tree.branchList.size();
	bool *ifBranchRemovable = new bool[branchCount];
	FOR(i, branchCount)
	{
		ifBranchRemovable[i] = false;
		FOR(j, i)
		{
			if( (tree.branchList[i].startI == tree.branchList[j].endI) && (tree.branchList[i].endI == tree.branchList[j].startI))
			{
				ifBranchRemovable[i] = true;
				break;
			}
		}
	}
	
	for(int i=branchCount-1;i>=0;i--)
	{
		if(ifBranchRemovable[i])
			tree.branchList.erase(tree.branchList.begin() + i);
	}
	
	
	
	tree.print();	
}

void removeCrossLinks(TXel2D *xel)
{
	//removal of crosslinks
	int branchCount = tree.branchList.size();
	for(int i=branchCount-1;i>=0;i--)
	{
		float meanY = 0.0;
		int pointCount = tree.branchList[i].pointList.size();
		if(pointCount<5)
			continue;
		for(int j=2;j<pointCount-2;j++)
		{
			TXY<float> p = xel[to_Ii(tree.branchList[i].pointList[j])].p;
			p = p/pitchX;
                        p=p*scale;
			TXY<int> p00(floor(p.x), floor(p.y));
			float y00 = (p00.x>=0 && p00.x<imageWidth && p00.y>=0 && p00.y<=imageHeight)?imgY[p00.y*imageWidth + p00.x]:0;
			float y01 = (p00.x+1>=0 && p00.x+1<imageWidth && p00.y>=0 && p00.y<=imageHeight)?imgY[p00.y*imageWidth + p00.x+1]:0;
			float y10 = (p00.x>=0 && p00.x<imageWidth && p00.y+1>=0 && p00.y+1<=imageHeight)?imgY[(p00.y+1)*imageWidth + p00.x]:0;
			float y11 = (p00.x+1>=0 && p00.x+1<imageWidth && p00.y+1>=0 && p00.y+1<=imageHeight)?imgY[(p00.y+1)*imageWidth + p00.x+1]:0;
			TXY<float> dp(p.x - p00.x, p.y - p00.y);
			meanY += (1-dp.y)*(y00*(1-dp.x) + y01*(dp.x)) + dp.y*(y10*(1-dp.x) + y11*(dp.x));
		}
		meanY /= pointCount;
		if(meanY < crossLinkThreshold)
		{	
			for(int j=1;j<pointCount-1;j++)
				cleanXel(xel, tree.branchList[i].pointList[j]);
			tree.branchList.erase(tree.branchList.begin() + i);
		}
	}
}
////////////////////////////////////////////////////////////////////////
