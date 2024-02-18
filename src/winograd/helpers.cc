#include <chrono>
#include <random>

#include "console.h"

namespace winograd {

double RandomValue(double min, double max) {
  static std::random_device rd;  // Only used once to initialise (seed) engine
  static std::mt19937 rng(
      rd());  // Random-number engine used (Mersenne-Twister in this case)
  static std::uniform_real_distribution<double> uni(
      min, max);  // Guaranteed unbiased

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

bool Printable(const Console::d_graph& gr, int xmax, int ymax) {
  bool printable = true;

  if (gr.get_cols() * 8 + gr.get_cols() - 1 > xmax) printable = false;

  if (gr.get_rows() > ymax) printable = false;

  return printable;
}

void print_result_window(WINDOW* output, double ms) {
  int x = 1, y = 1;
  box(output, 0, 0);
  mvwprintw(output, ++y, x, "Execution time = %lf ms", ms);
  wrefresh(output);
}

void print_error_window(WINDOW* output, const char* msg) {
  box(output, 0, 0);
  mvwprintw(output, 1, 1, "%s", msg);
  wrefresh(output);
}

} // namespace winograd
