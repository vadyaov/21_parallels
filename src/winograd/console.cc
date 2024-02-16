#include "console.h"
#include "winograd.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <random>

bool Printable(const Console::d_graph& gr, int xmax, int ymax) {
  bool printable = true;

  if (gr.get_cols() * 8 + gr.get_cols() - 1 > xmax)
    printable = false;

  if (gr.get_rows() > ymax)
    printable = false;

  return printable;
}

double RandomValue(double min, double max) {
  static std::random_device rd;     // Only used once to initialise (seed) engine
  static std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
  static std::uniform_real_distribution<double> uni(min,max); // Guaranteed unbiased

  return uni(rng);
}

SimpleGraph<double> RandomMatrix(int rows, int cols) {
  SimpleGraph<double> g(rows, cols);
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      g[i][j] = RandomValue(10, 50);
    }
  }
  return g;
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
 
void print_result_window(WINDOW* output, double ms) {
  int x = 1, y = 1;
  box(output, 0, 0);
  mvwprintw(output, ++y, x, "Execution time = %lf ms", ms);
  wrefresh(output);
}

void print_error_window(WINDOW* output, const char * msg) {
  box(output, 0, 0);
  mvwprintw(output, 1, 1, "%s", msg);
  wrefresh(output);
}

void Console::Run() {
  WINDOW* menu_win = nullptr;
  WINDOW* matr_win = nullptr;

  WINDOW* classic_res_win = nullptr;
  WINDOW* parallel_res_win = nullptr;


  setlocale(LC_ALL, "");

  const std::vector<std::string> choices = {
      "Select .txt files with 1st & 2nd matrices (path1 path2):",
      "Generate random matrices with size (r1 c1 r2 c2):",
      "Show matrix (lhs: 0, rhs: 1, res: 2, async_res: 3):",
      "Input number of threads to use:",
      "Input number of executions (N):",
      "RUN",
      "Exit"};

  int highlight = 1;
  int choice = NO_ACTION;

  initscr();
  clear();
  cbreak(); /* Line buffering disabled. pass on everything */
  curs_set(0);

  const int maxx = getmaxx(stdscr);
  const int maxy = getmaxy(stdscr);

  const int max_rows = maxy - (choices.size() + 2) - 7;

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
            char filepaths[256] = {0};
            mvwgetstr(menu_win, choice, 1 + choices[choice - 1].size() + 1, filepaths);
            std::stringstream sstr{filepaths};
            std::string path2;
            sstr >> path >> path2;
            try {
              mtrxs.first.LoadGraphFromFile(path);
              mtrxs.second.LoadGraphFromFile(path2);
            } catch (const std::exception& e) {
              wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
              wclrtoeol(menu_win);
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s ", e.what()); 
            }
            break;
          }
        case GENERATE:
          {
            char sizes[256] = {0};
            mvwgetstr(menu_win, choice, 1 + choices[choice - 1].size() + 1, sizes);
            std::stringstream sstr{sizes};
            int r1, r2, c1, c2;
            sstr >> r1 >> c1 >> r2 >> c2;
            try {
              mtrxs.first = RandomMatrix(r1, c1);
              mtrxs.second = RandomMatrix(r2, c2);
            } catch (const std::exception& e) {
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1,
                                                    "%s", e.what());
            }
            break;
          }
        case SHOW:
          {

            wClear(stdscr, choices.size() + 2, maxy - 7);

            char input[64] = {0};
            mvwgetstr(menu_win, choice, 1 + choices[choice - 1].size() + 1, input);
            std::stringstream sstr{input};
            int matr_num;
            sstr >> matr_num;
            if (matr_num == 0) {
              if (matr_win) delwin(matr_win);
              matr_win = newwin(mtrxs.first.get_rows() + 2, maxx, 10, 0);
              if (!Printable(mtrxs.first, maxx, max_rows)) {
                print_error_window(err_win, "Matrix is too big for correct printing");
              }

              PrintResMatrix(matr_win, mtrxs.first);
            } else if (matr_num == 1) {
              if (matr_win) delwin(matr_win);
              matr_win = newwin(mtrxs.second.get_rows() + 2, maxx, 10, 0);
              if (!Printable(mtrxs.second, maxx, max_rows))
                print_error_window(err_win, "Matrix is too big for correct printing");
              PrintResMatrix(matr_win, mtrxs.second);
            } else if (matr_num == 2) {
              if (!result.Empty()) {
                if (matr_win) delwin(matr_win);
                matr_win = newwin(result.get_rows() + 2, maxx, 10, 0);
                if (!Printable(result, maxx, max_rows))
                  print_error_window(err_win, "Matrix is too big for correct printing");
                PrintResMatrix(matr_win, result);
              }
            } else if (matr_num == 3) {
              if (!async_result.Empty()) {
                if (matr_win) delwin(matr_win);
                matr_win = newwin(async_result.get_rows() + 2, maxx, 10, 0);
                if (!Printable(async_result, maxx, max_rows))
                  print_error_window(err_win, "Matrix is too big for correct printing");
                PrintResMatrix(matr_win, async_result);
              }
            } else {
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d is invalid", matr_num); 
            }
            break;
          }
        case THREAD_NUM:
          {
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d", &threads_num);
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
            if (exec_num < 1 || threads_num < 1) break;

            if (classic_res_win) delwin(classic_res_win);
            classic_res_win = newwin(5, maxx / 2, maxy - 6, 0);

            if (parallel_res_win) delwin(parallel_res_win);
            parallel_res_win = newwin(5, maxx / 2, maxy - 6, maxx / 2);

            int executions = exec_num;
            try {
                auto t1 = std::chrono::high_resolution_clock::now();

                while (executions--) {
                  result = Winograd::Multiply(mtrxs.first, mtrxs.second);
                }

                auto t2 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> ms_double = t2 - t1;
                print_result_window(classic_res_win, ms_double.count());
            } catch (const std::exception& e) {
              print_error_window(classic_res_win, e.what());
            }

            executions = exec_num;
            try {
                auto t3 = std::chrono::high_resolution_clock::now();
                while (executions--) {
                  async_result = Winograd::AsyncMultiply(mtrxs.first, mtrxs.second, threads_num);
                }

                auto t4 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> ms_double = t4 - t3;
                print_result_window(parallel_res_win, ms_double.count());
            } catch (const std::exception& e) {
              print_error_window(parallel_res_win, e.what());
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
  /* clrtoeol(); */
  endwin();
}
