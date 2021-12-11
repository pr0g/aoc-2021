#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> lines;
  for (std::string line; std::getline(reader, line);) {
    lines.push_back(line);
  }

  std::vector<std::vector<int>> energies;
  for (const auto& row : lines) {
    energies.emplace_back();
    for (const auto energy : row) {
      int number = energy - '0';
      energies.back().push_back(number);
    }
  }

  // check
  // for (const auto& row : energies) {
  //   for (const auto energy : row) {
  //     std::cout << energy << ' ';
  //   }
  //   std::cout << '\n';
  // }

  // std::cout << '\n';

  struct offset_t
  {
    int row;
    int col;
  };

  std::vector<offset_t> offsets = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0},
                                   {1, 0},   {-1, 1}, {0, 1},  {1, 1}};

  int synchronize_step = -1;
  int total_flashes = 0;
  for (int i = 0; i < 500; ++i) {
    // energy levels increase by 1
    for (int row = 0; row < energies.size(); ++row) {
      for (int col = 0; col < energies.back().size(); ++col) {
        energies[row][col]++;
      }
    }

    int flashes = 0;
    bool flashed;
    std::unordered_set<int> octopus_flashed;
    do {
      flashed = false;
      for (int row = 0; row < energies.size(); ++row) {
        for (int col = 0; col < energies.back().size(); ++col) {
          const int energy = energies[row][col];
          if (
            energy > 9
            && octopus_flashed.find(row * energies.back().size() + col)
                 == octopus_flashed.end()) {
            octopus_flashed.insert(row * energies.back().size() + col);
            flashed = true;
            flashes++;
            for (auto offset : offsets) {
              int nr = row + offset.row;
              int nc = col + offset.col;
              if (
                nr >= 0 & nr < energies.size() && nc >= 0
                && nc < energies.back().size()) {
                if (
                  octopus_flashed.find(nr * energies.back().size() + nc)
                  == octopus_flashed.end()) {
                  energies[nr][nc]++;
                }
              }
            }
          }
        }
      }
    } while (flashed);

    for (auto f : octopus_flashed) {
      int r = f / energies.back().size();
      int c = f % energies.back().size();
      energies[r][c] = 0;
    }

    if (flashes == 100 && synchronize_step == -1) {
      synchronize_step = i;
    }

     // check
    //  for (const auto& row : energies) {
    //    for (const auto energy : row) {
    //      std::cout << energy << ' ';
    //    }
    //    std::cout << '\n';
    //  }

    //  std::cout << '\n';

    total_flashes += flashes;
    octopus_flashed.clear();
  }

  std::cout << "part 1: " << total_flashes << '\n';
  std::cout << "part 2: " << synchronize_step + 1 << '\n';

  return 0;
}
