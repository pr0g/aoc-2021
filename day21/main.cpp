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

int g_roll_count = 0;

int roll()
{
  static int roll_number = 0;
  roll_number = (roll_number + 1) % 100;
  g_roll_count++;
  return roll_number;
}

int wrap(int num) {
  while (num > 10) {
    num-=10;
  }
  return num;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  enum class player_e
  {
    one,
    two
  };

  int player1_position = 4;
  int player1_total_score = 0;
  int player2_position = 6;
  int player2_total_score = 0;

  player_e player = player_e::one;
  while (player1_total_score < 1000 && player2_total_score < 1000) {
    switch (player) {
      case player_e::one: {
        int score = player1_position;
        for (int i = 0; i < 3; ++i) {
          score += roll();
        }
        player1_position = wrap(score);
        player1_total_score += player1_position;
        player = player_e::two;
      } break;
      case player_e::two: {
        int score = player2_position;
        for (int i = 0; i < 3; ++i) {
          score += roll();
        }
        player2_position = wrap(score);
        player2_total_score += player2_position;
        player = player_e::one;
      } break;
    }
  }

  auto min = std::min(player1_total_score, player2_total_score);

  std::cout << "part 1: " << min * g_roll_count << '\n';
}
