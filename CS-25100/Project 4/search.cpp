#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "Node.h"
#include "RBTree.h"
#include "SimpleHTMLParser.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <locale>

using namespace std;
/*
 * @author Rohith Ravindranath
 * @version March 26, 2018
 * Search.h
*/
// Input stream operator overload for node elements
istream& operator>>(istream& in, vector<pair<string, int>>& e);

bool mySort(pair<string,int> pair1, pair<string,int> pair2){
    if(pair1.second > pair2.second)
        return true;
    else if(pair1.second < pair2.second)
        return false;
    else if(pair1.second == pair2.second){
        if(pair1.first.compare(pair2.first) > 0)
            return false;
        else
            return true;
    }
}

vector <pair <string, int>> sortUrls(vector <pair <string, int>> list){
    for(int x = 0; x < list.size(); x++){
        for(int y = 0;  y < list.size()-x-1; y++){
            if(!mySort(list[y],list[y+1])){
                pair <string, int> tempPair = list[y];
                list[y] = list[y+1];
                list[y+1] = tempPair;
            }
        }
    }
    return list;
}

vector <pair <string, int>> getIntersectedUrls(vector<vector<pair<string,int>>> listOfLists){
    vector <pair <string, int>> unique;
    if(listOfLists.size() <= 0)
        return unique;
    else if(listOfLists.size() == 1)
        return listOfLists[0];
    else{
        int total = 0;
        int index = 0;
        for(int x =0; x < listOfLists[0].size();x++){
            total = 0;
            index = 0;
            string url = listOfLists[0][x].first;
            total  +=listOfLists[0][x].second;
            for(int y = 1; y < listOfLists.size(); y++){
                vector <pair <string, int>> tempVector = listOfLists[y];
                for(int z = 0; z < tempVector.size(); z++){
                    if(url.compare(tempVector[z].first) == 0){
                        index = index + 1;
                        total += tempVector[z].second;
                        break;
                    }
                }
            }
            if(index == listOfLists.size()-1)
                unique.push_back(make_pair(url,total));
            total = 0;
            index = 0;
        }
        return unique;
    }
}

int main(int argc, char** argv) {
    if(argc < 3){
        cout << argv[2] << "\n";
		return -1;
    }
    //CREATE TREE
    RBTree<string,vector<pair<string,int>>>  bigTree;
    ifstream inFile;
    inFile.open(argv[1]);
    if(inFile.fail())
        return -1;
    inFile  >> bigTree;
    //CREATE LIST OF ELEMENTS
    istringstream stringReadable(argv[2]);
    string word;
    vector<vector<pair<string,int>>> listOfElements;
    vector<pair<string,int>> tempPair;
    Node<string,vector<pair<string,int>>> * node;
	for (int x = 2; x < argc; x++){
		word = argv[x];    
        for (int i=0; i<word.length(); i++)
            word[i] = tolower(word[i]);
        node = bigTree.search(word);
        if(!node)
            listOfElements.push_back(tempPair);
        else
            listOfElements.push_back(node->getElement());
    }
    //GET 
    vector <pair <string, int>> intersectionOfUrls = getIntersectedUrls(listOfElements);
    if(intersectionOfUrls.empty()){
        cout << "Not found\n";
        return 0;
    }
    //SORT URLS
    vector <pair <string, int>> sortedUrlList = sortUrls(intersectionOfUrls);
    //PRINT SORTED ORDER
    for(int x = 0; x < sortedUrlList.size(); x++)
        cout <<sortedUrlList[x].first << " " << sortedUrlList[x].second << "\n";
}

// This function overloads the input stream operator for Node
// elements. It has been provided to you and does not need to be
// altered.
istream& operator>>(istream& in, vector<pair<string, int>>& e) {
	// Clear any existing contents
	e.clear();

	// Read the number of elements
	size_t n;
	in >> n;
	for (int i = 0; i < n; i++) {
		// Read each member of each pair the vector stores
		string u;
		in >> u;
		int f;
		in >> f;
		// Add to vector
		e.push_back(make_pair(u, f));
	}

	return in;
}
