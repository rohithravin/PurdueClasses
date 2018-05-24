#ifndef MINHEAP_H
#define MINHEAP_H

#include "TreeNode.h"
#include <vector>


/*
 * MinHeap.h
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

using namespace std;

class MinHeap
{
	private:
		vector<TreeNode *> vectorHeap;
		int currentElements;
		
	public:
		TreeNode * minNode;
		MinHeap();
		TreeNode * removeMin(); //returns root of heap
		void insert(TreeNode * val); //adds element to heap
		int getSize(); //returns size of heap
		void bubbleUp(int index);
		void bubbleDown(int index);
		int parentIndex(int index);
		int rightChildIndex(int index);
		int leftChildIndex(int index);
		~MinHeap();
};

#endif
