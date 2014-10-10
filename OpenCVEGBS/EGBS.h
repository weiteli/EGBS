
#include <opencv2/opencv.hpp>
#include <iostream>
#include "DisjointSetForest.h"

using namespace std;
using namespace cv;

class EGBS{
public:
	EGBS();
	~EGBS();

	int applySegmentation(Mat& image, float sigma, float threshold, int min_component_size);
	Mat recolor(bool random_color);
	int numofConnectedComponent();

private:
	Mat img;
	Size imageSize;
	DisjointSetForest forest;
	inline float diff(Mat& rgb, int x1, int y1, int x2, int y2);
};