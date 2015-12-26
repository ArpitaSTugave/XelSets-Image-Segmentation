#include "input2d.h"
using namespace std;
using namespace cv;
void loadImage1()
{
        Mat image = imread(inputFile,CV_LOAD_IMAGE_GRAYSCALE);
	//dimensional init
	imageWidth = image.cols;//200
	imageHeight = image.rows;//150
	long pixelCount = imageWidth*imageHeight;
	blurimg1 = new float*[blurCount];
        for(int i=0;i<blurCount;i++)
        blurimg1[i]= new float [pixelCount];
	imgY = new float [pixelCount];
	int count=0;
        FOR(j, imageHeight)
        {
        FOR(k,imageWidth)
        {       count=count+1;
                Scalar intensity = image.at<uchar>(j, k);
		imgY[count] = intensity.val[0]/255;
                imgY[count] = 1 - imgY[count];
        }
        }	
        //buffer grey images for various blur count used in unihandler2d.cpp			
	FOR(i,blurCount)
        {
        imgY=blurY1(imgY, imageWidth, imageHeight, 1);
           FOR(count,pixelCount)
              {
              blurimg1[i][count] = imgY[count];
              }
        }
	//calculating xel grid size
	Nx = imageWidth/scale;
	Ny = (imageHeight*pitchX/pitchY)/scale;
	Np = Nx*Ny;

}

void varyblur1(float  *imgY,TXel2D *xel,int blurCount )
{
                                          FOR(i, Np)
	                                          {
		                                      TXY<float> p = xel[i].P;
		                                      p = p/pitchX;
                                                      p=p*scale;
                TXY<int> p00(floor(p.x), floor(p.y));
	        float y00 = (p00.x>=0 && p00.x<imageWidth && p00.y>=0 && p00.y<=imageHeight)?imgY[p00.y*imageWidth + p00.x]:0;
		float y01 = (p00.x+1>=0 && p00.x+1<imageWidth && p00.y>=0 && p00.y<=imageHeight)?imgY[p00.y*imageWidth + p00.x+1]:0;
		float y10 = (p00.x>=0 && p00.x<imageWidth && p00.y+1>=0 && p00.y+1<=imageHeight)?imgY[(p00.y+1)*imageWidth + p00.x]:0;
		float y11 = (p00.x+1>=0 && p00.x+1<imageWidth && p00.y+1>=0 && p00.y+1<=imageHeight)?imgY[(p00.y+1)*imageWidth + p00.x+1]:0;
		TXY<float> dp(p.x - p00.x, p.y - p00.y);
		xel[i].Y = (1-dp.y)*(y00*(1-dp.x) + y01*(dp.x)) + dp.y*(y10*(1-dp.x) + y11*(dp.x)); 	                                      
//bi-linear transformation
                                               }
                                
                                imageForceScale = ((xelSize * 10-xelSize)*blurCount)/10;
}

void resampleImage1(TXel2D *xel)
{
	//Sampling image on xel-space
	//space between pixels (pixel-pitch) is assumed to be equal to pitchX of xel-space
	FOR(i, Np)
	{
		TXY<float> p = xel[i].P;
		p = p/pitchX;
                p=p*scale;
		TXY<int> p00(floor(p.x), floor(p.y));
		float y00 = (p00.x>=0 && p00.x<imageWidth && p00.y>=0 && p00.y<=imageHeight)?imgY[p00.y*imageWidth + p00.x]:0;
		float y01 = (p00.x+1>=0 && p00.x+1<imageWidth && p00.y>=0 && p00.y<=imageHeight)?imgY[p00.y*imageWidth + p00.x+1]:0;
		float y10 = (p00.x>=0 && p00.x<imageWidth && p00.y+1>=0 && p00.y+1<=imageHeight)?imgY[(p00.y+1)*imageWidth + p00.x]:0;
		float y11 = (p00.x+1>=0 && p00.x+1<imageWidth && p00.y+1>=0 && p00.y+1<=imageHeight)?imgY[(p00.y+1)*imageWidth + p00.x+1]:0;
		TXY<float> dp(p.x - p00.x, p.y - p00.y);
		xel[i].Y = (1-dp.y)*(y00*(1-dp.x) + y01*(dp.x)) + dp.y*(y10*(1-dp.x) + y11*(dp.x)); //bi-linear transformation
                if(xel[i].Y > imageThreshold) 
                addMidPointToXelSet(xel, xel[i].I, 2);
                
	}
        //initialization for our bottoms
        int count=0;
        float arr[Nx][Ny];
        memset(arr,0,sizeof(arr));
        FOR(j,Ny){
             FOR(k,Nx){  
                 arr[k][j]=xel[count].Y;
                 if((k>1 && k <Nx) && (j>1 && j <Ny)){
                 if((arr[k-2][j-2] > arr[k-1][j-1]) && (arr[k-2][j-1] > arr[k-1][j-1]) && (arr[k-2][j] > arr[k-1][j-1]) && (arr[k-1][j] > arr [k-1][j-1]) && (arr[k][j] > arr[k-1][j-1]) && (arr[k][j-1] > arr[k-1][j-1]) && (arr[k][j-2] > arr[k-1][j-1]) && (arr[k-1][j-2] > arr[k-1][j-1]))
                 {
                    //display bottoms and cleaning up the neighbors
                    cout <<arr[k-1][j-1]<<endl;
                    cleanXel(xel, xel[count+2].I);
                    cleanXel(xel, xel[count+1].I);
                    cleanXel(xel, xel[count].I);
                    cleanXel(xel, xel[count-1].I);
                    cleanXel(xel, xel[count-2].I);
                   }
            }
            count=count+1;
       }
    }
}


	

//blurs image n times.
float* blurY1(float *Y, int W, int H, int n)
{
	//convolution kernel
	float f[7] = {1.0/1262.0, 30.0/1262.0, 300.0/1262.0, 600/1262.0, 
	300.0/1262.0, 30.0/1262.0, 1.0/1262.0};	

	float *yTemp = new float[W*H];

	FOR(i, n)
	{
		FOR(i, W*H)
			yTemp[i] = Y[i];

		//convolution
		FOR(y, H)
		FOR(x, W)
		{
			int i = y*W+x;
			Y[i] = 0.0;
			
			for(int dy=-3; dy<=3; dy++)
				for(int dx=-3; dx<=3; dx++)
				{
					int y2 = y+dy;
					int x2 = x+dx;
					if(x2>=0 && x2<W && y2>=0 && y2<H)
					{
						int i2 = y2*W + x2;
						Y[i] += yTemp[i2]*f[dy+3]*f[dx+3];
					}
				}
		}
	}
return Y;
}

