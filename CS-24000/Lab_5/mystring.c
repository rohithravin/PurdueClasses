
#include <stdlib.h>
#include "mystring.h"

int mystrlen(char * s) {
	int countLen = 0;
	while(*s != '\0'){
		countLen++;
		s++;	
	}
	return countLen;
}

char * mystrcpy(char * dest, char * src) {
	char* answer = dest;
	while(*src !='\0'){
		*dest = *src;
		src++;
		dest++;	
	}
	*dest = '\0';
	dest = answer;
	return dest;
	
}

char * mystrcat(char * dest, char * src) {
	char* answer = dest;
	while (*dest != '\0'){
		dest++;	
	}
	while(*src !='\0'){
		*dest = *src;
		src++;
		dest++;	
	}
	*dest = '\0';
	dest = answer;
	return dest;
}

int mystrcmp(char * s1, char * s2) {
	while(*s1 !='\0'){
		if(*s1 > *s2){
			return 1;
		}
		else if (*s1 < *s2){
			return -1;
		}		
		s1++;
		s2++;	
	}
	if(*s1 > *s2){
		return 1;
	}
	else if (*s1 < *s2){
		return -1;
	}
	return 0;
}

char * mystrstr(char * hay, char * needle) {
	while (*hay) 
	  {
		    char *begin = hay;
		    char *pat = needle;
		    while (*hay && *pat && *hay == *pat) {
			      hay++;
			      pat++;
		    }
		    if (!*pat){
		    	  return begin;
		    }
		    hay = begin + 1;
	  }
	  return NULL;
}

char * mystrdup(char * s) {
	int length = strlen (s) + 1;
	char *dest = malloc(length);
	char* answer = dest;
	int x =0;
	for(x =0; x < length;x++){
		*dest = *s;
		dest++;
		s++;
	}
	*dest = '\0';
	dest = answer;
	return dest;
}

char * mymemcpy(char * dest, char * src, int n)
{
	char* answer = dest;
	int x =0;
	for(x =0; x < n;x++){
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
	dest = answer;
	return dest;
}

