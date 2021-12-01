#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <numeric>

int increasing_differences(const std::vector<int>& numbers)
{
  std::vector<int> differences;
  differences.reserve(numbers.size());

  std::adjacent_difference(
    numbers.begin(), numbers.end(), std::back_inserter(differences));

  auto increasing_count =
    std::count_if(differences.begin() + 1, differences.end(), [](const int diff) {
      return diff > 0;
    });

  return increasing_count;
}

int main(int argc, char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<int> numbers;
  for (std::string line; std::getline(reader, line);) {
    numbers.push_back(std::stoi(line));
  }

  std::cout << "part 1: " << increasing_differences(numbers) << '\n';

  std::vector<int> window_numbers;
  auto current = numbers.begin();
  while (current + 3 <= numbers.end()) {
    auto sum = std::accumulate(current, current + 3, 0);
    window_numbers.push_back(sum);
    current++;
  }

  std::cout << "part 2: " << increasing_differences(window_numbers) << '\n';
}
