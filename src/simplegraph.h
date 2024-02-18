#ifndef SIMPLE_GRAPH_H_
#define SIMPLE_GRAPH_H_

#include <fstream>
#include <string>
#include <vector>

template <typename T>
class SimpleGraph {
 private:
  struct ProxyRow {
    ProxyRow(T* r) : row{r} {};
    ProxyRow(const T* r) : row{const_cast<T*>(r)} {};

    T* row;
    T& operator[](int n) { return row[n]; }
    const T& operator[](int n) const { return row[n]; }
  };

  bool Directed() const noexcept;

 public:
  SimpleGraph() : adjacent_{}, rows{0}, cols{0} /* directed{false} */ {}
  SimpleGraph(int r, int c) {
    if (r < 2 || c < 2)
      throw std::invalid_argument("please, create matrices, not rows or smth");

    adjacent_.resize(r * c);
    rows = r;
    cols = c;
  }

  void LoadGraphFromFile(const std::string& filename) {
    std::ifstream istrm;
    istrm.open(filename, std::ios_base::in);

    if (!istrm.is_open())
      throw std::invalid_argument("Can not open file " + filename);

    istrm >> rows >> cols;

    adjacent_.resize(rows * cols);

    for (std::size_t i = 0; i < adjacent_.size(); ++i) {
      istrm >> adjacent_[i];
    }

    /* directed = Directed(); */

    istrm.close();
  }

  /* bool IsDirect() const noexcept { return directed; } */

  bool Empty() const noexcept { return adjacent_.empty(); }

  int Size() const noexcept { return rows; }

  void SwapRows(int r1, int r2) {
    if (!(r1 < rows && r2 < rows) || !(r1 >= 0 && r2 >= 0))
      throw std::range_error("incorrect row");

    if (r1 == r2) return;

    for (int j = 0; j != cols; ++j)
      std::swap(adjacent_[r1 * cols + j], adjacent_[r2 * cols + j]);
  }

  int get_rows() const { return rows; }
  int get_cols() const { return cols; }

 public:
  ProxyRow operator[](int row) { return adjacent_.data() + row * cols; }

  const ProxyRow operator[](int row) const {
    return adjacent_.data() + row * cols;
  }

  void dump(std::ostream& os) const {
    for (int i = 0; i != rows; ++i) {
      for (int j = 0; j != cols; ++j) {
        os << adjacent_[i * cols + j] << " ";
      }
      os << "\n";
    }
  }

  bool IsEqual(const SimpleGraph<T>& gr) const {
    return rows == gr.rows && cols == gr.cols && adjacent_ == gr.adjacent_;
  }

 private:
  std::vector<T> adjacent_;
  int rows;
  int cols;
  /* bool directed; */
};

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const SimpleGraph<T>& g) {
  g.dump(os);
  return os;
}

template <typename T>
bool operator==(const SimpleGraph<T>& lhs, const SimpleGraph<T>& rhs) {
  return lhs.IsEqual(rhs);
}

#endif  // SIMPLE_GRAPH_H_
