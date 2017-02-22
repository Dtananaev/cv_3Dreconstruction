Computer vision: 3D reconstruction
====================================================

The tools for 3D reconstruction.

[![Build Status](https://travis-ci.org/Dtananaev/cv_segmentation.svg?branch=master)](https://travis-ci.org/Dtananaev/cv_segmentation)
[![BSD2 License](http://img.shields.io/badge/license-BSD2-brightgreen.svg)](https://github.com/Dtananaev/k_means/blob/master/LICENSE.md) 

* Disparity estimation naive - the naive implementation of the disparity estmation by using three different distances: Euclidian distance, absolute distance and cross entrophy
<p align="center">
  <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/Euclidiandist.jpg" width="150"/>
  <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/absolutedist.jpg" width="150"/>
   <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/entrophyresult.jpg" width="150"/>
   <img src="https://github.com/Dtananaev/cv_3Dreconstruction/blob/master/Disparity_naive/pictures/dispGT.JPG" width="150"/>   
</p>
     * To install use in terminal: 
         * cd k_means
         * make
     * To run: ./kmeans name_of_file(without .ppm) \<number of clusters\> \<number of iteration\>
     
     

* two region segmentation based on variational methods allows to segment noisy images where the k-means segmentation usually fails because it adds noise to one of the segment. Two region segmentation approach could be easily extended to multi region segmentation
<p align="center">
  <img src="https://github.com/Dtananaev/cv_segmentation/blob/master/pictures/withWholeEasier.jpeg" width="350"/>
  <img src="https://github.com/Dtananaev/cv_segmentation/blob/master/pictures/withWholeEasier_2r.jpeg" width="350"/>
</p>
     * To install use in terminal: 
         * cd two_region_segmentation
         * make
     * To run: ./segment2r name_of_file(without .pgm)
