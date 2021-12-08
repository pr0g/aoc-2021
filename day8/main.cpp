#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <vector>

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> digits;
  for (std::string line; std::getline(reader, line);) {
    digits.push_back(line);
  }

  int part1_digit_count = 0;
  for (const auto& digit : digits) {
    if (auto separator = digit.find('|'); separator != std::string::npos) {
      auto output_pattern = digit.substr(separator + 2, digit.size());
      std::vector<std::string> output_digits;
      std::istringstream output_pattern_ss(output_pattern);
      for (std::string line; std::getline(output_pattern_ss, line, ' ');) {
        output_digits.push_back(line);
      }
      for (const auto& output_digit : output_digits) {
        std::vector<int> segment_counts = { 2, 4, 3, 7 };
        if (std::any_of(segment_counts.begin(), segment_counts.end(), [output_digit](const auto& segment_count) {
          return output_digit.size() == segment_count;
        })) {
          part1_digit_count++;
        }
      }
    }
  }

  std::cout << "part 1: " << part1_digit_count << '\n';

  return 0;
}
