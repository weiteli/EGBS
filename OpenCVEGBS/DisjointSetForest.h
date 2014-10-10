

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

struct Edge{
	int a;
	int b;
	float weight;
};


struct SetNode{
	int rank;
	int parent;
	int size;
};

struct WeightComparator{
    bool operator()(const Edge& lhs, const Edge& rhs) const{
        return lhs.weight < rhs.weight;
    }
}; 

class DisjointSetForest{
public:
	DisjointSetForest();
	DisjointSetForest(int num_element);
	~DisjointSetForest();
	void init(int num_element);
	int find(int x);
	void connect(int x, int y);
	int size(int x);
	int numOfElement();
	void segmentGraph(int num_vertices, vector<Edge>&edges, float c);
private:
	int num;
	vector<SetNode>elements;
};