#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char ** argv) {
    
    for(int x =1; x<4;x++){
        if(argv[x] == NULL){
            printf("Usage:  convert <basefrom> <baseto> <number>\n");
            exit(1);
        }
    }
    
    int baseFromInput = atoi(argv[1]);
    int baseToInput = atoi(argv[2]);
    int numberInput = atoi(argv[3]);
    int base10Num = -1;
    if (numberInput == 0){
        char  *charInput = argv[3];
        printf("Number read in base %d: %s\n",baseFromInput,charInput);
        base10Num = convertBase1OChar(charInput,baseFromInput);
    }
    else{
        printf("Number read in base %d: %d\n",baseFromInput,numberInput);
        base10Num = convertBase10(numberInput,baseFromInput);
    }
    printf("Converted to base 10: %d\n", base10Num);
    int convertedNumber = convertToBase(base10Num,baseToInput);
    printf("Converted to base %d: %d\n",baseToInput ,convertedNumber);
}

int convertBase1OChar(char* input, int base ){
    int x =0;
    char c;
    int total =0;
    int num = 0;
    int index =((strlen(input)-1)-(x));
   // printf("%d\n",(strlen(input)-1)-(2));
    while( index >= 0){
        c =input[(strlen(input)-1)-x];
        if(c == 'A'){
            total += 10 * pow((double)base,(double)x);
        }
        else if(c == 'B'){
            total += 11 * pow((double)base,(double)x);
        }
        else if(c == 'C'){
            total += 12 * pow((double)base,(double)x);
        }
        else if(c == 'D'){
            total += 13 * pow((double)base,(double)x);
        }
        else if(c == 'E'){
            total += 14 * pow((double)base,(double)x);
        }
        else if(c == 'F'){
            total += 15 * pow((double)base,(double)x);
        }
        else{
            num = c - '0';
            total += num * pow((double)base,(double)x);
        }
        x++;
        index =((strlen(input)-1)-(x));
    }
    return total;
 
}

int convertBase10(int number, int base){
    int value = number;
    int r =0;
    int total = 0;
    int x =0;
    while (value >0){
        r = value%10;
        total += r * pow((double)base,(double)x);
        value = value /10;
        x++;
    }
    return total;
}

int convertToBase(int number, int base){
    int value = number;
    int total = 0;
    int x = 1;
    int rec =0;
    while (value != 0){
        rec = value%base;
        value = value/base;
        total = total + (rec*x);
        x *= 10;
    }
    return total;
    
}













