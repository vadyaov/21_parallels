#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "gauss.h"
#include "../consolebase.h"

class Console : public BaseConsole {
 public:

  Console() : BaseConsole(), matrix{nullptr} {}
  ~Console() { delete matrix; }

  void Run() override;

 private:
  SimpleMatrix* matrix;

};

#endif  // CONSOLE_H_
