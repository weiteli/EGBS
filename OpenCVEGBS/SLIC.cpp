//
//  SLIC.cpp
//  OpenCVEGBS
//
//  Created by Wei-Te Li on 14/9/8.
//  Copyright (c) 2014年 wade. All rights reserved.
//

#include "SLIC.h"

SLIC::SLIC(){
}

SLIC::~SLIC(){
    clear_data();
}

void SLIC::clear_data(){
    if( !image.empty() )
        image.release();
    
    center.clear();
    count.clear();
}

SLIC::SLIC(Mat &src, int num_superpixel, int m, int max_iterations){
    init(src, num_superpixel, m, max_iterations);
}


void SLIC::init(Mat &src, int num_superpixel, int m, int max_iterations){
    
    this->S = (int)sqrt(1.0 * src.rows * src.cols / num_superpixel);
    this->k = num_superpixel;
    this->m = m;
    this->max_iteration = max_iterations;
    
    cvtColor(src, image, CV_BGR2Lab);
    
    for (int y=S; y<image.rows-S/2; y+=S) {
        for (int x=S; x<image.cols-S/2; x+=S){
            Point2i adjust_cen = findLocalMinimum(image, Point2i(x,y));
            Vec3b color = image.at<Vec3b>(adjust_cen.y, adjust_cen.x);
            center.push_back(ColorRep(color, x, y));
        }
    }
    
    cluster = Mat(image.size(), CV_32SC1, Scalar(-1));
    distance = Mat(image.size(), CV_64FC1, Scalar(FLT_MAX));
    count = vector<int>(center.size(),0);
}


//Find local minimum within 3x3 region from the center position
Point2i SLIC::findLocalMinimum(Mat &img, Point2i center){
    Point2i minimumPoint(center.x, center.y);
    float min_grad = FLT_MAX;
    for (int y= center.y-1; y<center.y+2; y++) {
        for (int x = center.x-1; x<center.x+2; x++) {
            Vec3b a = image.at<Vec3b>(y,x);
            Vec3b b = image.at<Vec3b>(y+1,x);
            Vec3b c = image.at<Vec3b>(y,x+1);
            float grad_diff = abs(b[0]-a[0])+abs(c[0]-a[0]);
            if (grad_diff < min_grad){
                min_grad = grad_diff;
                minimumPoint.x = x;
                minimumPoint.y = y;
            }
        }
    }
    return minimumPoint;
}


void SLIC::generateSuperPixels(){
    for (int iter = 0; iter < max_iteration; iter++){
        distance = Scalar(FLT_MAX);
        
        for (int i=0; i<center.size(); i++) {
            ColorRep tmp_center = center[i];
            for (int y=tmp_center.y-S; y<tmp_center.y+S; y++) {
                
                Vec3b *ptr = image.ptr<Vec3b>(y);
                int *cluster_ptr = cluster.ptr<int>(y);
                double *dist_ptr = distance.ptr<double>(y);
                
                for (int x = tmp_center.x -S; x<tmp_center.x+S; x++){
                    if (withinRange(x, y)) {
                        Vec3b color = ptr[x];
                        //calculate the distance
                        double dist = calcDist(tmp_center, color, x,y);
                        if (dist < dist_ptr[x]){
                            dist_ptr[x] = dist;
                            cluster_ptr[x] = i;
                        }
                    }
                }
            }
        }
        
        center.assign(center.size(), ColorRep());
        count.assign(count.size(), 0);
        
        
        for (int y=0; y<image.rows; y++) {
            int *cluster_ptr = cluster.ptr<int>(y);
            for (int x=0; x<image.cols; x++){
                int cluster_id = cluster_ptr[x];
                if (cluster_id != -1){
                    Vec3b color = image.at<Vec3b>(y,x);
                    center[cluster_id].add(color, x, y);
                    count[cluster_id]++;
                }
            }
        }
        
        for (int i=0; i<center.size(); i++){
            center[i].div(count[i]);
        }
        
    }
}


inline bool SLIC::withinRange(int x, int y){
    return (x >= 0 && y >= 0 && x < image.cols && y < image.rows);
}


double SLIC::calcDist(ColorRep &cen, Vec3b &p, int x, int y){
    double dc = (cen.l-p[0])*(cen.l-p[0]) + (cen.a-p[1])*(cen.a-p[1]) + (cen.b-p[2])*(cen.b-p[2]);
    double ds = (cen.x -x)*(cen.x -x) + (cen.y -y)*(cen.y -y);
    return sqrt( dc + ds / (S * S) * (m * m) );
}


Mat SLIC::recolor(){
    Mat result = image.clone();
    vector<Vec3f>color(center.size());
    
    for (int y=0; y<result.rows; y++){
        int *cluster_ptr = cluster.ptr<int>(y);
        Vec3b *ptr = result.ptr<Vec3b>(y);
        for (int x=0; x<result.cols; x++) {
            color[cluster_ptr[x]] += ptr[x];
        }
    }
    
    for (int i=0; i<color.size(); i++) {
        color[i] /= count[i];
    }
    
    for (int y=0; y<result.rows; y++) {
        Vec3b *ptr = result.ptr<Vec3b>(y);
        int *cluster_ptr = cluster.ptr<int>(y);
        for (int x=0; x<result.cols; x++) {
            int cluster_idx = cluster_ptr[x];
            Vec3b c = color[cluster_idx];
            ptr[x] = Vec3b(c[0], c[1], c[2]);
        }
    }
    
    return result;
}


vector<Point2i>SLIC::getContour(){
    const int dx[8] = {-1, -1, -1, 0,  0,  1, 1, 1};
    const int dy[8] = { 1,  0, -1, 1, -1, -1, 0, 1};

    vector<Point2i>contours;
    vector<vector<bool> >visit(image.rows, vector<bool>(image.cols, false));
    
    for (int y=0; y<image.rows; y++) {
        int *cluster_ptr = cluster.ptr<int>(y);
        for (int x=0; x<image.cols; x++){
            int bd = 0;
            for (int i=0; i<8; i++){
                int nx = x+dx[i];
                int ny = y+dy[i];
                if (withinRange(nx, ny)){
                    if (!visit[ny][nx] &&  cluster_ptr[x] != cluster.at<int>(ny, nx)) {
                        bd++;
                        if (bd >1) {
                            break;
                        }
                    }
                }

                if (bd >= 1) {
                    contours.push_back(Point2i(x,y));
                    visit[y][x] = true;
                }
            }
        }
    }
    return contours;
}

void SLIC::drawContour(Mat &input, vector<Point2i>&contours){
    for (int i=0; i<contours.size(); i++){
        input.at<Vec3b>( contours[i].y, contours[i].x ) = Vec3b(255, 0, 255);
    }
}


vector<ColorRep>SLIC::getCenters(){
    //return vector<ColorRep>(center);
    return this->center;
}

vector<Point2i> SLIC::getClusterCenters(){
    vector<Point2i> result(center.size());
    
    for(int i=0; i<center.size(); i++){
        result[i].x = center[i].x;
        result[i].y = center[i].y;
    };
    
    return result;
}

Mat SLIC::getClustersIndex() {
    //return cluster.clone();
    return this->cluster;
}


int SLIC::getM(){
    return this->m;
}

int SLIC::getS(){
    return this->S;
}