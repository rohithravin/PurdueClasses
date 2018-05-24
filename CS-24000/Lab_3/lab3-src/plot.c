
#include <stdio.h>
#include <stdlib.h>

#include "rpn.h"

#define MAXCOLS 80
#define MAXROWS 40

char plot[MAXROWS][MAXCOLS];

void clearPlot()
{
  for (int i = 0; i < MAXROWS; i++) {
    for (int j = 0; j < MAXCOLS; j++) {
      plot[i][j] = ' ';
    }
  }
}

void printPlot()
{
  for (int i = 0; i < MAXROWS; i++) {
    for (int j = 0; j < MAXCOLS; j++) {
      printf("%c", plot[i][j]);
    }
    printf("\n");
  }
}

void plotXY(int x, int y, char c) {
  if ( x <0 || x>=MAXCOLS || y < 0 || y >=MAXROWS) return;
  plot[y][x]=c;
}

void createPlot( char * funcFile, double minX, double maxX) {
  int nvals = MAXCOLS;
  double yy[MAXCOLS];
  double xx[MAXCOLS];

  clearPlot();
  double x;
  double y;
  int a;
  for(a =0; a< MAXCOLS-1;x++){
    x = minX + (maxX-minX) * x / MAXCOLS; 
    y = rpn_eval(funcFile,x);
    xx[a] =x;
    yy[a] =y;
  }

  // Evaluate function and store in vector yy

  //Compute maximum and minimum y in vector yy
  
  //Plot x axis

  //Plot y axis

  // Plot function. Use scaling.
  // minX is plotted at column 0 and maxX is plotted ar MAXCOLS-1
  // minY is plotted at row 0 and maxY is plotted at MAXROWS-1

  printPlot();

}

int main(int argc, char ** argv)
{
  printf("RPN Plotter.\n");
  
  if (argc < 4) {
    printf("Usage: plot func-file xmin xmax\n");
    exit(1);
  }
  double xmin=0;
  double xmax=0;
  sscanf(argv[2],"%lf", &xmin);
  sscanf(argv[3],"%lf", &xmax);

  printf("%f %f \n",xmin,xmax );
  // Get arguments
  
  //createPlot(funcName, xmin, xmax);
}
