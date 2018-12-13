#include <cstdio>
#include <cstdlib>

#include <iostream>

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
#include "simpleCommand.hh"

SimpleCommand::SimpleCommand() {
  _arguments = std::vector<std::string *>();
}

SimpleCommand::~SimpleCommand() {
  // iterate over all the arguments and delete them
  for (auto & arg : _arguments) {
    delete arg;
  }
}

//THIS METHOD WILL SEE IF THERE IS A '~' IN THE AGRUMENT AND THEN EXPAND ON IT
std::string * SimpleCommand::tildeExpansion(std::string * argument){
  //CREATE A COPY OF THE ARGUMENT
  const char *arg = argument->c_str();
  //CHECK TO SEE IF THERE IS A TILDE IN THE TOKE OTHERWISE JUST RETURN NULL
  if(argument->at(0) == '~'){
    //IF THE TOKEN IS JUST ~ AND NOTHING ELSE, THEN JUST RETURN THE HOME ENVIROMENT
    if(argument->length() == 1 ){
      //ASSIGN THE ENVIROMENT EXPANSION TO THE ARGUMENT AND RETURN IN
      argument->assign(std::getenv("HOME"));
      return argument;
    }
    //IF LENGTH IS IS MORE
    else{
      //IF THERE IS A '/' AFTER THE ~ THEN THE EASY PART
      if (arg[1] == '/'){
        //ADD THE HOME ENVIROMENT TO AFTER THE '/' AND THEN JUST RETURN THAT NEW STRING
        //ASSIGN THE ENVIROMENT EXPANSION TO THE ARGUMENT AND RETURN IN
        argument->assign(strcat(strdup(std::getenv("HOME")),strdup(++arg)));
        return argument;
      }
      //CREATE VARAIBLES FOR PARSING TOKEN
      char * uName = (char *) malloc(50);
      char * user = uName;
      char * temp = strdup(arg);
      temp++;
      //CHECK IF TEMP IS NOT '/' AND THEN ADD IT TO UNAME
      while(*temp != '/' && *temp){
        *(uName++) = *(temp++);
      }
      //ADD A NULL POINTER TO THE END OF THE STRING
      *(--uName) = '/0';
      //IF TEMP STILL EXISTS AND THERE IS MORE
      if(*temp){
        //USES GETPWAN AND TO THE THE DIRECTORY OF IT AND COMBINE THE STRINGS
        argument->assign(strcat(getpwnam(user)->pw_dir, temp));
      }
      else{
        //OTHERWISE JUST THEN THE DIRECTORY FROM THE USER USING PWNAM
        argument->assign(strdup(getpwnam(user)->pw_dir));
      }
      return argument;
      //FREE THE MALLLOCED VARIABLES
      free(uName);
    }
  }
  //RETURN NULL IF THERE IS NO TILDE IN THE TOKEN
  return NULL;
}

void SimpleCommand::insertArgument( std::string * argument ) {
  //CREATE A NEW DUPLICATE STRING OF THE ARGUMENT
  //CHECK IF THERE IS A TILDE EXPRESSION
  std::string * tilde = tildeExpansion(new std::string(*argument));
  //IF THERE IS A TILDE EXPRESSION ASSIGN ARGUMENT TO THAT
  if(tilde){
      argument = tilde;
  }
  _arguments.push_back(argument);
}

// Print out the simple command
void SimpleCommand::print() {
  for (auto & arg : _arguments) {
    std::cout << "\"" << *arg << "\" \t";
  }
  // effectively the same as printf("\n\n");
  std::cout << std::endl;
}
