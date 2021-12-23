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
int g_player1_wins = 0;
int g_player2_wins = 0;

int wrap(int num, int range)
{
  while (num > range) {
    num -= range;
  }
  return num;
}

int wrap10(int num)
{
  return wrap(num, 10);
}

int wrap3(int num)
{
  return wrap(num, 3);
}

int roll_100_sided()
{
  static int roll_hundred_number = 1;
  int roll = roll_hundred_number;
  roll_hundred_number = wrap(roll_hundred_number + 1, 100);
  g_roll_count++;
  return roll;
}

int roll_3_sided()
{
  static int roll_three_number = 1;
  int roll = roll_three_number;
  roll_three_number = wrap3(roll_three_number + 1);
  return roll;
}

enum class player_e
{
  one,
  two
};

struct universe_t
{
  int player1_position;
  int player1_total_score;
  int player2_position;
  int player2_total_score;

  std::unique_ptr<universe_t> one;
  std::unique_ptr<universe_t> two;
  std::unique_ptr<universe_t> three;
};

struct state_t
{
  int player1_position;
  int player1_total_score;
  int player1_intermediate_score;
  int player2_position;
  int player2_total_score;
  int player2_intermediate_score;

  struct turn
  {
    int player;
    int roll;
  };

  std::vector<turn> turns;
};

void roll(state_t state, int depth, player_e player)
{
  player_e next_player;
  int wrapped_depth = wrap(depth, 6);
  if (wrapped_depth >= 1 && wrapped_depth <= 3) {
    next_player = player_e::one;
  } else if (wrapped_depth > 3 && wrapped_depth <= 6) {
    next_player = player_e::two;
  } else {
    next_player = player_e::one; // error
  }

  if (next_player != player) {
    if (player == player_e::one) {
      int score = state.player1_position + state.player1_intermediate_score;
      state.player1_position = wrap10(score);
      state.player1_total_score =
        state.player1_total_score + state.player1_position;
      state.player1_intermediate_score = 0;
    }
    if (player == player_e::two) {
      int score = state.player2_position + state.player2_intermediate_score;
      state.player2_position = wrap10(score);
      state.player2_total_score =
        state.player2_total_score + state.player2_position;
      state.player2_intermediate_score = 0;
    }

    if (state.player1_total_score >= 21) {
//      for (auto t : state.turns) {
//        std::cout << "p" << t.player + 1 << "r" << t.roll;
//      }
//      std::cout << '\n';
      int i;
      i = 0;
      g_player1_wins++;
      return;
    }
    if (state.player2_total_score >= 21) {
//      for (auto t : state.turns) {
//        std::cout << "p" << t.player + 1 << "r" << t.roll;
//      }
//      std::cout << '\n';
      int i;
      i = 0;
      g_player2_wins++;
      return;
    }
  }

  for (int r = 1; r < 4; ++r) {
    auto next_state = state;
    switch (next_player) {
      case player_e::one: {
        next_state.player1_intermediate_score += r;
        next_state.turns.push_back({(int)next_player, r});
      } break;
      case player_e::two: {
        next_state.player2_intermediate_score += r;
        next_state.turns.push_back({(int)next_player, r});
      } break;
    }
    roll(next_state, depth + 1, next_player);
  }
}

void part2(universe_t& root, player_e player, int roll)
{
  //  player_e player = player_e::one;
  //  while (player1_total_score < 21 && player2_total_score < 21) {
  //    int roll1 = 1;
  //    int roll2 = 2;
  //    int roll3 = 3;

  switch (player) {
    case player_e::one: {
      int score = root.player1_position;
      //        for (int i = 0; i < 3; ++i) {
      score += roll; // roll_3_sided();
      //        }
      root.player1_position = wrap10(score);
      root.player1_total_score += root.player1_position;
      //        player = player_e::two;
    } break;
    case player_e::two: {
      int score = root.player2_position;
      //        for (int i = 0; i < 3; ++i) {
      score += roll; // roll_3_sided();
      //        }
      root.player2_position = wrap10(score);
      root.player2_total_score += root.player2_position;
      //        player = player_e::one;
    } break;
  }
  //  }
}

void part2_recurse(universe_t& root, player_e player, int roll_count)
{
  for (int p = 0; p < 2; ++p) {
    for (int i = roll_count; i < 3; ++i) {
      int roll = i + 1;
      switch (p) {
        case 0: {
          int score = root.player1_position;
          //        for (int i = 0; i < 3; ++i) {
          score += roll; // roll_3_sided();
          //        }
          part2_recurse(root, player, i + 1);

          root.player1_position = wrap10(score);
          root.player1_total_score += root.player1_position;
          //        player = player_e::two;
        } break;
        case 1: {
          int score = root.player2_position;
          //        for (int i = 0; i < 3; ++i) {
          score += roll; // roll_3_sided();
          //        }
          root.player2_position = wrap10(score);
          root.player2_total_score += root.player2_position;
          part2_recurse(root, player, i + 1);
          //        player = player_e::one;
        } break;
        default:
          break;
      }
      //  }
    }
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  int player1_position = 4;
  int player1_total_score = 0;
  int player2_position = 8;
  int player2_total_score = 0;

  player_e player = player_e::one;
  while (player1_total_score < 1000 && player2_total_score < 1000) {
    switch (player) {
      case player_e::one: {
        int score = player1_position;
        for (int i = 0; i < 3; ++i) {
          score += roll_100_sided();
        }
        player1_position = wrap10(score);
        player1_total_score += player1_position;
        player = player_e::two;
      } break;
      case player_e::two: {
        int score = player2_position;
        for (int i = 0; i < 3; ++i) {
          score += roll_100_sided();
        }
        player2_position = wrap10(score);
        player2_total_score += player2_position;
        player = player_e::one;
      } break;
    }
  }

  auto min = std::min(player1_total_score, player2_total_score);
  std::cout << "part 1: " << min * g_roll_count << '\n';

  //  universe_t universe_root;
  state_t state;
  state.player1_position = 4;
  state.player1_total_score = 0;
  state.player1_intermediate_score = 0;
  state.player2_position = 6;
  state.player2_total_score = 0;
  state.player2_intermediate_score = 0;

  roll(state, 1, player_e::one);

  std::cout << g_player1_wins << '\n';
  std::cout << g_player2_wins << '\n';

  int i;
  i = 0;
}
