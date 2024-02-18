#ifndef ACO_H_
#define ACO_H_

#include "../simplegraph.h"

class AntColony {
 public:
  struct TsmResult {
    std::vector<int> vertices;
    double distance{0};
  };

  static TsmResult ClassicSolve(const SimpleGraph<int>& g, int n);
  static TsmResult ParallelSolve(const SimpleGraph<int>& g, int n);

 private:
  void CreatePathForCurrentAnt(TsmResult& path, int current_ant, int sz,
                               const std::vector<std::vector<double>>& dist,
                               const std::vector<std::vector<double>>& fero);
};

#endif  // ACO_H_
