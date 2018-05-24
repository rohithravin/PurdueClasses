#include <iostream>
#include <string>
#include "MinHeap.h"
#include "Encoder.h"
#include "Decoder.h"

/*
 * main.cpp
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

using namespace std;

void printHelp(){
	cout << "Invalid arguments." << endl;
	cout << "Type \"huffman [mode] [arguments]\" where mode and its arguments are one of the following:" << endl;
	cout << "-e [src] [dest]: encode the source file and place the result into the specified destination" << endl;
	cout << "-d [src] [dest]: decode the source file and place the result into the specified destination" << endl;
	exit(1);
}


int main (int argc, char** argv){
	if(argc == 4){
		string mode = argv[1];
		if(mode.compare( "-e") ==0){
			Encoder* code = new Encoder(argv[2]);
			code->encode();
			code->writeEncodedFile(argv[3]);
			delete code;
		}
		else if(mode.compare("-d") ==0){
			Decoder* decode = new Decoder(argv[2]);
			decode->decode();
			decode->writeUncompressedFile(argv[3]);
			delete decode;
			
		} 
	}
	else{
		printHelp();
	}

	return 0;
}
