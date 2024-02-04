#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "../consolebase.h"
#include "ant.h"

class Console : public BaseConsole {
 public:
  Console() : BaseConsole() {}

  void Run() override;

 private:
  SimpleGraph<int> g;
};

#endif  // CONSOLE_H_
