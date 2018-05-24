#include <iostream>
#include "TreeNode.h"

#include <string>

/*
 * TreeNode.cpp
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

TreeNode::TreeNode(unsigned char value, unsigned int frequency)
{
	this->val = value;
	this->frequency = frequency;
	left = NULL;
	right = NULL;
}

bool TreeNode::isLeafNode(){
	if(this->left == NULL && this->right == NULL)
		return true;
	return false;
}

TreeNode * TreeNode::getLeft(){
	return this->left;
}

TreeNode * TreeNode::getRight(){
	return this->right;
}

void TreeNode::join(TreeNode * left,TreeNode * right){
	if(left != NULL)
		this->left = left;
	if (right != NULL)
		this->right = right;
}

unsigned TreeNode::getFrequency()
{
	return this->frequency;
}
unsigned TreeNode::getVal()
{
	return this->val;
}

TreeNode::~TreeNode(){
	
}
