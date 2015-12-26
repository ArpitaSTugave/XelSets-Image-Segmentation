#include "uihandler2d.h"
#include "xel2d.h"
#include "param2d.h"
#include "input2d.h"
#include "opencv.hpp"
#include<iostream>
using namespace std;
using namespace cv;


//main
int main()
{
	//create instance of TXel2D
	TXel2D *xel;
        char imgname[10];
        cout<<"enter scale \n";
        cin>>scale;
        cout<<"enter image name \n";
        cin>>imgname;
///////////////////////////////////////////////pre-processing///////////////////////////////////////////////////////////////////////
        //read source image 
        //split it to rgb channels
        Mat dst,im,bgr[3],b,g,r,im_gray,image1,image2;
        Rect rect;
        int m=0,n=0,i=0,j=0,th=15,width=0,height=0;
        double minVal,maxVal,rmin,rmax,bmin,bmax,gmin,gmax,p1[2]={0,1000},r1[2]={(3000/sqrt(3)),1000},q1[2]={(3000/(2*sqrt(3))),(3000/6)},s1[2]={0,2000},t1[2]={(3000/(2*sqrt(3))),((5*3000)/6)},point1[2],point2[2],point3[2],h=0.5;  
        //points to cut mire at half its edges
        point1[0]=((h*q1[0]) + ((1-h)*p1[0]));
        point1[1]=((h*q1[1]) + ((1-h)*p1[1]));
        point2[0]=((h*q1[0]) + ((1-h)*r1[0]));
        point2[1]=((h*q1[1]) + ((1-h)*r1[1]));
        point3[0]=((h*t1[0]) + ((1-h)*s1[0]));
        point3[1]=((h*t1[1]) + ((1-h)*s1[1]));
        width=((point2[0])-(point1[0]));
        height=((point3[1])-(point1[1]));
        Point minLoc,maxLoc; 
        //read source image and reduce it's size to 1000x1000 for easy processing 
        //split it to rgb channels
        Mat image = imread(imgname,CV_LOAD_IMAGE_COLOR); 
        m   = image.rows; n   = image.cols;
        split(image,bgr);
        b=bgr[0];
        g=bgr[1];
        r=bgr[2];
        //get initial segmented image
        for (i=0;i<m;i=i+25){
           for(j=0;j<n;j=j+25){
                   Point x(i,j);
                   Point y(i+25,j+25);
                   rect = Rect(x,y);
                   im = r(rect);
                   minMaxLoc( im, &minVal, &maxVal, &minLoc, &maxLoc );
                   rmin=minVal;
                   rmax=maxVal;
                   rect = Rect(x,y);
                   im = g(rect);
                   minMaxLoc( im, &minVal, &maxVal, &minLoc, &maxLoc );
                   gmin=minVal;
                   gmax=maxVal;
                   rect = Rect(x,y);
                   im = b(rect);
                   minMaxLoc( im, &minVal, &maxVal, &minLoc, &maxLoc );
                   bmin=minVal;
                   bmax=maxVal;
                   if( ((rmax-rmin)>=th) || ((gmax-gmin)>=th) || ((bmax-bmin)>=th) )
                      {
                      image(rect).setTo(cv::Scalar::all(0));
                      }
             }
       }
        for (i=0;i<m;i=i+1){
                 for(j=0;j<n;j=j+1){
                      Point x(i,j);
                      Point y(i+1,j+1);
                      rect = Rect(x,y);
                      if( (j < (point1[1]+20)) || (j > (point3[1]-20)) || (i<(point1[0]+20)) || (i>(point2[0]-20)))
                      {
                         image(rect).setTo(cv::Scalar::all(255));
                      }
              }
       }
          //convert the image into grayscale to convert it into bw resize it to 150x200
          cvtColor(image,im_gray,CV_RGB2GRAY);
          Mat img_bw = im_gray > 0;
          resize(img_bw, dst, Size(150, 150), 0, 0, INTER_CUBIC); 
          imshow("opencvtest",dst);
          waitKey(5000);
          imwrite( "opencvtest.bmp", dst );
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//initialize xel grid
		loadImage1();
		xel = new TXel2D[Np];
		resampleImage1(xel);
		
	
	//intialize openGL interactive session
	initUI(xel);
      

}
////////////////////////////////////////////////////////////////////////
