/*
 * File: sgm.h
 *
 * Author: Denis Tananaev
 *
 * Date: 20.10.2016
 */

#ifndef SGM_H_
#define SGM_H_

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



class sgm{


public:
    sgm(CTensor<float> image_left, CTensor<float> image_right);
    ~sgm();

    CMatrix<float> messagePassingDepthX();
    float EuclidUnaryCost(int x, int y, int label);
    int PottsPairwiseCost(int label1,int label2);
    
    void setPathRadius(int radius);
    void setLambda(int lambda);
    void setLabel(int labels);

    std::vector<std::vector<float>> calcUnaryCostX(int y);
    std::vector<std::vector<float>> calcMsgsForwardX ( std::vector<std::vector<float>> unaryCost);
    std::vector<std::vector<float>> calcMsgsBackwardX (std::vector<std::vector<float>> unaryCost);    
private:
    int label_,lambda_,  patch_radius_, x_size_, y_size_;
    CMatrix<float> redL_, greenL_, blueL_;
    CMatrix<float> redR_, greenR_, blueR_;

   // std::vector<std::vector<float>> unaryCost_;
   // std::vector<std::vector<float>> msgsForward_;

};




#endif /* SGM_H_ */
