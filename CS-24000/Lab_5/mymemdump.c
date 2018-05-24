
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void mymemdump(FILE * fd, char * p , int len) {
    // Add your code here.
    // You may see p as an array.
    // p[0] will return the element 0 
    // p[1] will return the element 1 and so on
    int c, count, sec;
    count = 0;
    sec = 0;
    for(count =0; count < len; count += 16){
        fprintf(fd, "0x%016lX: ", (unsigned long) p+count); // Print address of the beginning of p. You need to print it every 16 bytes
        int i;
        for(i = count; i <len && i < count+16;i++){
            int c = p[i]&0xFF; // Get value at [p]. The &0xFF is to make sure you truncate to 8bits or one byte.
            // Print first byte as hexadecimal
            fprintf(fd, "%02X ", c);
        }
        int x = len; 
        for( x = len; x < count+16;x++){
            fprintf(fd, "%s ","  "); // change SPACE 
        }
        fprintf(fd, "%s"," ");
        int y = count;
       while(y < len && y < count+16 ){
            c = p[y]&0xFF;
            // Print first byte as character. Only print characters >= 32 that are the printable characters.
            fprintf(fd, "%c", (c>=32 && c<=127)?c:'.');
            y++;
      }
    fprintf(fd, "\n");
    }

}

