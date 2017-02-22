/*
 * File: message_passing.cpp
 *
 * Author: Denis Tananaev
 *
 * Date: 20.10.2016
 */

#include "message_passing.h"

sgm::sgm(CTensor<float> image_left, CTensor<float> image_right){

    patch_radius_=3;
    label_=15;
    lambda_=1;

    x_size_=image_left.xSize();
    y_size_=image_left.ySize();

    redL_=image_left.getMatrix(0);
    greenL_=image_left.getMatrix(1);   
    blueL_=image_left.getMatrix(2);   

    redR_=image_right.getMatrix(0);
    greenR_=image_right.getMatrix(1);   
    blueR_=image_right.getMatrix(2);   

}

sgm::~sgm(){
}


void sgm::setPathRadius(int radius){
    patch_radius_=radius;
}
void sgm::setLabel(int labels){
    label_=labels;
}

void sgm::setLambda(int lambda){
    lambda_=lambda;
}


float sgm::EuclidUnaryCost(int x, int y, int label){
    float dist=0;
    float R=0;
    float G=0;
    float B=0;
    for( int ty = -patch_radius_; ty <= patch_radius_; ++ty )
		  for( int tx = -patch_radius_; tx <= patch_radius_; ++tx )
		   {
			// clamp coordinates
			int p1x = std::min(x_size_-1,std::max(0,x+tx));//start and end of patch
			int p1y = std::min(y_size_-1,std::max(0,y+ty));
			int p2x = std::min(x_size_-1,std::max(0,x-label+tx));
			int p2y = std::min(y_size_-1,std::max(0,y+ty));
			float Rtmp = redL_(p1x,p1y)-redR_(p2x,p2y);		
			R += Rtmp*Rtmp;

            float Gtmp = greenL_(p1x,p1y)-greenR_(p2x,p2y);		
			G += Gtmp*Gtmp;

            float Btmp=blueL_(p1x,p1y)-blueR_(p2x,p2y);	
            B+=Btmp*Btmp;
		  }

         return dist=sqrt(R+G+B);
}

int sgm::PottsPairwiseCost(int label1,int label2){
    int i=1;
    if(label1==label2){
        i=0;
    }
    return i;
}

std::vector<std::vector<float>> sgm::calcUnaryCostX(int y){

std::vector<std::vector<float>> result;
    std::vector<float> temp;

    for(int x=0; x<x_size_;x++){
             temp.clear();
             for(int lbl1=0; lbl1<=label_;lbl1++){ //message passing only by x direction
                float uCost=0; 
               if (x - lbl1 < 0) { uCost= 1.0e9f;}
                else{

                 uCost=EuclidUnaryCost(x,y,lbl1);
                }
            
                temp.push_back(uCost);
            
            }
  
        result.push_back(temp);
      }

    return result;
}


std::vector<std::vector<float>> sgm::calcMsgsForwardX ( std::vector<std::vector<float>> unaryCost){
    //init first vector equal zero (no previous messages for the first pixel)

    std::vector<float> init(label_+1, 0.0);//size 16 for x=0
    std::vector<std::vector<float>> result;


    result.push_back(init);

    std::vector<float> temp; // calculate pairwise cost for each unary cost 
    std::vector<float> msgstemp;

    for(int x=1; x<x_size_;x++){

       
        msgstemp.clear(); 

            for(int lbl1=0; lbl1<=label_; lbl1++){           
                  temp.clear();    

 

                  for(int lbl2=0; lbl2<=label_;lbl2++){ 
                        int pCost=PottsPairwiseCost(lbl1,lbl2);
                        float res=unaryCost[x-1][lbl2]+lambda_*pCost + result[x-1][lbl2];        
                        temp.push_back(res);   

                        
                 }      

               float min_index = std::min_element(temp.begin(), temp.end())- temp.begin();                 
                msgstemp.push_back(temp[min_index]);

        }
     
        result.push_back(msgstemp);
    }

    return result;
}


//msgs backward starts from x 
//so for example for point x it backward msgs will be at [x_size_-x-1]
std::vector<std::vector<float>> sgm::calcMsgsBackwardX (std::vector<std::vector<float>> unaryCost){
   //init first vector equal zero (no previous messages for the first pixel)

    std::vector<float> init(label_+1, 0.0);//size 16 for x=0
    std::vector<std::vector<float>> result;


    result.push_back(init);

    std::vector<float> temp; // calculate pairwise cost for each unary cost 
    std::vector<float> msgstemp;

    for(int x=1; x<x_size_;x++){

       
        msgstemp.clear(); 

            for(int lbl1=0; lbl1<=label_; lbl1++){           
                  temp.clear();    

                     
           

                  for(int lbl2=0; lbl2<=label_;lbl2++){ 
                        int pCost=PottsPairwiseCost(lbl1,lbl2);
                        float res=unaryCost[x_size_-x][lbl2]+lambda_*pCost + result[x-1][lbl2];        
                        temp.push_back(res);   

                        
                 }      

               float min_index = std::min_element(temp.begin(), temp.end())- temp.begin();                 
                msgstemp.push_back(temp[min_index]);

        }
     
        result.push_back(msgstemp);
    }

    return result;
}




CMatrix<float> sgm::messagePassingDepthX(){
    
CMatrix<float> result(x_size_,y_size_);

 for(int y=0; y<y_size_;y++){
        std::vector<std::vector<float>> unary= calcUnaryCostX( y) ;
        std::vector<std::vector<float>> msgsForward=calcMsgsForwardX ( unary);
       std::vector<std::vector<float>> msgsBackward=calcMsgsBackwardX( unary);
        
    std::vector<float> temp;
        //calculate final decision
        for(int x=0;x<x_size_;x++){
             temp.clear();
                for(int lbl=0;lbl<=label_;lbl++){
                    
                        float res= unary[x][lbl]+ msgsForward[x][lbl]+msgsBackward[x_size_-x-1][lbl];               
          

                      temp.push_back(res);        
             
                }
  
                float min_index = std::min_element(temp.begin(), temp.end())- temp.begin();

             //  std::cout<<"disparity "<<min_index<<"\n";
                      
                result(x,y)=min_index;
                
        }
     //std::cin.get();  

    }
    return result;
}




int main(){
  
    CTensor<float> imageL;
    CTensor<float> imageR;
	imageL.readFromPPM("tsukubaL.ppm");
	imageR.readFromPPM("tsukubaR.ppm");

    sgm depth(imageL,imageR);
    depth.setPathRadius(0);//path radius for similarity check(for unary cost)
    depth.setLabel(15);//Labels max disparity
    depth.setLambda(100);//pairwise cost penalizer


    CMatrix<float> result=depth.messagePassingDepthX();

    result.normalize(0,255);
    result.writeToPGM("result.pgm");
    


   
}



