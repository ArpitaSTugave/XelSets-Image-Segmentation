#include "uihandler2d.h"
using namespace std;
using namespace cv;
////////////////////////////////////////////////////////////////////////
//global variables
int offsetX, offsetY;
int windowX, windowY;
TXel2D *xel;
TXY<float> xelHex[6];
#define RADIAN *3.14159/180.0

//flags that allows mouse interaction
bool drawNetworkData = false;
bool processFlag = false;
bool blurFlag=false;
bool extractFlag=false;
////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////
void initUI(TXel2D *xel)
{
	//keeping a global copy of structure
	::xel = xel;
		
	//dummy arguments
	int argc = 0;
	char** argv;
	
	//dimensional parameters
	offsetX = minorRadius + 10;
	offsetY = majorRadius + 10;
	windowX = (Nx - 1)*pitchX + offsetX*2.0 + minorRadius;
	windowY = (Ny - 1)*pitchY + offsetY*2.0;
	
	//initiating hexagonal sides
	FOR(i, 6)
		{
			xelHex[i].set(xelSize*cos((30 + 60*i) RADIAN), xelSize*sin((30 + 60*i) RADIAN));
		}
	
	//glut init
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowX,windowY);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Xel2D");
    
    //projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowX, windowY, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    
    //Setting Default Parameters
	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
   
	//assigning handler functions
    glutDisplayFunc(drawHandler);
    glutKeyboardFunc(keyHandler);
    glutIdleFunc(process);// can perform background processing tasks or continuous animation when window system events are not being received
    
    //initiate gl main loop
    glutMainLoop();

}

void drawHandler()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    fillGrid();
    drawGrid();
	
	drawPoints();
    
    glutSwapBuffers();
}

void keyHandler(unsigned char key, int x, int y)
{
	if((int)key == 27)
		exit(0);

	
	switch(key)
	{
		
	case('p'):
		{
			if(!drawNetworkData && !blurFlag && !extractFlag)
			{
				cout<<((processFlag)?"Process Ended":"Process Started")<<endl;
				processFlag = !processFlag;
			}
		break;
		}
        case('b'):
                { 
                        if(!processFlag && !drawNetworkData && !extractFlag)
                        {
				if(!blurFlag)
				{
                                    if(blurCount>1)    //we get new gray values for new blurCount
                                          blurCount=blurCount-1;
                                    else 
                                          blurCount=10;
                                          
                                          cout<<blurCount<<endl;
                                          imgY=blurimg1[blurCount-1];
                                          //we don't mess with p00 as it is used again in code as it represents finial points
                                          varyblur1(imgY,xel,blurCount );
                                }
                        
				blurFlag = !blurFlag;
				glutPostRedisplay();
                        }
                  break;
                }
	case('e'):
		{
			if(!processFlag && !blurFlag && !drawNetworkData)
			{
				if(!extractFlag)
				{
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
int N=34*(150/scale);//3/2*Ny
int M=35*(150/scale);//sqrt(3)*((2*Nx+1)/2);
cout<<Nx<<"\t"<<Ny<<"\t"<<N<<endl;
Mat dst;
       Mat imag(N,M, CV_8UC3, Scalar(0,0,0));  
int count=0;
TXY<float> q;
       FOR(j,N){
             FOR(k,M){ 
                      q.set(k,j);
                      TXY<int> I=getXelIndex( q);
                      int i = to_Ii(I);
//cout<<k<<"\t"<<j<<"\t"<<i<<endl;
                      if(xel[i].xelType!=0)
		         {        count=count+1;
                                  Point x1(k,j);
                                  Point y1(k+1,j+1);
                                  Rect rect;
                                  rect = Rect(x1,y1);
                                  imag(rect).setTo(cv::Scalar::all(255));
                         }
                      } 
                 }
cout<<"\n"<<count<<endl;
          resize(imag, dst, Size(500, 500), 0, 0, INTER_CUBIC); 
          imshow("opencvtest",dst);
          waitKey(5000);
          imwrite( "segment.bmp", dst );
/////////////////////////////////////////////////////////////////////////////////////////////////////////////	
				}
				extractFlag = !extractFlag;
				glutPostRedisplay();
			}
                 break;
		}
        
               
	case('n'):
		{
			if(!processFlag && !blurFlag && !extractFlag)
			{
				if(!drawNetworkData)
				{
						extractNetwork(xel);
						removeCrossLinks(xel);
						extractNetwork(xel);
				}
				drawNetworkData = !drawNetworkData;
				glutPostRedisplay();
			}
		}
        
               
	}
}

void reshape(int w, int h)
{
	
}

void process()
{
	if(processFlag)
	{       
		cpuRun(xel);
		glutPostRedisplay();
	}
}

void drawGrid()
{
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	
	FOR(i, Np)
	{
		float color = 0.50;
		glColor3f(color, color, color);
		glBegin(GL_POLYGON);
		FOR(j,6)
		{
			TXY<float> p = xelHex[j] + xel[i].P;
			glVertex2f(p.x + offsetX, p.y + offsetY);
		}		
		glEnd();
	}
}

void fillGrid()
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	
	FOR(i, Np)
	{
		float color = xel[i].Y * 0.75;
		glColor3f(color, color, color);
		glBegin(GL_POLYGON);
		FOR(j,6)
		{
			TXY<float> p = xelHex[j] + xel[i].P;
			glVertex2f(p.x + offsetX, p.y + offsetY);
		}		
		glEnd();
	}
}

void drawPoints()
{
	glPointSize(2.0);
	FOR(i,Np)
	{
		if(!drawNetworkData && !blurFlag && !extractFlag)
		{ //0 = empty, 1 = static, 2 = mobile
			if(xel[i].xelType == 0)
				continue;
			else
			if(xel[i].xelType == 2)
				glColor3f(1.0, 0.0, 0.0);
				
			glBegin(GL_POINTS);
			glVertex2f(xel[i].p.x + offsetX, xel[i].p.y + offsetY);
			glEnd();
		}
                else if(!drawNetworkData && !processFlag && !extractFlag)
    		{ 
			if(xel[i].xelType == 0)
				continue;
			else
			if(xel[i].xelType == 2)
				glColor3f(1.0, 1.0, 0.0);
				
			glBegin(GL_POINTS);
			glVertex2f(xel[i].p.x + offsetX, xel[i].p.y + offsetY);
			glEnd();
		}    
                else if(!drawNetworkData && !processFlag && !blurFlag)
    		{
			if(xel[i].xelType == 0)
				continue;
			else
			if(xel[i].xelType == 2)
				glColor3f(0.0, 1.0, 1.0);
				
			glBegin(GL_POINTS);
			glVertex2f(xel[i].p.x + offsetX, xel[i].p.y + offsetY);
			glEnd();
		}           
                else
		{
			bool drawLoops = false;
			bool drawLines = false;
			if(xel[i].networkType == -1)
				continue;
			else
			if(xel[i].networkType == 0)
			{
				drawLoops = true;
			}
			else
			if(xel[i].networkType == 1)
			{
				drawLoops = true;
				drawLines = true;
			}
			else
			if(xel[i].networkType == 2)
			{
				drawLines = true;
			}
			else
			if(xel[i].networkType == 3)
			{
				drawLoops = true;
				drawLines = true;
			}
				
			if(drawLoops)
			{
				glColor3f(0.0, 1.0, 0.0);
				glBegin(GL_LINE_LOOP);
					FOR(an, 90)
						glVertex2f(xel[i].p.x + offsetX + 5*cos(an*4 RADIAN), xel[i].p.y + offsetY + 5*sin(an*4 RADIAN));
				glEnd();
			}
			if(drawLines)
			{
				glColor3f(0.0, 0.0, 1.0);
				int dj = (xel[i].I.y % 2)*6;
	
				FOR(j, 6)
				{
					TXY<int> I2 = nbrI[j+dj] + xel[i].I;
					if(INRANGE(I2))
						if(xel[to_Ii(I2)].xelType != 0)
						{
							glBegin(GL_LINES);
							glVertex2f(xel[i].p.x + offsetX, xel[i].p.y + offsetY);
							glVertex2f(xel[to_Ii(I2)].p.x + offsetX, xel[to_Ii(I2)].p.y + offsetY);
							glEnd();
						}
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////
		
