#include "winograd.h"

#include <iostream>

SimpleGraph<double> Winograd::Multiply(const SimpleGraph<double>& g,
                                       const SimpleGraph<double>& h) {
  if (g.get_cols() != h.get_rows())
    throw std::invalid_argument("Incorrect matrix size for miltiplication");

  int a = g.get_rows();
  int b = g.get_cols();
  int c = h.get_cols();

  int d = b / 2;
  std::vector<double> rowFactor(a);
  // вычисление rowFactors для G
  for (int i = 0; i != a; ++i) {
    rowFactor[i] = g[i][0] * g[i][1];
    for (int j = 1; j != d; ++j) {
      rowFactor[i] += g[i][2 * j] * g[i][2 * j + 1];
    }
  }

  std::vector<double> columnFactor(c);
  // вычисление columnFactor для H
  for (int i = 0; i != c; ++i) {
    columnFactor[i] = h[0][i] * h[1][i];
    for (int j = 1; j != d; ++j) {
      columnFactor[i] += h[2 * j][i] * h[2 * j + 1][i];
    }
  }

  SimpleGraph<double> r(a, c);
  // вычисление матрицы R
  for (int i = 0; i != a; ++i) {
    for (int j = 0; j != c; ++j) {
      r[i][j] = -rowFactor[i] - columnFactor[j];
      for (int k = 0; k != d; ++k) {
        r[i][j] += (g[i][2 * k] + h[2 * k + 1][j]) * (g[i][2 * k + 1] + h[2 * k][j]);
      }
    }
  }

  // прибавление членов в случае нечетной общей размерности
  if (2 * d != b) {
    for (int i = 0; i != a; ++i) { 
      for (int j = 0; j != c; ++j) {
        r[i][j] += g[i][b - 1] * h[b - 1][j];
      }
    }
  }

  return r;
}

