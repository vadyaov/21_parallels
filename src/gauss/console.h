#ifndef CONSOLE_H_
#define CONSOLE_H_

#include "../consolebase.h"
#include "gauss.h"

class Console final : public BaseConsole {
 public:
  enum Action { NO_ACTION = 0, LOAD, EXEC_NUM, RUN, EXIT };

  Console() : BaseConsole() {}
  ~Console() {}

  void Run() override;

  void PrintGraph(WINDOW*);

 private:
  SimpleGraph<double> matrix;
};

#endif  // CONSOLE_H_
