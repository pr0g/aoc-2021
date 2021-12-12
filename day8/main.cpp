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
  int part2_total = 0;
  for (const auto& digit : digits) {
    if (auto separator = digit.find('|'); separator != std::string::npos) {
      auto input_pattern = digit.substr(0, separator);
      std::vector<std::string> input_digits;
      std::istringstream input_pattern_ss(input_pattern);
      for (std::string line; std::getline(input_pattern_ss, line, ' ');) {
        input_digits.push_back(line);
      }
      // check
      // for (const auto input_digit : input_digits) {
      //   std::cout << input_digit << ',';
      // }
      // std::cout << '\n';

      struct decoded_t
      {
        int digit;
        std::string pattern;
      };

      std::vector<decoded_t> decoded_values;
      for (int i = 0; i < input_digits.size(); ++i) {
        const auto input_digit = input_digits[i];
        if (input_digit.size() == 2) {
          decoded_values.push_back({1, input_digit});
          input_digits[i--] = input_digits.back();
          input_digits.pop_back();
        }
        if (input_digit.size() == 3) {
          decoded_values.push_back({7, input_digit});
          input_digits[i--] = input_digits.back();
          input_digits.pop_back();
        }
        if (input_digit.size() == 4) {
          decoded_values.push_back({4, input_digit});
          input_digits[i--] = input_digits.back();
          input_digits.pop_back();
        }
        if (input_digit.size() == 7) {
          decoded_values.push_back({8, input_digit});
          input_digits[i--] = input_digits.back();
          input_digits.pop_back();
        }
      }

      auto contains = [](std::string haystack, std::string needle) {
        for (auto c : needle) {
          if (haystack.find(c) == std::string::npos) {
            return false;
          }
        }
        return true;
      };

      while (!input_digits.empty()) {
        if (input_digits.size() == 1) {
          decoded_values.push_back({2, input_digits[0]});
          input_digits.pop_back();
        }
        for (int i = 0; i < input_digits.size(); i++) {
          if (input_digits[i].size() == 5) {
            auto one = std::find_if(
              decoded_values.begin(), decoded_values.end(),
              [](auto value) { return value.digit == 1; });
            if (contains(input_digits[i], one->pattern)) {
              decoded_values.push_back({3, input_digits[i]});
              input_digits[i--] = input_digits.back();
              input_digits.pop_back();
              continue;
            }
          }
          if (input_digits[i].size() == 6) {
            auto four = std::find_if(
              decoded_values.begin(), decoded_values.end(),
              [](auto value) { return value.digit == 4; });
            if (contains(input_digits[i], four->pattern)) {
              decoded_values.push_back({9, input_digits[i]});
              input_digits[i--] = input_digits.back();
              input_digits.pop_back();
              continue;
            }
          }
          if (input_digits[i].size() == 6) {
            auto seven = std::find_if(
              decoded_values.begin(), decoded_values.end(),
              [](auto value) { return value.digit == 7; });
            if (contains(input_digits[i], seven->pattern)) {
              decoded_values.push_back({0, input_digits[i]});
              input_digits[i--] = input_digits.back();
              input_digits.pop_back();
              continue;
            }
          }
          if (input_digits[i].size() == 6) {
            auto four = std::find_if(
              decoded_values.begin(), decoded_values.end(),
              [](auto value) { return value.digit == 4; });
            auto seven = std::find_if(
              decoded_values.begin(), decoded_values.end(),
              [](auto value) { return value.digit == 7; });
            if (four != decoded_values.end()) {
              if (
                !contains(input_digits[i], seven->pattern)
                && !contains(input_digits[i], four->pattern)) {
                decoded_values.push_back({6, input_digits[i]});
                input_digits[i--] = input_digits.back();
                input_digits.pop_back();
                continue;
              }
            }
          }
          if (input_digits[i].size() == 5) {
            auto six = std::find_if(
              decoded_values.begin(), decoded_values.end(),
              [](auto value) { return value.digit == 6; });
            if (six != decoded_values.end()) {
              if (contains(six->pattern, input_digits[i])) {
                decoded_values.push_back({5, input_digits[i]});
                input_digits[i--] = input_digits.back();
                input_digits.pop_back();
                continue;
              }
            }
          }
        }
      }

      auto output_pattern = digit.substr(separator + 2, digit.size());
      std::vector<std::string> output_digits;
      std::istringstream output_pattern_ss(output_pattern);
      for (std::string line; std::getline(output_pattern_ss, line, ' ');) {
        output_digits.push_back(line);
      }

      std::vector<int> digits;
      for (const auto& output_digit : output_digits) {
        std::vector<int> segment_counts = {2, 3, 4, 7};
        if (std::any_of(
              segment_counts.begin(), segment_counts.end(),
              [output_digit](const auto& segment_count) {
                return output_digit.size() == segment_count;
              })) {
          part1_digit_count++;
        }

        auto digit = std::find_if(
          decoded_values.begin(), decoded_values.end(),
          [output_digit](auto decoded_value) {
            return std::is_permutation(
                     output_digit.begin(), output_digit.end(),
                     decoded_value.pattern.begin())
                && output_digit.size() == decoded_value.pattern.size();
          });

        digits.push_back(digit->digit);
      }
      int mul = 1;
      int sum = 0;
      for (auto rit = digits.rbegin(); rit != digits.rend(); ++rit) {
        sum += *rit * mul;
        mul *= 10;
      }
      part2_total += sum;
    }
  }

  std::cout << "part 1: " << part1_digit_count << '\n';
  std::cout << "part 2: " << part2_total << '\n';

  return 0;
}
