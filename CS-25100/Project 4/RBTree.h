#ifndef PROJ4_RBTREE_H
#define PROJ4_RBTREE_H
#include <queue>
#include "Node.h"
#include <iostream>
#include <cstdlib>
#include <stdexcept>
/*
 * RBTree.h
 * @author Rohith Ravindranath
 * @version March 19, 2018
*/

using namespace std;

template <typename Key, typename Element>
class RBTree {
 private:
	
	//ATTRIBUTES
  	size_t sizeOfTree;
	Node <Key, Element>* root; 
 
	//HELPER METHODS
	void inorderRecurssion(Node<Key,Element> *,ostream& out);
  	void levelOrderRecurssion(ostream& out, Node<Key,Element> *, int);
  	void leftRotation(Node<Key,Element> * node);
  	void rightRotation(Node<Key,Element> * node);
	Node<Key, Element>* insertNode(const Key& key, const Element& element);

 public:
	
	//CONSTRUCTOR                                                    
	RBTree() { 
		this->sizeOfTree = 0; 
		this->root = NULL;  	
	}

	//ACTUAL METHODS THAT WILL BE CALLED
    	void insert(const Key& k, const Element& e);
    	Node<Key, Element>* search(const Key& k);
    	void del(const Key& k); // TODO 
    	void inorder(ostream& out);
    	void levelOrder(ostream& out);

	//GETTER METHODS
        size_t size() const;
        Node<Key, Element>* getRoot();
};

template <typename Key, typename Element>
Node<Key, Element>* RBTree<Key, Element>::insertNode(const Key& key, const Element& element){
	Node<Key,Element> *node;
	Node<Key,Element> *parent;
	Node<Key,Element> *tempNode;
	node = this->root;
	//FIND PARENT FOR THE NEW NODE
    
    
    if(this->root == NULL){
        tempNode = root = new Node<Key,Element>;
        tempNode->setKey(key);
        tempNode->setElement(element);
        tempNode->setColor(BLACK);
        return tempNode;
    }

	while(node != NULL){
		parent = node;
		if(key < node->getKey())
			node = node->getLeft();
		else
			node = node->getRight();
	}
	//CONNECT NEW NODE TO THE PARENT NODE WE JUST FOUND
	if(!parent){
		tempNode = root = new Node<Key,Element>;
		tempNode->setKey(key);
		tempNode->setElement(element);
		tempNode->setColor(BLACK);
	} else {
		tempNode = new Node<Key, Element>;
		tempNode->setKey(key);
		tempNode->setElement(element);
		tempNode->setColor(RED);
		tempNode->setParent(parent);
		//SET TO RIGHT OR LEFT OF PARENT
		if(tempNode->getKey() < parent->getKey())
			parent->setLeft(tempNode);
		else
			parent->setRight(tempNode);
	}
	return tempNode;
}

// Function to insert a new node with given data                                                   
template <typename Key, typename Element>
void RBTree<Key, Element>::insert(const Key& key, const Element& element){
	//INSERT NODE INTO TREE
	Node<Key,Element> * node = this->insertNode(key,element);
	//CHECK VIOLATIONS
	Node<Key,Element> * uncle;
 	bool sideOfParent;
	while (node->getParent() && node->getParent()->getColor() == RED){
		sideOfParent = (node->getParent() == node->getParent()->getParent()->getLeft());
		//GET UNCLE
        if(sideOfParent){
			uncle = node->getParent()->getParent()->getRight();
        }
        else{
            uncle = node->getParent()->getParent()->getLeft();
        }
		//CASE 1
      		if (uncle && uncle->getColor() == RED){
          		node->getParent()->setColor(BLACK);
          		uncle->setColor(BLACK);
          		node->getParent()->getParent()->setColor(RED);
          		node = node->getParent()->getParent();
        	}
		//CASE 2
		else {
			//RIGHT SIDE
          		if( sideOfParent && node == node->getParent()->getRight()){
              			node = node->getParent();
				this->leftRotation(node);
            		}
			// LEFT SIDE
			else if(!sideOfParent &&  node ==  node->getParent()->getLeft()){
              			node = node->getParent();
				this->rightRotation(node);
            		}
          		node->getParent()->setColor(BLACK);
          		node->getParent()->getParent()->setColor(RED);
			if (sideOfParent)
				this->rightRotation(node->getParent()->getParent());
			else
				this->leftRotation(node->getParent()->getParent());	
        	}
    	}
  	this->root->setColor(BLACK);
  	this->sizeOfTree++;
}

                                                                                      
template <typename Key, typename Element>
void RBTree<Key, Element>::leftRotation(Node<Key,Element> *node){
	Node<Key, Element> *tempNode;
	tempNode = node->getRight();
	node->setRight(tempNode->getLeft());
  	if(tempNode->getLeft())
      		tempNode->getLeft()->setParent(node);
	tempNode->setParent(node->getParent());
  	tempNode->setLeft(node);
  	if (!node->getParent())
      		root = tempNode;
	else if (node == node->getParent()->getLeft())
      		node->getParent()->setLeft(tempNode);
	else 
      		node->getParent()->setRight(tempNode);
  	node->setParent(tempNode);
}

                                                                                   
template <typename Key, typename Element>
void RBTree<Key, Element>::rightRotation(Node<Key,Element> *node){
  	Node<Key,Element> * tempNode;
  	tempNode = node->getLeft();
  	node->setLeft(tempNode->getRight());
  	if (tempNode->getRight())
      		tempNode->getRight()->setParent(node);
  	tempNode->setParent(node->getParent());
 	tempNode->setRight(node);
  	if (!node->getParent())
      		root = tempNode;
    	else if ( node == node->getParent()->getLeft())
      		node->getParent()->setLeft(tempNode);
    	else 
      		node->getParent()->setRight(tempNode);
  	node->setParent(tempNode);
}
                                                                    
template <typename Key, typename Element>
Node<Key, Element>* RBTree<Key, Element>::search(const Key& k) {
  	Node<Key,Element> * node;
	node = root;
  	while(node){
      		if (node->getKey() > k)
          		node = node->getLeft();
		else if (node->getKey() < k)
          		node = node->getRight();
		else
          		return node;
    	}
  	return NULL;
}

                                                                            
template <typename Key, typename Element>
void RBTree<Key, Element>::inorder(ostream& out){
  	if (this->root != NULL)                                                                    
  		inorderRecurssion(this->root,out);
}

template <typename Key, typename Element>
void RBTree<Key, Element>::inorderRecurssion(Node<Key,Element> * node, ostream& out){
  	if (node != NULL){
	  	inorderRecurssion(node->getLeft(),out);
	   	out<<* node<<endl;
	  	inorderRecurssion(node->getRight(),out);
	}
}
                                                          
template <typename Key, typename Element>
void RBTree<Key, Element>::levelOrder(ostream& out){
	for (int x = 1; x < this->sizeOfTree; x++)
		levelOrderRecurssion(out, this->root, x);
}

template <typename Key, typename Element>
void RBTree<Key, Element>::levelOrderRecurssion(ostream & out, Node<Key,Element> * node, int level) {
	if (node == NULL)
		return ;
	else if (level ==1)
		out << *node << "\n";
	else {
		levelOrderRecurssion(out, node->getLeft(), level - 1);
		levelOrderRecurssion(out, node->getRight(), level - 1);
	}
}

template <typename Key, typename Element>
size_t RBTree<Key, Element>::size() const { return this->sizeOfTree;  }


template <typename Key, typename Element>
Node<Key, Element>* RBTree<Key, Element>::getRoot() {  return this->root; }

// The following functions have been provided for you, and do not
// need to be modified:


// Output stream operator overload -- writes the contents of the
// entire RBTree to the specified output stream, using an in-order
// traversal. Note that there must be a similar operator overload
// for Key and Element types in order for this to compile.
// Example usage:
//        RBTree<K,E> rbt;
//        cout << rbt << endl;
template <typename Key, typename Element>
ostream& operator<<(ostream& out, const RBTree<Key, Element>& rbtree) {
    out << rbtree.size() << endl;
    rbtree.inorder(out);
    return out;
}

// Input stream operator overload -- reads the contents of an RBTree
// from the specified input stream, inserting each Node as it is
// read. Note that there must be a similar operator overload for Key
// and Element types in order for this to compile.
// Example usage:
//        RBTree<K,E> rbt;
//        cin >> rbt;
template <typename Key, typename Element>
istream& operator>>(istream& in, RBTree<Key, Element>& rbtree) {
    // Clear existing contents, if any
    rbtree = RBTree<Key, Element>();
    
    // Read in the number of elements to read
    size_t size;
    in >> size;
    for (size_t i = 0; i < size; i++) {
        // Read in each node as it appears in the stream
        Node<Key, Element> n;
        in >> n;
        // Insert the node into the RBTree
        rbtree.insert(n.getKey(), n.getElement());
    }
    
    return in;
}


#endif //PROJ4_RBTREE_H
