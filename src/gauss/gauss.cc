#include "gauss.h"
#include "simple_matrix.h"

namespace GaussMethod {

std::vector<double> Gauss::Solve(const SimpleMatrix& matr) {
  SimpleMatrix clone = matr;
  for (int row = 0; clone.at(0, 0) == 0; row++) {
  }

  return {};
}

} // namespace GaussMethod
