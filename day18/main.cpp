#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

struct pair_t
{
  using part_t = std::variant<int, std::unique_ptr<pair_t>>;
  part_t lhs;
  part_t rhs;
};

int build(pair_t& pair, const std::string& input)
{
  int offset = 0;
  bool right = false;
  while (true) {
    auto token = input[offset++];
    if (token == '\0') {
      return offset;
    }

    if (token == '[') {
      //      auto next_token = input[offset++];
      //      if (next_token == '[') {
      if (!right) {
        pair.lhs = std::make_unique<pair_t>();
        if (auto* lhs = std::get_if<std::unique_ptr<pair_t>>(&pair.lhs)) {
          offset += build(**lhs, input.substr(offset, input.size()));
        }
      } else {
        pair.rhs = std::make_unique<pair_t>();
        if (auto* rhs = std::get_if<std::unique_ptr<pair_t>>(&pair.rhs)) {
          offset += build(**rhs, input.substr(offset, input.size()));
        }
      }

      //      } else { // number
      //      pair.lhs = input[offset++] - '0';
      //        build(pair, input.substr(offset, input.size()));
      //      }
    } else if (token == ']') {
      return offset;
    } else if (token == ',') {
      right = true;
    } else {
      auto number = token;
      if (!right) {
        pair.lhs = number - '0';
      } else {
        pair.rhs = number - '0';
      }
    }
  }
}

void print_pair(const pair_t& pair)
{
  std::cout << '[';
  if (auto* lhs = std::get_if<std::unique_ptr<pair_t>>(&pair.lhs)) {
    print_pair(**lhs);
  }
  if (auto* lhs = std::get_if<int>(&pair.lhs)) {
    std::cout << *lhs;
  }
  std::cout << ',';
  if (auto* rhs = std::get_if<std::unique_ptr<pair_t>>(&pair.rhs)) {
    print_pair(**rhs);
  }
  if (auto* rhs = std::get_if<int>(&pair.rhs)) {
    std::cout << *rhs;
  }
  std::cout << ']';
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> rows;
  for (std::string line; std::getline(reader, line);) {
    rows.push_back(std::move(line));
  }
  for (const auto& row : rows) {
    //    std::cout << row << '\n';
  }

  // If any pair is nested inside four pairs,
  // the leftmost such pair explodes.
  // If any regular number is 10 or greater,
  // the leftmost such regular number splits.

  // do explode first s

//  auto example = std::string("[1,2]");
//  auto example = std::string("[[1,2],3]");
//  auto example = std::string("[9,[8,7]]");
//  auto example = std::string("[[1,9],[8,5]]");
//  auto example = std::string("[[[[1,2],[3,4]],[[5,6],[7,8]]],9]");
//  auto example = std::string("[[[9,[3,8]],[[0,9],6]],[[[3,7],[4,9]],3]]");
  auto example = std::string("[[[[1,3],[5,3]],[[1,3],[8,7]]],[[[4,9],[6,9]],[[8,2],[7,3]]]]");

  example = example.substr(1, example.size() - 2);

  pair_t pair;
  build(pair, example);
  print_pair(pair);

  std::cout << '\n';
}
