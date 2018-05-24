#include <stdio.h>
#include <sstream>
#include <iostream>
#include "Node.h"
#include "RBTree.h"
#include "SimpleHTMLParser.h"
#include <vector>
#include <string>
#include <fstream>

/*
 * Scan.cpp
 * @author Rohith Ravindranath
 * @version March 19, 2018
*/

using namespace std;

// Output stream operator overload for node elements
ostream& operator<<(ostream& out, const vector<pair<string, int>>& e);
int nodeCount = 0;

int main(int argc, char** argv) {
	string inputFile;
	string outputFile;
	if (argc != 3)
		return -1;
	inputFile = argv[1];
	outputFile = argv[2];
	ifstream fileReader;
	fileReader.open(inputFile);
	if(!fileReader)
		return 0;
	int numOfLinks;
	fileReader >> numOfLinks;
	string url;
	SimpleHTMLParser * parserMachine = new SimpleHTMLParser();
	RBTree<string,vector<pair<string,int>>> * tree = new RBTree<string,vector<pair<string,int>>>();
	//RUN THROUGH EACH URL
	for (int x = 0; x < numOfLinks; x++){
		fileReader >> url;
		//CHECK IF URL CAN BE PARSED
		if(parserMachine->parse(url)){
			//GET DOCUMENT TEXT
			istringstream stringReadable(parserMachine->getText());
			string word;
			//RUN THROUGH EACH WORD
			while(stringReadable >> word){
				Node<string,vector<pair<string,int>>> * node;
				node = tree->search(word);
				//CHECK IF THERE EXISTS A NODE WITH WORD
				if(!node){
					vector<pair<string,int>> element;
					pair<string,int> data;
					data.first = url;
					data.second = 1;
					element.insert(element.begin(),data);
					tree->insert(word,element);
					nodeCount++;
				}
				// IF THERE IS A NODE
				else{
					vector<pair<string,int>> element = node->getElement();
					pair<string,int> tempPair;
					int index = -1;
					if(url.empty())
						index = -1;
					for(int x = 0; x<element.size();x++){
						tempPair = element[x];
						if(tempPair.first.compare(url) == 0){
							index = x;
							break;
						}
					}
					//CHECK IF URL IS IN NODE
					if(index != -1){
						tempPair = element[index];
						tempPair.second++;
						element[index] = tempPair;
						node->setElement(element);
					}
					//IF THERE IS URL IN THE NODE
					else{
						tempPair.first = url;
						tempPair.second = 1;
						element.push_back(tempPair);
						node->setElement(element);
					}
				}
			}
		}
	}
	//PRINT EVERYTHING TO TEXT FILE
	ofstream fileWriter;
	fileWriter.open(outputFile);
	if(!fileWriter)
		return 1;
	fileWriter<<nodeCount<<endl;
	tree->inorder(fileWriter);
	fileWriter << endl;
	fileWriter.close();
	fileReader.close();
	return 0;
}




// This function overloads the output stream operator for Node
// elements. It has been provided to you and does not need to be
// altered.
ostream& operator<<(ostream& out, const vector<pair<string, int>>& e) {
    // Write the number of elements in the vector
    out << e.size() << endl;
    for (auto& uf : e)
        // Write each member of each pair the vector stores
        out << uf.first << " " << uf.second << endl;
    return out;
}




