#include <stdio.h>
//

int main(int argc, char **argv) {
  int gamePlay =1;
  int low = 1;
  int high = 100;
  int mid = (low+high)/2;
  int cont = 1;
  int c;
  int input;
  printf("Welcome to the High Low game...\n");
  
  while(gamePlay){

   low = 1;
   high = 100;
   mid = (low+high)/2;
   cont = 1;
  
  
  printf("Think of a number between 1 and 100 and press <enter>\n");
  getchar();
  while(cont){
  	printf("Is it higher than %d? (y/n)\n",mid);
  	c = getchar();
  	getchar();
  	if(c == 'y')
  		low=mid+1;
  	else if  (c == 'n')
  		high = mid - 1;
  	else{
  		printf("Type y or n\n");
  		printf("Is it higher than %d? (y/n)\n",mid);
  	}
  	if (high < low){
  		printf("\n>>>>>> The number is %d\n",low);
  		break;
  	}
  	mid = (low+high)/2;
  }
  printf("\nDo you want to continue playing (y/n)?");
  input = getchar();
  if(input == 'y'){
  		gamePlay =1;
  		getchar();
  	}
  else if  (input == 'n')
  		gamePlay =0; 
  }
  printf("\nThanks for playing!!!");
	
}

