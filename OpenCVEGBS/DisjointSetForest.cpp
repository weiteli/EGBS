
#include "DisjointSetForest.h"


DisjointSetForest::DisjointSetForest(){

}

DisjointSetForest::DisjointSetForest(int num_element){
	init(num_element);
}

DisjointSetForest::~DisjointSetForest(){

}

void DisjointSetForest::init(int num_element){
	this->elements.clear();
	this->elements.reserve(num_element);
	this->num = num_element;

	for (int i=0; i<num_element; i++){
		elements[i].rank = 0;
		elements[i].parent = i;
		elements[i].size = 1;
	}
}

int DisjointSetForest::size(int x){
	return elements[x].size;
}

int DisjointSetForest::numOfElement(){
	return num;
}

//Find the parent
int DisjointSetForest::find(int x){
	int tmp = x;
	while(tmp != elements[tmp].parent){
		tmp = elements[tmp].parent;
	}
	elements[x].parent = tmp;
	return tmp;
}

//Coonect two edges
void DisjointSetForest::connect(int x, int y){
	if (elements[x].rank >= elements[y].rank){
		elements[y].parent = x;
		elements[x].size += elements[y].size;
	} else{
		elements[x].parent = y;
		elements[y].size += elements[x].size;
        if( elements[x].rank == elements[y].rank )
            elements[y].rank++;
	}
	num--;
}

void DisjointSetForest::segmentGraph(int num_Vertices, vector<Edge>&edges, float c){
	init(num_Vertices);
	sort(edges.begin(), edges.end(), WeightComparator());
    
    vector<float> threshold(num_Vertices,c);
    /*
    for (int i=0; i<edges.size(); i++) {
        int tmp_a = this->find(edges[i].a);
        int tmp_b = this->find(edges[i].b);
        if (tmp_a != tmp_b){
            if (edges[i].weight <= threshold[tmp_a] && edges[i].weight <= threshold[tmp_b]) {
                this -> connect(tmp_a, tmp_b);
                tmp_a = this -> find(tmp_a);
                threshold[tmp_a] = edges[i].weight +c / this -> size(tmp_a);
            }
        }
    }
    */
    
    for (vector<Edge>::iterator it = edges.begin(); it!= edges.end(); it++){
        int tmp_a = this -> find((*it).a);
        int tmp_b = this -> find((*it).b);
        if (tmp_a != tmp_b){
            if ((*it).weight <= threshold[tmp_a] && (*it).weight <= threshold[tmp_b]) {
                this -> connect(tmp_a, tmp_b);
                tmp_a = this -> find(tmp_a);
                threshold[tmp_a] = (*it).weight +c / this -> size(tmp_a);
            }
        }
    }
    

}



