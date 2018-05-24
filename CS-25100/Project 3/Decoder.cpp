#include <iostream>
#include "TreeNode.h"
#include "MinHeap.h"
#include "HuffTree.h"
#include <fstream>
#include <bitset>
#include <string>
#include "Decoder.h"
#include <bitset>

/*
 * Decoder.cpp
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

Decoder::Decoder(string huff_file_path):frequency_table{0}{
	this->input_file_path = huff_file_path;
	this->heap = new MinHeap();
	this->tree = new HuffTree();
	this->numOfUniqueChars = 0;
	inputFile.open(this->input_file_path, ios::in | ios::binary);
	if(inputFile.fail())
		return;
}

void Decoder::buildFrequencyTableFromFile(){
	this->numOfUniqueChars = this->get2Byte(this->inputFile);
	int index;
	int frequencyValue;	
	for(int x = 0; x < this->numOfUniqueChars; x++){
		index = this->get1Byte(this->inputFile);
		frequencyValue = this->get4Byte(this->inputFile);
		this->frequency_table[index] = frequencyValue;
	}
}


int Decoder::get4Byte(ifstream& file){
	unsigned char byte2;
	unsigned char byte1;
	unsigned char byte3;
	unsigned char byte4;
	file.read(reinterpret_cast<char *>(&byte1),1);
	file.read(reinterpret_cast<char *>(&byte2),1);
	file.read(reinterpret_cast<char *>(&byte3),1);
	file.read(reinterpret_cast<char *>(&byte4),1);
	unsigned int answer = byte1 | (byte2 << 8) | (byte3 << 16) | (byte4 << 24);
	return answer;
}

int Decoder::get2Byte(ifstream& file){
	unsigned char byte2;
	unsigned char byte;
	file.read(reinterpret_cast<char *>(&byte),1);
	file.read(reinterpret_cast<char *>(&byte2),1);
	return (unsigned int)(byte2 << 8 | byte);
}

int Decoder::get1Byte(ifstream& file){
	unsigned char byte; 
	file.read(reinterpret_cast<char *>(&byte),1);
	return (unsigned int)byte;
}


void Decoder::decode(){
	this->buildFrequencyTableFromFile();
	for(int x = 0; x < 256; x++){
		if(frequency_table[x] != 0){
			TreeNode * nodes = new TreeNode(x,frequency_table[x]);
			this->heap->insert(nodes);
			//delete nodes;
		}	
	}
	this->tree->buildTree(this->heap);
	this->node = tree->getRoot();
}

bool Decoder::goRight(ofstream& file){
	this->node = this->node->getRight();
	if(this->node->isLeafNode()){
		if(this->frequency_table[this->node->getVal()] > (unsigned int)0){
			this->frequency_table[this->node->getVal()]--;
			file <<(char)this->node->getVal();
			this->node = tree->getRoot();
			return true;	
		}
		return false;
	}
	return true;
}

bool Decoder::goLeft(ofstream& file){
	this->node = this->node->getLeft();
	if(this->node->isLeafNode()){
		if(this->frequency_table[this->node->getVal()] > (unsigned int)0){
			this->frequency_table[this->node->getVal()]--;
			file <<(char)this->node->getVal();
			this->node = tree->getRoot();
			return true;	
		}
		return false;
	}
	return true;
}

void Decoder::writeUncompressedFile(string file_path){
	ofstream outputFile;
	outputFile.open(file_path);
	char c;
	bool done = true;
	while(inputFile.get(c)){
		for(int x = 7; x >= 0 ; x--){
			if (((c >> x )&1) == 1){
				if(!this->goLeft(outputFile)){
					done = false;
					break;	
				}
			}
			else {
				if(!this->goRight(outputFile)){
					done = false;				
					break;
				}
			}
		}
		if(!done)
			break;
	}
	outputFile.close();
	inputFile.close();
}

Decoder::~Decoder(){ 
	
	delete heap;
	delete tree;
}
