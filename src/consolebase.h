#pragma once

#include <string>

class BaseConsole {
  public:
    enum Action { NO_ACTION = 0, LOAD, EXEC_NUM, RUN, EXIT };

    BaseConsole() : exec_num{0} {}

    virtual ~BaseConsole() {}

    virtual void Run() = 0;

  protected:
    std::string path;
    int exec_num;

};
