#include "winograd.h"

#include <omp.h>

#include <thread>

namespace winograd {

SimpleGraph<double> Winograd::Multiply(const SimpleGraph<double>& g,
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

  // вычисление rowFactors для G
  for (int i = 0; i != a; ++i) {
    rowFactor[i] = g[i][0] * g[i][1];
    for (int j = 1; j != d; ++j) {
      rowFactor[i] += g[i][2 * j] * g[i][2 * j + 1];
    }
  }

  // вычисление columnFactor для H
  for (int i = 0; i != c; ++i) {
    colFactor[i] = h[0][i] * h[1][i];
    for (int j = 1; j != d; ++j) {
      colFactor[i] += h[2 * j][i] * h[2 * j + 1][i];
    }
  }

  // вычисление матрицы R
  for (int i = 0; i != a; ++i) {
    for (int j = 0; j != c; ++j) {
      r[i][j] = -rowFactor[i] - colFactor[j];
      for (int k = 0; k != d; ++k) {
        r[i][j] +=
            (g[i][2 * k] + h[2 * k + 1][j]) * (g[i][2 * k + 1] + h[2 * k][j]);
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
                                            const SimpleGraph<double>& h,
                                            int num_threads) {
  if (g.get_cols() != h.get_rows())
    throw std::invalid_argument("Incorrect matrix size for miltiplication");

  int n = std::thread::hardware_concurrency();
  if (num_threads < 2 || num_threads > n * 4)
    throw std::invalid_argument("Number o threads should be in [2; 4*cpu's]");
  if (num_threads % 2 != 0)
    throw std::invalid_argument("Number of threads should be even.");

  omp_set_num_threads(num_threads);

  int a = g.get_rows();
  int b = g.get_cols();

  int c = h.get_cols();
  int d = b / 2;

  std::vector<double> rowFactor(a);
  std::vector<double> columnFactor(c);

  SimpleGraph<double> r(a, c);

  // вычисление rowFactors для G
#pragma omp parallel for
  for (int i = 0; i != a; ++i) {
    rowFactor[i] = g[i][0] * g[i][1];
    for (int j = 1; j != d; ++j) {
      rowFactor[i] += g[i][2 * j] * g[i][2 * j + 1];
    }
  }

  // вычисление columnFactor для H
#pragma omp parallel for
  for (int i = 0; i != c; ++i) {
    columnFactor[i] = h[0][i] * h[1][i];
    for (int j = 1; j != d; ++j) {
      columnFactor[i] += h[2 * j][i] * h[2 * j + 1][i];
    }
  }

  // вычисление матрицы R
#pragma omp parallel for
  for (int i = 0; i != a; ++i) {
    for (int j = 0; j != c; ++j) {
      r[i][j] = -rowFactor[i] - columnFactor[j];
      for (int k = 0; k != d; ++k) {
        r[i][j] +=
            (g[i][2 * k] + h[2 * k + 1][j]) * (g[i][2 * k + 1] + h[2 * k][j]);
      }
    }
  }

  // прибавление членов в случае нечетной общей размерности
  if (2 * d != b) {
#pragma omp parallel for
    for (int i = 0; i != a; ++i) {
      for (int j = 0; j != c; ++j) {
        r[i][j] += g[i][b - 1] * h[b - 1][j];
      }
    }
  }

  return r;
}

SimpleGraph<double> Winograd::AsyncPipelineMultiply(
    const SimpleGraph<double>& g, const SimpleGraph<double>& h) {
  if (g.get_cols() != h.get_rows())
    throw std::invalid_argument("Incorrect matrix size for miltiplication");

  int a = g.get_rows();
  int b = g.get_cols();
  int c = h.get_cols();
  int d = b / 2;

  std::vector<double> rowFactor(a);
  std::vector<double> colFactor(c);

  SimpleGraph<double> r(a, c);

  std::thread row_fact_thread(RowFactorCompute, std::cref(g),
                              std::ref(rowFactor), a, d);
  std::thread col_fact_thread(ColFactorCompute, std::cref(h),
                              std::ref(colFactor), c, d);

  row_fact_thread.join();
  col_fact_thread.join();

  for (int i = 0; i != a; ++i) {
    for (int j = 0; j != c; ++j) {
      r[i][j] = -rowFactor[i] - colFactor[j];
      for (int k = 0; k != d; ++k) {
        r[i][j] +=
            (g[i][2 * k] + h[2 * k + 1][j]) * (g[i][2 * k + 1] + h[2 * k][j]);
      }
    }
  }

  std::thread ifeven_thread([=, &r, &g, &h]() {
    if (2 * d != b) {
      for (int i = 0; i != a; ++i) {
        for (int j = 0; j != c; ++j) {
          r[i][j] += g[i][b - 1] * h[b - 1][j];
        }
      }
    }
  });
  ifeven_thread.join();

  return r;
}

}  // namespace winograd
