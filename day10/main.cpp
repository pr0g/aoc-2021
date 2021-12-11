#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
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

  struct parsed_token_t
  {
    char token;
    int depth;
  };

  const std::vector<char> opening_tokens = {'{', '[', '<', '('};
  const std::vector<char> closing_tokens = {'}', ']', '>', ')'};

  std::unordered_map<char, int> scores = {
    {')', 3}, {']', 57}, {'}', 1197}, {'>', 25137}};

  std::vector<std::string> valid_lines;

  std::vector<char> invalid_tokens;
  for (const auto& line : lines) {
    int depth = 0;
    bool valid = true;
    std::vector<parsed_token_t> parsed_tokens;
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
          valid = false;
          invalid_tokens.push_back(token);
          break;
        }
        parsed_tokens.pop_back();
        depth--;
      }
    }
    if (valid) {
      std::string s;
      for (const auto t : parsed_tokens) {
        s.append(std::string(1, t.token));
      }
      valid_lines.push_back(s);
    }
  }

  int score = 0;
  for (const auto invalid_token : invalid_tokens) {
    score += scores[invalid_token];
  }

  std::cout << "part 1: " << score << '\n';

  std::unordered_map<char, int64_t> secondary_cores = {
    {')', 1}, {']', 2}, {'}', 3}, {'>', 4}};

  std::vector<int64_t> total_scores;
  for (const auto& line : valid_lines) {
    int64_t total_score = 0;
    for (auto rit = line.rbegin(); rit != line.rend(); ++rit) {
      total_score *= 5;
      if (*rit == '(') {
        total_score += secondary_cores[')'];
      }
      if (*rit == '[') {
        total_score += secondary_cores[']'];
      }
      if (*rit == '<') {
        total_score += secondary_cores['>'];
      }
      if (*rit == '{') {
        total_score += secondary_cores['}'];
      }
    }
    total_scores.push_back(total_score);
  }

  std::sort(total_scores.begin(), total_scores.end());

  std::cout << "part 2: " << total_scores[total_scores.size() / 2] << '\n';

  return 0;
}
