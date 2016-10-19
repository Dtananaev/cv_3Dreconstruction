/*
 * File: epipol.cpp
 *
 * Author: Denis Tananaev
 *
 * Date: 17.10.2016
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

typedef struct{
    int x;
    int y;
}point;

bool read_projMatrix(std::string filename, CMatrix<float> &pMatrix){


    pMatrix.setSize(3,4);

     std::ifstream file_stream_pose(filename);
    if (file_stream_pose.is_open()) { // check if file exsist
         std::string line;
         while (std::getline(file_stream_pose, line)) {
            std::istringstream file_stream_pose(line);
            
            float a11,a12,a13,a14,a21,a22,a23,a24,a31,a32,a33,a34;
            file_stream_pose>>a11>>a12>>a13>>a14>>a21>>a22>>a23>>a24>>a31>>a32>>a33>>a34;
            pMatrix(0,0)=a11; pMatrix(0,1)=a12; pMatrix(0,2)=a13; pMatrix(0,3)=a14;
            pMatrix(1,0)=a21; pMatrix(1,1)=a22; pMatrix(1,2)=a23; pMatrix(1,3)=a24;   
            pMatrix(2,0)=a31; pMatrix(2,1)=a32; pMatrix(2,2)=a33; pMatrix(2,3)=a34;    

         }
         file_stream_pose.close();
    }else{
       return false;
    }

    return true;
}

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
void calculateEpipolarProjection(int x, int y,CMatrix<float> F,CMatrix<float> &l){

    l(0,0)=F(0,0)*x + F(0,1)*y+F(0,2);
    l(1,0)=F(1,0)*x + F(1,1)*y+F(1,2);
    l(2,0)=F(2,0)*x + F(2,1)*y+F(2,2);
}

void drawEpipol( CMatrix<float> line, CMatrix<float>& red, CMatrix<float> & green, CMatrix<float>& blue ){
    std::vector<point> result;
    //ax+by+c=0   
    for(int x=0; x<red.xSize(); x++){
        float y = (-line(2,0)-line(0,0)*x)/line(1,0);
        point temp;
        temp.x=x;
        temp.y=y;
      //  std::cout<<"x "<<x<<"\n";
      //  std::cout<<"y "<<y<<"\n";
        result.push_back(temp);
    }

    for(unsigned int i=0; i<result.size();i++){

    if(result[i].y<red.ySize() &&result[i].y>=0){
    red(result[i].x, result[i].y)=0;
    green(result[i].x, result[i].y)=0;
    blue(result[i].x,result[i].y)=0;
 }
}



}
//int main(int argc, char** argv) {  
int main(){

    //read projection matrices of two camera

    CMatrix<float> p1(3,4);
    CMatrix<float> p2(3,4);



    if (!read_projMatrix( "proj_matrix00.dat", p1)){
        std::cout<<"Can't read proj_matrix00.dat"<<"\n";
    }

   
    if (!read_projMatrix( "proj_matrix01.dat", p2)){
        std::cout<<"Can't read proj_matrix01.dat"<<"\n";
    }

    //finding the camera center of the first camera



   CMatrix<float> U=p1; //should be 4x3
   CMatrix<float> S(3,3); 
   CMatrix<float> V(3,3);
   NMath::svd(U, S, V, true);

    //camera center
   CMatrix<float> C(4,1,1);

    C(0,0)=V(3,0);
    C(1,0)=V(3,1);   
    C(2,0)=V(3,2); 
   // C(3,0)=V(3,3); 
    std::cout<<"C(0,0)= "<<C(0,0)<<"\n";
    std::cout<<"C(1,0)= "<<C(1,0)<<"\n";
    std::cout<<"C(2,0)= "<<C(2,0)<<"\n";
    std::cout<<"C(3,0)= "<<C(3,0)<<"\n";
    //epipole in second camera
    CMatrix<float> e(3,1);

    
    e(0,0)= p2(0,0)*C(0,0)+p2(0,1)*C(1,0)+p2(0,2)*C(2,0)+p2(0,3)*C(3,0);    
    e(1,0)= p2(1,0)*C(0,0)+p2(1,1)*C(1,0)+p2(1,2)*C(2,0)+p2(1,3)*C(3,0); 
    e(2,0)= p2(2,0)*C(0,0)+p2(2,1)*C(1,0)+p2(2,2)*C(2,0)+p2(2,3)*C(3,0); 
    std::cout<<"e(0,0)= "<<e(0,0)<<"\n";
    std::cout<<"e(1,0)= "<<e(1,0)<<"\n";
    std::cout<<"e(2,0)= "<<e(2,0)<<"\n";

    //Create skew matrix

    CMatrix<float> skewE(3,3,0);

    skewE(0,1)=-e(2,0);  skewE(0,2)=e(1,0);
    skewE(1,0)=e(2,0);  skewE(1,2)=-e(0,0);
    skewE(2,0)=-e(1,0);  skewE(2,1)=e(0,0);

    //calculate pseudo-inverse of the first projection matrix

    S.inv();
    NMath::svdBack(U,S,V);

    //Calculate fundamental matrix
    CMatrix<float> F(3,3);
      CMatrix<float> temp(3,4);  

    temp(0,0)=  skewE(0,0)*p2(0,0)+skewE(0,1)*p2(1,0)+skewE(0,2)*p2(2,0);
    temp(0,1)=  skewE(0,0)*p2(0,1)+skewE(0,1)*p2(1,1)+skewE(0,2)*p2(2,1);
    temp(0,2)=  skewE(0,0)*p2(0,2)+skewE(0,1)*p2(1,2)+skewE(0,2)*p2(2,2);
    temp(0,3)=  skewE(0,0)*p2(0,3)+skewE(0,1)*p2(1,3)+skewE(0,2)*p2(2,3);

    temp(1,0)=  skewE(1,0)*p2(0,0)+skewE(1,1)*p2(1,0)+skewE(1,2)*p2(2,0);
    temp(1,1)=  skewE(1,0)*p2(0,1)+skewE(1,1)*p2(1,1)+skewE(1,2)*p2(2,1);
    temp(1,2)=  skewE(1,0)*p2(0,2)+skewE(1,1)*p2(1,2)+skewE(1,2)*p2(2,2);
    temp(1,3)=  skewE(1,0)*p2(0,3)+skewE(1,1)*p2(1,3)+skewE(1,2)*p2(2,3);

    temp(2,0)=  skewE(2,0)*p2(0,0)+skewE(2,1)*p2(1,0)+skewE(2,2)*p2(2,0);
    temp(2,1)=  skewE(2,0)*p2(0,1)+skewE(2,1)*p2(1,1)+skewE(2,2)*p2(2,1);
    temp(2,2)=  skewE(2,0)*p2(0,2)+skewE(2,1)*p2(1,2)+skewE(2,2)*p2(2,2);
    temp(2,3)=  skewE(2,0)*p2(0,3)+skewE(2,1)*p2(1,3)+skewE(2,2)*p2(2,3);



    F(0,0)=  temp(0,0)*U(0,0)+temp(0,1)*U(0,1)+temp(0,2)*U(0,2)+temp(0,3)*U(0,3);
    F(0,1)=  temp(1,0)*U(0,0)+temp(1,1)*U(0,1)+temp(1,2)*U(0,2)+temp(1,3)*U(0,3);
    F(0,2)=  temp(2,0)*U(0,0)+temp(2,1)*U(0,1)+temp(2,2)*U(0,2)+temp(2,3)*U(0,3);


    F(1,0)=  temp(0,0)*U(1,0)+temp(0,1)*U(1,1)+temp(0,2)*U(1,2)+temp(0,3)*U(1,3);
    F(1,1)=  temp(1,0)*U(1,0)+temp(1,1)*U(1,1)+temp(1,2)*U(1,2)+temp(1,3)*U(1,3);
    F(1,2)=  temp(2,0)*U(1,0)+temp(2,1)*U(1,1)+temp(2,2)*U(1,2)+temp(2,3)*U(1,3);

    F(2,0)=  temp(0,0)*U(2,0)+temp(0,1)*U(2,1)+temp(0,2)*U(2,2)+temp(0,3)*U(2,3);
    F(2,1)=  temp(1,0)*U(2,0)+temp(1,1)*U(2,1)+temp(1,2)*U(2,2)+temp(1,3)*U(2,3);
    F(2,2)=  temp(2,0)*U(2,0)+temp(2,1)*U(2,1)+temp(2,2)*U(2,2)+temp(2,3)*U(2,3);


    std::cout<<"F(0,0)= "<<F(0,0)<<"\n";
    std::cout<<"F(0,1)= "<<F(0,1)<<"\n";
    std::cout<<"F(0,2)= "<<F(0,2)<<"\n";

    std::cout<<"F(1,0)= "<<F(1,0)<<"\n";
    std::cout<<"F(1,1)= "<<F(1,1)<<"\n";
    std::cout<<"F(1,2)= "<<F(1,2)<<"\n";

    std::cout<<"F(2,0)= "<<F(2,0)<<"\n";
    std::cout<<"F(2,1)= "<<F(2,1)<<"\n";
    std::cout<<"F(2,2)= "<<F(2,2)<<"\n";
/*
CMatrix<float> l(3,1);

    l(0,0)=F(0,0)*300 + F(0,1)*100+F(0,2);
    l(1,0)=F(1,0)*300 + F(1,1)*100+F(1,2);
    l(2,0)=F(2,0)*300 + F(2,1)*100+F(2,2);

 std::cout<<"l(0,0)= "<<l(0,0)<<"\n";
 std::cout<<"l(1,0)= "<<l(1,0)<<"\n";
 std::cout<<"l(2,0)= "<<l(2,0)<<"\n";
*/


    CTensor<float> image_color;
	image_color.readFromPPM("Right.ppm");


	CMatrix<float> red(image_color.xSize(),image_color.ySize());
	CMatrix<float> green(image_color.xSize(),image_color.ySize());
	CMatrix<float> blue(image_color.xSize(),image_color.ySize());

    CTensor<float> result_color(image_color.xSize(),image_color.ySize(),image_color.zSize(),0);

	image2rgb(image_color,red, green, blue);
  
    //point on the first image
    int x =100;
    int y = 300;
    CMatrix<float> l(3,1);
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red, green,  blue );

    //second point
    x=10;
    y=100;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red, green,  blue );    

    //third point
    x=200;
    y=100;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red, green,  blue );    


    //fourth point
    x=100;
    y=100;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red, green,  blue ); 

    
    //fifth point
    x=300;
    y=300;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red, green,  blue ); 


	rgb2image(result_color,red, green, blue);    

	result_color.writeToPPM("Right_epipol.ppm");




   CTensor<float> image_color2;
	image_color2.readFromPPM("Left.ppm");


	CMatrix<float> red2(image_color2.xSize(),image_color2.ySize());
	CMatrix<float> green2(image_color2.xSize(),image_color2.ySize());
	CMatrix<float> blue2(image_color.xSize(),image_color2.ySize());


	CTensor<float> result_color2(image_color2.xSize(),image_color2.ySize(),image_color2.zSize(),0);

	image2rgb(image_color2,red2, green2, blue2);
    
    //point on the first image
     x =100;
     y = 300;
    F.trans();
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red2, green2,  blue2 );

    //second point
    x=10;
    y=100;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red2, green2,  blue2 );  

    //third point
    x=200;
    y=100;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red2, green2,  blue2 );


    //fourth point
    x=100;
    y=100;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red2, green2,  blue2 );

    
    //fifth point
    x=300;
    y=300;
    calculateEpipolarProjection( x, y, F,l);
    drawEpipol( l, red2, green2,  blue2 );
	rgb2image(result_color2,red2, green2, blue2);

 
	result_color2.writeToPPM("Left_epipol.ppm");



}

