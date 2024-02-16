#pragma once

#include "../consolebase.h"
#include "../simplegraph.h"

class Console final : public BaseConsole {
  public:
    using d_graph = SimpleGraph<double>;

    enum Action { NO_ACTION = 0, LOAD, GENERATE, SHOW, THREAD_NUM, EXEC_NUM, RUN, EXIT };

    Console() : BaseConsole(), threads_num{0} {}
    ~Console() {}

    void Run() override;
    void PrintResMatrix(WINDOW*, const d_graph& g);

  private:
    std::pair<d_graph, d_graph> mtrxs;
    d_graph result, async_result;
    int threads_num;
};
