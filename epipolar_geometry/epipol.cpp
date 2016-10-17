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
#include "CTensor.h"
#include "CVector.h"
#include <vector>
#include <string>
#include <fstream>
#include <NMath.h> //SVD



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

/*
   CMatrix<float> U(4,3); //should be 4x3
    U(0,0)=p1(0,0);  U(0,1)=p1(1,0);   U(0,2)=p1(2,0);     
    U(1,0)=p1(0,1);  U(1,1)=p1(1,1);   U(1,2)=p1(2,1); 
    U(2,0)=p1(0,2);  U(2,1)=p1(1,2);   U(2,2)=p1(2,2); 
    U(3,0)=p1(0,3);  U(3,1)=p1(1,3);   U(3,2)=p1(2,3); 
   CMatrix<float> S(4,4); 
   CMatrix<float> V(4,4);

*/

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
/*
    
    F(0,0)=  temp(0,0)*U(0,0)+temp(0,1)*U(1,0)+temp(0,2)*U(2,0)+temp(0,3)*U(3,0);
    F(0,1)=  temp(1,0)*U(0,0)+temp(1,1)*U(1,0)+temp(1,2)*U(2,0)+temp(1,3)*U(3,0);
    F(0,2)=  temp(2,0)*U(0,0)+temp(2,1)*U(1,0)+temp(2,2)*U(2,0)+temp(2,3)*U(3,0);


    F(1,0)=  temp(0,0)*U(0,1)+temp(0,1)*U(1,1)+temp(0,2)*U(2,1)+temp(0,3)*U(3,1);
    F(1,1)=  temp(1,0)*U(0,1)+temp(1,1)*U(1,1)+temp(1,2)*U(2,1)+temp(1,3)*U(3,1);
    F(1,2)=  temp(2,0)*U(0,1)+temp(2,1)*U(1,1)+temp(2,2)*U(2,1)+temp(2,3)*U(3,1);

    F(2,0)=  temp(0,0)*U(0,2)+temp(0,1)*U(1,2)+temp(0,2)*U(2,2)+temp(0,3)*U(3,2);
    F(2,1)=  temp(1,0)*U(0,2)+temp(1,1)*U(1,2)+temp(1,2)*U(2,2)+temp(1,3)*U(3,2);
    F(2,2)=  temp(2,0)*U(0,2)+temp(2,1)*U(1,2)+temp(2,2)*U(2,2)+temp(2,3)*U(3,2);


    std::cout<<"F(0,0)= "<<F(0,0)<<"\n";
    std::cout<<"F(0,1)= "<<F(0,1)<<"\n";
    std::cout<<"F(0,2)= "<<F(0,2)<<"\n";

    std::cout<<"F(1,0)= "<<F(1,0)<<"\n";
    std::cout<<"F(1,1)= "<<F(1,1)<<"\n";
    std::cout<<"F(1,2)= "<<F(1,2)<<"\n";

    std::cout<<"F(2,0)= "<<F(2,0)<<"\n";
    std::cout<<"F(2,1)= "<<F(2,1)<<"\n";
    std::cout<<"F(2,2)= "<<F(2,2)<<"\n";

*/


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

CMatrix<float> l(3,1);

    l(0,0)=F(0,0)*300 + F(0,1)*200+F(0,2);
    l(1,0)=F(1,0)*300 + F(1,1)*200+F(1,2);
    l(2,0)=F(2,0)*300 + F(2,1)*200+F(2,2);

 std::cout<<"l(0,0)= "<<l(0,0)<<"\n";
 std::cout<<"l(1,0)= "<<l(1,0)<<"\n";
 std::cout<<"l(2,0)= "<<l(2,0)<<"\n";
}

