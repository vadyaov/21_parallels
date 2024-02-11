#include "winograd.h"

#include <iostream>
#include <thread>

SimpleGraph<double> Winograd::Multiply(const SimpleGraph<double>& g,
                                       const SimpleGraph<double>& h) {
  if (g.get_cols() != h.get_rows())
    throw std::invalid_argument("Incorrect matrix size for miltiplication");

  int a = g.get_rows();
  int b = g.get_cols();

  int c = h.get_cols();
  int d = b / 2;

  std::vector<double> rowFactor(a);
  std::vector<double> columnFactor(c);

  SimpleGraph<double> r(a, c);

  // вычисление rowFactors для G
  for (int i = 0; i != a; ++i) {
    rowFactor[i] = g[i][0] * g[i][1];
    for (int j = 1; j != d; ++j) {
      rowFactor[i] += g[i][2 * j] * g[i][2 * j + 1];
    }
  }

  // вычисление columnFactor для H
  for (int i = 0; i != c; ++i) {
    columnFactor[i] = h[0][i] * h[1][i];
    for (int j = 1; j != d; ++j) {
      columnFactor[i] += h[2 * j][i] * h[2 * j + 1][i];
    }
  }

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

SimpleGraph<double> Winograd::AsyncMultiply(const SimpleGraph<double>& g,
                                       const SimpleGraph<double>& h) {
  if (g.get_cols() != h.get_rows())
    throw std::invalid_argument("Incorrect matrix size for miltiplication");

  int a = g.get_rows();
  int b = g.get_cols();

  int c = h.get_cols();
  int d = b / 2;

  std::vector<double> rowFactor(a);
  std::vector<double> colFactor(c);
  
  SimpleGraph<double> r(a, c);

  std::thread row_fact_thread(RowFactorCompute, std::cref(g), std::ref(rowFactor), a, d);
  std::thread col_fact_thread(ColFactorCompute, std::cref(h), std::ref(colFactor), c, d);

  row_fact_thread.join();
  col_fact_thread.join();

  // прибавление членов в случае нечетной общей размерности
  std::thread ifeven_thread([=, &r, &g, &h]() {
      if (2 * d != b) {
        for (int i = 0; i != a; ++i) { 
          for (int j = 0; j != c; ++j) {
            r[i][j] += g[i][b - 1] * h[b - 1][j];
          }
        }
      }});

  // вычисление матрицы R
  for (int i = 0; i != a; ++i) {
    for (int j = 0; j != c; ++j) {
      r[i][j] = -rowFactor[i] - colFactor[j];
      for (int k = 0; k != d; ++k) {
        r[i][j] += (g[i][2 * k] + h[2 * k + 1][j]) * (g[i][2 * k + 1] + h[2 * k][j]);
      }
    }
  }

  ifeven_thread.join();

  return r;
}
