#pragma once

#include "../consolebase.h"
#include "../simplegraph.h"

class Console final : public BaseConsole {
  public:
    using d_graph = SimpleGraph<double>;

    const std::vector<std::string> choices = {
      "Select .txt files with 1st & 2nd matrices (path1 path2):",
      "Generate random matrices with size (r1 c1 r2 c2):",
      "Show matrix (lhs: 0, rhs: 1, res: 2, async_res: 3):",
      "Input number of threads to use:",
      "Input number of executions (N):",
      "RUN (classic)",
      "RUN (parallel 'n' threads)",
      "RUN (pipeline parallel)",
      "Exit"
    };

    enum Action { NO_ACTION = 0, LOAD, GENERATE, SHOW, THREAD_NUM, EXEC_NUM, RUN_1, RUN_2, RUN_3, EXIT };
    enum MATRIX { FIRST = 0, SECOND, CL_RES, PAR_RES };

    Console() : BaseConsole(), threads_num{0} {}
    ~Console() {}

    void Run() override;
    void PrintResMatrix(WINDOW*, const d_graph& g);

  private:
    std::pair<d_graph, d_graph> mtrxs;
    d_graph result, async_result;
    int threads_num;

  private:
    void LoadMatrices(WINDOW* menu);
    void GenerateMatrices(WINDOW* menu);
    void ShowProcess(WINDOW* menu, WINDOW* matr, WINDOW* err);
    void ShowMatrix(const d_graph& mtr, WINDOW* matr, WINDOW* err);
};
