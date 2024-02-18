#include "console.h"
#include "winograd.h"

#include <chrono>
#include <sstream>
#include <functional>

SimpleGraph<double> RandomMatrix(int rows, int cols);
bool Printable(const Console::d_graph& gr, int xmax, int ymax);
void print_result_window(WINDOW* output, double ms);
void print_error_window(WINDOW* output, const char * msg);

template<typename Function, typename... Args>
double RunMultiplication(int exec_num, Console::d_graph& result, Function&& func, Args&&... args) {
  auto t1 = std::chrono::high_resolution_clock::now();

  while (exec_num--) {
      result = std::invoke(std::forward<Function>(func), std::forward<Args>(args)...);
  }

  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> ms_double = t2 - t1;
  return ms_double.count();
}

void Console::Run() {
  WINDOW* menu_win = nullptr;
  WINDOW* matr_win = nullptr;

  WINDOW* classic_res_win = nullptr;
  WINDOW* parallel_res_win = nullptr;

  int highlight = 1;
  int choice = NO_ACTION;

  WINDOW* err_win = newwin(1, maxx, 9, 0);

  menu_win = newwin(choices.size() + 2, maxx, 0, 0);
  keypad(menu_win, TRUE); /* enable to use ARROW BUTNS in menu window */

  PrintMenu(menu_win, highlight, choices);

  while (1) {
    noecho();
    int c = wgetch(menu_win);
    switch (c) {
      case KEY_UP:
        if (highlight == 1)
          highlight = choices.size();
        else
          --highlight;
        break;
      case KEY_DOWN:
        if (highlight == (int)choices.size())
          highlight = 1;
        else
          ++highlight;
        break;
      case 10:
        choice = highlight;
        break;
    }

    PrintMenu(menu_win, highlight, choices);

    if (choice != NO_ACTION) {
      echo();
      wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
      wclrtoeol(menu_win);
      PrintMenu(menu_win, 0, choices);
      wattron(menu_win, A_BOLD);

      switch (choice) {
        case LOAD:
          LoadMatrices(menu_win);
          break;
        case GENERATE:
          GenerateMatrices(menu_win);
          break;
        case SHOW:
          ShowProcess(menu_win, matr_win, err_win);
          break;
        case THREAD_NUM:
          mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d", &threads_num);
          break;
        case EXEC_NUM:
          mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d", &exec_num);
          if (exec_num < 1)
            mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d is invalid", exec_num);
          break;
        case RUN_1: // run classic and parallel with 'n' threads
          {
          if (exec_num < 1 || threads_num < 1) break;

          if (classic_res_win) delwin(classic_res_win);
          classic_res_win = newwin(5, maxx / 2, maxy - 6, 0);
          try {
            double cl_time = RunMultiplication(exec_num, result,
                                  &Winograd::Multiply, mtrxs.first, mtrxs.second);
            print_result_window(classic_res_win, cl_time);
          } catch (const std::exception& e) {
            print_error_window(classic_res_win, e.what());
          }

          break;
          }
        case RUN_2: // run classic and pipeline parallel
          {
          if (exec_num < 1 || threads_num < 1) break;

          if (parallel_res_win) delwin(parallel_res_win);
          parallel_res_win = newwin(5, maxx / 2, maxy - 6, maxx / 2);
          try {
            double pr_time = RunMultiplication(exec_num, async_result,
                              &Winograd::AsyncMultiply, mtrxs.first, mtrxs.second,
                              threads_num);
            print_result_window(parallel_res_win, pr_time);
          } catch (const std::exception& e) {
            print_error_window(parallel_res_win, e.what());
          }

          break;
          }
        case RUN_3:
          {
            if (parallel_res_win) delwin(parallel_res_win);
            parallel_res_win = newwin(5, maxx / 2, maxy - 6, maxx / 2);

            try {
              double pr_time = RunMultiplication(exec_num, result,
                                  &Winograd::AsyncPipelineMultiply, mtrxs.first, mtrxs.second);
                print_result_window(parallel_res_win, pr_time);
            } catch (const std::exception& e) {
              print_error_window(parallel_res_win, e.what());
            }
            break;
          }
      }
      wattroff(menu_win, A_BOLD);
      PrintMenu(menu_win, highlight, choices);
      noecho();
    }

    if (choice == EXIT) break;

    choice = NO_ACTION;
  }

  delwin(menu_win);
  delwin(matr_win);
  delwin(classic_res_win);
  delwin(parallel_res_win);
  delwin(err_win);
}

void Console::LoadMatrices(WINDOW* menu) {
  char filepaths[256] = {0};
  mvwgetstr(menu, LOAD, 1 + choices[LOAD - 1].size() + 1, filepaths);

  std::stringstream sstr{filepaths};
  std::string path2;
  sstr >> path >> path2;
  try {
    mtrxs.first.LoadGraphFromFile(path);
    mtrxs.second.LoadGraphFromFile(path2);
  } catch (const std::exception& e) {
    wmove(menu, LOAD, 1 + choices[LOAD - 1].size() + 1);
    wclrtoeol(menu);
    mvwprintw(menu, LOAD, 1 + choices[LOAD - 1].size() + 1, "%s ", e.what()); 
  }
}

void Console::GenerateMatrices(WINDOW* menu) {
  char sizes[256] = {0};
  mvwgetstr(menu, GENERATE, 1 + choices[GENERATE - 1].size() + 1, sizes);
  std::stringstream sstr{sizes};
  int r1, r2, c1, c2;
  sstr >> r1 >> c1 >> r2 >> c2;

  mtrxs.first = RandomMatrix(r1, c1);
  mtrxs.second = RandomMatrix(r2, c2);
}

void Console::ShowMatrix(const d_graph& mtr, WINDOW* matr, WINDOW* err) {
  matr= newwin(mtr.get_rows() + 2, maxx, 10, 0);
  if (!Printable(mtr, maxx, maxy - (choices.size() + 2) - 7)) {
    print_error_window(err, "Matrix is too big for correct printing");
  }

  PrintResMatrix(matr, mtr);
}

void Console::ShowProcess(WINDOW* menu, WINDOW* matr, WINDOW* err) {
  wClear(stdscr, choices.size() + 2, maxy - 7);

  char input[2] = {0};
  mvwgetstr(menu, SHOW, 1 + choices[SHOW - 1].size() + 1, input);
  std::stringstream sstr{input};
  int matr_num;
  sstr >> matr_num;

  if (matr) delwin(matr);
  switch(matr_num) {
    case FIRST:
      ShowMatrix(mtrxs.first, matr, err);
      break;
    case SECOND:
      ShowMatrix(mtrxs.second, matr, err);
      break;
    case CL_RES:
      if (!result.Empty())
        ShowMatrix(result, matr, err);
      break;
    case PAR_RES:
      if (!async_result.Empty())
        ShowMatrix(async_result, matr, err);
      break;
    default:
      mvwprintw(menu, SHOW, 1 + choices[SHOW - 1].size() + 1, "%d is invalid", matr_num); 
      break;
  }
}

void Console::PrintResMatrix(WINDOW* output, const d_graph& g) {
  int x = 1, y = 1;
  box(output, 0, 0);
  for (int i = 0; i != g.get_rows(); ++i) {
    for (int j = 0; j != g.get_cols(); ++j) {
      mvwprintw(output, y, x, "%.2e", g[i][j]);
      x += 9;
    }
    y++;
    x = 1;
  }
  wrefresh(output);
}
