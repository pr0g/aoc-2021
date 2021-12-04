#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

std::vector<int> str_to_numbers(const std::string& numbers_str, char delim)
{
  std::istringstream iss(numbers_str);
  std::string item;
  std::vector<int> numbers;
  while (std::getline(iss, item, delim)) {
    numbers.push_back(std::stoi(item));
  }
  return numbers;
}

std::vector<int> str_to_numbers_whitespace(const std::string& numbers_str)
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

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");

  std::string random_numbers_str;
  std::getline(reader, random_numbers_str);

  auto random_numbers = str_to_numbers(random_numbers_str, ',');

  // for (auto num : random_numbers) {
  //   std::cout << num << ' ';
  // }

  // std::cout << "\n\n";

  std::string empty;
  std::getline(reader, empty);

  std::vector<std::string> matrix_str;
  std::vector<std::vector<std::string>> matrices_str;
  for (std::string line; std::getline(reader, line);) {
    if (line.empty() && !matrix_str.empty()) {
      matrices_str.push_back(matrix_str);
      matrix_str.clear();
    } else {
      matrix_str.push_back(line);
    }
  }

  matrices_str.push_back(matrix_str);

  // check
  // for (const auto& m : matrices_str) {
  //   for (const auto& row : m) {
  //     std::cout << row << '\n';
  //   }
  //   std::cout << "\n";
  // }

  struct elem_t
  {
    int num;
    bool found;
  };

  std::vector<std::vector<std::vector<elem_t>>> matrices;

  for (const auto& m : matrices_str) {
    std::vector<std::vector<elem_t>> matrix;
    for (const auto& row_str : m) {
      matrix.emplace_back();
      auto row = str_to_numbers_whitespace(row_str);
      for (auto elem : row) {
        matrix.back().push_back({elem, false});
      }
    }
    matrices.push_back(matrix);
    matrix.clear();
  }

  // check
  // for (const auto& m : matrices) {
  //   for (const auto& row : m) {
  //     for (const auto elem : row) {
  //       std::cout << elem.num << ' ';
  //     }
  //     std::cout << '\n';
  //   }
  //   std::cout << "\n";
  // }

  int first_matching_matrix = -1;

  int first_match_last_number = 0;
  int last_match_last_number = 0;
  std::vector<std::vector<elem_t>> first_matching_board;
  std::vector<std::vector<elem_t>> last_matching_board;
  for (const auto& random_number : random_numbers) {
    // search all matrices
    for (auto& matrix : matrices) {
      for (auto& row : matrix) {
        for (auto& elem : row) {
          if (elem.num == random_number) {
            elem.found = true;
          }
        }
      }
    }

    auto row_size = 5;
    for (int i = 0; i < matrices.size(); ++i) {
      auto& matrix = matrices[i];
      for (int col = 0; col < row_size; ++col) {
        bool match = true;
        for (auto row : matrix) {
          if (!row[col].found) {
            match = false;
            break;
          }
        }
        if (match) {
          if (first_matching_matrix == -1) {
            first_match_last_number = random_number;
            first_matching_matrix = i;
            first_matching_board = matrix;
          }
          last_match_last_number = random_number;
          last_matching_board = matrix;
          matrices[i] = matrices.back();
          matrices.pop_back();
          i--;
          break;
        }
      }

      for (auto& row : matrix) {
        if (std::all_of(row.begin(), row.end(), [](const auto& elem) {
              return elem.found;
            })) {
          if (first_matching_matrix == -1) {
            first_match_last_number = random_number;
            first_matching_matrix = i;
            first_matching_board = matrix;
          }
          last_match_last_number = random_number;
          last_matching_board = matrix;
          matrices[i] = matrices.back();
          matrices.pop_back();
          i--;
          break;
        }
      }
    }
  }
  int sum = 0;
  for (const auto& row : first_matching_board) {
    for (auto elem : row) {
      if (!elem.found) {
        sum += elem.num;
      }
    }
  }
  int sum2 = 0;
  for (const auto& row : last_matching_board) {
    for (auto elem : row) {
      if (!elem.found) {
        sum2 += elem.num;
      }
    }
  }

  std::cout << "part 1: " << sum * first_match_last_number << '\n';
  std::cout << "part 2: " << sum2 * last_match_last_number << '\n';
}
