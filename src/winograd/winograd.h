#ifndef WINOGRAD_H_
#define WINOGRAD_H_

#include "../simplegraph.h"

class Winograd {
  public:
    static SimpleGraph<double> Multiply(const SimpleGraph<double>& g,
                                        const SimpleGraph<double>& h);
    static SimpleGraph<double> AsyncMultiply(const SimpleGraph<double>& g,
                                             const SimpleGraph<double>& h);

  private:
    static void RowFactorCompute(const SimpleGraph<double>& g,
                           std::vector<double>& row_fact, int r, int c) {
      for (int i = 0; i != r; ++i) {
        row_fact[i] = g[i][0] * g[i][1];
        for (int j = 1; j != c; ++j) {
          row_fact[i] += g[i][2 * j] * g[i][2 * j + 1];
        }
      }
    }

    static void ColFactorCompute(const SimpleGraph<double>& h,
                           std::vector<double>& col_fact, int r, int c) {
      for (int i = 0; i != r; ++i) {
        col_fact[i] = h[0][i] * h[1][i];
        for (int j = 1; j != c; ++j) {
          col_fact[i] += h[2 * j][i] * h[2 * j + 1][i];
        }
      }
    }
};

#endif // WINOGRAD_H_
