

#include "EGBS.h"
#include <map>

EGBS::EGBS(){
}

EGBS::~EGBS(){

}

float EGBS::diff(Mat &rgb, int x1, int y1, int x2, int y2){
	Vec3f pix1 = rgb.at<Vec3f>(y1,x1);
	Vec3f pix2 = rgb.at<Vec3f>(y2,x2);
	return sqrt((pix1 - pix2).dot(pix1 - pix2));
}


int EGBS::applySegmentation(Mat& image, float sigma, float threshold, int min_component_size){
    this->img = image.clone();
	this->imageSize = image.size();
	
	Mat smooth;
	image.convertTo(smooth, CV_32FC3);
	GaussianBlur(smooth, smooth, Size(5,5), sigma);
    
    vector<Edge>edges;
    
	for (int y = 0; y<imageSize.height; y++){
		for (int x =0; x<imageSize.width; x++){
            Edge tmp;
            if (x+1 < imageSize.width){
                tmp.a = y*imageSize.width +x;
                tmp.b = y*imageSize.width +x+1;
                tmp.weight = diff(smooth, x, y, x+1, y);
                edges.push_back(tmp);
            }
            
            if (y+1 < imageSize.height){
                tmp.b = y*imageSize.width +x+1;
                tmp.weight = diff(smooth, x, y, x, y+1);
                edges.push_back(tmp);
            }
            
            if ((x+1 < imageSize.width) && (y+1 < imageSize.height)){
                tmp.b = (y+1)*imageSize.width +x+1;
                tmp.weight = diff(smooth, x, y, x+1, y+1);
                edges.push_back(tmp);
            }
            
            if ((x+1 < imageSize.width) && (y >0)){
                tmp.b = (y-1)*imageSize.width +x+1;
                tmp.weight = diff(smooth, x, y, x+1, y-1);
                edges.push_back(tmp);
            }
		}
	}

	forest.segmentGraph((int)edges.size(), edges, threshold);
    
    for(int i=0; i<edges.size(); i++) {
        int a = forest.find(edges[i].a );
        int b = forest.find(edges[i].b );
        if( (a != b) && (( forest.size(a) < min_component_size) || (forest.size(b) < min_component_size)) ) {
            forest.connect(a, b);
        }
    }

	return forest.numOfElement();
}

int EGBS::numofConnectedComponent(){
	return forest.numOfElement();
}

Mat EGBS::recolor(bool random_color){
	Mat result(imageSize, CV_8UC3, Scalar(0,0,0));
    map<int, Vec3f> colors;
	if (!random_color){
        map<int, int>count;
        for (int y =0; y<imageSize.height; y++){
            Vec3b *ptr = img.ptr<Vec3b>(y);
            for (int x=0; x<imageSize.width; x++){
                int component = forest.find(y*imageSize.width+x);
                Vec3f c = Vec3f(ptr[x][0], ptr[x][1], ptr[x][2]);
                colors[component] = colors[component] + c;
                count[component]++;
            }
        }
        for (map<int, Vec3f>::iterator it = colors.begin(); it != colors.end(); it++){
            colors[it->first] = colors[it->first] /count[it->first];
        }
        
	} else {
        for (int y=0; y<imageSize.height; y++){
            for (int x=0; x<imageSize.width; x++){
                int component = forest.find(y*imageSize.width+x);
                if (colors.count(component)==0) {
                    colors[component] = Vec3f(rand() % 255, rand() % 255, rand() % 255);
                }
            }
        }
	}
    
    for (int y=0; y<imageSize.height; y++){
        Vec3b *ptr = result.ptr<Vec3b>(y);
        for (int x=0; x<imageSize.width; x++){
            int component = forest.find(y*imageSize.width+x);
            Vec3f cc = colors[component];
            ptr[x] = Vec3b( cc[0], cc[1], cc[2]);
        }
    }
    
    return result;
}


