//
//  SLIC.h
//  OpenCVEGBS
//
//  Created by Wei-Te Li on 14/9/8.
//  Copyright (c) 2014年 wade. All rights reserved.
//

#ifndef __OpenCVEGBS__SLIC__
#define __OpenCVEGBS__SLIC__

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

struct ColorRep;

class SLIC{
public:
    SLIC();
    ~SLIC();
    SLIC(Mat &src, int num_superpixel, int m = 10, int max_iterations=10);
    void init(Mat &src, int num_superpixel, int m = 10, int max_iterations=10);
    void generateSuperPixels();
    vector<Point2i>getContour();
    Mat recolor();
    void drawContour(Mat &input, vector<Point2i>&contours);
    vector<ColorRep> getCenters();
    vector<Point2i> getClusterCenters();
    Mat getClustersIndex();
    int getS();
    int getM();
private:
    Mat cluster;
    Mat distance;
    Mat image;
    vector<ColorRep>center;
    vector<int>count;
    int k;
    int S;
    int m;
    int max_iteration;
    Point2i findLocalMinimum(Mat& image, Point2i center);
    inline bool withinRange(int x, int y);
    double calcDist(ColorRep &center, Vec3b &p, int x, int y);
    void clear_data();
};



struct ColorRep{
    float l = 0;
    float a = 0;
    float b = 0;
    float x = 0;
    float y = 0;
    
    ColorRep(){}
    
    ColorRep( Vec3b& color, Point2i coord ) {
        init( color, coord.x, coord.y );
    }
    
    ColorRep( Vec3b& color, int x, int y ) {
        init( color, x, y );
    }
    
    void init( Vec3b& color, int x, int y ) {
        this->l = color[0];
        this->a = color[1];
        this->b = color[2];
        this->x = x;
        this->y = y;
    }
    
    void add( Vec3b& color, int x, int y ) {
        this->l += color[0];
        this->a += color[1];
        this->b += color[2];
        this->x += x;
        this->y += y;
    }
    
    void divColor( float divisor ) {
        this->l /= divisor;
        this->a /= divisor;
        this->b /= divisor;
    }
    
    void div( float divisor ) {
        this->l /= divisor;
        this->a /= divisor;
        this->b /= divisor;
        this->x /= divisor;
        this->y /= divisor;
    }
    
    double colorDist( const ColorRep& other ) {
        return (this->l - other.l) * (this->l - other.l)
        + (this->a - other.a) * (this->a - other.a)
        + (this->b - other.b) * (this->b - other.b);
    }
    
    
    double coordDist( const ColorRep& other ) {
        return (this->x - other.x) * (this->x - other.x)
        + (this->y - other.y) * (this->y - other.y);
    }
    
    string toString() {
        stringstream ss;
        ss << l << " " << a << " " << b << " " << x << " " << y;
        return ss.str();
    }
};



#endif /* defined(__OpenCVEGBS__SLIC__) */
