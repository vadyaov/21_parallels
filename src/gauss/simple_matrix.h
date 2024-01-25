#ifndef SIMPLE_MATRIX_H_
#define SIMPLE_MATRIX_H_

#include <vector>
#include <string>
#include <fstream>

#include <iostream>
#include <iomanip>

class SimpleMatrix {
  std::vector<double> data; // last col is column of constants !
  int rows;
  int cols;

  public:
    SimpleMatrix(const std::string& path) {
      std::ifstream istrm{path};
      if (!istrm.is_open())
        throw std::invalid_argument("can't open file" + path);

      istrm >> rows >> cols;
      if (rows < 1 || cols < 1)
        throw std::invalid_argument("incorrect filedata");

      data.reserve(rows * cols);
      double tmp;
      for (int it = 0; it != data.capacity(); ++it) {
        istrm >> tmp;
        data.push_back(tmp);
      }
    }

    void SwapRows(int r1, int r2) {
      if (!(r1 < rows && r2 < rows) || !(r1 >= 0 && r2 >= 0))
        throw std::range_error("incorrect row");

      if (r1 == r2) return;

      for (int j = 0; j != cols; ++j)
        std::swap(this->at(r1, j), this->at(r2, j));
    }

    double& at(int i, int j) {
      std::cout << "at " << i << " " << j << "\n";
      return data.at(i * rows + j);
    }

    const double& at(int i, int j) const {
      return data.at(i * rows + j);
    }

    int get_rows() const noexcept { return rows; }

    int get_cols() const noexcept { return cols; }
    


    void dump(std::ostream& os) const {
      for (int i = 0; i != rows; ++i) {
        for (int j = 0; j != cols; ++j) {
          os << this->at(i, j) << std::setw(4);
        }
        os << "\n";
      }
    }

};

std::ostream& operator<<(std::ostream& os, const SimpleMatrix& m) {
  m.dump(os);
  return os;
}

#endif // SIMPLE_MATRIX_H_
