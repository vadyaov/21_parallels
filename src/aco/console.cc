#include "console.h"

#include <locale.h>

#include <chrono>
#include <limits>

namespace ant {

void print_result_window(WINDOW* output, const AntColony::TsmResult& res,
                         double ms) {
  int x = 1, y = 1;
  box(output, 0, 0);
  mvwprintw(output, y++, x, "Distance: %lf", res.distance);
  mvwprintw(output, y, x, "Path: ");
  x += 6;  // because string "path " is 6 size
  for (int v : res.vertices) {
    mvwprintw(output, y, x, "%d", v);
    int add = 2;
    while (v >= 10) {
      v /= 10;
      add++;
    }
    x += add;
  }

  x = 1;
  mvwprintw(output, ++y, x, "Execution time = %lf ms", ms);

  wrefresh(output);
}

void Console::PrintGraph(WINDOW* graph_win) {
  int x = 1, y = 1;
  box(graph_win, 0, 0);
  for (std::size_t i = 0, sz = g.Size(); i != sz; ++i) {
    for (std::size_t j = 0; j != sz; ++j) {
      mvwprintw(graph_win, y, x, "%d", g[i][j]);
      x += 4;
    }
    y++;
    x = 1;
  }
  wrefresh(graph_win);
}

void Console::Run() {
  WINDOW* menu_win = nullptr;
  WINDOW* matr_win = nullptr;
  WINDOW* classic_aco_win = nullptr;
  WINDOW* parallel_aco_win = nullptr;

  setlocale(LC_ALL, "");

  const std::vector<std::string> choices = {
      "Select .txt file with matrix(graph):", "Input number of executions (N):",
      "RUN", "Exit"};

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
        case LOAD: {
          char filepath[100] = {0};
          mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s",
                   filepath);
          path = std::string(filepath);
          try {
            g.LoadGraphFromFile(path);

            // if win already has content -> clear to avoid text overlay
            if (matr_win) {
              delwin(matr_win);
            }
            matr_win =
                newwin(g.Size() + 2, g.Size() * 4, 12, maxx / 2 - g.Size() * 2);

            PrintGraph(matr_win);
          } catch (const std::invalid_argument& e) {
            wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
            wclrtoeol(menu_win);
            mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1,
                      "%s", e.what());
          }
          break;
        }
        case EXEC_NUM: {
          mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d",
                   &exec_num);
          if (exec_num < 1)
            mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1,
                      "%d is invalid", exec_num);
          break;
        }
        case RUN: {
          if (exec_num > 0 && !g.Empty()) {
            if (classic_aco_win) delwin(classic_aco_win);
            classic_aco_win = newwin(5, maxx / 2, maxy - 6, 0);

            if (parallel_aco_win) delwin(parallel_aco_win);
            parallel_aco_win = newwin(5, maxx / 2, maxy - 6, maxx / 2);

            try {
              AntColony::TsmResult classic_res = AntColony::ClassicSolve(g, 25);
              int executions = exec_num;

              auto t1 = std::chrono::high_resolution_clock::now();
              while (--executions) {
                auto tmp = AntColony::ClassicSolve(g, 25);
                if (tmp.distance < classic_res.distance) classic_res = tmp;
              }
              auto t2 = std::chrono::high_resolution_clock::now();
              std::chrono::duration<double, std::milli> ms_double = t2 - t1;

              print_result_window(classic_aco_win, classic_res,
                                  ms_double.count());

              auto parallel_res = AntColony::ParallelSolve(g, 25);
              executions = exec_num;
              t1 = std::chrono::high_resolution_clock::now();
              while (--executions) {
                auto tmp = AntColony::ParallelSolve(g, 25);
                if (tmp.distance < parallel_res.distance) parallel_res = tmp;
              }
              t2 = std::chrono::high_resolution_clock::now();
              ms_double = t2 - t1;

              print_result_window(parallel_aco_win, parallel_res,
                                  ms_double.count());
            } catch (const std::exception& e) {
              // exception routine
            }
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
  delwin(classic_aco_win);
  delwin(parallel_aco_win);
}

}  // namespace ant
