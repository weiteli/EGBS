//
//  SLICSpectralClustering.h
//  OpenCVEGBS
//
//  Created by Wei-Te Li on 14/9/9.
//  Copyright (c) 2014年 wade. All rights reserved.
//

#ifndef __OpenCVEGBS__SLICSpectralClustering__
#define __OpenCVEGBS__SLICSpectralClustering__

#include <iostream>
#include <opencv2/opencv.hpp>
#include "SLIC.h"

using namespace std;
using namespace cv;

class SLICSpectralClustering{
public:
    SLICSpectralClustering();
    SLICSpectralClustering(Size image_size, float sigma);
    
    void init(Size image_size, float sigma);
    void calculateEigenvectors(vector<ColorRep>& clusters_centers, int slic_s, int slic_m);
    Mat applySegmentation( int no_of_clusters, Mat& clusters_index );
    Mat getClusterMask();
    Mat createAdjacency(vector<ColorRep>& points, int slic_s, int slic_m);
private:
    Mat degreeMatrix(Mat& adjacency);
    Mat labels;
    Mat eigenvectors;
    Mat clusterMask;
    float sigma;
};

#endif /* defined(__OpenCVEGBS__SLICSpectralClustering__) */
