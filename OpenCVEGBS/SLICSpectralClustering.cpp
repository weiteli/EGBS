//
//  SLICSpectralClustering.cpp
//  OpenCVEGBS
//
//  Created by Wei-Te Li on 14/9/9.
//  Copyright (c) 2014年 wade. All rights reserved.
//

#include "SLICSpectralClustering.h"

SLICSpectralClustering::SLICSpectralClustering(){
    
}

SLICSpectralClustering::SLICSpectralClustering(Size imageSize, float sigma){
    init(imageSize, sigma);
}

void SLICSpectralClustering::init(Size imgSize, float sigma){
    this->sigma = sigma;
    this->clusterMask = Mat(imgSize, CV_8UC1, Scalar(0));
}

void SLICSpectralClustering::calculateEigenvectors(vector<ColorRep>& clusters_centers, int slic_s, int slic_m){
    
    Mat adjacent = createAdjacency(clusters_centers, slic_s, slic_m);
    Mat degMat = degreeMatrix(adjacent);
    
    Mat deg05;
    pow(degMat, -0.5, deg05);
    deg05 = Mat::diag(deg05);
    
    Mat Lap = Mat::diag(degMat)-adjacent;
    Lap = (deg05*Lap)*deg05;
    
    Mat eigenvalue;
    eigen(Lap, eigenvalue, eigenvectors);
}

Mat SLICSpectralClustering::applySegmentation(int no_of_clusters, Mat& clusters_index){
    if( no_of_clusters < 1 ){
        cout << "Error with number of cluster" << endl;
    }
    if (eigenvectors.empty()){
        cout << "Error when computing eigenvector" << endl;
    }
    
    Mat small_eigv = eigenvectors.rowRange(eigenvectors.rows - no_of_clusters, eigenvectors.rows).t();
    
    Mat label;
    kmeans(small_eigv, no_of_clusters, label, TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 1000, 1e-5), 2, KMEANS_RANDOM_CENTERS);
    
    label = label.t();
    int *labels_ptr = label.ptr<int>();
    
    
    for( int y = 0; y < clusters_index.rows; y++ ) {
        int * cluster_ptr = clusters_index.ptr<int>(y);
        uchar * mask_ptr  = clusterMask.ptr<uchar>(y);
        for( int x = 0; x < clusters_index.cols; x++)
            mask_ptr[x] = labels_ptr[cluster_ptr[x]];
    }
    return clusterMask.clone();
}

Mat SLICSpectralClustering::degreeMatrix(Mat &adj){
    Mat deg(1, adj.cols, CV_32FC1);
    float *dd = deg.ptr<float>(0);
    for (int i=0; i<adj.cols; i++){
        dd[i] = sum(adj.col(i))[0];
    }
    return deg;
}

Mat SLICSpectralClustering::createAdjacency(vector<ColorRep>& points, int slic_s, int slic_m){
    int size = (int)points.size();
    Mat adj(size, size, CV_32FC1, Scalar(0.0));
    double ratio = 1.0 * (slic_m * slic_m)/(slic_s*slic_s);
    double gaussian_dum = 2.0*sigma*sigma;
    
    for(int i=0; i<points.size(); i++){
        float *adj_elem = adj.ptr<float>(i);
        for (int j=0; j<points.size(); j++) {
            if (i>=j) {
                continue;
            }
            double dc = points[i].colorDist(points[j]);
            double ds = points[i].coordDist(points[j]);
            adj_elem[j] =  -sqrt(dc+ds*ratio)/gaussian_dum;
            
        }
    }
    
    completeSymm(adj);
    exp(adj, adj);
    adj = adj - Mat::eye(size, size, CV_32FC1 );
    return adj;
}







