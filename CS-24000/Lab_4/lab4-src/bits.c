#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// It prints the bits in bitmap as 0s and 1s.
void printBits(unsigned int bitmap)
{
	int x;

	unsigned int bits;

	for (x = 31; x >= 0 && (bits=(1<<x));x--) {
		(bitmap&bits)?
			putchar('1') : putchar('0');
	}

	printf("\n");
	printf("10987654321098765432109876543210\n");
}



int convertToBits(unsigned int bitmap){
	unsigned int x = bitmap;
	if (x == 0) 
		return 0;
    if (x == 1) 
    	return 1;                 
    return (x % 2) + 10 * convertToBits(x / 2);
}




// Sets the ith bit in *bitmap with the value in bitValue (0 or 1)
void setBitAt( unsigned int *bitmap, int i, int bitValue ) {
	// Add your code here
	unsigned int bits=(1<<i);
	(bitValue)?
		(*bitmap=( bits |(*bitmap))) : (*bitmap=( (*bitmap) & (~bits) ) );
}

// It returns the bit value (0 or 1) at bit i
int getBitAt( unsigned int bitmap, unsigned int i) {
	
	unsigned int bits=(1<<i);

	unsigned int value  = bitmap;

	return (bits&value)? 
				1:0;

}

// It returns the number of bits with a value "bitValue".
// if bitValue is 0, it returns the number of 0s. If bitValue is 1, it returns the number of 1s.
int countBits( unsigned int bitmap, unsigned int bitValue) {
	// Add your cod
	unsigned int bits;
	int i;
	int x;
	int y;
	for (x = 0, i = 0, y = 0; (i<32) && (bits=(1<<i));i++) 
		(bitmap&bits)?
			x++:y++;

	return (bitValue)?
				x:y;	
}

// It returns the number of largest consecutive 1s in "bitmap".
// Set "*position" to the beginning bit index of the sequence.
int maxContinuousOnes(unsigned int bitmap, int * position) {
	// Add your code here
}


