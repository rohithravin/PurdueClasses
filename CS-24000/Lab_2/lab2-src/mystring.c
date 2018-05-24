
#include <stdlib.h>
#include "mystring.h"

// Type "man string" to see what every function expects.

int mystrlen(char * s) {
	int num,x;
	num = x = 0;
	if (s == NULL)
		return 0;
	while(s[x] != '\0'){
		num++;
		x++;
	}
	return num;
}

char * mystrcpy(char * dest, char * src) {
	int x = 0;
	while (src[x] != '\0'){
		dest[x] = src[x];
		x++;
	}
	dest[x] = '\0';
	return dest;

}

char * mystrcat(char * dest, char * src) {
	int x,y;
	for(x = 0; dest[x] != '\0';x++);
	for(y =0; src[y] != '\0';y++)
		dest[x+y] = src[y];
	dest[x+y] = '\0';
	return dest; 
}

int mystrcmp(char * s1, char * s2) {
	int x =0;
	while (s1[x] != '\0' || s2[x] != '\0' ){
		if (s1[x] > s2[x])
			return 1;
		if (s1[x] < s2[x])
			return -1;
		else
			x++;
	}
	if(mystrlen(s1) > mystrlen(s2))
		return 1;
	if(mystrlen(s1) < mystrlen(s2))
		return 1;
	else 
		return 0;
}

char * mystrstr(char * hay, char * needle) {
    if (!*needle)
        return hay;
    char *flag1 = hay;
    while(*flag1){
        char *flag1Begin = flag1;
        char *flag2 = needle;
        while(*flag1 && *flag2 && *flag1 == *flag2){
            flag1++;
            flag2++;
        }
        if(!*flag2)
            return flag1Begin;
        flag1 = flag1Begin + 1;
    }
    return NULL;
}

char * mystrdup(char * s) {
	int len = mystrlen(s)+1;
	char *str = malloc(len);
	return (char *) mymemcpy(str,s,len);
}

char * mymemcpy(char * dest, char * src, int n)
{
    for (int x = 0; x < n; x++){
        dest[x] = src[x];
    }
	return dest;
}

	