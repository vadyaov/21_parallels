#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "../consolebase.h"
#include "ant.h"

class Console final : public BaseConsole {
 public:
  Console() : BaseConsole() {}
  ~Console()  {}

  void Run() override;

  void PrintGraph(WINDOW*) override;

 private:
  SimpleGraph<int> g;
};

#endif  // CONSOLE_H_
