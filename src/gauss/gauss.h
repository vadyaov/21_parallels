#ifndef GAUSS_H_
#define GAUSS_H_

#include "simple_matrix.h"

#include <vector>

namespace GaussMethod {

  class Gauss {
    public:
      static std::vector<double> Solve(const SimpleMatrix& matr);
  };

} // namespace GaussMethod

#endif // GAUSS_H_
