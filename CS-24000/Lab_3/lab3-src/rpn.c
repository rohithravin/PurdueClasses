
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#include "rpn.h"
#include "nextword.h"
#include "stack.h"

double rpn_eval(char * fileName, double x) {
	double a, b, c;
	int z;
  FILE *fd;
  double y;
  int check = 0;
  // Open file in fileName it.
   fd = fopen(fileName,"r");
   if (fd == NULL) {
	  printf("Could not open file %s\n", fileName);
	  exit(1);
	}
    char * word;
    while (word = nextword(fd)) {
    	double y= atof(word);
    	z =(strlen(word));

    	//printf("%c %d\n",*word,z );
    	if(*word == '+' && z == 1){
    		double a = stack_pop();
    		double b =stack_pop();
    		if (a == (double)(-1) || b==(double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}

    		double c = b+a;
    		stack_push(c);
    		//stack_print();
    		check =1;
    	}
    	else if(*word == '-' && z == 1){
    		double a = stack_pop();
    		double b =stack_pop();
    		if (a == (double)(-1) || b==(double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double c = b-a;
    		stack_push(c);
    		check =1;
    	}
    	else if(*word == '*'){
    		double a = stack_pop();
    		double b =stack_pop();
    		if (a == (double)(-1) || b==(double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double c = b*a;
    		stack_push(c);
    		check =1;
    	}
    	else if(*word == 'x'){
    		//printf("hi\n");
    		//double a = stack_pop();
    		//double b =stack_pop();
    		//if (a == (double)(-1) || b==(double)(-1)){
    ///		printf("Stack underflow\n");
   // 			exit(4);
   // 		}
    
   			//double c = b*a;
   			//printf("%f\n",x );
    		stack_push(x);
    		//stack_print();
    		//check =1;
    	}
    	else if(*word == '/'){
    		double a = stack_pop();
    		double b =stack_pop();
    		if (a == (double)(-1) || b==(double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double c = b/a;
    		stack_push(c);
    		check =1;
    	}
    	else if(*word == 'c' && z == 3){
    		double a = stack_pop();
    		if (a == (double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double b =0;
    		double c = cos(a);
    		stack_push(c);
    		check =1;
    	}
    	else if(*word == 's' && z == 3){
    		double a = stack_pop();
    		if (a == (double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double b =0;
    		double c = sin(a);
    		stack_push(c);
    		check =1;
    	}
    	else if(*word == 'p' && z == 3){
    		double a = stack_pop();
    		double b =stack_pop();
    		if (a == (double)(-1) || b==(double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double c = pow(b,a);
    		stack_push(c);
    		check =1;

    	}
    	else if(*word == 'e' && z == 3){
    		double a = stack_pop();
    		if (a == (double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double b =0;
    		double c = exp(a);
    		stack_push(c);
    		check =1;
    	}
    	else if(*word == 'l' && z == 3){
    		double a = stack_pop();
    		if (a == (double)(-1)){
    			printf("Stack underflow\n");
    			exit(4);
    		}
    		double b =0;
    		double c = log(a);
    		stack_push(c);
    		check =1;
    	}

    	else{
    		stack_push((double)(y));
    	}
    	// Add your implementation here
    	//
 	}
  // Check for errors
  //
  // Return result
 	if(check ==1){
  		double result =  stack_pop();
  		return result;
	}
	else{
		printf("Elements remain in the stack\n");
		exit(2);
	}
}

