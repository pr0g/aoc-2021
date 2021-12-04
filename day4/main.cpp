#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

std::vector<int> str_to_numbers(std::string numbers_str, char delim)
{
  std::istringstream iss(numbers_str);
  std::string item;
  std::vector<int> numbers;
  while (std::getline(iss, item, delim)) {
    numbers.push_back(std::stoi(item));
  }
  return numbers;
}

std::vector<int> str_to_numbers_whitespace(std::string numbers_str)
{
  std::istringstream iss(numbers_str);
  std::string item;
  std::vector<int> numbers;
  int num;
  while (iss >> num) {
    numbers.push_back(num);
  }
  return numbers;
}

int main(int argc, char** argv)
{
  std::ifstream reader("input.txt");

  std::string random_numbers_str;
  std::getline(reader, random_numbers_str);

  auto random_numbers = str_to_numbers(random_numbers_str, ',');

  for (auto num : random_numbers) {
    std::cout << num << ' ';
  }

  std::cout << "\n\n";

  std::string empty;
  std::getline(reader, empty);

  std::vector<std::string> matrix_str;
  std::vector<std::vector<std::string>> matrices_str;
  for (std::string line; std::getline(reader, line);) {
    if (line.empty()) {
      matrices_str.push_back(matrix_str);
      matrix_str.clear();
    }
    matrix_str.push_back(line);
  }

  matrices_str.push_back(matrix_str);

  // check
  for (const auto m : matrices_str) {
    for (const auto row : m) {
      std::cout << row << '\n';
    }
    std::cout << "\n";
  }

  struct elem_t {
    int num;
    bool found;
  };

  std::vector<std::vector<std::vector<elem_t>>> matrices;

  for (const auto& m : matrices_str) {
    std::vector<std::vector<elem_t>> matrix;
    for (const auto& row_str : m) {
      matrix.push_back(std::vector<elem_t>());
      auto row = str_to_numbers_whitespace(row_str);
      for (auto elem : row) {
        matrix.back().push_back({elem, false});
      }
    }
    matrices.push_back(matrix);
    matrix.clear();
  }

  // check
  for (const auto m : matrices) {
    for (const auto row : m) {
      for (const auto elem : row) {
        std::cout << elem.num << ' ';
      }
      std::cout << '\n';
    }
    std::cout << "\n";
  }
}
