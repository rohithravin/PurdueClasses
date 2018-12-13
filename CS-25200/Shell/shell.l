
/*
 *
 * CS-252
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include "y.tab.hh"
#include "shell.hh"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>
#include <pwd.h>
#include "command.hh"

////////////  Start added code ///////////
extern "C" char * read_line();

int mygetc(FILE * f) {
 static char *p;
 char ch;

 if (!isatty(0)) {
 // stdin is not a tty. Call real getc
   return getc(f);
 }

 // stdin is a tty. Call our read_line.
 if (p==NULL || *p == 0) {
   char * s = read_line();
   p = s;
 }

 ch = *p;
 p++;

 return ch;
}

#undef getc
#define getc(f) mygetc(f)

/////////// End added code ///////////

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

%}

%%

"source ".* {

  char * file2 = strdup((&yytext[7]));
	//get the file name after source key word

	//SET PROMPT TO FALSE SO IT DOESN'T PRINT

	//open the file from the from name that was just taken
	FILE * _fd = fopen(file2, "r");
	if( _fd == NULL){
		perror("source file error.");
	}
  Shell::setPromptFlag(false);
	// pushes this file commands as a buffer and pushes it as a new state on the stack on top.
	// this will make the shell execute the commands in the file first
	yypush_buffer_state(yy_create_buffer(_fd, YY_BUF_SIZE));

	//parse the commands in the file name
	yyparse();

	//put the prompt back into the the terminal;
	Shell::setPromptFlag(true);

	//resetting the buffer state to null, since yyparse() is over and needs to be reset.
	yypop_buffer_state();
}





\$\(.*\) {

	//STEP ONE PARSE TOKEN
	char * command = strdup((&yytext[2]));
	char * pointer = command;

	while(*pointer != ')'){
		pointer++;
	}
	*pointer = '\0';
	strcat(command, "\n");

	//STEP TWO CREATE PIPES
  int tempout = dup(1);
	int tempin = dup(0);
	int toSubshell[2];
	int fromSubshell[2];

	//CREAT PIPES FOR SUBSHELL TO COMMUNIATE WITH
	pipe(toSubshell);
	pipe(fromSubshell);

	//WRITE INPUT TO SUBSHELL
	write(toSubshell[1], command, strlen(command));
	dup2(toSubshell[0], 0);
	dup2(fromSubshell[1],1);
	close(fromSubshell[1]);
  close(toSubshell[1]);
  close(toSubshell[0]);


	//START FORK PROCESSES
	int ret = fork();
	if (ret == 0){
		execvp("/proc/self/exe", NULL);
		_exit(1);
	}
	else if (ret < 0){
		perror("fork");
		exit(1);
	}
	//MAIN CHILD PROCESS
	else{
		//WAITING UNTIL PROCESS IS COMPLETE
		while(waitpid(ret, NULL, 0) == -1);
		dup2(tempin, 0);
		dup2(tempout, 1);

		//CLOSE SUBSHELL IN AND OUT
    close(fromSubshell[1]);
    close(toSubshell[1]);
		close(toSubshell[0]);


		//CREATE BUFFER TO READ OUPUT FROM SUBSHELL
		char buffer[32192];
		char * bufferPointer = buffer;
		char * temp = bufferPointer;

		//READING FROM SUBSHELL TO NORMAL SHELL
		while(read(fromSubshell[0], bufferPointer, 1)) {
			if(*bufferPointer == '\n'){
				*bufferPointer = ' ';
			}
			bufferPointer++;
		}
		*bufferPointer = '\0';
		char * pointer = buffer + strlen(buffer);
    close(fromSubshell[0]);

		//UNPUTTING IN REVERSE
		while(pointer != buffer){
			//CHECK FOR NULL POINTER
			if(*pointer != '\0'){
				unput(*pointer);
			}
			pointer--;
		}
		unput(*(pointer));
	}
}



((\\[&<>]|[^&<> \t\n|<>])+|\".*\") {

	// GET TOKEN STRING
	char * str = strdup(yytext);

	//CHECK IF STARTING IS \ FOR ESCAPE CHARACTER
	if(yytext[0] == '\"'){
		str++;
		str[strlen(str) - 1] = '\0';
	}
	//CREATE TEMP CHARS
	char * str2 = (char *)malloc(sizeof(char) * (strlen(str)+10));
	char * str4 = str2;
	char * str3 = str;

	//ADD THE CHARACTER TO THE TEMP TOKEN
	while (*str3 != '\0'){
    (*str3 == '\\')?*(str4++) = *(++str3):*(str4++) = *(str3);
    str3++;
	}
	*str4 = '\0';

	//SET THE TOKEN TO THE GLOBAL VARIABLE STRING
	yylval.cpp_string = new std::string(strdup(str2));
  free(str2);
  return WORD;
}

\n { return NEWLINE; }
[ \t] { /* Discard spaces and tabs */ }
">" { return GREAT; }
">>&" { return GREATGREATAMPERSAND; }
">&" { return GREATAMPERSAND; }
">>" { return GREATGREAT; }
"&" { return AMPERSAND; }
"|" { return PIPE; }
"<" { return LESS; }
"2>" {return TWOGREAT; }






[^ \t\n|<>][^ \t\n|<>]*  {

  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  printf("bob->%s\n", yylval.cpp_string->c_str());
  return WORD;
}

{
	return NOTOKEN;
}
