#include <iostream>
#include <climits>
#include <cstring>
#include <cstdint>

#include "MinHeap.h"
#define ARRAYMAX 10000

/*
 * MinHeap.cpp
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

using namespace std;
//implement the methods in MinHeap.h

MinHeap::MinHeap() { currentElements = 0; }

int MinHeap::parentIndex(int childIndex){ return (childIndex-1)/2; }

int MinHeap::rightChildIndex(int parentIndex){ return 2*parentIndex +2; }

int MinHeap::leftChildIndex(int parentIndex){ return 2*parentIndex +1; }

int MinHeap::getSize() { return this->currentElements; }

void MinHeap::insert(TreeNode * val){
	vectorHeap.push_back(val);
	currentElements++;
	this->bubbleUp(currentElements -1);
}

void MinHeap::bubbleUp(int position){
	int child = currentElements -1;
	int parent = this->parentIndex(position);
	while(child > 0){
		if(vectorHeap[child]->getFrequency() >= vectorHeap[parent]->getFrequency() )
			break;
		TreeNode * temp = vectorHeap[child];
		vectorHeap[child] = vectorHeap[parent];
		vectorHeap[parent] = temp;
		child = parent;
		parent = this->parentIndex(child);
				
	}
}



TreeNode * MinHeap::removeMin() {
	if(currentElements == 0)
		return NULL;
	minNode = vectorHeap[0];
	vectorHeap[0] = vectorHeap[currentElements-1];
	vectorHeap.erase(vectorHeap.end()-1);
	bubbleDown(0);
	currentElements--;
	return minNode;
}

void MinHeap::bubbleDown(int position){
	int rightChild = this->rightChildIndex(position);
	int leftChild = this->leftChildIndex(position);
	int minIndex = position;
	if(leftChild < currentElements){
		if(vectorHeap[leftChild]->getFrequency() < vectorHeap[position]->getFrequency())
			minIndex = leftChild;
	}
	if(rightChild < currentElements){
		if( vectorHeap[rightChild]->getFrequency() < vectorHeap[minIndex]->getFrequency())
			minIndex = rightChild;
	} 
	if(minIndex != position){
			TreeNode * temp = vectorHeap[minIndex];
			vectorHeap[minIndex] = vectorHeap[position];
			vectorHeap[position] = temp;
			bubbleDown(minIndex);
			
	}
}


MinHeap::~MinHeap(){ 
	for(int x = 0; x < vectorHeap.size(); x++)
		delete vectorHeap[x];
}

