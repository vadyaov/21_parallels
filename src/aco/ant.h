#ifndef ACO_H_
#define ACO_H_

#include <fstream>
#include <string>
#include <vector>

class ACO {
 private:
  struct ProxyRow {
    ProxyRow(int* r) : row{r} {};
    ProxyRow(const int* r) : row{const_cast<int*>(r)} {};

    int* row;
    int& operator[](int n) { return row[n]; }
    const int& operator[](int n) const { return row[n]; }
  };

  bool Directed() const noexcept;

 public:
  struct TsmResult {
    std::vector<int> vertices;
    double distance{0};
    // double time;
  };

  ACO() : adjacent_{}, size{0}, directed{false} {}

  // loading graph from file with adjacent matrix
  void LoadGraphFromFile(const std::string& filename);

  // exporting graph to .gv (.dot) file
  /* void ExportGraphToDot(const std::string& filename) const; */

  bool IsDirect() const noexcept;

  bool Empty() const noexcept;

  int Size() const noexcept;

  TsmResult ClassicACO(int n);

 public:
  /* friend std::ostream& operator<<(std::ostream& os, const ACO& g); */

  /* ProxyRow operator[](int row) { */
  /*   return adjacent_.data() + row * size; */
  /* } */

  const ProxyRow operator[](int row) const {
    return adjacent_.data() + row * size;
  }

 private:
  std::vector<int> adjacent_;  // adjacent matrix
  int size;            // vertex number
  bool directed;
};

#endif  // ACO_H_
