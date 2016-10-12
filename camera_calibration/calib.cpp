/*
 * File: calib.cpp
 *
 * Author: Denis Tananaev
 *
 * Date: 09.10.2016
 */
 

#include <cstdlib>   /// contains: EXIT_SUCCESS
#include <iostream>  /// contains: std::cout etc.
#include "CMatrix.h"
#include "CTensor.h"
#include <vector>
#include <string>
#include <fstream>
#include <NMath.h> //SVD
typedef struct{
    float x;
    float y;
}point;


bool read_file(std::string filename, std::vector<point> &output){

     std::ifstream file_stream_pose(filename);
    if (file_stream_pose.is_open()) { // check if file exsist
         std::string line;
         while (std::getline(file_stream_pose, line)) {
            std::istringstream file_stream_pose(line);

            //each row of txt file contain 4 points related to the corners of the black square
            point p1;
            point p2;            
            point p3;   
            point p4; 
             
           file_stream_pose>>p1.x>>p1.y>>p2.x>>p2.y>>p3.x>>p3.y>>p4.x>>p4.y;
           output.push_back(p1);
           output.push_back(p2);
           output.push_back(p3);
           output.push_back(p4);
         }
         file_stream_pose.close();
    }else{
       return false;
    }

    return true;
}
/*
//Create matrix A
CMatrix<float> createMatrix(  std::vector<point> model , std::vector<point> data ){

CMatrix<float> A(2*model.size(), 9);

    //std::cout<<"model size "<<model.size()<<"\n";
    //std::cout<<"data size "<<data.size()<<"\n";
    for(unsigned int i=0; i< model.size(); i++){
       // std::cout<<"i= "<<i<<"\n";
       // std::cout<<"------------------------"<<"\n";       
 //first equation     
      
        A(2*i,0)=0;

        A(2*i,1)=0;
        A(2*i,2)=0; 
        A(2*i,3)=-model[i].x;
        A(2*i,4)=-model[i].y;       
        A(2*i,5)=-1;  
        A(2*i,6)=model[i].x*data[i].y;   
        A(2*i,7)=model[i].y*data[i].y;   
        A(2*i,8)=data[i].y;

        //second equation
        A(2*i+1,0)=model[i].x;
        A(2*i+1,1)=model[i].y;
        A(2*i+1,2)=1; 
        A(2*i+1,3)=0;
        A(2*i+1,4)=0;       
        A(2*i+1,5)=0;  
        A(2*i+1,6)=-model[i].x*data[i].x;   
        A(2*i+1,7)=-model[i].y*data[i].x;   
        A(2*i+1,8)=-data[i].x;    

    //std::cin.get();
    }


return A;
}
*/
CMatrix<float> createMatrix(  std::vector<point> model , std::vector<point> data ){

CMatrix<float> A(9, 2*model.size());

    //std::cout<<"model size "<<model.size()<<"\n";
    //std::cout<<"data size "<<data.size()<<"\n";
    for(unsigned int i=0; i< model.size(); i++){
       // std::cout<<"i= "<<i<<"\n";
       // std::cout<<"------------------------"<<"\n";       
 //first equation     
      
        A(0,2*i)=0;

        A(1,2*i)=0;
        A(2,2*i)=0; 
        A(3, 2*i)=-model[i].x;
        A(4,2*i)=-model[i].y;       
        A(5,2*i)=-1;  
        A(6,2*i)=model[i].x*data[i].y;   
        A(7,2*i)=model[i].y*data[i].y;   
        A(8,2*i)=data[i].y;
/* 
    std::cout<<"A["<<2*i<<"]"<<"["<<0<<"]= "<<A(2*i,0)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<1<<"]= "<<A(2*i,1)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<2<<"]= "<<A(2*i,2)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<3<<"]= "<<A(2*i,3)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<4<<"]= "<<A(2*i,4)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<5<<"]= "<<A(2*i,5)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<6<<"]= "<<A(2*i,6)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<7<<"]= "<<A(2*i,7)<<"\n";
    std::cout<<"A["<<2*i<<"]"<<"["<<8<<"]= "<<A(2*i,8)<<"\n";
*/
        //second equation
        A(0,2*i+1)=model[i].x;
        A(1,2*i+1)=model[i].y;
        A(2,2*i+1)=1; 
        A(3,2*i+1)=0;
        A(4,2*i+1)=0;       
        A(5,2*i+1)=0;  
        A(6,2*i+1)=-model[i].x*data[i].x;   
        A(7,2*i+1)=-model[i].y*data[i].x;   
        A(8,2*i+1)=-data[i].x;    
  /*  
    std::cout<<"A["<<2*i+1<<"]"<<"["<<0<<"]= "<<A(2*i+1,0)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<1<<"]= "<<A(2*i+1,1)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<2<<"]= "<<A(2*i+1,2)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<3<<"]= "<<A(2*i+1,3)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<4<<"]= "<<A(2*i+1,4)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<5<<"]= "<<A(2*i+1,5)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<6<<"]= "<<A(2*i+1,6)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<7<<"]= "<<A(2*i+1,7)<<"\n";
    std::cout<<"A["<<2*i+1<<"]"<<"["<<8<<"]= "<<A(2*i+1,8)<<"\n"; 
        std::cout<<"------------------------"<<"\n";    
*/
    //std::cin.get();
    }


return A;
}

void CalcMeanSigma(const std::vector<point> data, float point::*member, float &mean, float &sigma){
   mean = 0;
    sigma=0;
    //calculate mean
    for(auto it = data.begin(), end = data.end(); it != end; ++it)
        mean += (*it).*member;

    mean= mean/ data.size();

    //sigma

    for(auto it = data.begin(), end = data.end(); it != end; ++it)
        sigma += ( (*it).*member - mean)  * ( (*it).*member - mean);

    sigma=sqrt(sigma/(data.size()-1));
   
}



CMatrix<float> CalcTransform(const std::vector<point> data ){

 CMatrix<float> transform(3,3,0);
//Calculate mean and sigma
float meanX, meanY;
float sigmaX, sigmaY;

CalcMeanSigma(data, &point::x, meanX, sigmaX);
CalcMeanSigma(data, &point::y, meanY, sigmaY);
      std::cout<<"mean x "<<meanX<<"\n";
      std::cout<<"mean y "<<meanY<<"\n";
      std::cout<<"sigma x "<<sigmaX<<"\n";
      std::cout<<"sigma y "<<sigmaY<<"\n";

//Z transform Z= (x-mean)/sigma
                //x=z*sigma +mean
    transform(0,0)=1/sigmaX;
    transform(0,2)=meanX/sigmaX;
    transform(1,1)=1/sigmaY;
    transform(1,2)=meanY/sigmaY;
    transform(2,2)=1;

    return transform;
}
void normalize(std::vector<point> &data, CMatrix<float> transform){

//Z transform Z= (x-mean)/sigma

    for(unsigned int i=0; i<data.size(); i++){
             data[i].x=  transform(0,0)*data[i].x+ transform(0,2);
             data[i].y=transform(1,1)*data[i].y +transform(1,2);
    }


}

CMatrix<float> getHomography(CMatrix<float> V, CMatrix<float> Tmodel, CMatrix<float> Tdata){

   CMatrix<float> homography(3,3);
        
    CMatrix<float> H(3,3);
    
    H(0,0)= V(8,0);
    H(0,1) =V(8,1);
    H(0,2) =V(8,2);    
    H(1,0) =V(8,3);  
    H(1,1) =V(8,4);  
    H(1,2) =V(8,5);  
    H(2,0) =V(8,6);  
    H(2,1) =V(8,7);  
    H(2,2) =V(8,8);  

 std::cout<<"H00 "<<H(0,0)<<"\n";
    std::cout<<"H01 "<< H(0,1)<<"\n";    
    std::cout<<"H02 "<<H(0,2)<<"\n";
    std::cout<<"H10 "<<H(1,0)<<"\n";  
    std::cout<<"H11 "<<H(1,1)<<"\n";
    std::cout<<"H12 "<<H(1,2)<<"\n";  
    std::cout<<"H20 "<<H(2,0)<<"\n";  
    std::cout<<"H21 "<<H(2,1)<<"\n";
    std::cout<<"H22 "<<H(2,2)<<"\n"; 
//homography = Tdata.inv * H *Tmodel
Tdata.inv();

CMatrix<float> temp(3,3);
    //temp
    temp(0,0)= Tdata(0,0)*H(0,0)+Tdata(0,1)*H(1,0)+Tdata(0,2)*H(2,0);
    temp(0,1)= Tdata(0,0)*H(0,1)+Tdata(0,1)*H(1,1)+Tdata(0,2)*H(2,1);
    temp(0,2)= Tdata(0,0)*H(0,2)+Tdata(0,1)*H(1,2)+Tdata(0,2)*H(2,2);
    temp(1,0)= Tdata(1,0)*H(0,0)+Tdata(1,1)*H(1,0)+Tdata(1,2)*H(2,0);
    temp(1,1)= Tdata(1,0)*H(0,1)+Tdata(1,1)*H(1,1)+Tdata(1,2)*H(2,1);
    temp(1,2)= Tdata(1,0)*H(0,2)+Tdata(1,1)*H(1,2)+Tdata(1,2)*H(2,2);
    temp(2,0)= Tdata(2,0)*H(0,0)+Tdata(2,1)*H(1,0)+Tdata(2,2)*H(2,0);
    temp(2,1)= Tdata(2,0)*H(0,1)+Tdata(2,1)*H(1,1)+Tdata(2,2)*H(2,1);
    temp(2,2)= Tdata(2,0)*H(0,2)+Tdata(2,1)*H(1,2)+Tdata(2,2)*H(2,2);

    //homography
    homography(0,0)= temp(0,0)*Tmodel(0,0)+temp(0,1)*Tmodel(1,0)+temp(0,2)*Tmodel(2,0);
    homography(0,1)= temp(0,0)*Tmodel(0,1)+temp(0,1)*Tmodel(1,1)+temp(0,2)*Tmodel(2,1);
    homography(0,2)= temp(0,0)*Tmodel(0,2)+temp(0,1)*Tmodel(1,2)+temp(0,2)*Tmodel(2,2);
    homography(1,0)= temp(1,0)*Tmodel(0,0)+temp(1,1)*Tmodel(1,0)+temp(1,2)*Tmodel(2,0);
    homography(1,1)= temp(1,0)*Tmodel(0,1)+temp(1,1)*Tmodel(1,1)+temp(1,2)*Tmodel(2,1);
    homography(2,0)= temp(2,0)*Tmodel(0,0)+temp(2,1)*Tmodel(1,0)+temp(2,2)*Tmodel(2,0);
    homography(1,2)= temp(1,0)*Tmodel(0,2)+temp(1,1)*Tmodel(1,2)+temp(1,2)*Tmodel(2,2);
    homography(2,1)= temp(2,0)*Tmodel(0,1)+temp(2,1)*Tmodel(1,1)+temp(2,2)*Tmodel(2,1);
    homography(2,2)= temp(2,0)*Tmodel(0,2)+temp(2,1)*Tmodel(1,2)+temp(2,2)*Tmodel(2,2);

    std::cout<<"homography["<<0<<"]"<<"["<<0<<"]= "<<homography(0,0)<<"\n";
    std::cout<<"homography["<<0<<"]"<<"["<<1<<"]= "<<homography(0,1)<<"\n";
    std::cout<<"homography["<<0<<"]"<<"["<<2<<"]= "<<homography(0,2)<<"\n";
    std::cout<<"homography["<<1<<"]"<<"["<<3<<"]= "<<homography(1,0)<<"\n";
    std::cout<<"homography["<<1<<"]"<<"["<<4<<"]= "<<homography(1,1)<<"\n";
    std::cout<<"homography["<<1<<"]"<<"["<<5<<"]= "<<homography(1,2)<<"\n";
    std::cout<<"homography["<<2<<"]"<<"["<<6<<"]= "<<homography(2,0)<<"\n";
    std::cout<<"homography["<<2<<"]"<<"["<<7<<"]= "<<homography(2,1)<<"\n";
    std::cout<<"homography["<<2<<"]"<<"["<<8<<"]= "<<homography(2,2)<<"\n";


    //homography= Tdata.inv() * H * Tmodel;
    return homography;

}




void makeProjection2image(CMatrix<float> Homography,  std::vector<point> model, CMatrix<float> &image){

std::vector<point> image_points;

    for(int i=0;i<model.size();i++){
        float x=Homography(0,0)*model[i].x+Homography(0,1)*model[i].y+Homography(0,2);
        float y=Homography(1,0)*model[i].x+Homography(1,1)*model[i].y+Homography(1,2);
        float n=Homography(2,0)*model[i].x+Homography(2,1)*model[i].y+Homography(2,2);
            
        point result;
        result.x=x/n;
        result.y=y/n;
       // std::cout<<"x "<<result.x<<"\n";
      //  std::cout<<"y "<<result.y<<"\n";
        image_points.push_back(result);                                  
    }

     for(int i=0;i<image_points.size();i++){

     image(image_points[i].x,image_points[i].y)=0;
    }


}

CMatrix<float> HomographyConstraintsMatrix(CMatrix<float> H1, CMatrix<float> H2, CMatrix<float> H3, CMatrix<float> H4, CMatrix<float> H5 ){
    CMatrix<float> V(6,10);//constraints matrix

    //for H1 
        //first row
    V(0,0)=H1(0,0)*H1(1,0);
    V(1,0)=H1(0,0)*H1(1,1) + H1(0,1)*H1(1,0);
    V(2,0)=H1(0,1)*H1(1,1);
    V(3,0)=H1(0,2)*H1(1,0)+H1(0,0)*H1(1,2);
    V(4,0)=H1(0,2)*H1(1,1)+H1(0,1)*H1(1,2);
    V(5,0)=H1(0,2)*H1(1,2);
       //second row 
    V(0,1)=H1(0,0)*H1(0,0)- H1(1,0)*H1(1,0);
    V(1,1)=( H1(0,0)*H1(0,1) + H1(0,1)*H1(0,0) ) - ( H1(1,0)*H1(1,1) + H1(1,1)*H1(1,0) );
    V(2,1)=H1(0,1)*H1(0,1) -H1(1,1)*H1(1,1);
    V(3,1)= (H1(0,2)*H1(0,0)+H1(0,0)*H1(0,2)) -(H1(1,2)*H1(1,0)+H1(1,0)*H1(1,2)  );
    V(4,1)=(H1(0,2)*H1(0,1)+H1(0,1)*H1(0,2) ) - (H1(1,2)*H1(1,1)+H1(1,1)*H1(1,2));
    V(5,1)=H1(0,2)*H1(0,2)-H1(1,2)*H1(1,2);
    
    //for H2 
        //first row
    V(0,2)=H2(0,0)*H2(1,0);
    V(1,2)=H2(0,0)*H2(1,1) + H2(0,1)*H2(1,0);
    V(2,2)=H2(0,1)*H2(1,1);
    V(3,2)=H2(0,2)*H2(1,0)+H2(0,0)*H2(1,2);
    V(4,2)=H2(0,2)*H2(1,1)+H2(0,1)*H2(1,2);
    V(5,2)=H2(0,2)*H2(1,2);
       //second row 
    V(0,3)=H2(0,0)*H2(0,0)- H2(1,0)*H2(1,0);
    V(1,3)=( H2(0,0)*H2(0,1) + H2(0,1)*H2(0,0) ) - ( H2(1,0)*H2(1,1) + H2(1,1)*H2(1,0) );
    V(2,3)=H2(0,1)*H2(0,1) -H2(1,1)*H2(1,1);
    V(3,3)= (H2(0,2)*H2(0,0)+H2(0,0)*H2(0,2)) -(H2(1,2)*H2(1,0)+H2(1,0)*H2(1,2)  );
    V(4,3)=(H2(0,2)*H2(0,1)+H2(0,1)*H2(0,2) ) - (H2(1,2)*H2(1,1)+H2(1,1)*H2(1,2));
    V(5,3)=H2(0,2)*H2(0,2)-H2(1,2)*H2(1,2);    


    //for H3 
        //first row
    V(0,4)=H3(0,0)*H3(1,0);
    V(1,4)=H3(0,0)*H3(1,1) + H3(0,1)*H3(1,0);
    V(2,4)=H3(0,1)*H3(1,1);
    V(3,4)=H3(0,2)*H3(1,0)+H3(0,0)*H3(1,2);
    V(4,4)=H3(0,2)*H3(1,1)+H3(0,1)*H3(1,2);
    V(5,4)=H3(0,2)*H3(1,2);
       //second row 
    V(0,5)=H3(0,0)*H3(0,0)- H3(1,0)*H3(1,0);
    V(1,5)=( H3(0,0)*H3(0,1) + H3(0,1)*H3(0,0) ) - ( H3(1,0)*H3(1,1) + H3(1,1)*H3(1,0) );
    V(2,5)=H3(0,1)*H3(0,1) -H3(1,1)*H3(1,1);
    V(3,5)= (H3(0,2)*H3(0,0)+H3(0,0)*H3(0,2)) -(H3(1,2)*H3(1,0)+H3(1,0)*H3(1,2)  );
    V(4,5)=(H3(0,2)*H3(0,1)+H3(0,1)*H3(0,2) ) - (H3(1,2)*H3(1,1)+H3(1,1)*H3(1,2));
    V(5,5)=H3(0,2)*H3(0,2)-H3(1,2)*H3(1,2);    

    //for H4 
        //first row
    V(0,6)=H4(0,0)*H4(1,0);
    V(1,6)=H4(0,0)*H4(1,1) + H4(0,1)*H4(1,0);
    V(2,6)=H4(0,1)*H4(1,1);
    V(3,6)=H4(0,2)*H4(1,0)+H4(0,0)*H4(1,2);
    V(4,6)=H4(0,2)*H4(1,1)+H4(0,1)*H4(1,2);
    V(5,6)=H4(0,2)*H4(1,2);
       //second row 
    V(0,7)=H4(0,0)*H4(0,0)- H4(1,0)*H4(1,0);
    V(1,7)=( H4(0,0)*H4(0,1) + H4(0,1)*H4(0,0) ) - ( H4(1,0)*H4(1,1) + H4(1,1)*H4(1,0) );
    V(2,7)=H4(0,1)*H4(0,1) -H4(1,1)*H4(1,1);
    V(3,7)= (H4(0,2)*H4(0,0)+H4(0,0)*H4(0,2)) -(H4(1,2)*H4(1,0)+H4(1,0)*H4(1,2)  );
    V(4,7)=(H4(0,2)*H4(0,1)+H4(0,1)*H4(0,2) ) - (H4(1,2)*H4(1,1)+H4(1,1)*H4(1,2));
    V(5,7)=H4(0,2)*H4(0,2)-H4(1,2)*H4(1,2); 


    //for H5 
        //first row
    V(0,8)=H5(0,0)*H5(1,0);
    V(1,8)=H5(0,0)*H5(1,1) + H5(0,1)*H5(1,0);
    V(2,8)=H5(0,1)*H5(1,1);
    V(3,8)=H5(0,2)*H5(1,0)+H5(0,0)*H5(1,2);
    V(4,8)=H5(0,2)*H5(1,1)+H5(0,1)*H5(1,2);
    V(5,8)=H5(0,2)*H5(1,2);
       //second row 
    V(0,9)=H5(0,0)*H5(0,0)- H5(1,0)*H5(1,0);
    V(1,9)=( H5(0,0)*H5(0,1) + H5(0,1)*H5(0,0) ) - ( H5(1,0)*H5(1,1) + H5(1,1)*H5(1,0) );
    V(2,9)=H5(0,1)*H5(0,1) -H5(1,1)*H5(1,1);
    V(3,9)= (H5(0,2)*H5(0,0)+H5(0,0)*H5(0,2)) -(H5(1,2)*H5(1,0)+H5(1,0)*H5(1,2)  );
    V(4,9)=(H5(0,2)*H5(0,1)+H5(0,1)*H5(0,2) ) - (H5(1,2)*H5(1,1)+H5(1,1)*H5(1,2));
    V(5,9)=H5(0,2)*H5(0,2)-H5(1,2)*H5(1,2);

    return V;
}


CMatrix<float> calculateInternalParam( CMatrix<float> V, float &lambda){

    CMatrix<float> K(3,3,0);
    //B parameters
    float B11=V(5,0);
    float B12=V(5,1);
    float B22=V(5,2);
    float B13=V(5,3);
    float B23=V(5,4);
    float B33=V(5,5);

    float c=sqrt(B11*B11 + B12*B12+B22*B22+B13*B13+B23*B23+B33*B33);
   std::cout<<"c "<<c<<"\n";

    std::cout<<"B11 "<<B11<<"\n";
    std::cout<<"B12 "<<B12<<"\n";    
    std::cout<<"B22 "<<B22<<"\n";
    std::cout<<"B13 "<<B13<<"\n";  
    std::cout<<"B23 "<<B23<<"\n";
    std::cout<<"B33 "<<B33<<"\n";  


    float y0=(B12*B13-B11*B23)/(B11*B22 -B12*B12);
  lambda=B33-(B13*B13+y0*(B12*B13-B11*B23))/B11;
     std::cout<<"lambda "<<lambda/B11<<"\n";     
  float ax=sqrt(lambda/B11);
  float ay=sqrt(lambda*B11/(B11*B22-B12*B12));
  float s=-B12*ax*ax*ay/lambda;
  float x0=s*y0/ay - B13*ax*ax/lambda;
    

    K(0,0)=ax;
    K(0,1)=s;
    K(0,2)=x0;
    K(1,1)=ay;
    K(1,2)=y0;
    K(2,2)=1;
    std::cout<<"ax "<<ax<<"\n";
    std::cout<<"ay "<<ay<<"\n";    
    std::cout<<"x0 "<<x0<<"\n";
    std::cout<<"y0 "<<y0<<"\n";  
    std::cout<<"s "<<s<<"\n";
    std::cout<<"lambda "<<lambda<<"\n";  

return K;

}

int main(int argc, char** argv) {  


    


    //read files with 2D point correspondences

    std::vector<point> model;
    if (!read_file( "model.txt", model)){
        std::cout<<"Can't read model.txt"<<"\n";
    }

    std::vector<point> data1;
    if (!read_file( "data1.txt", data1)){
        std::cout<<"Can't read data1.txt"<<"\n";
    }

    std::vector<point> data2;
    if (!read_file( "data2.txt", data2)){
        std::cout<<"Can't read data2.txt"<<"\n";
    }

    std::vector<point> data3;
    if (!read_file( "data3.txt", data3)){
        std::cout<<"Can't read data3.txt"<<"\n";
    }
    std::vector<point> data4;
    if (!read_file( "data4.txt", data4)){
        std::cout<<"Can't read data4.txt"<<"\n";
    }
    std::vector<point> data5;
    if (!read_file( "data5.txt", data5)){
        std::cout<<"Can't read data5.txt"<<"\n";
    }

    //transformation matrices for normalization
    CMatrix<float> Tmodel=CalcTransform(model);
    CMatrix<float> Tdata1=CalcTransform(data1); 
    CMatrix<float> Tdata2=CalcTransform(data2); 
    CMatrix<float> Tdata3=CalcTransform(data3); 
    CMatrix<float> Tdata4=CalcTransform(data4);  
    CMatrix<float> Tdata5=CalcTransform(data5);   
    //normalization
    normalize(model, Tmodel);
    normalize(data1, Tdata1); 
    normalize(data2, Tdata2); 
    normalize(data3, Tdata3); 
    normalize(data4, Tdata4); 
    normalize(data5, Tdata5); 


    CMatrix<float> A1=createMatrix(model,data1 );
    CMatrix<float> A2=createMatrix(model,data2 );
    CMatrix<float> A3=createMatrix(model,data3 );
    CMatrix<float> A4=createMatrix(model,data4 );
    CMatrix<float> A5=createMatrix(model,data5 );

    std::cout<<"A1 size x "<<A1.xSize()<<"\n";
    std::cout<<"A1 size y "<<A1.ySize()<<"\n";

    //calculate SVD for each matrix A
    CMatrix<float> S1(9,9); 
    CMatrix<float> V1(9,9);
    NMath::svd(A1, S1, V1, true);

    CMatrix<float> S2(9,9); 
    CMatrix<float> V2(9,9);
    NMath::svd(A2, S2, V2, true);

    CMatrix<float> S3(9,9); 
    CMatrix<float> V3(9,9);
    NMath::svd(A3, S3, V3, true);

    CMatrix<float> S4(9,9); 
    CMatrix<float> V4(9,9);
    NMath::svd(A4, S4, V4, true);

    CMatrix<float> S5(9,9); 
    CMatrix<float> V5(9,9);
    NMath::svd(A5, S5, V5, true);
    
    //get homography
    CMatrix<float> H1=getHomography(V1, Tmodel, Tdata1 );
    CMatrix<float> H2=getHomography(V2, Tmodel, Tdata2 );
    CMatrix<float> H3=getHomography(V3, Tmodel, Tdata3 );
    CMatrix<float> H4=getHomography(V4, Tmodel, Tdata4 );
    CMatrix<float> H5=getHomography(V5, Tmodel, Tdata5 );

    


 CMatrix<float> U=HomographyConstraintsMatrix(H1,  H2,  H3,  H4,  H5 );


    CMatrix<float> S(6,6); 
    CMatrix<float> V(6,6);
    NMath::svd(U, S, V, true);
    float lambda;
  CMatrix<float> K=calculateInternalParam(  V,  lambda);
    
CMatrix<float> image;

image.readFromPGM("CalibIm5.pgm");

makeProjection2image(H5,  model, image);
image.writeToPGM("result.pgm");
//  return 0;
}
