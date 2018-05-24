
#include <stdio.h>
#include <stdlib.h>

//
// Separates the file into words

//

#define MAXWORD 200
char word[MAXWORD];
char returnWord[MAXWORD];
int wordLength;

// It returns the next word from fd.
// If there are no more more words it returns NULL. 
char * nextword(FILE * fd) {
	char c;
	int x =0;
	//printf("helldo\n");
	c = fgetc(fd);
	while(c != EOF){
		//printf("hello");
		if (c!= ' ' && c!= '\t' && c!= '\r' && c!='\n' ) {
			word[x] = c;
			x++;
			wordLength++;
		}
		else if(word[0] != 0){
			
			for(int y =0; y< 200;y++)
				returnWord[y] = word[y];
			for(int y =0; y< 200;y++)
				word[y] =0;
			return returnWord;
		}
		//word = word[MAXWORD];
		c = fgetc(fd);
		
	}	
	


	// While it is not EOF read char
		// While it is not EOF and it is a non-space char
		// store character in word.
		// if char is not in word return word so far.
		//
	// Return null if there are no more words
	return NULL;
}

