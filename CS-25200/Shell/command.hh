#ifndef command_hh
#define command_hh

#include "simpleCommand.hh"

// Command Data Structure

struct Command {
  std::vector<SimpleCommand *> _simpleCommands;
  std::string * _outFile;
  std::string * _inFile;
  std::string * _errFile;
  bool _background;
  int _append;
  int _inputCounter;
  int _outputCounter;

  Command();
  void insertSimpleCommand( SimpleCommand * simpleCommand );
  std::string * enviromentExpansion(std::string * argument);

  void setLastArg();
  void clear();
  void print();
  void execute();
  void reset();

  static SimpleCommand *_currentSimpleCommand;
  static char *  _lastArg;
  static int _lastPid;
  static int _lastStatus;

};

#endif
