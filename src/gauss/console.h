#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "gauss.h"
#include "../consolebase.h"

class Console : public BaseConsole {
 public:

  Console() : BaseConsole() {}
  ~Console() {}

  void Run() override;

 private:
  SimpleGraph<double> matrix;

};

#endif  // CONSOLE_H_
