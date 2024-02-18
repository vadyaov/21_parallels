#include "console.h"

#include <limits>
#include <locale.h>
#include <chrono>

void Console::PrintGraph(WINDOW* graph_win) {
  int x = 1, y = 1;
  box(graph_win, 0, 0);
  for (int i = 0, sz = matrix.get_rows(); i != sz; ++i) {
    for (int j = 0; j != matrix.get_cols(); ++j) {
      mvwprintw(graph_win, y, x, "%.1lf", matrix[i][j]);
      x += 5;
    }
    y++;
    x = 1;
  }
  wrefresh(graph_win);
}

void print_result_window(WINDOW* output, const std::vector<double>& res, double ms) {
  int x = 1, y = 1;
  box(output, 0, 0);
  for (const double i : res) {
    mvwprintw(output, y, x, "%.1lf", i);
    x += 4;
    if (x >= getmaxx(output)) {
      x = 1;
      ++y;
    }
    if (y == 2) break;
  }
  x = 1;
  mvwprintw(output, ++y, x, "Execution time = %lf ms", ms);

  wrefresh(output);
}

void print_result_error_window(WINDOW* output, int flag) {
  box(output, 0, 0);

  if (flag == GaussMethod::Gauss::NONE)
    mvwprintw(output, 1, 1, "NO SOLUTION EXISTS");
  else
    mvwprintw(output, 1, 1, "A LOT OF SOLUTIONS");

  wrefresh(output);
}

void Console::Run() {
  WINDOW* menu_win = nullptr;
  WINDOW* matr_win = nullptr;

  WINDOW* classic_res_win = nullptr;
  WINDOW* parallel_res_win = nullptr;

  setlocale(LC_ALL, "");

  const std::vector<std::string> choices = {
      "Select .txt file with matrix:",
      "Input number of executions (N):",
      "RUN",
      "Exit"};

  int highlight = 1;
  int choice = NO_ACTION;

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

      // move cursor in menu_win to the start of text input
      wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
      // delete old input from cursos to the end of line
      wclrtoeol(menu_win);
      // remove the highlight
      PrintMenu(menu_win, 0, choices);

      wattron(menu_win, A_BOLD);

      switch (choice) {
        case LOAD:
          {
            char filepath[100] = {0};
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s", filepath);
            path = std::string(filepath);
            try {
              matrix.LoadGraphFromFile(path);

              // if win already has content -> clear to avoid text overlay
              if (matr_win) {
                delwin(matr_win);
              }
              matr_win = newwin(matrix.get_rows() + 2, matrix.get_cols() * 6,
                                          12, maxx / 2 - matrix.get_cols() * 2);

              PrintGraph(matr_win);
            } catch (const std::exception& e) {
              wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
              wclrtoeol(menu_win);
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s ", e.what()); 
            }
            break;
          }
        case EXEC_NUM:
          {
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d", &exec_num);
            if (exec_num < 1)
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1,
                                                    "%d is invalid", exec_num);
            break;
          }
        case RUN:
          {
            if (exec_num < 1) break;

              if (classic_res_win) delwin(classic_res_win);
              classic_res_win = newwin(5, maxx / 2, maxy - 6, 0);

              if (parallel_res_win) delwin(parallel_res_win);
              parallel_res_win = newwin(5, maxx / 2, maxy - 6, maxx / 2);

              try {
                std::vector<double> solution;
                int opt = GaussMethod::Gauss::Solve(matrix, solution);
                int executions = exec_num;

                if (opt != GaussMethod::Gauss::ONE) {
                  print_result_error_window(classic_res_win, opt);
                  print_result_error_window(parallel_res_win, opt);
                } else {
                  auto t1 = std::chrono::high_resolution_clock::now();

                  while (executions--) {
                    GaussMethod::Gauss::Solve(matrix, solution);
                  }

                  auto t2 = std::chrono::high_resolution_clock::now();
                  std::chrono::duration<double, std::milli> ms_double = t2 - t1;
                  print_result_window(classic_res_win, solution, ms_double.count());


                  executions = exec_num;

                  auto t3 = std::chrono::high_resolution_clock::now();
                  while (executions--) {
                    GaussMethod::Gauss::ParallelSolve(matrix, solution);
                  }
                  auto t4 = std::chrono::high_resolution_clock::now();
                  ms_double = t4 - t3;

                  print_result_window(parallel_res_win, solution, ms_double.count());
                }
              } catch (const std::exception& e) {

              }
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
}
