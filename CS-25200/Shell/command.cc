/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>

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
#include <unistd.h>

#include "command.hh"
#include "shell.hh"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern FILE * yyin;
int yyparse(void);
unsigned int microseconds = 100;

Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    _append = 0;
    _inputCounter = 0;
    _outputCounter = 0;

}

std::string * Command::enviromentExpansion(std::string * argument){
	const char * args = argument->c_str();
  char * arg = strdup(args);

  //CREATE THE SEARCH POINTES FOR '$' AND '{}'
  const char * checkDollar = strchr(args, '$');
  const char * checkBraces = strchr(args, '{');

  char * replace = (char *) malloc(sizeof(args) * 100);
  char * temp = replace;

  //MAKE SURE THAT BOTH OF THE POINTERS ARE NOT NULL OTHERWISE RETURN NULL
  if(checkDollar && checkBraces){
    //PUT EVERYTHING BEFORE THE ${} IN TEMP
    while(*args != '$'){
      *(temp++) = *(arg++);
    }
    //ADD THE NULL POINTER
    *temp = '\0';
    //WHILE THE STRING EXISTS STARTING FRO THE '$'
    while(checkDollar){
      //CHECK TO MAKE SURE THAT {} ARE NOT NEXT TO EACH OTHER AND THERE IS SOMETING INSIDE
      if(checkDollar[1] == '{' && checkDollar[2] != '}'){
        char * temp2 = strdup(checkDollar) + 2;
        char * env = (char *) malloc(20);
        char * envTemp = env;
        //GET EVERYTHING IN BETWEEN THE { } INTO A VARAIBLE
        while(*temp2 != '}'){
          *(envTemp++) = *(temp2++);
        }
        //ADD NULL POINTER
        *envTemp = '\0';
        //CHECK IF THE EXPANSION IS $
        if(!strcmp(env, "$")){
          usleep(microseconds);
          char * pid= (char *) malloc(sizeof(char) * 10);
          //$ IS THE CURRENT PID, SO ADD THAT TO THE REPLACE VARIABLE USING CONCAT
          sprintf(pid, "%d", getpid());
          strcat(replace, pid);
          free(pid);
        }
          //CHECK IF THE EXPANSION IS SHELL
        else if(!strcmp(env, "SHELL")){
          char * path= (char *) malloc(sizeof(char) * 1048);
          //ADD TEH readlink TO REAPLCE
          readlink("/proc/self/exe", path, 1048 );
          strcat(replace, path);
          free(path);
        }
          //CHECK IF THE EXPANSION IS ?
        else if(!strcmp(env, "?")){
          char * lastExit= (char *) malloc(sizeof(char) * 10);
          //ADD THE LAST STATUS TO REPLACE
          sprintf(lastExit, "%d", WEXITSTATUS(Command::_lastStatus));
          strcat(replace, lastExit);
          free(lastExit);
        }
          //CHECK IF THE EXPANSION IS _
        else if(!strcmp(env, "_")){
          char * pidBack= (char *) malloc(sizeof(char) * 10);
          //ADD THE LASTARG TO THE REPACE VARAIBLE
           if(Command::_lastArg != NULL){
                 sprintf(pidBack, "%s", Command::_lastArg);
            }
            else{
                 sprintf(pidBack, "");
            }
            strcat(replace, pidBack);
          free(pidBack);
        }
          //CHECK IF THE EXPANSION IS !
        else if(!strcmp(env, "!")){
          char * pidBack= (char *) malloc(sizeof(char) * 10);
          //CHECK IF LASTPID IS NOT PARENT
           if(Command::_lastPid != 0){
                 sprintf(pidBack, "%d", Command::_lastPid);
            }
            else{
                 sprintf(pidBack, "");
            }
            //THEN ADD TO REPLACE
            strcat(replace, pidBack);
          free(pidBack);
        }

        else{
          //IF NONE OF THOSE, THEN GET THE ANSWER USING THE GETENV METHOD
          //THEN ADD IT TO THE REPLACE METHOD
          char * getAnswer = getenv(env);
          strcat(replace, getAnswer);
        }
        //SKIP TO AFTER }
        while(*(arg-1) != '}'){
          arg++;
        }

        char * buffer = (char *) malloc(20);
        char * tempBuffer = buffer;

        //GO THE THE NEXT $
        while(*arg != '$' && *arg){
          *(tempBuffer++) = *(arg++);
        }

        //ADD THE NULL POINTER
        *tempBuffer =  '\0';
        strcat(replace,buffer);
        free(buffer);
      }
      checkDollar++;
      //FIND THE NEXT OCCURENCE OF THE $ FOR THE NEXT WHILE LOOP START
      checkDollar = strchr(checkDollar, '$');

    }
    //ASSIGN THE REPACE TO ARGUMENT AND THEN RETURN THAT
    argument->assign(strdup(replace));
    free(replace);
    return argument;
  }
  return NULL;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
  int size = simpleCommand->_arguments.size();
	for (int x = 0; x < size; x++){
    //FOR EACH ARGUMENT IN THE SIMPLE COMMAND, NEED TO MAKE SURE OF VARAIBLE EXPANSION
		std::string * argument = simpleCommand->_arguments[x];
		std::string * exp =  new std::string(*argument);
    //CHECK FOR EACH AGRUGMENT IF IS HAS THE EXPANSION
		std::string * expand = enviromentExpansion(exp);
    //MAKE SURE THAT IT IS NOT NULL
		if(expand){
		    simpleCommand->_arguments[x]= expand;
		}
	}
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();
    if (_errFile == _outFile){
        _errFile = NULL;
        _outFile = NULL;
    }
    if ( _outFile ) {
        delete _outFile;
    }
    _outFile = NULL;

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile ) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
    _append = 0;
    _inputCounter = 0;
    _outputCounter = 0;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::reset(){
	clear();
    Shell::prompt();
}

//WILL SET THE LAST ARGUMENT OF THE SIMPLE COMMAND TO THE THE VARAIBLE
void Command::setLastArg(){
  int size = _simpleCommands.size();
  //GET THE VECTOR OF ARGUMENTS FROM THE SIMPLE COMMANDS
  std::vector<std::string *> args = _simpleCommands[size - 1]->_arguments;
  bool flag = false;
  //SIZE OF THE VECTORY
  int size2 = args.size();
  for(int x = 0; x < size2; x++){
    std::string * str = args[x];
    //MAKE SURE U DON'T INCLUDE THE FILE REDIRCTIONS
    if(!str->compare("2>") || !str->compare(">&") || !str->compare(">>") || !str->compare(">>&")){
      Command::_lastArg = strdup(args[x-1]->c_str());
      flag = true;
    }
  }
  //IF NOT FILE REDICRTIONS, JUST SET VARAIBLE TO LAST ARGUMENT IN COMMANDS
  if(!flag)
    Command::_lastArg = strdup(args[args.size() - 1]->c_str());
}

void Command::execute() {
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        Shell::prompt();
        return;
    }

    //MAKE SURE TO SET THE LAST ARG OF THE SIMPLE COMMAND FOR VARAIBLE EXPANSION
    //NOT INCLUDE THE FILE REDIRECTS
    Command::setLastArg();

    //exiting shell if user enters 'exit'
    if (!strcmp((_simpleCommands[0]->_arguments[0])->c_str(),"exit") || !strcmp((_simpleCommands[0]->_arguments[0])->c_str(),"bi")){
        printf(ANSI_COLOR_GREEN "\nThanks for using me!!!\n" ANSI_COLOR_GREEN);
        printf(ANSI_COLOR_GREEN "\nGOTTA BOUNCE BOUNCE BOUNCE!!!\n\n" ANSI_COLOR_GREEN);
        exit(1);
    }

    //for change directory
    if(!strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "cd")){
    		// go to home
            if(_simpleCommands[0]->_arguments.size() == 1)
                chdir(getenv("HOME"));
            // go one directory up
            else if (_simpleCommands[0]->_arguments.size() == 0)
                chdir("..");
            //change directory base on user input
            else if (_simpleCommands[0]->_arguments.size() == 2){
                if(chdir(_simpleCommands[0]->_arguments[1]->c_str())){
                    fprintf(stderr, "cd: can't cd to %s\n", _simpleCommands[0]->_arguments[1]->c_str());
                }
            }
            reset();
            return;
        }

    // check if there is ambigous redirection
    if (_inputCounter > 1 || _outputCounter > 1) {
		printf("Ambiguous output redirect.\n");
		reset();
		return;
	}

    // Print contents of Command data structure

   // print();

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

    //create file descriptors for in our and err
    int temp_in = dup(0);
    int temp_out = dup(1);
    int temp_err = dup(2);

    // file descriptors to run the programs that will change
    int in;
    int out;
    int err;
    int ret;


    //set up error file
    if (_errFile){
    	if(_append){
    		err = open(_errFile->c_str(), O_WRONLY| O_CREAT | O_APPEND, 0600);
    	}
    	else{
    		err = open(_errFile->c_str(), O_WRONLY| O_CREAT | O_TRUNC, 0600);
    	}
    }else{
    	err = dup(temp_err);
    }

    //set up input file
    if(_inFile){
    	in = open(_inFile->c_str(), O_RDONLY);
    }
    else{
    	in = dup(temp_in);
    }

    dup2(err, 2);
    close(err);
    int num_simple_commands = _simpleCommands.size(); // number of simple commands

    for (int x = 0; x < num_simple_commands; x++){


    	dup2(in,0);
    	close(in);


        // if this is the last command
        if(x == num_simple_commands-1){

            //set up output file
            if (_outFile){
            	//check if append flag is there or not
                if(_append){
                    out = open(_outFile->c_str(), O_WRONLY| O_CREAT | O_APPEND, 0600);
                }
                else{
                    out = open(_outFile->c_str(), O_WRONLY| O_CREAT | O_TRUNC, 0600);
                }
            }else{
                out = dup(temp_out);
            }

            //set up input file
            if (_errFile){
            	//check if append flag is there or not
                if(_append){
                    err = open(_errFile->c_str(), O_WRONLY| O_CREAT | O_APPEND, 0600);
                }
                else{
                    err = open(_errFile->c_str(), O_WRONLY| O_CREAT | O_TRUNC, 0600);
                }
            }else{
                err = dup(temp_err);
            }

        }
        else{ // if not last command create a pipe and redirect input and ouput
        	int fdpipe[2];
            pipe(fdpipe);
            out = fdpipe[1];
            in = fdpipe[0];
        }

        // close output
    	dup2(out,1);
    	close(out);

    	//enviroment variable for printenv
        if (!strcmp(_simpleCommands[x]->_arguments[0]->c_str(), "printenv")) {
                char ** enviroment = environ;
                while(*enviroment != NULL){
                    printf("%s\n",*enviroment);
                    enviroment++;
                }
                fflush(stdout);
        }
    	//enviroment variable for unsetenv
        else if(!strcmp(_simpleCommands[x]->_arguments[0]->c_str(), "unsetenv")){
             if(!(_simpleCommands[x]->_arguments.size() >= 2 ) ){
                perror("too few arguments for unsetenv");
                exit(-3);
             }
             else{
                int error = unsetenv(_simpleCommands[x]->_arguments[1]->c_str());
                if (error != 0){
                    perror("unsetenv");
                }
                reset();
                return;
             }
        }
    	//enviroment variable for setenv
        else if(!strcmp(_simpleCommands[x]->_arguments[0]->c_str(), "setenv")){
        	//printf("check1->%s\n", _simpleCommands[x]->_arguments[2]->c_str());
            int error = setenv(_simpleCommands[x]->_arguments[1]->c_str(),_simpleCommands[x]->_arguments[2]->c_str(),1);
            if (error != 0 ){
                perror("setenv");
            }
            reset();
            return;
        }
        else{
        	ret = fork(); // fork process, child will run command, parent will prompt user
    	    if(ret == 0){
                //convert vector<string> to vector<char *> for execvp
                std::vector<char*> argc;
                for (auto const& a : _simpleCommands[x]->_arguments)
                    argc.emplace_back(const_cast<char*>(a->c_str()));
                argc.push_back(nullptr);
                //execute the commands
                execvp(_simpleCommands[x]->_arguments[0]->c_str(), argc.data());
                perror("\nperror");
                exit(2);

    	    } else if (ret < 0){
    	    	perror("\nfork");
    	    }
        }
    }

    // close all input and output and err descriptors
    dup2(temp_in,0);
    dup2(temp_out,1);
    dup2(temp_err,2);

    //close the descriptors
    close(temp_in);
    close(temp_out);
    close(temp_err);

    // if there is not background wait for everything to finish
    if(!_background){
        int status;
    	waitpid(ret,&status,0);
      //GET THE STATUS OF THE OTHER PROCESSES
        Command::_lastStatus = status;
    }
    else{
      //GET THE PID NUMBER OF THE FORK
        Command::_lastPid = ret;
    }

    // Clear to prepare for next command
    reset();
}



SimpleCommand * Command::_currentSimpleCommand;
//NEW VARIABLE THAT NEED TO BE USED FOR VARAIABLE EXPANSION
int Command::_lastStatus;
char * Command::_lastArg;
pid_t Command::_lastPid;
