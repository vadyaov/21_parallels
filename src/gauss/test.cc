#include "gauss.h"

#include <iostream>

/* template<typename T> */
/* void foo(const SimpleMatrix<T>& m) { */
/*   std::cout << "inside foo()" << std::endl; */
/* } */

int main() {

  SimpleMatrix m("../txts/gauss/matrix_5.txt");
  std::cout << "m:\n" << m << "\n";
  auto answ = GaussMethod::Gauss::Solve(m);
  for (size_t i = 0; i != answ.size(); ++i) {
    std::cout << "x" << i + 1 << " = " << answ[i] << "\n";
  }

  return 0;
}
