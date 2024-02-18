#ifndef GAUSS_H_
#define GAUSS_H_

#include <vector>

#include "../simplegraph.h"

namespace gaussmethod {

class Gauss {
 public:
  enum { NONE = 0, ONE, LOT };
  constexpr static double EPS = 1e-6;

  static int Solve(SimpleGraph<double> matr, std::vector<double>& answer);
  static int ParallelSolve(SimpleGraph<double> matr,
                           std::vector<double>& answer);
};

}  // namespace gaussmethod

#endif  // GAUSS_H_
