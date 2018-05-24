#ifndef DECODER_H
#define DECODER_H
#include "TreeNode.h"
#include "MinHeap.h"
#include "HuffTree.h"
#include <string>
#include <fstream>
#include <ostream>

/*
 * Decoder.h
 * @version 02/24/2018
 * @author Rohith Ravindranath 
*/

using namespace std;

class Decoder
{
	private:
		unsigned int frequency_table[256];
		string input_file_path;
		MinHeap * heap;
		HuffTree * tree;
		int numOfUniqueChars;
		ifstream inputFile;
		TreeNode * node;
		
		bool goRight(ofstream& file);
		bool goLeft(ofstream& file);		
		int get1Byte(ifstream& file);
		int get2Byte(ifstream& file);
		int get4Byte(ifstream& file);
		int get1Bit(ifstream& file);
		// You need to add more class memeber and methods

	public:
		//huff_file_path is the input (encoded) file that we 
		//want to decode
		Decoder(string huff_file_path);

		//Fills up the frequency_table array where frequency_table[i] 
		//will contain the frequency of char with ASCII code i			
		//This method will read the header of the encoded file to 
		//extract the chars and their frequency
		void buildFrequencyTableFromFile();

		//Creates a min-heap and builds the Huffman tree
		void decode();

		//Writes the uncompressed file and save it as file_path
		void writeUncompressedFile(string file_path);

		~Decoder();

};

#endif
