
/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */

%code requires
{

#include <string>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT NEWLINE PIPE AMPERSAND GREATGREAT GREATAMPERSAND GREATGREATAMPERSAND LESS TWOLESS TWOGREAT

%{
//#define yylex yylex
#include <cstdio>
#include "shell.hh"

void yyerror(const char * s);
int yylex();
void expandWildCards(std::string * argc);
void expandWildCardsRecurrsion(char * prefix, char * suffix, std::vector<std::string> &allArgs);

%}

%%

goal:
  commands
  ;

commands:
  command
  | commands command
  ;

command: simple_command
       ;

simple_command:
  pipe_list iomodifier_list background NEWLINE {
    Shell::_currentCommand.execute();
  }
  | NEWLINE
  | error NEWLINE { yyerrok; }
  ;

command_and_args:
  command_word argument_list {
    Shell::_currentCommand.insertSimpleCommand( Command::_currentSimpleCommand );
  }
  ;

argument_list:
  argument_list argument
  | /* can be empty */
  ;

argument:
  WORD {
    expandWildCards($1);
  }
  ;

command_word:
  WORD {
    Command::_currentSimpleCommand = new SimpleCommand();
    Command::_currentSimpleCommand->insertArgument( $1 );
  }
  ;

iomodifier_opt:
  GREAT WORD {
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._outputCounter++;

  }
  | GREATGREATAMPERSAND WORD {
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand._outputCounter++;
    Shell::_currentCommand._append = 1;
  }
  | GREATGREAT WORD {
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._outputCounter++;
    Shell::_currentCommand._append = 1;
  }
  | LESS WORD {
    Shell::_currentCommand._inFile = $2;
    Shell::_currentCommand._inputCounter++;
  }
  | GREATAMPERSAND WORD {
    Shell::_currentCommand._outFile = $2;
    Shell::_currentCommand._errFile = $2;
    Shell::_currentCommand._outputCounter++;
  }
  | TWOGREAT WORD {
    Shell::_currentCommand._errFile = $2;
  }
  ;

pipe_list:
  pipe_list PIPE command_and_args
  | command_and_args
  ;

iomodifier_list:
  iomodifier_list iomodifier_opt
  | iomodifier_opt
  | /* can be empty */
  ;

background:
  AMPERSAND {
    Shell::_currentCommand._background = true;
  }
  | /* can be empty */
  ;


%%

void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}


//METHOD THAT CHECKS IF THE TOKEN HAS A '?' OR A '*' INDICATING A WILDCARD
//WILL THEN CALL A RECURRSIVE METHOD TO RETRIVE AL THE FILES
void expandWildCards(std::string * argc){
  char * argument = strdup(argc->c_str());
  const char  * questionPoint = strchr(argument,'?');

  //CHECK IF * OR ? IS IN THE TOKEN
  if(strchr(argument,'*') == NULL && questionPoint == NULL){
    Command::_currentSimpleCommand->insertArgument( argc );
    return;
  }

  //CHECK IF THERE IS A ?? WHICH IS AN INCORRECT USE OF WILDCARDS
  if((questionPoint != NULL && *(questionPoint+1) == '?' ) || (strchr(argument,'*') != NULL && *(strchr(argument,'*') +1) == '*') || (questionPoint != NULL && *(questionPoint + 1) == '}' && *(questionPoint - 1) == '{')){
    Command::_currentSimpleCommand->insertArgument( argc );
    return;
  }

  //VARAIBLE TO STORE ALL THE ARGS FROM THE WILDCARD
  std::vector<std::string> allArgs;

  //IF FORMAT IS / START AFTER THAT
  if(*argument == '/'){
    expandWildCardsRecurrsion("/", argument + 1, allArgs);
  }
  //IF FORMAT IS ./ START AFTER THAT
  else if (*argument == '.' && *(argument +1) == '/'){
    expandWildCardsRecurrsion("", argument + 2, allArgs);
  }
  //IF NO FORMAT, THEN JUST START FROM THE BEGINING
  else{
    expandWildCardsRecurrsion("", argument, allArgs);
  }
  //SORT ALL THE ARGS FROM THE WILDCARDS
  std::sort(allArgs.begin(), allArgs.end());

  //ADD ALL THE ARGUMENTS FROM THE VECTORY TO THE CURRENT SIMPLE COMMAND
  for(int x = 0; x < allArgs.size(); x++){
    std::string * newArg = new std::string(allArgs[x].size(),'a');
    newArg->assign(allArgs[x]);
    Command::_currentSimpleCommand->insertArgument( newArg );
  }
  //FREE MALLOCED VARIABLE
  free(argument);
}

//THIS RECURRSIVE METHOD WILL FIND ALL THE FILES OR FOLDERS FROM THE WILDCARD AND ADD IT TO A GLOBAL VECTOR
void expandWildCardsRecurrsion(char * prefix, char * suffix, std::vector<std::string> &allArgs){

  //IF THE SUFFIX IS NULL THEN JUST ADD THE PREFIX TO THE VECTOR OF ALL ARGS
  if(*suffix == '\0'){
    allArgs.push_back(std::string(prefix));
    return;
  }

  //FIND THE CHARACTER OF A SLASH IN SUFFIX
  char * slashPointer = strchr(suffix,'/');
  char buf [1024];

  //IF SLASH IS THERE ADD IS TO THE SUFFIX AND ADD TO BUF
  if (slashPointer != NULL){
    strncpy(buf,suffix, slashPointer - suffix);
    suffix = slashPointer + 1;
  }
  //IF NOT JUST ADD THE LEGTH OF THE ARRAY BUF
  else{
    strcpy(buf,suffix);
    suffix += strlen(suffix);
  }

  //CREATE SEARCH POINTERS FOR * AND ? IN NEW TOKEN BUF
  char newPrefix[2048];
  char * search1 = strchr(buf,'*');
  char * search2 = strchr(buf,'?');


  //MAKE SURE IT IS NULL FOR THIS SPECIAL CASE
  if(search1 == NULL && search2 == NULL){
    //IF THE PREFIX LENGTH IS 0 THEN JUST ADD THE PREFIX AND BUF TO THE NEW TOKEN
    int length = strlen(prefix);
    (length == 0 || (length == 1 && prefix[0] == '/'))?sprintf(newPrefix,"%s%s",prefix,buf):sprintf(newPrefix,"%s/%s",prefix,buf);
    expandWildCardsRecurrsion(newPrefix,suffix,allArgs);
    return;
  }

  //CREATE POINTERS FOR THE REGREX
  char * reg = (char *)malloc(2*strlen(buf) + 10);
  char * a = buf;
  char * r = reg;
  //THIS REGREX CONVERTS THE TOKEN TO REGREX EXPRESS
  //THIS IS FROM GUSTAVO'S TEXTBOOK
  *(r++) = '^';
  while(*a){
      if(*a == '*') {
        *(r++)='.';
        *(r++)='*';
      }
      else if(*a == '?'){
        *(r++)='.';
      }
      else if(*a == '.'){
        *(r++)='\\';
        *(r++)='.';
      }
      else{
        *(r++)=*a;
      }
      a++;
  }
  *(r++) = '$';
  *r = 0;

  //CREATE A DIRECTORY VARIABLE
  DIR * directory;
  (strlen(prefix) != 0)?directory = opendir(prefix):directory = opendir(".");

  //MAKE SURE THAT THE OPENED DIRECTORY IS NOT NULL, IF SO RETURN
  if (directory == NULL){ return; }

  //CREATE VARIABLES HERE
  struct dirent * openDirectory;

  //READ THE OPEN DIRECTORY AND MAKE SURE THAT IT IS NOT NULL, OTHERWISE JUST RETURN NULL
  while((openDirectory=readdir(directory)) != NULL){
    //MATCHED REGREX
    regmatch_t matchRegex;
    regex_t rbin;
    regcomp(&rbin, reg, 0);
    //EXECTE THE REGREX FUNCTION AND SEE IF THERE IS A MATCH
    char * dir_name  = openDirectory->d_name;

    if(regexec(&rbin, dir_name,1,&matchRegex,0) == 0){
      // IF THE OPEN DIRECTORY IS OF THE CURRENT DIRECTORY
      int length = strlen(prefix);
      if(dir_name[0] == '.' ){
        // MAKE SURE THAT BUFF ALSO HAS '.'
        if(buf[0] == '.'){
          //IF THE LENGTH OF THE PREFIX IS 0 THEN JUST ALL THE ADD THE DIRECOTRY NAME AND PREFIX
          (length == 0 || (length == 1 && prefix[0] == '/'))?sprintf(newPrefix,"%s%s", prefix, dir_name):sprintf(newPrefix,"%s/%s", prefix, dir_name);

          //RECURRSIVLY CALL THE METHOD AGAIN SO IT CAN GO ONE STEP FUTHER AND KEEP PUSHING IT DOWN THE STACK
          expandWildCardsRecurrsion(newPrefix,suffix,allArgs);
        }
      }
      else{
        //IF THE LENGTH OF THE PREFIX IS 0 THEN JUST ALL THE ADD THE DIRECOTRY NAME AND PREFIX
        (length == 0 || (length == 1 && prefix[0] == '/'))?sprintf(newPrefix,"%s%s", prefix, dir_name):sprintf(newPrefix,"%s/%s", prefix, dir_name);

        //RECURRSIVLY CALL THE METHOD AGAIN SO IT CAN GO ONE STEP FUTHER AND KEEP PUSHING IT DOWN THE STACK
        expandWildCardsRecurrsion(newPrefix,suffix,allArgs);
      }
    }
  }
  //CLOSE DIRECTORY AND FREE THE MALLOCED VARAIBLE
  closedir(directory);
  free(reg);
  return;
}

#if 0
main()
{
  yyparse();
}
#endif
