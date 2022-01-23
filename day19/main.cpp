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

#include <as/as-math-ops.hpp>

as::vec3i rotatate_point(as::vec3i point, int rotation)
{
  auto [x, y, z] = point;
  switch (rotation) {
    case 0:
      return {x, y, z};
    case 1:
      return {x, -z, y};
    case 2:
      return {x, -y, -z};
    case 3:
      return {x, z, -y};
    case 4:
      return {-x, -y, z};
    case 5:
      return {-x, -z, -y};
    case 6:
      return {-x, y, -z};
    case 7:
      return {-x, z, y};
    case 8:
      return {y, x, -z};
    case 9:
      return {y, -x, z};
    case 10:
      return {y, z, x};
    case 11:
      return {y, -z, -x};
    case 12:
      return {-y, x, z};
    case 13:
      return {-y, -x, -z};
    case 14:
      return {-y, -z, x};
    case 15:
      return {-y, z, -x};
    case 16:
      return {z, x, y};
    case 17:
      return {z, -x, -y};
    case 18:
      return {z, -y, x};
    case 19:
      return {z, y, -x};
    case 20:
      return {-z, x, -y};
    case 21:
      return {-z, -x, y};
    case 22:
      return {-z, y, x};
    case 23:
      return {-z, -y, -x};
    default:
      return {-1, -1, -1};
  }
}

bool operator==(const as::vec3i& lhs, const as::vec3i& rhs)
{
  return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct vec3i_hash
{
  std::size_t operator()(const as::vec3i& vec) const
  {
    std::size_t seed = 0;
    hash_combine(seed, vec.x);
    hash_combine(seed, vec.y);
    hash_combine(seed, vec.z);
    return seed;
  }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");

  std::string ignore;
  std::getline(reader, ignore);

  std::vector<std::vector<std::string>> scanner_strs;
  scanner_strs.emplace_back();
  for (std::string line; std::getline(reader, line);) {
    if (line.empty()) {
      scanner_strs.emplace_back();
      std::getline(reader, ignore);
      continue;
    }
    scanner_strs.back().push_back(std::move(line));
  }

  // check
  for (int i = 0; i < scanner_strs.size(); ++i) {
    const auto& beacon_strs = scanner_strs[i];
    std::cout << "--- scanner " << i << " ---\n";
    for (const auto& beacon_str : beacon_strs) {
      std::cout << beacon_str << '\n';
    }
  }

  std::cout << "---\n";

  struct scanner_t
  {
    std::vector<as::vec3i> beacons;
    bool found = false;
  };

  std::vector<scanner_t> scanners;
  for (const auto& scanner : scanner_strs) {
    scanners.emplace_back();
    for (const auto& beacon_str : scanner) {
      as::vec3i vec;
      as::index i = 0;
      std::istringstream reader(beacon_str);
      for (std::string elem; std::getline(reader, elem, ',');) {
        vec[i++] = std::stoi(elem);
      }
      scanners.back().beacons.push_back(vec);
    }
  }

  int scanner_index = 0;
  for (const auto& scanner : scanners) {
    std::cout << "scanner " << scanner_index++ << '\n';
    for (const auto& beacon : scanner.beacons) {
      std::cout << beacon.x << ',' << beacon.y << ',' << beacon.z << '\n';
    }
    std::cout << '\n';
  }

  int beacons = 0;
  for (const auto& scanner : scanners) {
    beacons += scanner.beacons.size();
  }
  beacons -= (scanners.size() - 1) * 12;

  auto generate_orientated_positions = [](as::vec3i position) {
    auto vec3_from_vec3i = [](const as::vec3i& v) {
      return as::vec3(v.x, v.y, v.z);
    };
    auto vec3i_from_vec3 = [](const as::vec3& v) {
      return as::vec3i(std::lround(v.x), std::lround(v.y), std::lround(v.z));
    };
    std::vector<as::vec3i> orientated_positions;
    const float angles[] = {0.0f, 90.0f, 180.0f, 270.0f};
    for (int z = 0; z < 3; ++z) {
      for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
          auto orientated_position = as::mat_mul(
            vec3_from_vec3i(position),
            as::mat3_rotation_xyz(
              as::radians(angles[x]), as::radians(angles[y]),
              as::radians(angles[z])));
          orientated_positions.push_back(vec3i_from_vec3(orientated_position));
        }
      }
    }
    return orientated_positions;
  };

  struct orientation_state_t
  {
    int x = 0;
    int y = 0;
    int z = 0;
    bool done = false;
    int count = 0;
  };

  std::vector<as::mat3> rotations;
  {
    //    as::mat3 rot1 = as::mat3::identity();
    //    as::mat3 rot2 = as::mat3_rotation_z(as::radians(90.0f));
    //    as::mat3 rot3 = as::mat3_rotation_z(as::radians(180.0f));
    //    as::mat3 rot4 = as::mat3_rotation_z(as::radians(270.0f));
    //
    //    as::mat3 rot5 = as::mat3_rotation_y(as::radians(90.0f));
    //    as::mat3 rot6 = as::mat3_rotation_y(as::radians(90.0f))
    //                  * as::mat3_rotation_z(as::radians(90.0f));
    //    as::mat3 rot7 = as::mat3_rotation_y(as::radians(90.0f))
    //                  * as::mat3_rotation_z(as::radians(180.0f));
    //    as::mat3 rot8 = as::mat3_rotation_y(as::radians(90.0f))
    //                  * as::mat3_rotation_z(as::radians(270.0f));
    //
    //    as::mat3 rot9 = as::mat3_rotation_y(as::radians(180.0f));
    //    as::mat3 rot10 = as::mat3_rotation_y(as::radians(180.0f))
    //                   * as::mat3_rotation_z(as::radians(90.0f));
    //    as::mat3 rot11 = as::mat3_rotation_y(as::radians(180.0f))
    //                   * as::mat3_rotation_z(as::radians(180.0f));
    //    as::mat3 rot12 = as::mat3_rotation_y(as::radians(180.0f))
    //                   * as::mat3_rotation_z(as::radians(270.0f));
    //
    //    as::mat3 rot13 = as::mat3_rotation_y(as::radians(270.0f));
    //    as::mat3 rot14 = as::mat3_rotation_y(as::radians(270.0f))
    //                   * as::mat3_rotation_z(as::radians(90.0f));
    //    as::mat3 rot15 = as::mat3_rotation_y(as::radians(270.0f))
    //                   * as::mat3_rotation_z(as::radians(180.0f));
    //    as::mat3 rot16 = as::mat3_rotation_y(as::radians(270.0f))
    //                   * as::mat3_rotation_z(as::radians(270.0f));
    //
    //    as::mat3 rot17 = as::mat3_rotation_x(as::radians(90.0f));
    //    as::mat3 rot18 = as::mat3_rotation_x(as::radians(90.0f))
    //                   * as::mat3_rotation_z(as::radians(90.0f));
    //    as::mat3 rot19 = as::mat3_rotation_x(as::radians(90.0f))
    //                   * as::mat3_rotation_z(as::radians(180.0f));
    //    as::mat3 rot20 = as::mat3_rotation_x(as::radians(90.0f))
    //                   * as::mat3_rotation_z(as::radians(270.0f));
    //
    //    as::mat3 rot21 = as::mat3_rotation_x(as::radians(-90.0f));
    //    as::mat3 rot22 = as::mat3_rotation_x(as::radians(-90.0f))
    //                   * as::mat3_rotation_z(as::radians(90.0f));
    //    as::mat3 rot23 = as::mat3_rotation_x(as::radians(-90.0f))
    //                   * as::mat3_rotation_z(as::radians(180.0f));
    //    as::mat3 rot24 = as::mat3_rotation_x(as::radians(-90.0f))
    //                   * as::mat3_rotation_z(as::radians(270.0f));
    //
    //    rotations = {rot1,  rot2,  rot3,  rot4,  rot5,  rot6,  rot7,  rot8,
    //                 rot9,  rot10, rot11, rot12, rot13, rot14, rot15, rot16,
    //                 rot17, rot18, rot19, rot20, rot21, rot22, rot23, rot24};
    //  }

    {
      as::mat3 rot1 = as::mat3(1, 0, 0, 0, 1, 0, 0, 0, 1);
      as::mat3 rot2 = as::mat3(1, 0, 0, 0, 0, 1, 0, -1, 0);
      as::mat3 rot3 = as::mat3(1, 0, 0, 0, -1, 0, 0, 0, -1);
      as::mat3 rot4 = as::mat3(1, 0, 0, 0, 0, -1, 0, 1, 0);
      //
      as::mat3 rot5 = as::mat3(0, 1, 0, -1, 0, 0, 0, 0, 1); // pos
      as::mat3 rot6 = as::mat3(0, 1, 0, 0, 0, 1, 1, 0, 0);
      as::mat3 rot7 = as::mat3(0, 1, 0, 1, 0, 0, 0, 0, -1);
      as::mat3 rot8 = as::mat3(0, 1, 0, 0, 0, -1, -1, 0, 0);
      //
      as::mat3 rot9 = as::mat3(-1, 0, 0, 0, -1, 0, 0, 0, 1);
      as::mat3 rot10 = as::mat3(-1, 0, 0, 0, 0, -1, 0, -1, 0);
      as::mat3 rot11 = as::mat3(-1, 0, 0, 0, 1, 0, 0, 0, -1);
      as::mat3 rot12 = as::mat3(-1, 0, 0, 0, 0, 1, 0, 1, 0);

      as::mat3 rot13 = as::mat3(0, -1, 0, 1, 0, 0, 0, 0, 1); // pos
      as::mat3 rot14 = as::mat3(0, -1, 0, 0, 0, -1, 1, 0, 0);
      as::mat3 rot15 = as::mat3(0, -1, 0, -1, 0, 0, 0, 0, -1);
      as::mat3 rot16 = as::mat3(0, -1, 0, 0, 0, 1, -1, 0, 0);

      as::mat3 rot17 = as::mat3(0, 0, 1, 0, 1, 0, -1, 0, 0);
      as::mat3 rot18 = as::mat3(0, 0, 1, 1, 0, 0, 0, 1, 0);
      as::mat3 rot19 = as::mat3(0, 0, 1, 0, -1, 0, 1, 0, 0);
      as::mat3 rot20 = as::mat3(0, 0, 1, -1, 0, 0, 0, -1, 0);

      as::mat3 rot21 = as::mat3(0, 0, -1, 0, -1, 0, -1, 0, 0);
      as::mat3 rot22 = as::mat3(0, 0, -1, -1, 0, 0, 0, 1, 0);
      as::mat3 rot23 = as::mat3(0, 0, -1, 0, 1, 0, 1, 0, 0);
      as::mat3 rot24 = as::mat3(0, 0, -1, 1, 0, 0, 0, -1, 0);

      rotations = {rot1,  rot2,  rot3,  rot4,  rot5,  rot6,  rot7,  rot8,
                   rot9,  rot10, rot11, rot12, rot13, rot14, rot15, rot16,
                   rot17, rot18, rot19, rot20, rot21, rot22, rot23, rot24};

      //    rotations = {rot13};
      //    rotations = {rot5};
    }

    auto generate_orientated_position = [](
                                          orientation_state_t state,
                                          as::vec3i position) {
      auto vec3_from_vec3i = [](const as::vec3i& v) {
        return as::vec3(v.x, v.y, v.z);
      };
      auto vec3i_from_vec3 = [](const as::vec3& v) {
        return as::vec3i(std::lround(v.x), std::lround(v.y), std::lround(v.z));
      };
      auto vec4_from_vec3i = [](const as::vec3i& v) {
        return as::vec4(v.x, v.y, v.z, 1);
      };
      auto vec3i_from_vec4 = [](const as::vec4& v) {
        return as::vec3i(std::lround(v.x), std::lround(v.y), std::lround(v.z));
      };
      const float angles[] = {0.0f, 90.0f, 180.0f, 270.0f};
      return vec3i_from_vec4(as::mat_mul(
        vec4_from_vec3i(position),
        as::mat4_from_mat3(as::mat3_rotation_zxy(
          as::radians(angles[state.x]), as::radians(angles[state.y]),
          as::radians(angles[state.z])))));
    };

    auto vec3_from_vec3i = [](const as::vec3i& v) {
      return as::vec3(v.x, v.y, v.z);
    };
    auto vec3i_from_vec3 = [](const as::vec3& v) {
      return as::vec3i(std::lround(v.x), std::lround(v.y), std::lround(v.z));
    };

    for (const auto r : rotations) {
      auto v =
        vec3i_from_vec3(as::mat_mul(vec3_from_vec3i(as::vec3i(1, 2, 3)), r));
      int i;
      i = 0;
    }

    //  {
    //    orientation_state_t orientation_state;
    //    orientation_state.done = false;
    //    orientation_state.count = 0;
    //    while (!orientation_state.done) {
    //      std::vector<as::vec3i> orientated_beacons_rhs;
    //      for (; orientation_state.y <= 3;) {
    //        for (; orientation_state.x <= 3;) {
    //          for (; orientation_state.z <= 3;) {
    //            orientation_state.count++;
    //            auto v = generate_orientated_position(
    //              orientation_state, as::vec3i(-1, -1, 1));
    //            orientation_state.z++;
    //            goto next3;
    //          }
    //          orientation_state.x++;
    //          orientation_state.z = 0;
    //        }
    //        orientation_state.y++;
    //        orientation_state.x = 0;
    //      }
    //      orientation_state.y = 0;
    //      orientation_state.done = true;
    //    next3:
    //      int test;
    //      //      break;
    //    }
    //  }

    std::unordered_set<as::vec3i, vec3i_hash> all_beacons;
    all_beacons.insert(scanners[0].beacons.begin(), scanners[0].beacons.end());

    std::vector<scanner_t> found_scanners(1, scanners[0]);
    std::vector<scanner_t> remaining_scanners;
    remaining_scanners.insert(
      remaining_scanners.end(), scanners.begin() + 1, scanners.end());

    while (!remaining_scanners.empty()) {
      for (const auto& found_scanner : found_scanners) {
        for (int remaining_scanner_index = 0;
             remaining_scanner_index < remaining_scanners.size();
             ++remaining_scanner_index) {
          const auto& remaining_scanner =
            remaining_scanners[remaining_scanner_index];
          for (int rot = 0; rot < 24; ++rot) {
            std::unordered_map<as::vec3i, int64_t, vec3i_hash> offsets;
            for (const auto& found_beacon : found_scanner.beacons) {
              for (const auto& remaining_beacon : remaining_scanner.beacons) {
                auto rotated_point = rotatate_point(remaining_beacon, rot);
                auto offset = rotated_point - found_beacon;
                offsets[offset]++;
              }
            }
            for (const auto& offset : offsets) {
              if (offset.second >= 12) {
                scanner_t next_found_scanner;
                for (const auto& remaining_beacon : remaining_scanner.beacons) {
                  const auto adjusted_beacon =
                    rotatate_point(remaining_beacon, rot) - offset.first;
                  next_found_scanner.beacons.push_back(adjusted_beacon);
                  all_beacons.insert(adjusted_beacon);
                }
                found_scanners.push_back(next_found_scanner);
                remaining_scanners.erase(remaining_scanners.begin() + remaining_scanner_index);
                goto end;
              }
            }
          }
        }
      }
    end:
      int dummy;
    }

    std::cout << "part 1: " << all_beacons.size() << '\n';

    auto next_scanner = as::vec3i::zero();
    const int overlap_count = 12;

    // start with scanner 0
    // go to first detected beacon
    // move to next scanner, start with first detected beacon

    //        for (auto& scanner : scanners) {
    //          std::sort(
    //            scanner.beacons.begin(), scanner.beacons.end(),
    //            [](const as::vec3i& lhs, const as::vec3i& rhs) {
    //              return lhs.y < rhs.y; // && lhs.y < rhs.y && lhs.z <
    //              rhs.z;
    //            });
    //        }

    for (int s_lhs = 0; s_lhs < scanners.size() - 1; ++s_lhs) {
      for (int s_rhs = s_lhs + 1; s_rhs < scanners.size(); ++s_rhs) {
        const auto& beacons_lhs = scanners[s_lhs].beacons;
        const auto& beacons_rhs = scanners[s_rhs].beacons;

        // calculate distances between all pairs of beacons
        std::vector<as::vec3i> differences_lhs;
        for (int b_lhs = 0; b_lhs < beacons_lhs.size() - 1; ++b_lhs) {
          for (int b_rhs = b_lhs + 1; b_rhs < beacons_lhs.size(); ++b_rhs) {
            differences_lhs.push_back(beacons_lhs[b_rhs] - beacons_lhs[b_lhs]);
          }
        }

        for (int rotation_index = 0; rotation_index < rotations.size();
             rotation_index++) {
          //          const auto rotation = rotations[rotation_index];
          std::vector<as::vec3i> orientated_beacons_rhs;
          orientated_beacons_rhs.reserve(beacons_rhs.size());
          for (auto beacons_rh : beacons_rhs) {
            orientated_beacons_rhs.push_back(
              rotatate_point(beacons_rh, rotation_index));
            //            orientated_beacons_rhs.push_back(vec3i_from_vec3(
            //              as::mat_mul(vec3_from_vec3i(beacons_rh),
            //              rotation)));
          }

          std::vector<as::vec3i> differences_rhs;
          for (int b_lhs = 0; b_lhs < orientated_beacons_rhs.size() - 1;
               ++b_lhs) {
            for (int b_rhs = b_lhs + 1; b_rhs < orientated_beacons_rhs.size();
                 ++b_rhs) {
              differences_rhs.push_back(
                orientated_beacons_rhs[b_rhs] - orientated_beacons_rhs[b_lhs]);
            }
          }

          int count = 0;
          for (const auto diff : differences_lhs) {
            if (auto found = std::find_if(
                  differences_rhs.begin(), differences_rhs.end(),
                  [diff](const as::vec3i d) {
                    return diff.x == d.x && diff.y == d.y && diff.z == d.z;
                  });
                found != differences_rhs.end()) {
              count++;
            }
          }

          if (count == 66) {
            int j;
            j = 0;
            scanners[s_rhs].found = true;
            //            scanners[s_rhs].beacons = orientated_beacons_rhs;
          }

          int i;
          i = 0;
        }
      }
    }

    for (int s_lhs = 0; s_lhs < scanners.size() - 1; ++s_lhs) {
      for (int s_rhs = s_lhs + 1; s_rhs < scanners.size(); ++s_rhs) {
        const auto& beacons_lhs = scanners[s_lhs].beacons;
        const auto& beacons_rhs = scanners[s_rhs].beacons;

        std::vector<float> differences_lhs;
        std::transform(
          beacons_lhs.begin(), beacons_lhs.end() - 1, beacons_lhs.begin() + 1,
          std::back_inserter(differences_lhs),
          [vec3_from_vec3i](const as::vec3i lhs, const as::vec3i rhs) -> float {
            return as::vec_length(vec3_from_vec3i(rhs) - vec3_from_vec3i(lhs));
          });

        for (int rotation_index = 0; rotation_index < rotations.size();
             rotation_index++) {
          const auto rotation = rotations[rotation_index];
          std::vector<as::vec3i> orientated_beacons_rhs;
          orientated_beacons_rhs.reserve(beacons_rhs.size());
          for (auto beacons_rh : beacons_rhs) {
            orientated_beacons_rhs.push_back(vec3i_from_vec3(
              as::mat_mul(vec3_from_vec3i(beacons_rh), rotation)));
          }

          std::sort(
            orientated_beacons_rhs.begin(), orientated_beacons_rhs.end(),
            [](const as::vec3i& lhs, const as::vec3i& rhs) {
              return lhs.y < rhs.y; // && lhs.y < rhs.y && lhs.z < rhs.z;
            });

          std::vector<float> differences_rhs;
          std::transform(
            orientated_beacons_rhs.begin(), orientated_beacons_rhs.end() - 1,
            orientated_beacons_rhs.begin() + 1,
            std::back_inserter(differences_rhs),
            [vec3_from_vec3i](
              const as::vec3i lhs, const as::vec3i rhs) -> float {
              return as::vec_length(
                vec3_from_vec3i(rhs) - vec3_from_vec3i(lhs));
            });

          int matching = 0;
          for (const auto lhs_diff : differences_lhs) {
            if (auto match_it = std::find_if(
                  differences_rhs.begin(), differences_rhs.end(),
                  [lhs_diff](const float diff) {
                    return as::real_near(diff, lhs_diff);
                  });
                match_it != differences_rhs.end()) {
              matching++;
            }
          }

          if (matching == 11) {
            int i;
            i = 0;
          }
          int i;
          i = 0;
        }
      }
    }

    // relative distances from first to all others

    // calculate all relative distances
    {
      struct pos_del
      {
        as::vec3i pos;
        as::vec3i del;
        float distance;
      };

      {
        // calculate delta between all positions for first set
        // calculate deltas for all of second set, keep going until 12 match
        // take first delta and subtract second delta to give position of
        // other scanner
        for (int s_lhs = 0; s_lhs < scanners.size() - 1; ++s_lhs) {
          for (int s_rhs = s_lhs + 1; s_rhs < scanners.size(); ++s_rhs) {
            const auto& beacons_lhs = scanners[s_lhs].beacons;
            const auto& beacons_rhs = scanners[s_rhs].beacons;
            std::vector<pos_del> relative_deltas;
            relative_deltas.reserve(beacons_lhs.size());
            for (int b_lhs = 0; b_lhs < beacons_lhs.size(); ++b_lhs) {
              relative_deltas.push_back(pos_del{
                beacons_lhs[b_lhs], beacons_lhs[b_lhs] - beacons_lhs[0],
                as::vec_distance(
                  vec3_from_vec3i(beacons_lhs[b_lhs]),
                  vec3_from_vec3i(beacons_lhs[0]))});
            }
            for (int rotation_index = 0; rotation_index < rotations.size();
                 rotation_index++) {
              const auto rotation = rotations[rotation_index];
              std::vector<as::vec3i> orientated_beacons_rhs;
              orientated_beacons_rhs.reserve(beacons_rhs.size());
              for (auto beacons_rh : beacons_rhs) {
                orientated_beacons_rhs.push_back(vec3i_from_vec3(
                  as::mat_mul(vec3_from_vec3i(beacons_rh), rotation)));
              }

              int chosen = 0;
              while (chosen < orientated_beacons_rhs.size()) {
                std::vector<pos_del> next_relative_deltas;
                next_relative_deltas.reserve(orientated_beacons_rhs.size());

                auto first = orientated_beacons_rhs[chosen];
                for (auto& b_rhs : orientated_beacons_rhs) {
                  next_relative_deltas.push_back(pos_del{
                    b_rhs, b_rhs - first,
                    as::vec_distance(
                      vec3_from_vec3i(b_rhs), vec3_from_vec3i(first))});
                }

                std::vector<as::vec3i> overlapping;
                as::vec3i position_relative_to_previous_scanner;

                auto count = 0;
                for (const auto rel_del : relative_deltas) {
                  auto f = std::find_if(
                    next_relative_deltas.begin(), next_relative_deltas.end(),
                    [rel_del](const auto& rd) {
                      //                    return as::real_near(rd.distance,
                      //                    rel_del.distance);
                      return rd.del.x == rel_del.del.x
                          && rd.del.y == rel_del.del.y
                          && rd.del.z == rel_del.del.z;
                    });
                  if (f != next_relative_deltas.end()) {
                    overlapping.push_back(rel_del.pos);
                    position_relative_to_previous_scanner =
                      rel_del.pos - f->pos;
                    count++;
                  }
                }

                if (count == 12) {
                  scanners[s_rhs].beacons = orientated_beacons_rhs;
                  int a;
                  a = 0;
                }

                chosen++;
              }
            }
          }
        }

        //    orientation_state_t orientation_state;
        for (int s_lhs = 0; s_lhs < scanners.size() - 1; ++s_lhs) {
          for (int s_rhs = 0 /*s_lhs + 1*/; s_rhs < scanners.size(); ++s_rhs) {
            if (s_lhs == s_rhs) {
              continue;
            }
            const auto& scanner_lhs = scanners[s_lhs];
            const auto& scanner_rhs = scanners[s_rhs];
            const auto& beacons_lhs = scanner_lhs.beacons;
            const auto& beacons_rhs = scanner_rhs.beacons;
            std::vector<pos_del> relative_deltas;
            relative_deltas.reserve(beacons_lhs.size());
            for (int b_lhs = 0; b_lhs < beacons_lhs.size(); ++b_lhs) {
              relative_deltas.push_back(
                {beacons_lhs[b_lhs], beacons_lhs[b_lhs] - beacons_lhs[0]});
            }
            for (int rotation_index = 0; rotation_index < rotations.size();
                 rotation_index++) {
              std::vector<as::vec3i> orientated_beacons_rhs;
              orientated_beacons_rhs.reserve(beacons_rhs.size());
              std::vector<as::vec3i> coordinates;
              for (auto beacons_rh : beacons_rhs) {
                orientated_beacons_rhs.push_back(vec3i_from_vec3(as::mat_mul(
                  vec3_from_vec3i(beacons_rh), rotations[rotation_index])));
              }
              //        orientation_state.done = false;
              //        orientation_state.count = 0;
              //        while (!orientation_state.done) {
              //          std::vector<as::vec3i> orientated_beacons_rhs;
              //          for (; orientation_state.y <= 3;) {
              //            for (; orientation_state.x <= 3;) {
              //              for (; orientation_state.z <= 3;) {
              //                orientation_state.count++;
              //                for (int b_rhs = 0; b_rhs <
              //                beacons_rhs.size();
              //                ++b_rhs) {
              //                  orientated_beacons_rhs.push_back(generate_orientated_position(
              //                    orientation_state, beacons_rhs[b_rhs]));
              //                }
              //                orientation_state.z++;
              //                goto next;
              //              }
              //              orientation_state.x++;
              //              orientation_state.z = 0;
              //            }
              //            orientation_state.y++;
              //            orientation_state.x = 0;
              //          }
              //          orientation_state.y = 0;
              //          orientation_state.done = true;
              //          break;
              //        next:
              for (int b_rhs = 0; b_rhs < orientated_beacons_rhs.size();
                   ++b_rhs) {
                int found_count = 0;
                for (const auto& relative_delta : relative_deltas) {
                  auto next_position =
                    orientated_beacons_rhs[b_rhs] + relative_delta.del;
                  auto found = std::find_if(
                    orientated_beacons_rhs.begin(),
                    orientated_beacons_rhs.end(),
                    [next_position](const auto& beacon) {
                      return next_position.x == beacon.x
                          && next_position.y == beacon.y
                          && next_position.z == beacon.z;
                    });
                  if (found != orientated_beacons_rhs.end()) {
                    coordinates.push_back(relative_delta.pos);
                    auto s0 =
                      relative_delta.pos - orientated_beacons_rhs[b_rhs];
                    found_count++;
                  }
                }
                if (found_count == 12) {
                  //              auto s0 = coordinates.back()
                  int i;
                  i = 0;
                  //            orientation_state.done = true;
                  break;
                }
                //            rotation_index++;
              }
            }
          }
        }
      }
      //  }

      {
        orientation_state_t orientation_state;
        for (int s_lhs = 0; s_lhs < scanners.size() - 1; ++s_lhs) {
          for (int s_rhs = s_lhs + 1; s_rhs < scanners.size(); ++s_rhs) {
            const auto& scanner_lhs = scanners[s_lhs];
            const auto& scanner_rhs = scanners[s_rhs];
            const auto& beacons_lhs = scanner_lhs.beacons;
            const auto& beacons_rhs = scanner_rhs.beacons;
            for (int b_lhs = 0; b_lhs < beacons_lhs.size(); ++b_lhs) {
              orientation_state.done = false;
              orientation_state.count = 0;
              while (!orientation_state.done) {
                std::vector<as::vec3i> orientated_beacons_rhs;
                for (; orientation_state.y < 3;) {
                  for (; orientation_state.x < 3;) {
                    for (; orientation_state.z < 3;) {
                      orientation_state.count++;
                      for (int b_rhs = 0; b_rhs < beacons_rhs.size(); ++b_rhs) {
                        orientated_beacons_rhs.push_back(
                          generate_orientated_position(
                            orientation_state, beacons_rhs[b_rhs]));
                      }
                      orientation_state.z++;
                      goto next2;
                    }
                    orientation_state.x++;
                    orientation_state.z = 0;
                  }
                  orientation_state.y++;
                  orientation_state.x = 0;
                }
                orientation_state.y = 0;
                orientation_state.done = true;
                break;
              next2:
                int found = 0;
                for (int b_rhs = 0; b_rhs < orientated_beacons_rhs.size();
                     ++b_rhs) {
                  auto beacon_from_scanner_lhs = beacons_lhs[b_lhs];
                  auto beacon_from_scanner_rhs = orientated_beacons_rhs[b_rhs];
                  auto relative =
                    beacon_from_scanner_lhs - beacon_from_scanner_rhs;
                  auto other = relative + beacons_lhs[b_lhs];
                  auto f = std::find_if(
                    beacons_lhs.begin(), beacons_lhs.end(),
                    [other](const auto& b) {
                      return b.x == other.x && b.y == other.y && b.z == other.z;
                    });
                  if (f != beacons_lhs.end()) {
                    found++;
                  } else {
                    // no match
                    // break;
                  }
                }
                if (found == 12) {
                  int i;
                  i = 0;
                }
              }
            }
          }
        }
      }

      // need two loops
      for (int s = 0; s < scanners.size() - 1; ++s) {
        const auto& scanner_lhs = scanners[s];
        const auto& scanner_rhs = scanners[s + 1];

        // potential position
        const auto& beacons_lhs = scanner_lhs.beacons;
        const auto& beacons_rhs = scanner_rhs.beacons;
        int potential_position;
        for (int bl = 0; bl < beacons_lhs.size(); ++bl) {
          potential_position = 0;
          while (potential_position < beacons_rhs.size()) {
            auto beacon_from_scanner_lhs = beacons_lhs[bl];
            auto beacon_from_scanner_rhs_options =
              generate_orientated_positions(beacons_rhs[potential_position]);
            //        auto beacon_from_scanner_rhs =
            //        beacons_rhs[potential_position];
            for (const auto beacon_from_scanner_rhs :
                 beacon_from_scanner_rhs_options) {
              auto scanner_rhs_position =
                beacon_from_scanner_lhs - beacon_from_scanner_rhs;
              int found = 0;
              for (int br = 0; br < beacons_rhs.size(); ++br) {
                auto internal_beacon_from_scanner_rhs_options =
                  generate_orientated_positions(beacons_rhs[br]);
                for (const auto internal :
                     internal_beacon_from_scanner_rhs_options) {
                  auto r = scanner_rhs_position + internal /*beacons_rhs[br]*/;
                  auto f = std::find_if(
                    beacons_lhs.begin(), beacons_lhs.end(), [r](const auto& b) {
                      return b.x == r.x && b.y == r.y && b.z == r.z;
                    });
                  if (f != beacons_lhs.end()) {
                    found++;
                  } else {
                    // no match
                    break;
                  }
                }
              }
              if (found == overlap_count) {
                next_scanner = scanner_rhs_position;
                found = 0;
              }
            }
            potential_position++;
          }
        }
      }
    }
  }
}