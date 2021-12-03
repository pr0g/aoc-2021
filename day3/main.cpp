#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

std::vector<int> number_from_string(const std::string& str)
{
  std::vector<int> number;
  for (const char c : str) {
    number.push_back(std::stoi(std::string(1, c)));
  }
  return number;
}

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

std::vector<std::vector<int>> get_secondary_binary_numbers(
  std::vector<std::string> binary_numbers)
{
  std::vector<std::vector<int>> secondary_binary_numbers;
  secondary_binary_numbers.resize(binary_numbers.front().size());
  for (const auto& binary_number : binary_numbers) {
    int i = 0;
    for (const auto digit : binary_number) {
      secondary_binary_numbers[i++].push_back(std::stoi(std::string(1, digit)));
    }
  }
  return secondary_binary_numbers;
}

std::vector<int> get_final_binary_number(
  std::vector<std::vector<int>> secondary_binary_numbers, int bit)
{
  std::vector<int> final_binary_number;
  for (const auto& secondary_binary_number : secondary_binary_numbers) {
    auto interested_bits_set = std::count_if(
      secondary_binary_number.begin(), secondary_binary_number.end(),
      [bit](const int number) { return number == bit; });
    auto uninterested_bits_set = secondary_binary_number.size() - interested_bits_set;
    // need to switch to be min not max here... comparator
    if (interested_bits_set >= uninterested_bits_set) {
      final_binary_number.push_back(bit);
    } else {
      int other = bit == 1 ? 0 : 1;
      final_binary_number.push_back(other);
    }
  }
  return final_binary_number;
}

std::string find_number(
  std::vector<int> final_binary_number, std::vector<std::string> binary_numbers, int bit)
{
  std::vector<std::string> next_binary_numbers;
  for (int i = 0; i < final_binary_number.size(); ++i) {
    for (auto binary_number : binary_numbers) {
      auto digit = std::stoi(std::string(1, binary_number[i]));
      if (digit == final_binary_number[i]) {
        next_binary_numbers.push_back(binary_number);
      }
    }
    binary_numbers = next_binary_numbers;
    next_binary_numbers.clear();
    if (binary_numbers.size() == 1) {
      break;
    }
    final_binary_number =
      get_final_binary_number(get_secondary_binary_numbers(binary_numbers), bit);
  }

  return binary_numbers.front();
}

int main(int argc, char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> binary_numbers;
  for (std::string line; std::getline(reader, line);) {
    binary_numbers.push_back(line);
  }

  auto secondary_binary_numbers = get_secondary_binary_numbers(binary_numbers);
  auto final_binary_number_ones = get_final_binary_number(secondary_binary_numbers, 1);

  int gamma_rate = decimal_from_binary(final_binary_number_ones);
  int epsilon_rate = decimal_from_binary(flip(final_binary_number_ones));

  std::cout << "part 1: " << gamma_rate * epsilon_rate << '\n';

  // oxygen generator rating
  // co2 scrubber rating

  auto oxygen_str = find_number(final_binary_number_ones, binary_numbers, 1);
  auto oxygen = decimal_from_binary(number_from_string(oxygen_str));
  auto final_binary_number_zeros = get_final_binary_number(secondary_binary_numbers, 0);
  auto co2_str = find_number(final_binary_number_zeros, binary_numbers, 0);
  auto co2 = decimal_from_binary(number_from_string(co2_str));

  int i;
  i = 0;
}
