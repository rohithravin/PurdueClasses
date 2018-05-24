
#include <stdio.h>

#include "array.h"

// Return sum of the array
double sumArray(int n, double * array) {
  double sum = 0;
  
  double * p = array;
  double * pend = p+n;

  while (p < pend) {
    sum += *p;
    p++;
  }

  return sum;
}

// Return maximum element of array
double maxArray(int n, double * array) {
  double max = -1;
  double * p = array;
  double * pend = p+n;

  while (p < pend) {
    if(*p >max)
      max = *p;
    p++;
  }

  return max;
}

// Return minimum element of array
double minArray(int n, double * array) {
  double min = 1000000;
  double * p = array;
  double * pend = p+n;

  while (p < pend) {
    if(*p <=min)
      min = *p;
    p++;
  }

  return min;
}

// Find the position int he array of the first element x 
// such that min<=x<=max or -1 if no element was found
int findArray(int n, double * array, double min, double max) {
  double *p = array;
  int x =0;
  for(x =0; x < n; x++){
    if( *(p +x) <= max && *(p +x) >= min){
      
      return x;
    }
  }
  return -1;
}

// Sort array without using [] operator. Use pointers 
// Hint: Use a pointer to the current and another to the next element
int sortArray(int n, double * array) {
  double temp =0;
  int x =0;
  int y =0;
  double *p = array;
  for(x =0; x < n;x++){
    for(y= x+1; y< n; y++){
        if( *(p +x) > *(p +y)){
          temp = *(p +x);
          *(p +x) =*(p +y);
          *(p +y) = temp;
        }
    }
  }
}

// Print array
void printArray(int n, double * array) {
  double * p = array;
  double * pend = p+n;
  int x =0;
  while (p < pend) {
    printf("%d:%f\n",x,*p );
    p++;
    x++;
  }
}

