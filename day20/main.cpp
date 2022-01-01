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

struct vec2
{
  int x;
  int y;
};

uint64_t bin_to_dec(const std::string& bin)
{
  uint64_t binary = 0;
  uint64_t base_2 = 1;
  for (auto digit_it = bin.rbegin(); digit_it != bin.rend(); ++digit_it) {
    if (*digit_it == '1') {
      binary += base_2;
    }
    base_2 <<= 1;
  }
  return binary;
}

template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

bool operator==(const vec2& lhs, const vec2& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

vec2 operator+(const vec2& lhs, const vec2& rhs)
{
  return {lhs.x + rhs.x, lhs.y + rhs.y};
}

vec2 operator-(const vec2& lhs, const vec2& rhs)
{
  return {lhs.x - rhs.x, lhs.y - rhs.y};
}

struct vec2_hash
{
  std::size_t operator()(const vec2& vec) const
  {
    std::size_t seed = 0;
    hash_combine(seed, vec.x);
    hash_combine(seed, vec.y);
    return seed;
  }
};

using image_t = std::unordered_map<vec2, bool, vec2_hash>;

bool output_pixel(
  const image_t& image, const std::string& enhancement_algo, const vec2& coord,
  int count)
{
  vec2 offsets[] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {0, 0},
                    {1, 0},   {-1, 1}, {0, 1},  {1, 1}};
  std::string binary;
  for (auto offset : offsets) {
    auto next = coord + offset;
    if (auto it = image.find(next); it != image.end()) {
      binary.push_back(it->second ? '1' : '0');
    } else {
      binary.push_back(count % 2 == 0 ? '0' : '1');
    }
  }
  auto offset = bin_to_dec(binary);
  return enhancement_algo[offset] == '#';
}

int calculate_lit_pixels(
  image_t working_image, const std::string& enhancement_algo, int enhancements)
{
  for (int enhancement = 0; enhancement < enhancements; ++enhancement) {
    auto min_x = std::min_element(
      working_image.begin(), working_image.end(),
      [](auto lhs, auto rhs) { return lhs.first.x < rhs.first.x; });
    auto min_y = std::min_element(
      working_image.begin(), working_image.end(),
      [](auto lhs, auto rhs) { return lhs.first.y < rhs.first.y; });
    auto max_x = std::max_element(
      working_image.begin(), working_image.end(),
      [](auto lhs, auto rhs) { return lhs.first.x < rhs.first.x; });
    auto max_y = std::max_element(
      working_image.begin(), working_image.end(),
      [](auto lhs, auto rhs) { return lhs.first.y < rhs.first.y; });

    auto next_min_x = min_x->first.x - 2;
    auto next_max_x = max_x->first.x + 2;
    auto next_min_y = min_y->first.y - 2;
    auto next_max_y = max_y->first.y + 2;

    image_t next_image;
    for (int row = next_min_y; row < next_max_y; ++row) {
      for (int col = next_min_x; col < next_max_x; ++col) {
        auto next_output = output_pixel(
          working_image, enhancement_algo, vec2{col, row}, enhancement);
        next_image[vec2{col, row}] = next_output;
      }
    }

    working_image = next_image;

    // check
    // for (int row = next_min_y; row < next_max_y; ++row) {
    //   for (int col = next_min_x; col < next_max_x; ++col) {
    //     std::cout << (working_image[vec2{col, row}] ? '#' : '.');
    //   }
    //   std::cout << '\n';
    // }
  }

  return std::count_if(
    working_image.begin(), working_image.end(),
    [](const auto pixel) { return pixel.second; });
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");

  std::string image_enhancement_algo;
  std::getline(reader, image_enhancement_algo);

  std::string ignore;
  std::getline(reader, ignore);

  std::vector<std::string> input_image_strs;
  for (std::string line; std::getline(reader, line);) {
    input_image_strs.push_back(std::move(line));
  }

  // std::cout << image_enhancement_algo << "\n\n";

  // check
  // for (const auto& image_row : input_image_strs) {
  //   std::cout << image_row << '\n';
  // }

  // std::cout << '\n';

  std::unordered_map<vec2, bool, vec2_hash> image;
  for (int row = 0; row < input_image_strs.size(); row++) {
    for (int col = 0; col < input_image_strs.back().size(); col++) {
      image[vec2{col, row}] = input_image_strs[row][col] == '#';
    }
  }

  // check
  // for (int row = 0; row < input_image_strs.size(); row++) {
  //   for (int col = 0; col < input_image_strs.back().size(); col++) {
  //     std::cout << image[vec2{col, row}];
  //   }
  //   std::cout << '\n';
  // }

  // std::cout << '\n';

  std::cout << "part 1: " << calculate_lit_pixels(image, image_enhancement_algo, 2) << '\n';
  std::cout << "part 2: " << calculate_lit_pixels(image, image_enhancement_algo, 50) << '\n';

  return 0;
}
