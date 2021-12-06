#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

int64_t number_of_lanternfish_after(
  int days, const std::vector<int>& lanternfish_school_original)
{
  std::array<int64_t, 10> lantern_fish_store{};
  for (int fish : lanternfish_school_original) {
    lantern_fish_store[fish]++;
  }

  for (int64_t day = 0; day < days; ++day) {
    int64_t zeros = lantern_fish_store[0];
    lantern_fish_store[9] += zeros;
    lantern_fish_store[7] += zeros;
    lantern_fish_store[0] -= zeros;
    std::rotate(
      lantern_fish_store.begin(), lantern_fish_store.begin() + 1,
      lantern_fish_store.end());
  }

  int64_t number = 0;
  for (int64_t i : lantern_fish_store) {
    number += i;
  }

  return number;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<int> lanternfish_school_original;
  for (std::string line; std::getline(reader, line, ',');) {
    lanternfish_school_original.push_back(std::stoi(line));
  }

  // check
  // for (const auto lanternfish : lanternfish_school_original) {
  //   std::cout << lanternfish << ", ";
  // }
  // std::cout << '\n';

  {
    const int64_t days = 80;
    auto lanternfish_school = lanternfish_school_original;
    for (int64_t day = 0; day < days; ++day) {
      const int64_t lantern_fish_size = lanternfish_school.size();
      for (int64_t fish = 0; fish < lantern_fish_size; ++fish) {
        if (lanternfish_school[fish] == 0) {
          lanternfish_school.push_back(8);
          lanternfish_school[fish] = 7;
        }
        lanternfish_school[fish]--;
      }
    }
  }

  std::cout << "part 1: "
            << number_of_lanternfish_after(
                 80 /*days*/, lanternfish_school_original)
            << '\n';

  std::cout << "part 2: "
            << number_of_lanternfish_after(
                 256 /*days*/, lanternfish_school_original)
            << '\n';

  return 0;
}
