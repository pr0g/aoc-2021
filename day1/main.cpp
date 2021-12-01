#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>

int main(int argc, char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<int> numbers;
  for (std::string line; std::getline(reader, line);) {
    numbers.push_back(std::stoi(line));
  }
  std::vector<int> differences;
  differences.reserve(numbers.size());
  std::adjacent_difference(
    numbers.begin(), numbers.end(), std::back_inserter(differences));

  auto increasing_count =
    std::count_if(differences.begin() + 1, differences.end(), [](const int diff) {
      return diff > 0;
    });

  std::cout << increasing_count << '\n';
}
