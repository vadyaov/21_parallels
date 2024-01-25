#include "simple_matrix.h"

#include <iostream>

/* template<typename T> */
/* void foo(const SimpleMatrix<T>& m) { */
/*   std::cout << "inside foo()" << std::endl; */
/* } */

int main() {

  SimpleMatrix m("../txts/gauss/matrix1.txt");

  std::cout << m << std::endl;

  m.SwapRows(0, 1);

  std::cout << m << std::endl;

  return 0;
}
