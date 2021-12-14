#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::string polymer_template;
  for (std::string line; std::getline(reader, line);) {
    if (line.empty()) {
      break;
    }
    polymer_template = std::move(line);
  }

  // std::cout << polymer_template << '\n';

  std::vector<std::string> pair_insertion_strs;
  for (std::string line; std::getline(reader, line);) {
    pair_insertion_strs.push_back(std::move(line));
  }

  // check
  // for (const auto& pair_insertion : pair_insertion_strs) {
  //   std::cout << pair_insertion << '\n';
  // }

  struct pair_insertion_t
  {
    std::string lhs;
    std::string rhs;
  };

  std::vector<pair_insertion_t> pair_insertions;
  for (const auto& pair_insertion : pair_insertion_strs) {
    auto lhs = pair_insertion.substr(0, 2);
    auto split = pair_insertion.find("->");
    auto rhs = pair_insertion.substr(split + 3, 1);
    pair_insertions.push_back({lhs, rhs});
  }

  // check
  // for (const auto& pair : pair_insertions) {
  //   std::cout << pair.lhs << ' ' << pair.rhs << '\n';
  // }

  const int steps = 10;
  for (int step = 0; step < steps; step++) {
    std::string next_polymer_template = polymer_template;
    for (int i = 0, n = 1; i < polymer_template.size() - 1; ++i, n += 2) {
      auto view = polymer_template.substr(i, 2);
      if (auto lookup = std::find_if(
            pair_insertions.begin(), pair_insertions.end(),
            [view](const auto& pair_insertion) {
              return view == pair_insertion.lhs;
            });
          lookup != pair_insertions.end()) {
        next_polymer_template.insert(n, lookup->rhs);
      }
    }
    polymer_template = next_polymer_template;
  }

  // std::cout << polymer_template << '\n';

  std::vector<int64_t> counts;
  for (const auto& pair_insertion : pair_insertions) {
    for (const auto c : pair_insertion.lhs) {
      counts.push_back(std::count(polymer_template.begin(), polymer_template.end(), c));
    }
  }

  auto min = std::min_element(std::begin(counts), std::end(counts));
  auto max = std::max_element(std::begin(counts), std::end(counts));

  std::cout << "part 1: " << *max - *min << '\n';
}
