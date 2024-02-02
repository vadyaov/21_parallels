#include "gauss.h"

namespace GaussMethod {

int FindPivotRow(const SimpleMatrix& matrix, int row, int col) {
  int max_row = row;
  for (int i = row + 1; i != matrix.get_rows(); ++i) {
    if (matrix.at(i, col) > matrix.at(max_row, col))
      max_row = i;
  }

  if (std::abs(matrix.at(max_row, col)) < Gauss::EPS) throw std::runtime_error("continue");

  return max_row;
}

int Gauss::Solve(SimpleMatrix matr, std::vector<double>& answer) {
  const int n = matr.get_rows();
  const int m = matr.get_cols() - 1;

  // прямой ход
  std::vector<int> where(m, -1);
  for (int row = 0, col = 0; row < n && col < m; ++col) {
      try {
        matr.SwapRows(row, FindPivotRow(matr, row, col));
      } catch (...) {
        continue;
      }
      where[col] = row;

    for (int i = row + 1; i != n; ++i) {
      auto coef = matr.at(i, col) / matr.at(row, col);
      for (int j = col; j <= m; ++j) {
        matr.at(i, j) -= matr.at(row, j) * coef;
      }
    }
    ++row;
  }

  /* std::cout.precision(1); */
  /* std::cout << matr << std::endl; */

  // обратный ход
  answer.assign(m, 0);
  for (int row = n - 1; row >= 0; --row) {
    double sum = 0;
    for (int col = m - 1; col >= 0; --col) {
      sum += matr.at(row, col);
    }

    if (std::abs(sum) < EPS && std::abs(matr.at(row, m)) > EPS) {
      return NONE;
    }

    for (int col = matr.get_cols() - 1; col >= 0; --col)
      matr.at(row, col) /= matr.at(row, row);

    for (int i = row - 1; i >= 0; --i) {
      double K = matr.at(i, row) / matr.at(row, row);
      for (int j = matr.get_cols() - 1; j >= 0; --j) {
        matr.at(i, j) = matr.at(i, j) - matr.at(row, j) * K;
      }
    }

  }

  for (int i = 0; i != m; ++i)
    if (where[i] == -1) return LOT;


  for (int i = 0; i != n; ++i) {
    answer[i] = matr.at(i, n);
  }

  return ONE;
}

} // namespace GaussMethod
