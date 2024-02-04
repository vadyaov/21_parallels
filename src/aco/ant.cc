#include "ant.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <random>

#include <thread>

namespace {

double RandomValue() {
  static std::random_device rd;
  static std::mt19937 engine(rd());
  static std::uniform_real_distribution<double> normal_distrib(0.0, 1.0);

  return normal_distrib(engine);
}

std::vector<std::vector<double>> NormalizedGraph(const SimpleGraph<int>& graph) {
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

AntColony::TsmResult MinimalSolution(
    const std::vector<AntColony::TsmResult>& ants_data) {
  AntColony::TsmResult min = ants_data[0];

  for (std::size_t i = 1; i != ants_data.size(); ++i) {
    if (ants_data[i].distance < min.distance) min = ants_data[i];
  }

  return min;
}

void UpdateFeromones(std::vector<std::vector<double>>& feromones,
                     std::vector<AntColony::TsmResult>& paths) {
  
  static const double reduce = 0.6;
  static const double Q = 320.0;
  static const int sz = feromones.size();

  for (int i = 0; i != sz; ++i)
    for (int j = 0; j != sz; ++j) feromones[i][j] *= reduce;

  for (int ant = 0; ant < sz; ++ant) {
    double delta_fero = Q / paths[ant].distance;
    for (std::size_t i = 0; i < paths[ant].vertices.size() - 1; ++i) {
      feromones[paths[ant].vertices[i]][paths[ant].vertices[i + 1]] +=
          delta_fero;
    }
  }
}
std::vector<double> CalculateChances(const std::vector<std::vector<double>>& dist,
                                     const std::vector<std::vector<double>>& fero,
                                     const std::vector<bool>& visited,
                                     int current_point) {

  static const double alpha = 1.0;
  static const double beta = 4.0;

  std::vector<double> wish(dist.size());
  for (size_t j = 0; j != dist.size(); ++j)
    if (!visited[j])
      wish[j] = std::pow(fero[current_point][j], alpha) *
                std::pow(dist[current_point][j], beta);

  double wish_sum = std::accumulate(wish.begin(), wish.end(), 0.0);

  std::vector<double> chances(dist.size());
  for (size_t j = 0; j != dist.size(); ++j) chances[j] = wish[j] / wish_sum;

  return chances;

}

} // namespace

struct CreatePathForOneAnt {
  const SimpleGraph<int>& gr;
  AntColony::TsmResult& tsm;
  const std::vector<std::vector<double>>& d;
  const std::vector<std::vector<double>>& f;

  CreatePathForOneAnt(const SimpleGraph<int>& aco, AntColony::TsmResult& t_, std::vector<std::vector<double>>& d_, 
       std::vector<std::vector<double>>& f_) : gr{aco}, tsm{t_}, d{d_}, f{f_} {}

  void operator()(int ant) {
      int curr_point = ant;
      const int sz = gr.Size();

      std::vector<bool> visited(sz, false);

      tsm.vertices[0] = tsm.vertices[sz] = curr_point;

      // creating the path for ant No.i
      for (int i = 0; i < sz - 1; ++i) {
        visited[curr_point] = true;

        std::vector<double> chances = CalculateChances(d, f, visited, curr_point);

        int prev_point = curr_point;
        curr_point = Roulette(chances); // choose the next vertex to go

        if (curr_point == -1)
          throw std::runtime_error("Cannot find the solution");

        tsm.vertices[i + 1] = curr_point;
        tsm.distance += gr[prev_point][curr_point];
      }

      tsm.distance += gr[curr_point][ant];
  }
};

AntColony::TsmResult AntColony::ClassicSolve(const SimpleGraph<int>& g, int n) {
  const int sz = g.Size();
  if (sz == 0) throw std::invalid_argument("Empty graph");

  TsmResult min_path{{}, std::numeric_limits<double>::max()};

  std::vector<std::vector<double>> dist = NormalizedGraph(g);
  std::vector<std::vector<double>> fero(sz, std::vector<double>(sz, 0.2));

  for (int iter = 0; iter < n; ++iter) { // number of populations
    std::vector<TsmResult> ants_path(sz, {std::vector<int>(sz + 1, 0), 0});

    for (int ant = 0; ant < sz; ++ant) { // ants number is always equal to vertex number
      CreatePathForOneAnt(g, ants_path[ant], dist, fero)(ant);
    }

    UpdateFeromones(fero, ants_path);

    if (min_path.distance > MinimalSolution(ants_path).distance)
      min_path = MinimalSolution(ants_path);
  }

  return min_path;
}

AntColony::TsmResult AntColony::ParallelSolve(const SimpleGraph<int>& g, int n) {
  const int sz = g.Size();
  if (sz == 0) throw std::invalid_argument("Empty graph");

  TsmResult min_path{{}, std::numeric_limits<double>::max()};

  std::vector<std::vector<double>> dist = NormalizedGraph(g);
  std::vector<std::vector<double>> fero(sz, std::vector<double>(sz, 0.2));

  std::vector<std::thread> threads;

  for (int iter = 0; iter < n; ++iter) { // number of populations
    std::vector<TsmResult> ants_path(sz, {std::vector<int>(sz + 1, 0), 0});

    threads.clear();

    for (int ant = 0; ant < sz; ++ant) {
      threads.emplace_back(CreatePathForOneAnt(g, ants_path[ant], dist, fero), ant);
      /* CreatePathForOneAnt(*this, ants_path[ant], dist, fero)(ant); */
    }

    std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
        t.join();
      });

    UpdateFeromones(fero, ants_path);

    if (min_path.distance > MinimalSolution(ants_path).distance)
      min_path = MinimalSolution(ants_path);
  }

  return min_path;
}
