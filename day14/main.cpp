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

  struct pair_insertion2_t
  {
    std::string lhs;
    std::string rhs1;
    std::string rhs2;
    std::string mid;
  };

  std::vector<pair_insertion_t> pair_insertions;
  std::vector<pair_insertion2_t> pair_insertions2;
  for (const auto& pair_insertion : pair_insertion_strs) {
    auto lhs = pair_insertion.substr(0, 2);
    auto split = pair_insertion.find("->");
    auto rhs = pair_insertion.substr(split + 3, 1);
    pair_insertions.push_back({lhs, rhs});
    auto rhs1 = std::string(1, lhs[0]) + std::string(1, rhs[0]);
    auto rhs2 = std::string(1, rhs[0]) + std::string(1, lhs[1]);
    auto mid =
      std::string(1, lhs[0]) + std::string(1, rhs[0]) + std::string(1, lhs[1]);
    pair_insertions2.push_back({lhs, rhs1, rhs2, mid});
  }

  // check
  // for (const auto& pair : pair_insertions2) {
  //   std::cout << pair.lhs << ' ' << pair.rhs1 << ' ' << pair.rhs2 << '\n';
  // }

  std::unordered_map<std::string, int64_t> count;
  for (int i = 0; i < polymer_template.size() - 1; ++i) {
    auto view = polymer_template.substr(i, 2);
    count[view]++;
  }

  //  std::unordered_map<std::string, int64_t> sub_count;
  //  for (int i = 0; i < polymer_template.size() - 2; ++i) {
  //    auto view = polymer_template.substr(i, 3);
  //    sub_count[view]++;
  //  }

  // check
  // for (const auto& elem : count) {
  //   std::cout << elem.first << ' ';
  // }
  // std::cout << '\n';

  const int steps = 40;
  for (int step = 0; step < steps; step++) {
    auto next_count = count;
    next_count.clear();
    for (auto& elem : count) {
      if (auto lookup = std::find_if(
            pair_insertions2.begin(), pair_insertions2.end(),
            [elem](const auto& pair_insertion) {
              return elem.first == pair_insertion.lhs;
            });
          lookup != pair_insertions2.end()) {
        next_count[lookup->rhs1] += elem.second;
        next_count[lookup->rhs2] += elem.second;
      }
    }
    count = next_count;
  }

  std::unordered_map<char, int64_t> counts;
  for (const auto& pair : count) {
    counts[pair.first[0]] += pair.second;
  }

  counts[polymer_template.back()]++;

  const int steps_part1 = 10;
  for (int step = 0; step < steps_part1; step++) {
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

  {
    std::vector<int64_t> counts_p1;
    for (const auto& pair_insertion : pair_insertions) {
      for (const auto c : pair_insertion.lhs) {
        counts_p1.push_back(
          std::count(polymer_template.begin(), polymer_template.end(), c));
      }
    }

    auto min = std::min_element(std::begin(counts_p1), std::end(counts_p1));
    auto max = std::max_element(std::begin(counts_p1), std::end(counts_p1));

    std::cout << "part 1: " << *max - *min << '\n';
  }

  auto min = std::min_element(
    std::begin(counts), std::end(counts),
    [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });
  auto max = std::max_element(
    std::begin(counts), std::end(counts),
    [](const auto& lhs, const auto& rhs) { return lhs.second < rhs.second; });

  std::cout << "part 2: " << max->second - min->second << '\n';
}
