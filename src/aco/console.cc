#include "console.h"

#include <ncurses.h>
#include <limits>
#include <locale.h>

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

void print_graph(WINDOW* graph_win, const ACO& g) {
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

void print_result_window(WINDOW* output, const ACO::TsmResult& res = {}) {
  int x = 1, y = 1;
  box(output, 0, 0);
  mvwprintw(output, y++, x, "Distance: %lf", res.distance);
  mvwprintw(output, y, x, "Path: ");
  x += 6; // because string "path " is 6 size
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
  mvwprintw(output, ++y, x, "Execution time = ");

  wrefresh(output);
}

void Console::Run() {
  WINDOW* menu_win = nullptr;
  WINDOW* matr_win = nullptr;
  WINDOW* classic_aco_win = nullptr;
  WINDOW* parallel_aco_win = nullptr;

  setlocale(LC_ALL, "");

  const std::vector<std::string> choices = {
      "Select .txt file with matrix(graph):",
      "Input number of executions (N):",
      "Input number of ant populations (n):",
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
            graph_path = std::string(path);
            try {
              g.LoadGraphFromFile(graph_path);

              // if win already has content -> clear to avoid text overlay
              if (matr_win) {
                wclear(matr_win);
                wrefresh(matr_win);
              }

              matr_win = newwin(g.Size() + 2, g.Size() * 4, 12, maxx / 2 - g.Size() * 2);
              print_graph(matr_win, g);
            } catch (const std::invalid_argument& e) {
              wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
              wclrtoeol(menu_win);
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s", e.what()); 
            }
            break;
          }
        case EXEC_NUM:
          {
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d", &exec_num);
            break;
          }
        case POPUL_NUM:
          {
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%d", &popul_num);
            break;
          }
        case RUN:
          {
            // create window for classic aco
            classic_aco_win = newwin(5, maxx / 2, maxy - 6, 0);

            // create window for parallel aco
            parallel_aco_win = newwin(5, maxx / 2, maxy - 6, maxx / 2);

            // CALCULATE paths and time
            auto classic = g.ClassicACO(popul_num);

            /* auto parallel = g.ParallelACO(popul_num); */

            print_result_window(classic_aco_win, classic);
            print_result_window(parallel_aco_win, {});

          }
      }
      wattroff(menu_win, A_BOLD);
      print_menu(menu_win, highlight, choices);
      noecho();
    }

    if (choice == EXIT) break;

    choice = NO_ACTION;
  }

  /* clrtoeol(); */
  endwin();
}
