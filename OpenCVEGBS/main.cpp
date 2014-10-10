//
//  main.cpp
//  OpenCVEGBS
//
//  Created by Wei-Te Li on 14/9/5.
//  Copyright (c) 2014年 wade. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "EGBS.h"
#include "SLIC.h"
#include "SLICSpectralClustering.h"
#include <ctime>

using namespace std;
using namespace cv;

double countTime(){
	const clock_t begin_time = clock();
	return (double)begin_time;
}

int main(int argc, const char * argv[]){
    double t1 = countTime();
    Mat image = imread( "image.jpg");
    
    // Gaussian blur parameter
    float sigma = 0.8;
    
    // Bigger threshold means bigger clusters
    float threshold = 700;
    
    // Weed out clusters that are smaller than this size
    int min_component_size = 50;
    
    EGBS egbs;
    egbs.applySegmentation(image, sigma, threshold, min_component_size );
    
    Mat random_color  = egbs.recolor(true);
    Mat average_color = egbs.recolor(false);
    
    imshow( "original", image );
    imshow( "average color", average_color );
    imshow( "random color", random_color );
    double t2 = countTime();
    cout << (t2-t1)/CLOCKS_PER_SEC << endl;
    //waitKey();
    
    SLIC slic(image, 500, 20, 10);
    slic.generateSuperPixels();
    
    SLICSpectralClustering segmenter(image.size(), 1.0f);
    vector<ColorRep>centers = slic.getCenters();
    Mat cluster_idx = slic.getClustersIndex();
    
    
    segmenter.calculateEigenvectors( centers, slic.getS(), slic.getM());
    Mat mask = segmenter.applySegmentation(2, cluster_idx);
    
    Mat img1, img2;
    image.copyTo( img1, mask == 0);
    image.copyTo( img2, mask == 1);
    
    imshow("0", img1);
    imshow("1", img2);
    
    Mat res = slic.recolor();
    cvtColor(res, res, CV_Lab2BGR);
    imshow("SLIC", res);
    vector<Point2i>contour = slic.getContour();
    slic.drawContour(res, contour);
    imshow("SLIC_Contours", res);
    
    
    waitKey(0);
    return 0;
}

