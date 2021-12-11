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
  std::vector<std::string> lines;
  for (std::string line; std::getline(reader, line);) {
    lines.push_back(line);
  }

  // stack
  // open - depth (pop at same depth)

  struct parsed_token_t
  {
    char token;
    int depth;
  };

  const std::vector<char> opening_tokens = {'{', '[', '<', '('};
  const std::vector<char> closing_tokens = {'}', ']', '>', ')'};

  std::unordered_map<char, int> scores = {
    {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}};

  std::vector<char> invalid_tokens;
  std::vector<parsed_token_t> parsed_tokens;
  for (const auto& line : lines) {
    int depth = 0;
    for (const auto token : line) {
      auto opening = std::any_of(
        opening_tokens.begin(), opening_tokens.end(),
        [token](const auto t) { return t == token; });
      auto closing = std::any_of(
        closing_tokens.begin(), closing_tokens.end(),
        [token](const auto t) { return t == token; });
      if (opening) {
        parsed_tokens.push_back({token, depth++});
      }
      if (closing) {
        auto last = parsed_tokens.back();
        if (
          (last.token == '{' && token != '}')
          || (last.token == '<' && token != '>')
          || (last.token == '(' && token != ')')
          || (last.token == '[' && token != ']')) {
          invalid_tokens.push_back(token);
          break;
        }
        parsed_tokens.pop_back();
        depth--;
      }
    }
  }

  int score = 0;
  for (const auto invalid_token : invalid_tokens) {
    score += scores[invalid_token];
  }

  std::cout << "part 1: " << score << '\n';

  return 0;
}
