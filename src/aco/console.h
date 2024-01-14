#ifndef CONSOLE_H_
#define CONSOLE_H_

/* #include <ncurces.h> */

#include "ant.h"

class Console {
 public:
  enum Action { NO_ACTION = 0, LOAD, EXEC_NUM, POPUL_NUM, RUN, EXIT };

  Console() : exec_num{0}, popul_num{0} {}

  void Run();

 private:
  ACO g;
  std::string graph_path;
  int exec_num;
  int popul_num;
};

#endif  // CONSOLE_H_
