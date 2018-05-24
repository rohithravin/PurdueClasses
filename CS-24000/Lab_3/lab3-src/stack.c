
#include <stdio.h>
#include "stack.h"
#include <stdlib.h>

int top=0;
double stack[MAXSTACK];

void stack_clear() 
{
  top = 0;
}

double stack_pop()
{	
	double number;
	if(stack_is_empty()){
		//printf("Stack is empty");
		return -1;
	}
		
	number = stack[top];
	stack[top] = stack[top-1];
	top--;
	return number;
}

void stack_push(double val)
{	
		top++;
		stack[top] = val;
		//printf("%d\n",top );
}

void stack_print()
{
	int x;
	printf("Stack:\n");
	if(stack_is_empty()){
		printf("Stack is empty");
		return;
	}
	for(x = 1; x <=top; x++){
		printf("%d: %f\n",x-1,stack[x]);
	}
}

int stack_top()
{
  return top;
}

int stack_max()
{
  return MAXSTACK;
}

int stack_is_empty()
{
  return top == 0;
}


