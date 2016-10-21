/*
 * File: depth.cpp
 *
 * Author: Denis Tananaev
 *
 * Date: 19.10.2016
 */

#include <cstdlib>   /// contains: EXIT_SUCCESS
#include <iostream>  /// contains: std::cout etc.
#include "CMatrix.h"
#include "CVector.h"
#include <vector>
#include <string>
#include <fstream>
#include <NMath.h> //SVD
#include "CTensor.h"
#include <cmath>
#include <algorithm> 

///For colored images
void image2rgb(CTensor<float> image_color, CMatrix<float>& red, CMatrix<float>& green, CMatrix<float>& blue){
	for(long int x=0;x<image_color.xSize();x++)
		for(long int y=0;y<image_color.ySize();y++)
		{
			red(x,y)=image_color(x,y,0);
			green(x,y)=image_color(x,y,1);
			blue(x,y)=image_color(x,y,2);
		}

}

void rgb2image(CTensor<float>& result_color, CMatrix<float> red, CMatrix<float> green, CMatrix<float> blue){
	for(long int x=0;x<result_color.xSize();x++)
		for(long int y=0;y<result_color.ySize();y++)
		{
			result_color(x,y,0)=red(x,y);
			result_color(x,y,1)=green(x,y);
			result_color(x,y,2)=blue(x,y);
		}
}

float patch_distance(  CMatrix<float> img1, CMatrix<float> img2,  
	int x1, int y1, 
	int x2, int y2, 
	int patch_radius )
{
	const int x_size = img1.xSize();
	const int y_size = img1.ySize();

	float ssd = 0;
	for( int ty = -patch_radius; ty <= patch_radius; ++ty )
		for( int tx = -patch_radius; tx <= patch_radius; ++tx )
		{
			// clamp coordinates
			int p1x = std::min(x_size-1,std::max(0,x1+tx));//start and end of patch
			int p1y = std::min(y_size-1,std::max(0,y1+ty));
			int p2x = std::min(x_size-1,std::max(0,x2+tx));
			int p2y = std::min(y_size-1,std::max(0,y2+ty));
			float tmp = img1(p1x,p1y)-img2(p2x,p2y);		
			ssd += tmp*tmp;

		}


		return ssd;
}


CMatrix<float> EuclidDistDepth(CMatrix<float> redL, CMatrix<float> greenL,CMatrix<float> blueL,CMatrix<float> redR, CMatrix<float> greenR, CMatrix<float> blueR, int patch_radius){

    CMatrix<float> result(redL.xSize(), redL.ySize());
    std::vector<double> distance;
      const int x_size = redL.xSize();
	  const int y_size = redL.ySize();


  for(int y =0;y<redL.ySize();y++){
    for(int x =0; x<redL.xSize();x++){

         distance.clear();
  
  //find euclid distance between pixels
        for(int j=0; j<redL.xSize();j++){

   
	     float R=0;
         float G=0;
         float B=0;
	      for( int ty = -patch_radius; ty <= patch_radius; ++ty )
		  for( int tx = -patch_radius; tx <= patch_radius; ++tx )
		   {
			// clamp coordinates
			int p1x = std::min(x_size-1,std::max(0,x+tx));//start and end of patch
			int p1y = std::min(y_size-1,std::max(0,y+ty));
			int p2x = std::min(x_size-1,std::max(0,j+tx));
			int p2y = std::min(y_size-1,std::max(0,y+ty));
			float Rtmp = redL(p1x,p1y)-redR(p2x,p2y);		
			R += Rtmp*Rtmp;

            float Gtmp = greenL(p1x,p1y)-greenR(p2x,p2y);		
			G += Gtmp*Gtmp;

            float Btmp=blueL(p1x,p1y)-blueR(p2x,p2y);	
            B+=Btmp*Btmp;
		  }

         double dist=sqrt( R +G+B  );
        distance.push_back(dist);
    
        }

        int min_index = std::min_element(distance.begin(), distance.end()) - distance.begin();
        result(x,y)=x-min_index;

        //outlier detection 
        if(result(x,y)>20){
            result(x,y)=0;
        
        }else if(result(x,y)<0){
            result(x,y)=0;
        
        }

      std::cout<<" Min distance= "<<result(x,y)<<"\n";
    
        
    }
  }
    return result;
}


CMatrix<float> AbsoluteDistDepth(CMatrix<float> redL, CMatrix<float> greenL,CMatrix<float> blueL,CMatrix<float> redR, CMatrix<float> greenR, CMatrix<float> blueR, int patch_radius){

    CMatrix<float> result(redL.xSize(), redL.ySize());
    std::vector<double> distance;
      const int x_size = redL.xSize();
	  const int y_size = redL.ySize();


  for(int y =0;y<redL.ySize();y++){
    for(int x =0; x<redL.xSize();x++){

         distance.clear();
  
  //find euclid distance between pixels
        for(int j=0; j<redL.xSize();j++){

   
	     float R=0;
         float G=0;
         float B=0;
	      for( int ty = -patch_radius; ty <= patch_radius; ++ty )
		  for( int tx = -patch_radius; tx <= patch_radius; ++tx )
		   {
			// clamp coordinates
			int p1x = std::min(x_size-1,std::max(0,x+tx));//start and end of patch
			int p1y = std::min(y_size-1,std::max(0,y+ty));
			int p2x = std::min(x_size-1,std::max(0,j+tx));
			int p2y = std::min(y_size-1,std::max(0,y+ty));
			float Rtmp = redL(p1x,p1y)-redR(p2x,p2y);		
			R += abs(Rtmp);

            float Gtmp = greenL(p1x,p1y)-greenR(p2x,p2y);		
			G += abs(Gtmp);

            float Btmp=blueL(p1x,p1y)-blueR(p2x,p2y);	
            B+=abs(Btmp);

		  }
     //    float R=patch_distance( redL,redR,x,y, j, y, patch_radius );
    //     float G=patch_distance( greenL,greenR, x,y, j, y, patch_radius );
     //    float B=patch_distance( blueL,blueR, x,y, j, y, patch_radius );

      //      double R= redL(x,y)-redR(j,y);
       //     double G= greenL(x,y)-greenR(j,y);   
       //    double B= blueL(x,y)-blueR(j,y);  

           //  double dist=sqrt( R *R+G*G +B*B  );

         double dist= R +G+B;

        distance.push_back(dist);

    
        }

        int min_index = std::min_element(distance.begin(), distance.end()) - distance.begin();
        result(x,y)=x-min_index;

        //outlier detection 
       if(result(x,y)>20){
            result(x,y)=0;
        
        }else if(result(x,y)<0){
            result(x,y)=0;
       
        }
      std::cout<<" Abs distance= "<<result(x,y)<<"\n";
    
        
    }
  }
    return result;
}

CMatrix<float> CrossEntrophyDepth(CMatrix<float> redL, CMatrix<float> greenL,CMatrix<float> blueL,CMatrix<float> redR, CMatrix<float> greenR, CMatrix<float> blueR, int patch_radius){

    CMatrix<float> result(redL.xSize(), redL.ySize());
    std::vector<double> distance;
      const int x_size = redL.xSize();
	  const int y_size = redL.ySize();


  for(int y =0;y<redL.ySize();y++){
    for(int x =0; x<redL.xSize();x++){

         distance.clear();
  
  //find cross entrophy distance between pixels
        for(int j=0; j<redL.xSize();j++){

         CMatrix<float> RedL(2*patch_radius+1,2*patch_radius+1,0);
         CMatrix<float> GreenL(2*patch_radius+1,2*patch_radius+1,0);
         CMatrix<float> BlueL(2*patch_radius+1,2*patch_radius+1,0);
	     float mRL=0;
         float mGL=0;
         float mBL=0;
         CMatrix<float> RedR(2*patch_radius+1,2*patch_radius+1,0);
         CMatrix<float> GreenR(2*patch_radius+1,2*patch_radius+1,0);
         CMatrix<float> BlueR(2*patch_radius+1,2*patch_radius+1,0);
	     float mRR=0;
         float mGR=0;
         float mBR=0;
	      for( int ty = -patch_radius; ty <= patch_radius; ++ty )
		  for( int tx = -patch_radius; tx <= patch_radius; ++tx )
		   {
			// clamp coordinates
			int p1x = std::min(x_size-1,std::max(0,x+tx));//start and end of patch
			int p1y = std::min(y_size-1,std::max(0,y+ty));
			int p2x = std::min(x_size-1,std::max(0,j+tx));
			int p2y = std::min(y_size-1,std::max(0,y+ty));
            RedL(tx+patch_radius,ty+patch_radius)=redL(p1x,p1y);
            RedR(tx+patch_radius,ty+patch_radius)=redR(p2x,p2y);
            mRL+=redL(p1x,p1y);
            mRR+=redR(p2x,p2y); 

 
            GreenL(tx+patch_radius,ty+patch_radius)=greenL(p1x,p1y);
            GreenR(tx+patch_radius,ty+patch_radius)=greenR(p2x,p2y);
            mGL+=greenL(p1x,p1y);
            mGR+=greenR(p2x,p2y);    

            BlueL(tx+patch_radius,ty+patch_radius)=blueL(p1x,p1y);
            BlueR(tx+patch_radius,ty+patch_radius)=blueR(p2x,p2y);
            mBL+=blueL(p1x,p1y);
            mBR+=blueR(p2x,p2y);      

		  }

        mRL=mRL/(2*patch_radius+1);
        mRR=mRR/(2*patch_radius+1);

        mGL=mGL/(2*patch_radius+1);
        mGR=mGR/(2*patch_radius+1);
        mBL=mBL/(2*patch_radius+1);
        mBR=mBR/(2*patch_radius+1);
        double A=0;
        double B=0;
        double C=0;       
         for( int a =0; a < 2*patch_radius+1; ++a )
		  for( int b = 0; b <2* patch_radius+1; ++b )
		   {       
                double first=  ( RedL(a,b)-mRL  +  GreenL(a,b)-mGL + BlueL(a,b)-mBL );
                double second=    ( RedR(a,b)-mRR  +  GreenR(a,b)-mGR + BlueR(a,b)-mBR );       

                A+=  first *second;
                B+= first*first;
                C+=second*second;

            }
         double dist= A/ sqrt(B*C);
       // std::cout<<"Entrophy "<<dist<<"\n";
        distance.push_back(dist);

    
        }

        int max_index = std::max_element(distance.begin(), distance.end()) - distance.begin();
        result(x,y)=x-max_index;

        //outlier detection 
       if(result(x,y)>20){
            result(x,y)=0;
        
        }else if(result(x,y)<0){
            result(x,y)=0;
       
        }

        std::cout<<"Entrophy "<<distance[max_index]<<"\n";
      std::cout<<" Abs distance= "<<result(x,y)<<"\n";
    
        
    }
  }
    return result;
}


int main(){
  
    CTensor<float> imageL;

	imageL.readFromPPM("tsukubaL.ppm");


	CMatrix<float> redL(imageL.xSize(),imageL.ySize());
	CMatrix<float> greenL(imageL.xSize(),imageL.ySize());
	CMatrix<float> blueL(imageL.xSize(),imageL.ySize());
	image2rgb(imageL,redL, greenL, blueL);



    CTensor<float> imageR;

	imageR.readFromPPM("tsukubaR.ppm");


	CMatrix<float> redR(imageR.xSize(),imageR.ySize());
	CMatrix<float> greenR(imageR.xSize(),imageR.ySize());
	CMatrix<float> blueR(imageR.xSize(),imageR.ySize());
	image2rgb(imageR,redR, greenR, blueR);

    int patch_radius=10;
   // CMatrix<float> result=pixelwiseEuclidDist( redL,  greenL, blueL, redR,  greenR, blueR,patch_radius);
  //  CMatrix<float> absresult=AbsoluteDistDepth(redL,  greenL, blueL, redR,  greenR, blueR,patch_radius);
      CMatrix<float> entrophyresult=CrossEntrophyDepth(redL,  greenL, blueL, redR,  greenR, blueR,patch_radius);
   // result.normalize(0,255);
    //result.writeToPGM("result.pgm");
   // absresult.normalize(0,255);
   // absresult.writeToPGM("absresult.pgm");
    entrophyresult.normalize(0,255);
   entrophyresult.writeToPGM("entrophyresult.pgm");
}



