#ifndef CONSOLE_H_
#define CONSOLE_H_

/* #include <ncurces.h> */

#include "../consolebase.h"
#include "ant.h"

class Console : public BaseConsole {
 public:
  Console() : BaseConsole() {}

  void Run() override;

 private:
  ACO g;
};

#endif  // CONSOLE_H_
