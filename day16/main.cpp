#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::string hex_code_to_bin(const char hex_code)
{
  switch (std::toupper(hex_code)) {
    case '0':
      return "0000";
    case '1':
      return "0001";
    case '2':
      return "0010";
    case '3':
      return "0011";
    case '4':
      return "0100";
    case '5':
      return "0101";
    case '6':
      return "0110";
    case '7':
      return "0111";
    case '8':
      return "1000";
    case '9':
      return "1001";
    case 'A':
      return "1010";
    case 'B':
      return "1011";
    case 'C':
      return "1100";
    case 'D':
      return "1101";
    case 'E':
      return "1110";
    case 'F':
      return "1111";
    default:
      return "";
  }
}

std::string hex_to_bin(const std::string& hex)
{
  std::string binary;
  for (const auto hex_code : hex) {
    binary.append(hex_code_to_bin(hex_code));
  }
  return binary;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::string message;
  std::getline(reader, message);

  // check
  std::cout << message << '\n';

  std::string binary_rep = hex_to_bin(message);

  // check
  std::cout << binary_rep << '\n';
}
