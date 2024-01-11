#ifndef CONSOLE_H_
#define CONSOLE_H_

/* #include <ncurces.h> */

#include "ant.h"

class Console {
 public:
  enum Action { LOAD = 1, ANT_CL, ANT_PRL };

  void Run();

 private:
  ACO g;
  int width = 60;
  int heigh = 12;
};

#endif  // CONSOLE_H_
