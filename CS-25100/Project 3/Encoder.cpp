#include <iostream>
#include "TreeNode.h"
#include "MinHeap.h"
#include "Encoder.h"
#include "HuffTree.h"
#include <fstream>
#include <bitset>
#include <string>

/*
 * Encoder.cpp
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

using namespace std;

Encoder::Encoder(string file_path):frequency_table{0}{ 
	this->input_file_path = file_path;
	this->heap = new MinHeap();
	this->tree = new HuffTree();
}

void Encoder::buildFrequencyTable(){
	ifstream inputFile;
	inputFile.open(input_file_path);
	if(inputFile.fail()){
		return;
	}
	char value;
	while(inputFile.get(value)){
			unsigned char convert = value;			
			frequency_table[convert]++;
	}
	inputFile.close();
}

void Encoder::encode(){
	this->buildFrequencyTable();
	for(int x = 0; x < 256; x++){
		if(frequency_table[x] != 0){
			TreeNode * node = new TreeNode(x,frequency_table[x]);
			this->heap->insert(node);
		}	
	}
	this->tree->buildTree(heap);
	this->tree->generateCodes();
}


void Encoder::writeEncodedFile(string output_file_path){
	ofstream encodedFile;
	//HEADER	
	encodedFile.open(output_file_path, ios::out | ios::binary);
	if (encodedFile.fail())
		return;
	int numOfUniqueChars = this->tree->charVector.size();
	encodedFile.write((const char *)&numOfUniqueChars,2);
	for(int x = 0; x < 256; x++){
		if(frequency_table[x] != 0){
			int charFrequency = frequency_table[x];;
			encodedFile.write((const char *)&x,1);
			encodedFile.write((const char *)&charFrequency,4);
		}	
	}
	//BODY
	ifstream inputFile;
	inputFile.open(input_file_path);
	if(inputFile.fail())
		return;
	char c;
	string bitCodeLine = "";
	while(inputFile.get(c) ){
		int asciiVal = (int)(c);
		string charCode = this->tree->getCharCode(asciiVal);
		bitCodeLine += charCode;
	}
	int index = 0;
	string convert;
	while(index < bitCodeLine.length()){
		int eightBitCount = 0;
		convert = "";
		while(eightBitCount < 8 && index < bitCodeLine.length() ){
			convert+= bitCodeLine[index];		
			eightBitCount++;
			index++;
		}
		int convertLength = convert.length();
		while(convertLength < 8){
			convert += "0";
			convertLength++;		
		}
		bitset<8> bit(convert);
		unsigned long bits = bit.to_ulong();
		encodedFile.write((const char *)&bits,1);	
	}
	inputFile.close();
	encodedFile.close();
	
}

Encoder::~Encoder(){
	delete heap;
	delete tree;
}


