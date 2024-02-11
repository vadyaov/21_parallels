#pragma once

#include "../consolebase.h"
#include "../simplegraph.h"

class Console final : public BaseConsole {
  public:
    using d_graph = SimpleGraph<double>;

    enum Action { NO_ACTION = 0, LOAD, GENERATE, SHOW, EXEC_NUM, RUN, EXIT };

    Console() : BaseConsole() {}
    ~Console() {}

    void Run() override;
    void PrintResMatrix(WINDOW*, const d_graph& g);

  private:
    std::pair<d_graph, d_graph> mtrxs;
    d_graph result, async_result;
};
