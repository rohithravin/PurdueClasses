/**
 *  CS251 - Spring 2018: Project 1
 *  @author Rohith Ravindranath
 *  @version Jan 17 2018
 */

#include <iostream>
#include <fstream>	
using namespace std;

int main(int argc, char** argv)
{	
	int column;
	cin >> column;
	int value;
	int numOfDashes = column*6-3;
	int index = 0;
	while(cin >> value){
		if (index == 0){
			for(int x = 0; x < numOfDashes;x++){
				cout << "-";			
			}
			cout << "\n";
			index++;		
		}
		if(index != 1)	
			cout<<" ";
		if(value/10 == 0){
			cout << "  " << value;
						
		}
		else if (value/10 > 0 && value/10 < 10){
			cout << " " << value;
		}
		else
			cout<<value;
		if(index == column){
			cout<<"\n";
			index = 0;
		}
		else{
			cout<<" |";
			index++;
		}		
	} 
	for(int x = 0; x < numOfDashes;x++){
		cout << "-";			
	}
	return 0;
}
