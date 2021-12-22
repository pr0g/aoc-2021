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

int build_tree_impl(pair_t& pair, const std::string& input)
{
  int offset = 0;
  bool right = false;
  while (true) {
    auto token = input[offset++];
    if (token == '\0') {
      return offset;
    }
    if (token == '[') {
      if (!right) {
        pair.lhs = std::make_unique<pair_t>();
        if (auto* lhs = std::get_if<std::unique_ptr<pair_t>>(&pair.lhs)) {
          offset += build_tree_impl(**lhs, input.substr(offset, input.size()));
        }
      } else {
        pair.rhs = std::make_unique<pair_t>();
        if (auto* rhs = std::get_if<std::unique_ptr<pair_t>>(&pair.rhs)) {
          offset += build_tree_impl(**rhs, input.substr(offset, input.size()));
        }
      }
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

void build_tree(pair_t& pair, const std::string& input)
{
  std::string safe_input = input.substr(1, input.size() - 2);
  build_tree_impl(pair, safe_input);
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

struct pair_arr_t
{
  int number;
  int depth;
};

void build_array(const pair_t& pair, std::vector<pair_arr_t>& array, int depth)
{
  if (auto* lhs = std::get_if<std::unique_ptr<pair_t>>(&pair.lhs)) {
    build_array(**lhs, array, depth + 1);
  }
  if (auto* lhs = std::get_if<int>(&pair.lhs)) {
    array.push_back(pair_arr_t{*lhs, depth});
  }
  if (auto* rhs = std::get_if<std::unique_ptr<pair_t>>(&pair.rhs)) {
    build_array(**rhs, array, depth + 1);
  }
  if (auto* rhs = std::get_if<int>(&pair.rhs)) {
    array.push_back(pair_arr_t{*rhs, depth});
  }
}

std::vector<pair_arr_t> add_numbers(
  const std::vector<pair_arr_t>& lhs, const std::vector<pair_arr_t>& rhs)
{
  std::vector<pair_arr_t> result;
  result.reserve(lhs.size() + rhs.size());
  result.insert(result.end(), lhs.begin(), lhs.end());
  result.insert(result.end(), rhs.begin(), rhs.end());
  for (auto& pair : result) {
    pair.depth++;
  }
  return result;
}

void explode_and_split(std::vector<pair_arr_t>& array)
{
  // explode
  bool did_explode;
  bool did_split;
  do {
    did_explode = false;
    did_split = false;
    for (int i = 0; i < array.size(); ++i) {
      auto depth = array[i].depth;
      if (array[i].depth == 4) {
        if (i > 0) {
          array[i - 1].number += array[i].number;
        }
        if (i < array.size() - 2) {
          array[i + 2].number += array[i + 1].number;
        }
        array.erase(array.begin() + i);
        array.erase(array.begin() + i);
        array.insert(array.begin() + i, pair_arr_t{0, depth - 1});
        did_explode = true;
        goto end;
      }
    }
    for (int i = 0; i < array.size(); ++i) {
      auto depth = array[i].depth;
      if (array[i].number >= 10) {
        auto left = array[i].number / 2;
        auto right =
          static_cast<int>(std::lround((float)array[i].number / 2.0f));
        array.erase(array.begin() + i);
        array.insert(array.begin() + i, pair_arr_t{right, depth + 1});
        array.insert(array.begin() + i, pair_arr_t{left, depth + 1});
        did_split = true;
        goto end;
      }
    }
  end:
    int blah;
  } while (did_explode || did_split);
}

std::vector<pair_arr_t> process_row(const std::string& row)
{
  pair_t pair;
  build_tree(pair, row);
  std::vector<pair_arr_t> array;
  build_array(pair, array, 0);
  explode_and_split(array);
  return array;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> rows;
  for (std::string line; std::getline(reader, line);) {
    rows.push_back(std::move(line));
  }

  // check
  // for (const auto& row : rows) {
  //   std::cout << row << '\n';
  // }

  // If any pair is nested inside four pairs,
  // the leftmost such pair explodes.
  // If any regular number is 10 or greater,
  // the leftmost such regular number splits.

  // do explode first s

  // auto example = std::string("[1,2]");
  // auto example = std::string("[[1,2],3]");
  // auto example = std::string("[9,[8,7]]");
  // auto example = std::string("[[1,9],[8,5]]");
  // auto example = std::string("[[[[1,2],[3,4]],[[5,6],[7,8]]],9]");
  // auto example = std::string("[[[9,[3,8]],[[0,9],6]],[[[3,7],[4,9]],3]]");
  // auto example =
  // std::string("[[[[1,3],[5,3]],[[1,3],[8,7]]],[[[4,9],[6,9]],[[8,2],[7,3]]]]");

  //    auto example = std::string("[[[[[9,8],1],2],3],4]");
  // auto example = std::string("[7,[6,[5,[4,[3,2]]]]]");
  //  auto example = std::string("[[6,[5,[4,[3,2]]]],1]");
  //  auto example = std::string("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]");
  //  auto example = std::string("[[3,[2,[8,0]]],[9,[5,[4,[3,2]]]]]");
  // auto example = std::string("[[[[[4,3],4],4],[7,[[8,4],9]]],[1,1]]");

//  auto example = std::string("[[1,2],[[3,4],5]]");
//  auto example = std::string("[[[[0,7],4],[[7,8],[6,0]]],[8,1]]");
//  auto example = std::string("[[[[1,1],[2,2]],[3,3]],[4,4]]");
//  auto example = std::string("[[[[3,0],[5,3]],[4,4]],[5,5]]");
//  auto example = std::string("[[[[5,0],[7,4]],[5,5]],[6,6]]");
  auto example = std::string("[[[[8,7],[7,7]],[[8,6],[7,7]]],[[[0,7],[6,6]],[8,7]]]");
  auto test = process_row(example);

  int deepest = 0;
  for (auto number : test) {
    deepest = std::max(number.depth, deepest);
  }

  while (deepest >= 0) {
    for (int i = 0; i < test.size() - 1; ++i) {
      if (test[i].depth == deepest) {
        int depth = test[i].depth;
        auto number = 3 * test[i].number + 2 * test[i + 1].number;
        test.erase(test.begin() + i);
        test.erase(test.begin() + i);
        test.insert(test.begin() + i, {number, depth - 1});
      }
    }
    deepest--;
  }

  std::cout << test[0].number << '\n';

  //  auto test_str = convert_to_string(test);

  //  std::cout << test_str << '\n';

  //  pair_t pair;
  //  build_tree(pair, example);
  //  print_pair(pair);

  std::cout << '\n';

  //  std::vector<pair_arr_t> array;
  //  build_array(pair, array, 0);

  //  auto added = add_snailfish_number("[[[[4,3],4],4],[7,[[8,4],9]]]",
  //  "[1,1]");

  auto number = process_row(rows[0]);
  //  auto prev_number = rows[0];
  for (int i = 1; i < rows.size(); ++i) {
    //    auto next_number = add_snailfish_number(prev_number, rows[i]);
    auto next_number = process_row(rows[i]);
    auto combined = add_numbers(number, next_number);
    //    auto combined = process_row(next_number);
    explode_and_split(combined);
    number = combined;
    int j;
    j = 0;
  }

  //  for (auto pair_arr : number) {
  //    std::cout << pair_arr.number << ' ' << pair_arr.depth << '\n';
  //  }

  std::cout << "---\n";

  //  for (auto pair_arr : array) {
  //    std::cout << pair_arr.number << ' ' << pair_arr.depth << '\n';
  //  }
}

// [1,1]
// [2,2]

// [1,1] + [2,2]
// [[1,1],[2,2]]
