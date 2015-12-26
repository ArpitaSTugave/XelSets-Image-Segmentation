#include "xeltree2d.h"

////////// printing tree data into a file
void TXelTree::print()
{
	ofstream treeFile("tree.txt", ios::out | ios::trunc);
	
	//writing nodes
	treeFile<<"No. of Nodes : "<<nodeList.size()<<endl;
	FOR(i, nodeList.size())
	{
		treeFile<<"("<<nodeList[i].x<<", "<<nodeList[i].y<<"), ";
	}
	treeFile<<endl<<endl<<endl;
	
	treeFile<<"No. of Branches : "<<branchList.size()<<endl<<endl;
	FOR(i, branchList.size())
	{
		treeFile<<"Branch Index : " <<i<<endl;
		treeFile<<"Starting from : ("<<branchList[i].startI.x<<", "<<branchList[i].startI.y<<")\t nodeIndex : "<<branchList[i].startNodeIndex<<"\n";
		treeFile<<"Ending at	 : ("<<branchList[i].endI.x  <<", "<<branchList[i].endI.y  <<")\t nodeIndex : "<<branchList[i].endNodeIndex<<"\n";
		treeFile<<"No. of points : "<<branchList[i].pointList.size()<<endl;
		FOR(j, branchList[i].pointList.size())
		{
			treeFile<<"("<<branchList[i].pointList[j].x<<", "<<branchList[i].pointList[j].y<<"), ";
		}
		
		treeFile<<endl<<endl;
	}
	treeFile<<endl<<endl;
	
	
	treeFile.close();
}
