#include <cstdio>

#include "shell.hh"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>
#include <pwd.h>

int yyparse(void);


void Shell::prompt() {

	if (isatty(0) && _promptFlag){
	  printf("myshell>");
	  fflush(stdout);
	}
	else{
		_promptFlag = true;
	}
}

void Shell::setPromptFlag(bool flag){
	Shell::_promptFlag = flag;
}

extern "C" void controlC(int signal){
	printf("\n");
	//Shell::prompt();
}

extern "C" void zombie(int signal){
	int pid = wait3(0,0,NULL);
	while(waitpid(-1,NULL,WNOHANG) > 0);
}



int main() {

	//control c process
	struct sigaction sa1;
	sa1.sa_handler = controlC;
	sigemptyset(&sa1.sa_mask);
	sa1.sa_flags = SA_RESTART;
	if(sigaction(SIGINT, &sa1, NULL)){
		perror("sigaction");
		exit(-1);
	}

	//zombie process
	struct sigaction sa2;
	sa2.sa_handler = zombie;
	sigemptyset(&sa2.sa_mask);
	sa2.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa2, NULL)){
		perror("sigaction");
		exit(-1);
	}

  Shell::prompt();
  yyparse();
}

bool Shell::_promptFlag = true;
Command Shell::_currentCommand;
