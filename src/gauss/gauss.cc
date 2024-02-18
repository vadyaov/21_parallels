#include "gauss.h"

#include <iostream>
#include <mutex>
#include <thread>

namespace gaussmethod {

int FindPivotRow(const SimpleGraph<double>& matrix, int row, int col) {
  int max_row = row;
  for (int i = row + 1; i != matrix.get_rows(); ++i) {
    if (matrix[i][col] > matrix[max_row][col]) max_row = i;
  }

  if (std::abs(matrix[max_row][col]) < Gauss::EPS)
    throw std::runtime_error("continue");

  return max_row;
}

int Gauss::Solve(SimpleGraph<double> matr, std::vector<double>& answer) {
  const int n = matr.get_rows();
  const int m = matr.get_cols() - 1;

  // straight way
  std::vector<int> where(m, -1);
  for (int row = 0, col = 0; row < n && col < m; ++col) {
    // pivot searching
    try {
      matr.SwapRows(row, FindPivotRow(matr, row, col));
    } catch (...) {
      continue;
    }
    where[col] = row;

    // making triangle matrix
    for (int i = row + 1; i != n; ++i) {
      auto coef = matr[i][col] / matr[row][col];
      for (int j = col; j <= m; ++j) {
        matr[i][j] -= matr[row][j] * coef;
        if (std::abs(matr[i][j]) < EPS) matr[i][j] = 0;
      }
    }
    ++row;
  }

  // way back
  answer.assign(m, 0);
  for (int row = n - 1; row >= 0; --row) {
    // check sum of coefs near 'x'
    double sum = 0;
    for (int col = m - 1; col >= 0; --col) sum += matr[row][col];

    if (std::abs(sum) < EPS && std::abs(matr[row][m]) > EPS) {
      return NONE;
    }

    // works because matrix is triangle now
    for (int col = m; col >= row /* 0 */; --col)
      matr[row][col] /= matr[row][row];

    // making diagonal matrix
    for (int i = row - 1; i >= 0; --i) {
      double K = matr[i][row] / matr[row][row];
      for (int j = m; j >= 0; --j) {
        matr[i][j] = matr[i][j] - matr[row][j] * K;
      }
    }
  }

  // here we have diagonal main matrix, so answers are free members

  for (int i = 0; i != m; ++i)
    if (where[i] == -1) return LOT;

  for (int i = 0; i != n; ++i) {
    answer[i] = matr[i][n];
  }

  return ONE;
}

struct StraightWay {
  SimpleGraph<double>& matr_ref;
  std::vector<int>& where_ref;
  std::mutex& matr_mtx_ref;

  void operator()(int n, int m) {
    matr_mtx_ref.lock();
    for (int row = 0, col = 0; row < n && col < m; ++col) {
      try {
        matr_ref.SwapRows(row, FindPivotRow(matr_ref, row, col));
      } catch (...) {
        continue;
      }
      where_ref[col] = row;

      for (int i = row + 1; i != n; ++i) {
        double coef = matr_ref[i][col] / matr_ref[row][col];
        for (int j = 0; j <= m; ++j) {
          matr_ref[i][j] -= matr_ref[row][j] * coef;

          if (std::abs(matr_ref[i][j]) < Gauss::EPS) matr_ref[i][j] = 0;
        }
      }
      ++row;
    }
    matr_mtx_ref.unlock();
  }
};

struct BackwardWay {
  SimpleGraph<double>& matr_ref;
  std::mutex& matr_mtx_ref;

  void operator()(int n, int m) {
    matr_mtx_ref.lock();
    for (int row = n - 1; row >= 0; --row) {
      // check sum of coefs near 'x'
      double sum = 0;
      for (int col = m - 1; col >= 0; --col) sum += matr_ref[row][col];

      if (std::abs(sum) < Gauss::EPS &&
          std::abs(matr_ref[row][m]) > Gauss::EPS) {
        /* return NONE; */
      }

      for (int col = m; col >= row /* 0 */; --col)
        matr_ref[row][col] /= matr_ref[row][row];

      for (int i = row - 1; i >= 0; --i) {
        double K = matr_ref[i][row] / matr_ref[row][row];
        for (int j = m; j >= 0; --j) {
          matr_ref[i][j] = matr_ref[i][j] - matr_ref[row][j] * K;
        }
      }
    }
    matr_mtx_ref.unlock();
  }
};

int Gauss::ParallelSolve(SimpleGraph<double> matr,
                         std::vector<double>& answer) {
  const int n = matr.get_rows();
  const int m = matr.get_cols() - 1;

  std::mutex matr_mtx;

  std::vector<int> where(m, -1);

  StraightWay strw{matr, where, matr_mtx};
  BackwardWay bckw{matr, matr_mtx};

  std::thread straight_way(strw, n, m);
  std::thread backward_way(bckw, n, m);

  straight_way.join();
  backward_way.join();

  for (int i = 0; i != m; ++i)
    if (where[i] == -1) return LOT;

  answer.assign(n, 0);
  for (int i = 0; i != n; ++i) {
    answer[i] = matr[i][n];
  }

  return ONE;
}

}  // namespace gaussmethod
