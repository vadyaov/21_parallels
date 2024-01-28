#include "console.h"

#include <ncurses.h>
#include <limits>
#include <locale.h>
#include <chrono>

void print_menu(WINDOW* menu_win, int highlight,
                const std::vector<std::string>& choices) {
  int x = 1, y = 1;

  box(menu_win, 0, 0);
  for (std::size_t i = 0; i < choices.size(); ++i) {
    if (highlight == static_cast<int>(i + 1)) {
      wattron(menu_win, A_STANDOUT); // turn on highlighting
      mvwprintw(menu_win, y, x, "%s", choices[i].data());
      wattroff(menu_win, A_STANDOUT);
    } else
      mvwprintw(menu_win, y, x, "%s", choices[i].data());
    ++y;
  }
  wrefresh(menu_win);
}

void print_graph(WINDOW* graph_win, const SimpleMatrix* g) {
  int x = 1, y = 1;
  box(graph_win, 0, 0);
  for (int i = 0, sz = g->get_rows(); i != sz; ++i) {
    for (int j = 0; j != g->get_cols(); ++j) {
      mvwprintw(graph_win, y, x, "%.1lf", g->at(i, j));
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
  }
  y++;
  x = 1;
  mvwprintw(output, ++y, x, "Execution time = %lf ms", ms);

  wrefresh(output);
}

void Console::Run() {
  WINDOW* menu_win = nullptr;
  WINDOW* matr_win = nullptr;
  WINDOW* classic_aco_win = nullptr;
  WINDOW* parallel_aco_win = nullptr;

  setlocale(LC_ALL, "");

  const std::vector<std::string> choices = {
      "Select .txt file with matrix:",
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

  menu_win = newwin(choices.size() + 2, maxx, 0, 0);
  keypad(menu_win, TRUE); /* enable to use ARROW BUTNS in menu window */

  print_menu(menu_win, highlight, choices);
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

    print_menu(menu_win, highlight, choices);

    if (choice != NO_ACTION) {
      echo();

      // move cursor in menu_win to the start of text input
      wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
      // delete old input from cursos to the end of line
      wclrtoeol(menu_win);
      // remove the highlight
      print_menu(menu_win, 0, choices);

      wattron(menu_win, A_BOLD);

      switch (choice) {
        case LOAD:
          {
            char path[100] = {0};
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s", path);
            matrix_path = std::string(path);
            try {
              delete matrix;
              matrix = new SimpleMatrix(matrix_path);

              // if win already has content -> clear to avoid text overlay
              if (matr_win) {
                delwin(matr_win);
              }
              matr_win = newwin(matrix->get_rows() + 2, matrix->get_cols() * 6,
                                          12, maxx / 2 - matrix->get_cols() * 2);

              print_graph(matr_win, matrix);
            } catch (const std::exception& e) {
              wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
              wclrtoeol(menu_win);
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s", e.what()); 
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
            if (exec_num > 0) {
              if (classic_aco_win) delwin(classic_aco_win);
              classic_aco_win = newwin(5, maxx / 2, maxy - 6, 0);

              if (parallel_aco_win) delwin(parallel_aco_win);
              parallel_aco_win = newwin(5, maxx / 2, maxy - 6, maxx / 2);

              try {
                auto classic_res = GaussMethod::Gauss::Solve(*matrix);
                int executions = exec_num;

                auto t1 = std::chrono::high_resolution_clock::now();
                while (--executions) {
                  classic_res = GaussMethod::Gauss::Solve(*matrix);
                }
                auto t2 = std::chrono::high_resolution_clock::now();
                /* auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1); */
                std::chrono::duration<double, std::milli> ms_double = t2 - t1;

                print_result_window(classic_aco_win, classic_res, ms_double.count());


                auto parallel_res = GaussMethod::Gauss::Solve(*matrix);
                executions = exec_num;
                t1 = std::chrono::high_resolution_clock::now();
                while (--executions) {
                  parallel_res = GaussMethod::Gauss::Solve(*matrix);
                }
                t2 = std::chrono::high_resolution_clock::now();
                /* ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1); */
                ms_double = t2 - t1;

                print_result_window(parallel_aco_win, parallel_res, ms_double.count());
              } catch (const std::exception& e) {
                // exception routine
              }
            }
          }
      }
      wattroff(menu_win, A_BOLD);
      print_menu(menu_win, highlight, choices);
      noecho();
    }

    if (choice == EXIT) break;

    choice = NO_ACTION;
  }

  delwin(menu_win);
  /* clrtoeol(); */
  endwin();
}
