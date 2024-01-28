#ifndef CONSOLE_H_
#define CONSOLE_H_

/* #include <ncurces.h> */

#include "gauss.h"

class Console {
 public:
  enum Action { NO_ACTION = 0, LOAD, EXEC_NUM, RUN, EXIT };

  Console() : matrix{nullptr}, exec_num{0} {}
  ~Console() { delete matrix; }

  void Run();

 private:
  SimpleMatrix* matrix;
  std::string matrix_path;
  int exec_num;
};

#endif  // CONSOLE_H_
