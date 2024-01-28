#include "gauss.h"

namespace GaussMethod {

int FindFirstNonZeroRow(const SimpleMatrix& matrix, int k) {
  int row = k;
  while (row != matrix.get_rows() && matrix.at(row, k) == 0)
    ++row;

  if (row == matrix.get_rows()) throw std::runtime_error("continue");

  return row;
}

std::vector<double> Gauss::Solve(const SimpleMatrix& matr) {
  SimpleMatrix clone = matr;

  // прямой ход
  for (int k = 0; k < clone.get_rows(); ++k) {
    if (clone.at(k, k) == 0) {
      try {
        clone.SwapRows(k, FindFirstNonZeroRow(clone, k));
      } catch (...) {
        continue;
      }
    }

    for (int i = k + 1; i != clone.get_rows(); ++i) {
      auto coef = clone.at(i, k) / clone.at(k, k);
      for (int j = k; j != clone.get_cols(); ++j) {
        clone.at(i, j) = clone.at(i, j) - clone.at(k, j) * coef;
      }
    }
  }

  /* matr = clone; */

  /* std::cout << clone << std::endl; */

  for (int row = clone.get_rows() - 1; row >= 0; --row) {
    for (int col = clone.get_cols() - 1; col >= 0; --col)
      clone.at(row, col) /= clone.at(row, row);

    for (int i = row - 1; i >= 0; --i) {
      double K = clone.at(i, row) / clone.at(row, row);
      for (int j = clone.get_cols() - 1; j >= 0; --j) {
        clone.at(i, j) = clone.at(i, j) - clone.at(row, j) * K;
      }
    }

  }

  std::vector<double> answer(clone.get_rows());
  for (int i = 0; i != clone.get_rows(); ++i) {
    answer[i] = clone.at(i, clone.get_rows());
  }

  std::cout << clone << std::endl;

  return answer;
}

} // namespace GaussMethod
