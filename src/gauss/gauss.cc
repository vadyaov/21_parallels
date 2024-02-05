#include "gauss.h"

#include <thread>
#include <mutex>

namespace GaussMethod {

int FindPivotRow(const SimpleGraph<double>& matrix, int row, int col) {
  int max_row = row;
  for (int i = row + 1; i != matrix.get_rows(); ++i) {
    if (matrix[i][col] > matrix[max_row][col])
      max_row = i;
  }

  if (std::abs(matrix[max_row][col]) < Gauss::EPS) throw std::runtime_error("continue");

  return max_row;
}

int Gauss::Solve(SimpleGraph<double> matr, std::vector<double>& answer) {
  const int n = matr.get_rows();
  const int m = matr.get_cols() - 1;

  // straight way
  std::vector<int> where(m, -1);
  for (int row = 0, col = 0; row < n && col < m; ++col) {
      try {
        matr.SwapRows(row, FindPivotRow(matr, row, col));
      } catch (...) {
        continue;
      }
      where[col] = row;

    for (int i = row + 1; i != n; ++i) {
      auto coef = matr[i][col] / matr[row][col];
      for (int j = col; j <= m; ++j) {
        matr[i][j] -= matr[row][j] * coef;
      }
    }
    ++row;
  }

  // way back
  answer.assign(m, 0);
  for (int row = n - 1; row >= 0; --row) {
    double sum = 0;
    for (int col = m - 1; col >= 0; --col) {
      sum += matr[row][col];
    }

    if (std::abs(sum) < EPS && std::abs(matr[row][m]) > EPS) {
      return NONE;
    }

    for (int col = matr.get_cols() - 1; col >= 0; --col)
      matr[row][col] /= matr[row][row];

    for (int i = row - 1; i >= 0; --i) {
      double K = matr[i][row] / matr[row][row];
      for (int j = matr.get_cols() - 1; j >= 0; --j) {
        matr[i][j] = matr[i][j] - matr[row][j] * K;
      }
    }

  }

  for (int i = 0; i != m; ++i)
    if (where[i] == -1) return LOT;


  for (int i = 0; i != n; ++i) {
    answer[i] = matr[i][n];
  }

  return ONE;
}

struct ForwardTask {
  SimpleGraph<double>& matr;
  int row, col;

  void operator()(int i) {
    double coef = matr[i][col] / matr[row][col];
    for (int j = col; j != matr.get_cols(); ++j) {
      matr[i][j] -= matr[row][j] * coef;
    }
  }
};

struct BackwardTask {
  SimpleGraph<double>& matr;
  int row;

  void operator()(int i) {
    double K = matr[i][row] / matr[row][row];
    for (int j = matr.get_cols() - 1; j >= 0; --j) {
      matr[i][j] -= matr[row][j] * K;
    }
  }
};

int Gauss::ParallelSolve(SimpleGraph<double> matr, std::vector<double>& answer) {
  const int n = matr.get_rows();
  const int m = matr.get_cols() - 1;

  std::vector<std::thread> threads;

  std::vector<int> where(m, -1);
  for (int row = 0, col = 0; row < n && col < m; ++col) {
      try {
        matr.SwapRows(row, FindPivotRow(matr, row, col));
      } catch (...) {
        continue;
      }
      where[col] = row;

    ForwardTask t{matr, row, col};
    for (int i = row + 1; i != n; ++i) {
      threads.emplace_back(t, i);
    }

    for (auto& th : threads) th.join();
    threads.clear();

    ++row;
  }

  answer.assign(m, 0);
  for (int row = n - 1; row >= 0; --row) {
    double sum = 0;
    for (int col = m - 1; col >= 0; --col) {
      sum += matr[row][col];
    }

    if (std::abs(sum) < EPS && std::abs(matr[row][m]) > EPS) {
      return NONE;
    }

    for (int col = matr.get_cols() - 1; col >= 0; --col)
      matr[row][col] /= matr[row][row];

    BackwardTask t{matr, row};
    for (int i = row - 1; i >= 0; --i) {
      threads.emplace_back(t, i);
    }

    for (auto& th : threads) th.join();
    threads.clear();

  }

  for (int i = 0; i != m; ++i)
    if (where[i] == -1) return LOT;


  for (int i = 0; i != n; ++i) {
    answer[i] = matr[i][n];
  }

  return ONE;
}

} // namespace GaussMethod
