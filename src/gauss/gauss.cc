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

struct StraightWay {
  SimpleGraph<double>& matr;
  std::vector<int>& where;

  StraightWay(SimpleGraph<double>& m, std::vector<int>& wh) :
    matr{m}, where{wh} {}

  void operator()(int& row, int col) {
        try {
          matr.SwapRows(row, FindPivotRow(matr, row, col));
        } catch (...) {
          return;
        }
        where[col] = row;

      for (int i = row + 1; i != matr.get_rows(); ++i) {
        auto coef = matr[i][col] / matr[row][col];
        for (int j = col; j <= matr.get_cols() - 1; ++j) {
          matr[i][j] -= matr[row][j] * coef;
        }
      }
      ++row;
  }
};

int Gauss::ParallelSolve(SimpleGraph<double> matr, std::vector<double>& answer) {
  const int n = matr.get_rows();
  const int m = matr.get_cols() - 1;

  std::vector<std::thread> threads;
  std::mutex matr_mtx;

  std::vector<int> where(m, -1);
  StraightWay stw(matr, where);
  for (int row = 0, col = 0; col < m; ++col) {
    matr_mtx.lock();
    threads.push_back(std::thread(stw, std::ref(row), col));
    matr_mtx.unlock();
  }

  for (auto& th : threads)
    th.join();

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

} // namespace GaussMethod
