#include "console.h"
#include "winograd.h"

#include <chrono>

void Console::PrintResMatrix(WINDOW* output, const d_graph& g) {
  int x = 1, y = 1;
  box(output, 0, 0);
  for (int i = 0; i != g.get_rows(); ++i) {
    for (int j = 0; j != g.get_cols(); ++j) {
      mvwprintw(output, y, x, "%.2e", g[i][j]);
      x += 10;
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
  WINDOW* res_matr_win = nullptr;
  WINDOW* async_res_matr_win = nullptr;

  WINDOW* classic_res_win = nullptr;
  WINDOW* parallel_res_win = nullptr;

  setlocale(LC_ALL, "");

  const std::vector<std::string> choices = {
      "Select .txt file with first matrix:",
      "Select .txt file with second matrix:",
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
        case LOAD_1:
          {
            char filepath[100] = {0};
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s", filepath);
            path = std::string(filepath);
            try {
              mtrxs.first.LoadGraphFromFile(path);
            } catch (const std::exception& e) {
              wmove(menu_win, choice, 1 + choices[choice - 1].size() + 1);
              wclrtoeol(menu_win);
              mvwprintw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s ", e.what()); 
            }
            break;
          }
        case LOAD_2:
          {
            char filepath[100] = {0};
            mvwscanw(menu_win, choice, 1 + choices[choice - 1].size() + 1, "%s", filepath);
            path = std::string(filepath);
            try {
              mtrxs.second.LoadGraphFromFile(path);
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
                auto t1 = std::chrono::high_resolution_clock::now();
                int executions = exec_num;

                d_graph result;

                while (executions--) {
                  result = Winograd::Multiply(mtrxs.first, mtrxs.second);
                }

                if (res_matr_win) delwin(res_matr_win);
                res_matr_win = newwin(result.get_rows() + 2, maxx, 10, 0);
                PrintResMatrix(res_matr_win, result);

                // result matrix

                auto t2 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> ms_double = t2 - t1;
                print_result_window(classic_res_win, ms_double.count());

                executions = exec_num;
                d_graph async_result;

                auto t3 = std::chrono::high_resolution_clock::now();
                while (executions--) {
                  async_result = Winograd::AsyncMultiply(mtrxs.first, mtrxs.second);
                }

                auto t4 = std::chrono::high_resolution_clock::now();
                ms_double = t4 - t3;

                if (async_res_matr_win) delwin(async_res_matr_win);
                async_res_matr_win = newwin(async_result.get_rows() + 2, maxx, 10 + result.get_rows() + 3, 0);
                PrintResMatrix(async_res_matr_win, async_result);


                print_result_window(parallel_res_win, ms_double.count());
            } catch (const std::exception& e) {
              print_error_window(classic_res_win, e.what());
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
