#ifndef HUFFTREE_H
#define HUFFTREE_H

#include "MinHeap.h"
#include "TreeNode.h"
#include <string>
#include <vector>

#define MAXCODELENGTH 256
#define BYTESIZE 8

/*
 * HuffTree.h
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

using namespace std;

class HuffTree
{
	private:
		//Add class members and methods			
		void generateCodesRecursion(TreeNode * root, string bitCode);
		TreeNode * root;		
		
		
	public:
		
		vector<char> charVector;
		vector<string> stringBitVector;
		HuffTree();

		//build a hiffman  tree  give a minheap
		void buildTree(MinHeap * mh);

		//generate codes by traversing the huffman tree
		//and store them in an internal data structure (array of strings for example)
		void generateCodes();
	

		//returns root of the tree
		TreeNode * getRoot();

		//returns huffman code from  the ascii code
		string getCharCode(int c);
		~HuffTree();
		
};


#endif
