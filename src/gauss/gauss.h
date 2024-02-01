#ifndef GAUSS_H_
#define GAUSS_H_

#include "simple_matrix.h"

#include <vector>

namespace GaussMethod {

  class Gauss {
    public:
      constexpr static double EPS = 1e-6;
      enum {NONE = 0, ONE, LOT};
      static int Solve(SimpleMatrix matr, std::vector<double>& answer);
  };

} // namespace GaussMethod

#endif // GAUSS_H_
