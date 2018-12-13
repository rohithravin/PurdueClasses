#ifndef shell_hh
#define shell_hh

#include "command.hh"

struct Shell {

  static void prompt();
  static void setPromptFlag(bool flag);
  static Command _currentCommand;
  static bool _promptFlag;

};

#endif
