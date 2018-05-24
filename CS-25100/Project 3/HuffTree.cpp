#include <iostream>
#include "HuffTree.h"
#include "TreeNode.h"
#include "MinHeap.h"

/*
 * HuffTree.cpp
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

using namespace std;

HuffTree::HuffTree(){ }

TreeNode * HuffTree::getRoot(){ return this->root; }

void HuffTree::buildTree(MinHeap * mh){
	TreeNode * rightChild;
	TreeNode * leftChild;
	TreeNode * parent;	
	while( mh->getSize() > 1 ){
		leftChild = mh->removeMin();
		rightChild = mh->removeMin();
		parent = new TreeNode((char)(0), leftChild->getFrequency() + rightChild->getFrequency());
		parent->join(leftChild,rightChild);
		mh->insert(parent);
	}
	mh->removeMin();
	this->root = parent;
}

void HuffTree::generateCodes(){
	this->generateCodesRecursion(this->getRoot(),"");
}

void HuffTree::generateCodesRecursion(TreeNode * rootNode, string bitCode){

	if(rootNode->isLeafNode() == true){
		charVector.push_back(rootNode->getVal());
		stringBitVector.push_back(bitCode);
		return;
	}
	this->generateCodesRecursion(rootNode->getLeft(), bitCode + "1" );
	this->generateCodesRecursion(rootNode->getRight(),bitCode + "0");
}

string HuffTree::getCharCode(int asciiCode){
	for(int x = 0; x < charVector.size();x++){
		if(asciiCode == (char)(charVector[x])){
			return stringBitVector[x];	
		}
	}
	return NULL;
}


void Delete(TreeNode * node){
	if(node->isLeafNode())
		delete node;
	else{
		Delete(node->getLeft());
		Delete(node->getRight());
		delete node;	
	}
		
}

HuffTree::~HuffTree(){
	Delete(this->root);
}

