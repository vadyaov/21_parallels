#pragma once

#include <string>
#include <vector>

#include <ncurses.h>

class BaseConsole {
  public:
    enum Action { NO_ACTION = 0, LOAD, EXEC_NUM, RUN, EXIT };

    BaseConsole() : exec_num{0} {}

    virtual ~BaseConsole() {}

    virtual void Run() = 0;

    virtual void PrintGraph(WINDOW* graph_win) = 0;

    virtual void PrintMenu(WINDOW* menu_win, int highlight,
                              const std::vector<std::string>& choices) {
      int x = 1, y = 1;

      box(menu_win, 0, 0);
      for (std::size_t i = 0; i < choices.size(); ++i) {
        if (highlight == static_cast<int>(i + 1)) {
          wattron(menu_win, A_STANDOUT);
          mvwprintw(menu_win, y, x, "%s", choices[i].data());
          wattroff(menu_win, A_STANDOUT);
        } else
          mvwprintw(menu_win, y, x, "%s", choices[i].data());
        ++y;
      }
      wrefresh(menu_win);
    }

  protected:
    std::string path;
    int exec_num;

};
