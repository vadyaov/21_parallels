#ifndef GAUSS_H_
#define GAUSS_H_

#include "../simplegraph.h"

#include <vector>

namespace GaussMethod {

  class Gauss {
    public:
      constexpr static double EPS = 1e-6;
      enum {NONE = 0, ONE, LOT};

      static int Solve(SimpleGraph<double> matr, std::vector<double>& answer);
      static int ParallelSolve(SimpleGraph<double> matr, std::vector<double>& answer);
  };

} // namespace GaussMethod

#endif // GAUSS_H_
