#include "ant.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>


void ACO::LoadGraphFromFile(const std::string& filename) {
  std::ifstream istrm;
  istrm.open(filename, std::ios_base::in);

  if (!istrm.is_open())
    throw std::invalid_argument("Can not open file " + filename);

  istrm >> size;

  adjacent_.resize(size * size);

  for (std::size_t i = 0; i < adjacent_.size(); ++i) {
    istrm >> adjacent_[i];
  }

  directed = Directed();

  istrm.close();
}

std::string GetName(const std::string& filename) {
  std::string name;
  std::string::const_iterator it =
      filename.begin() + filename.find_last_of('.') - 1;
  if (it < filename.begin())
    throw std::invalid_argument("Incorrect file extension");

  for (; it >= filename.cbegin() && *it != '/'; --it) {
    name.insert(name.begin(), *it);
  }

  return name;
}

/* void ACO::ExportGraphToDot(const std::string& filename) const { */
/*   if (adjacent_.empty()) return; */

/*   std::ofstream ostrm; */
/*   ostrm.open(filename, std::ios::out); */

/*   if (!ostrm.is_open()) */
/*     throw std::invalid_argument("Can not open file " + filename); */

/*   const std::string tab(4, ' '); */
/*   const std::string connection = directed ? " -> " : " -- "; */

/*   std::string name = GetName(filename); */

/*   std::string graph_title = (directed ? "digraph " : "graph ") + name; */

/*   ostrm << graph_title << " {\n"; */

/*   std::size_t last = 0; */
/*   for (std::size_t i = 0; i < size; ++i) { */
/*     for (std::size_t j = directed ? 0 : i; j < size; ++j) { */
/*       if (adjacent_[i * size + j] != 0) { */
/*         if (i + 1 == last) { */
/*           ostrm << connection << j + 1; */
/*         } else { */
/*           if (last) ostrm << std::endl; */
/*           ostrm << tab << i + 1 << connection << j + 1; */
/*         } */

/*         last = j + 1; */
/*       } */
/*     } */
/*   } */

/*   ostrm << "\n}\n"; */

/*   ostrm.close(); */
/* } */

bool ACO::Directed() const noexcept {
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      if (i != j && adjacent_[i * size + j] != adjacent_[j * size + i])
        return true;
    }
  }

  return false;
}

bool ACO::IsDirect() const noexcept { return directed; }

bool ACO::Empty() const noexcept { return size == 0 || adjacent_.empty(); }

int ACO::Size() const noexcept { return size; }

namespace {

double RandomValue() {
  static std::random_device rd;
  static std::mt19937 engine(rd());
  static std::uniform_real_distribution<double> normal_distrib(0.0, 1.0);

  return normal_distrib(engine);
}

std::vector<std::vector<double>> NormalizedGraph(const ACO& graph) {
  const std::size_t sz = graph.Size();

  std::vector<std::vector<double>> normalized(sz, std::vector<double>(sz));

  for (std::size_t i = 0; i != sz; ++i)
    for (std::size_t j = 0; j != sz; ++j) {
      if ((graph[i][j] == 0 || graph[j][i] == 0) && i != j)
        throw std::runtime_error("Graph is not full");

      // line with magic value
      normalized[i][j] = 200.0 / graph[i][j];
    }

  return normalized;
}

int Roulette(const std::vector<double>& chance) {
  int next_point = -1;
  double cumulative_probability = 0.0;

  for (std::size_t i = 0; i < chance.size() && next_point == -1; ++i) {
    if (chance[i] > 0) {
      cumulative_probability += chance[i];
      if (RandomValue() <= cumulative_probability)
        next_point = static_cast<int>(i);
    }
  }

  return next_point;
}

ACO::TsmResult MinimalSolution(
    const std::vector<ACO::TsmResult>& ants_data) {
  ACO::TsmResult min = ants_data[0];

  for (std::size_t i = 1; i != ants_data.size(); ++i) {
    if (ants_data[i].distance < min.distance) min = ants_data[i];
  }

  return min;
}

void UpdateFeromones(std::vector<std::vector<double>>& feromones,
                     std::vector<ACO::TsmResult>& paths, int size,
                     double Q, double reduce) {
  for (int i = 0; i != size; ++i)
    for (int j = 0; j != size; ++j) feromones[i][j] *= reduce;

  for (int ant = 0; ant < size; ++ant) {
    double delta_fero = Q / paths[ant].distance;
    for (std::size_t i = 0; i < paths[ant].vertices.size() - 1; ++i) {
      feromones[paths[ant].vertices[i]][paths[ant].vertices[i + 1]] +=
          delta_fero;
    }
  }
}

} // namespace

ACO::TsmResult ACO::SolveTravelingSalesmanProblem(int n) {
  if (size == 0) throw std::invalid_argument("Empty graph");

  TsmResult min_path{{}, std::numeric_limits<double>::max()};

  const double Q = 320.0;
  const double fero_reduce = 0.6;

  const double alpha = 1.0;
  const double beta = 4.0;

  std::vector<std::vector<double>> dist = NormalizedGraph(*this);
  std::vector<std::vector<double>> fero(size, std::vector<double>(size, 0.2));

  for (int iter = 0; iter < n; ++iter) {
    std::vector<TsmResult> ants_path(size, {std::vector<int>(size + 1, 0), 0});

    for (int ant = 0; ant < size; ++ant) {
      int curr_point = ant;
      std::vector<bool> visited(size, false);

      ants_path[ant].vertices[0] = ants_path[ant].vertices[size] = curr_point;

      for (int i = 0; i < size - 1; ++i) {
        visited[curr_point] = true;

        std::vector<double> wish(size);
        for (int j = 0; j != size; ++j)
          if (!visited[j])
            wish[j] = std::pow(fero[curr_point][j], alpha) *
                      std::pow(dist[curr_point][j], beta);

        double wish_sum = std::accumulate(wish.begin(), wish.end(), 0.0);

        std::vector<double> chance(size);
        for (int j = 0; j != size; ++j) chance[j] = wish[j] / wish_sum;

        int prev_point = curr_point;
        curr_point = Roulette(chance);

        if (curr_point == -1)
          throw std::runtime_error("Cannot find the solution");

        ants_path[ant].vertices[i + 1] = curr_point;
        ants_path[ant].distance += adjacent_[prev_point * size + curr_point];
      }

      ants_path[ant].distance += adjacent_[curr_point * size + ant];
    }

    UpdateFeromones(fero, ants_path, size, Q, fero_reduce);

    if (min_path.distance > MinimalSolution(ants_path).distance)
      min_path = MinimalSolution(ants_path);
  }

  return min_path;
}
