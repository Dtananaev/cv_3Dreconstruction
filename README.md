Computer vision: 3D reconstruction
====================================================

The tools for 3D reconstruction.

[![Build Status](https://travis-ci.org/Dtananaev/cv_3Dreconstruction.svg?branch=master)](https://travis-ci.org/Dtananaev/cv_3Dreconstruction)
[![BSD2 License](http://img.shields.io/badge/license-BSD2-brightgreen.svg)](https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/LICENSE.md) 

* Disparity estimation naive - the naive implementation of the disparity estmation by using three different distances: Euclidian distance, absolute distance and cross entrophy
<p align="center">
  <b>  Euclidian distance   |   absolute distance   |    cross entropy   |    ground truth </b><br>
  <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/Euclidiandist.jpg" width="120"/>
  <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/absolutedist.jpg" width="120"/>
   <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/entrophyresult.jpg" width="120"/>
   <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/dispGT.JPG" width="120"/>   
</p>
     * To install use in terminal: 
         * cd Disparity_naive
         * make
     * To run: ./disp name_of_fileLeft(without .ppm) name_of_fileRight(without .ppm) patch_size
     
     
