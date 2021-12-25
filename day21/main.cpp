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
const int g_winning_score = 21;

template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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

enum class player_e
{
  one,
  two
};

struct roll_t
{
  int total;
  int combination;
};

struct state_t
{
  int player1_position;
  int player1_total_score;
  int player1_intermediate_score;
  roll_t roll;
  int rcount;
  int depth;
  int player2_position;
  int player2_total_score;
  int player2_intermediate_score;
  player_e player;
};

bool operator==(const state_t& lhs, const state_t& rhs)
{
  return lhs.player1_position == rhs.player1_position
      && lhs.player1_total_score == rhs.player1_total_score
      //      && lhs.player1_intermediate_score ==
      //      rhs.player1_intermediate_score
      && lhs.roll.combination == rhs.roll.combination
      && lhs.player2_position == rhs.player2_position
      && lhs.player2_total_score == rhs.player2_total_score
      && lhs.depth == rhs.depth
      && lhs.rcount == rhs.rcount
      //      && lhs.player2_intermediate_score ==
      //      rhs.player2_intermediate_score
      && lhs.player == rhs.player;
}

struct state_hash_t
{
  std::size_t operator()(const state_t& state) const
  {
    std::size_t seed = 0;
    hash_combine(seed, state.player1_position);
    hash_combine(seed, state.player1_total_score);
    //    hash_combine(seed, state.player1_intermediate_score);
    hash_combine(seed, state.roll.combination);
    hash_combine(seed, state.depth);
    hash_combine(seed, state.rcount);
    hash_combine(seed, state.player2_position);
    hash_combine(seed, state.player2_total_score);
    //    hash_combine(seed, state.player2_intermediate_score);
    hash_combine(seed, state.player);
    return seed;
  }
};

struct outcome_t
{
  int64_t player1_wins = 0;
  int64_t player2_wins = 0;
};

enum class winner_e
{
  one,
  two,
  none
};

// rolls, player, depth
using cache_t = std::unordered_map<state_t, outcome_t, state_hash_t>;
winner_e roll(state_t state, int depth, player_e player, outcome_t& outcome)
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
      const int score =
        state.player1_position + state.player1_intermediate_score;
      state.player1_position = wrap10(score);
      state.player1_total_score =
        state.player1_total_score + state.player1_position;
      state.player1_intermediate_score = 0;
    }
    if (player == player_e::two) {
      const int score =
        state.player2_position + state.player2_intermediate_score;
      state.player2_position = wrap10(score);
      state.player2_total_score =
        state.player2_total_score + state.player2_position;
      state.player2_intermediate_score = 0;
    }

    if (state.player1_total_score >= g_winning_score) {
      return winner_e::one;
    }
    if (state.player2_total_score >= g_winning_score) {
      return winner_e::two;
    }
  }
  for (int r = 1; r < 4; ++r) {
    auto next_state = state;
    switch (next_player) {
      case player_e::one: {
        next_state.player1_intermediate_score += r;
      } break;
      case player_e::two: {
        next_state.player2_intermediate_score += r;
      } break;
    }
    auto winner = roll(next_state, depth + 1, next_player, outcome);
    if (winner == winner_e::one) {
      outcome.player1_wins++;
    } else if (winner == winner_e::two) {
      outcome.player2_wins++;
    }
  }
  return winner_e::none;
}

outcome_t roll2(
  state_t state, player_e player, outcome_t outcome, cache_t& cache, int depth)
{
  //  if (auto it = cache.find(state); it != cache.end()) {
  //    return it->second;
  //  }

  if (state.player1_total_score >= g_winning_score) {
    outcome.player1_wins++;
    cache[state] = outcome;
    return outcome;
  }
  if (state.player2_total_score >= g_winning_score) {
    outcome.player2_wins++;
    cache[state] = outcome;
    return outcome;
  }

  //    cache[state] = outcome;
  const std::vector<roll_t> rolls = {
    {3, 111}, {4, 112}, {4, 121}, {4, 211}, {5, 122}, {5, 212}, {5, 113},
    {5, 311}, {5, 131}, {5, 221}, {6, 123}, {6, 321}, {6, 132}, {6, 312},
    {6, 213}, {6, 231}, {6, 222}, {7, 223}, {7, 133}, {7, 313}, {7, 232},
    {7, 322}, {7, 331}, {8, 332}, {8, 323}, {8, 233}, {9, 333}};
  for (int r = 0; r < rolls.size(); ++r) {
    const auto& roll = rolls[r];
    auto next_state = state;
    next_state.player1_intermediate_score = 0;
    next_state.player2_intermediate_score = 0;
    next_state.roll = roll;
    next_state.rcount = r;
    next_state.player = player;
    next_state.depth = depth;
    switch (player) {
      case player_e::one: {
        next_state.player1_intermediate_score += roll.total;
      } break;
      case player_e::two: {
        next_state.player2_intermediate_score += roll.total;
      } break;
    }
    if (player == player_e::one) {
      const int score =
        next_state.player1_position + next_state.player1_intermediate_score;
      next_state.player1_position = wrap10(score);
      next_state.player1_total_score =
        next_state.player1_total_score + next_state.player1_position;
    }
    if (player == player_e::two) {
      const int score =
        next_state.player2_position + next_state.player2_intermediate_score;
      next_state.player2_position = wrap10(score);
      next_state.player2_total_score =
        next_state.player2_total_score + next_state.player2_position;
    }
    outcome = roll2(
      next_state, player == player_e::one ? player_e::two : player_e::one,
      outcome, cache, depth + 1);
    //    cache[next_state] = outcome;
  }
  return outcome;
}

outcome_t roll3(state_t state, player_e player, cache_t& cache, int depth)
{
  if (state.player1_total_score >= g_winning_score) {
    return outcome_t{1, 0};
  }
  if (state.player2_total_score >= g_winning_score) {
    return outcome_t{0, 1};
  }

  //    cache[state] = outcome;
  const std::vector<roll_t> rolls = {
    {3, 111}, {4, 112}, {4, 121}, {4, 211}, {5, 122}, {5, 212}, {5, 113},
    {5, 311}, {5, 131}, {5, 221}, {6, 123}, {6, 321}, {6, 132}, {6, 312},
    {6, 213}, {6, 231}, {6, 222}, {7, 223}, {7, 133}, {7, 313}, {7, 232},
    {7, 322}, {7, 331}, {8, 332}, {8, 323}, {8, 233}, {9, 333}};
  outcome_t outcome;
  for (int r = 0; r < rolls.size(); ++r) {
    const auto& roll = rolls[r];
    auto next_state = state;
    next_state.player1_intermediate_score = 0;
    next_state.player2_intermediate_score = 0;
    next_state.roll = roll;
    next_state.rcount = r;
    next_state.player = player;
    next_state.depth = depth;
    switch (player) {
      case player_e::one: {
        next_state.player1_intermediate_score += roll.total;
      } break;
      case player_e::two: {
        next_state.player2_intermediate_score += roll.total;
      } break;
    }
    if (player == player_e::one) {
      const int score =
        next_state.player1_position + next_state.player1_intermediate_score;
      next_state.player1_position = wrap10(score);
      next_state.player1_total_score =
        next_state.player1_total_score + next_state.player1_position;
    }
    if (player == player_e::two) {
      const int score =
        next_state.player2_position + next_state.player2_intermediate_score;
      next_state.player2_position = wrap10(score);
      next_state.player2_total_score =
        next_state.player2_total_score + next_state.player2_position;
    }
    outcome_t next;
    if (auto it = cache.find(next_state); it != cache.end()) {
      next = it->second;
    } else {
      next = roll3(
        next_state, player == player_e::one ? player_e::two : player_e::one,
        cache, depth + 1);
      cache[next_state] = next;
    }
    outcome.player1_wins += next.player1_wins;
    outcome.player2_wins += next.player2_wins;
  }
  return outcome;
}

void test()
{
  std::vector<int> numbers = {1, 1, 1, 1};

  auto print = [&numbers]() {
    for (auto number : numbers) {
      std::cout << number << ' ';
    }
    std::cout << '\n';
  };

  int count = 0;
  int padding = 1;
  int number = numbers.size();
  int index = number - 1;
  int combinations = std::pow(number, number);
  while (combinations--) {
    print();
    numbers[index]++;
    if (numbers[index] > numbers.size()) {
      count++;
      //      if (count == number) {
      if (count % 16 == 0 || count % 24 == 0 || count % 32 == 0) {
        padding += 2;
      } else if (count % number == 0) {
        padding++;
        // count = 0;
      }
      for (int p = 0; p < padding; ++p) {
        numbers[index] = 1;
        index -= 1;
        numbers[index]++;
      }
      index += padding;
      if (padding > 1) {
        padding--;
      }
    }
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
//  test();

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
  }

  //  {
  //    state_t state{};
  //    state.player1_position = 4;
  //    state.player1_total_score = 0;
  //    state.player1_intermediate_score = 0;
  //    state.player2_position = 6;
  //    state.player2_total_score = 0;
  //    state.player2_intermediate_score = 0;
  //
  //    outcome_t outcome;
  //    cache_t cache;
  //    roll(state, 1, player_e::one, outcome);
  //
  //    std::cout << outcome.player1_wins << '\n';
  //    std::cout << outcome.player2_wins << '\n';
  //  }

  //  {
  //    state_t state{};
  //    state.player1_position = 4;
  //    state.player1_total_score = 0;
  //    state.player1_intermediate_score = 0;
  //    state.player2_position = 6;
  //    state.player2_total_score = 0;
  //    state.player2_intermediate_score = 0;
  //
  //    outcome_t outcome;
  //    cache_t cache;
  //    outcome_t result = roll2(state, player_e::one, outcome, cache, 1);
  //
  //    std::cout << result.player1_wins << '\n';
  //    std::cout << result.player2_wins << '\n';
  //  }

  {
    state_t state{};
    state.player1_position = 4;
    state.player1_total_score = 0;
    state.player1_intermediate_score = 0;
    state.player2_position = 6;
    state.player2_total_score = 0;
    state.player2_intermediate_score = 0;

    cache_t cache;
    outcome_t result = roll3(state, player_e::one, cache, 1);

    std::cout << "part 2: "
              << std::max(result.player1_wins, result.player2_wins) << '\n';
  }

  //  {
  //    state_t begin_state{};
  //    begin_state.player1_position = 4;
  //    begin_state.player1_total_score = 0;
  //    begin_state.player1_intermediate_score = 0;
  //    begin_state.player2_position = 6;
  //    begin_state.player2_total_score = 0;
  //    begin_state.player2_intermediate_score = 0;
  //
  //    outcome_t outcome;
  //    outcome.player1_wins = 0;
  //    outcome.player2_wins = 0;
  //
  //    //    int depth = 1;
  //    player_e player = player_e::one;
  //    int roll = 1;
  //    std::vector<state_t> states;
  //    states.push_back(begin_state);
  //
  //    while (!states.empty()) {
  //      auto state = states.back();
  //      player_e next_player;
  //      int wrapped_roll = wrap(roll, 6);
  //      if (wrapped_roll >= 1 && wrapped_roll <= 3) {
  //        next_player = player_e::one;
  //      } else {
  //        next_player = player_e::two;
  //      }
  //
  //      if (next_player != player) {
  //        if (player == player_e::one) {
  //          const int score =
  //            state.player1_position + state.player1_intermediate_score;
  //          state.player1_position = wrap10(score);
  //          state.player1_total_score =
  //            state.player1_total_score + state.player1_position;
  //          state.player1_intermediate_score = 0;
  //        }
  //        if (player == player_e::two) {
  //          const int score =
  //            state.player2_position + state.player2_intermediate_score;
  //          state.player2_position = wrap10(score);
  //          state.player2_total_score =
  //            state.player2_total_score + state.player2_position;
  //          state.player2_intermediate_score = 0;
  //        }
  //        const int winning_score = 10;
  //        if (state.player1_total_score >= winning_score) {
  //          outcome.player1_wins++;
  //          states.pop_back();
  //          auto& prev_state = states.back();
  //          prev_state.player1_roll++;
  //          roll--;
  //          continue;
  //        }
  //        if (state.player2_total_score >= winning_score) {
  //          outcome.player2_wins++;
  //          states.pop_back();
  //          auto& prev_state = states.back();
  //          prev_state.player2_roll++;
  //          roll--;
  //          continue;
  //        }
  //      }
  //      int r;
  //      for (r = next_player == player_e::one ? state.player1_roll
  //                                            : state.player2_roll;
  //           r < 4;) {
  //        auto next_state = state;
  //        switch (next_player) {
  //          case player_e::one: {
  //            next_state.player1_intermediate_score += r;
  //            states.push_back(next_state);
  //          } break;
  //          case player_e::two: {
  //            next_state.player2_intermediate_score += r;
  //            states.push_back(next_state);
  //          } break;
  //        }
  //        roll++;
  //        player = next_player;
  //        //        state = next_state;
  //        break;
  //        //        roll(next_state, depth + 1, next_player, outcome);
  //      }
  //      if (r == 4) {
  //        states.pop_back();
  //        auto& prev_state = states.back();
  //        prev_state.player2_roll++;
  //        roll -= 2;
  //      }
  //    }
  //  }
}
