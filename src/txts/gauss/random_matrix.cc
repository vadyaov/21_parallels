#include <fstream>
#include <random>

double RandomValue(double min, double max) {
  static std::random_device rd;     // Only used once to initialise (seed) engine
  static std::mt19937 rng(rd());    // Random-number engine used (Mersenne-Twister in this case)
  static std::uniform_real_distribution<double> uni(min,max); // Guaranteed unbiased

  return uni(rng);
}

void CreateRandomMatrix(int rows, int cols, const std::string& name) {
  std::ofstream ostrm(name);

  ostrm << rows << " " << cols << "\n";
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < cols; ++j) {
      ostrm << RandomValue(1, 100) << " ";
    }
    ostrm << "\n";
  }
}

int main(int argc, char** argv) {

  if (argc < 2) return -1;

  for (int i = 1; i < argc; ++i)
    CreateRandomMatrix(50, 51, argv[i]);

  return 0;
}
