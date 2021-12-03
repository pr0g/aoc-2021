#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

int decimal_from_binary(const std::vector<int>& binary)
{
  int base = 1;
  int decimal = 0;
  for (auto rit = binary.rbegin(); rit != binary.rend(); ++rit) {
    decimal += *rit * base;
    base <<= 1;
  }
  return decimal;
}

std::vector<int> flip(std::vector<int> binary)
{
  for (auto& digit : binary) {
    if (digit == 0) {
      digit = 1;
    } else if (digit == 1) {
      digit = 0;
    }
  }
  return binary;
}

int main(int argc, char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> binary_numbers;
  for (std::string line; std::getline(reader, line);) {
    binary_numbers.push_back(line);
  }

  std::vector<std::vector<int>> secondary_binary_numbers;
  secondary_binary_numbers.resize(binary_numbers.front().size());

  for (const auto& binary_number : binary_numbers) {
    int i = 0;
    for (const auto digit : binary_number) {
      secondary_binary_numbers[i++].push_back(std::stoi(std::string(1, digit)));
    }
  }

  std::vector<int> final_binary_number;
  for (const auto& secondary_binary_number : secondary_binary_numbers) {
    auto one_bits_set = std::count_if(
      secondary_binary_number.begin(), secondary_binary_number.end(),
      [](const int number) { return number == 1; });

    if (one_bits_set > secondary_binary_number.size() / 2) {
      final_binary_number.push_back(1);
    } else {
      final_binary_number.push_back(0);
    }
  }

  // std::cout << decimal_from_binary(final_binary_number) << '\n';

  int gamma_rate = decimal_from_binary(final_binary_number);
  int epsilon_rate = decimal_from_binary(flip(final_binary_number));

  std::cout << "part 1: " << gamma_rate * epsilon_rate << '\n';
}
