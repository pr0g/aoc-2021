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

  for (const auto& scanner : scanners) {
    for (const auto& beacon : scanner.beacons) {
      std::cout << beacon.x << ',' << beacon.y << ',' << beacon.z << '\n';
    }
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
          auto orientated_position =
            as::mat3_rotation_xyz(
              as::radians(angles[x]), as::radians(angles[y]),
              as::radians(angles[z]))
            * vec3_from_vec3i(position);
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

  as::mat3 rot1 = as::mat3::identity();
  as::mat3 rot2 = as::mat3_rotation_z(as::radians(90.0f));
  as::mat3 rot3 = as::mat3_rotation_z(as::radians(180.0f));
  as::mat3 rot4 = as::mat3_rotation_z(as::radians(270.0f));

  as::mat3 rot5 = as::mat3_rotation_y(as::radians(90.0f));
  as::mat3 rot6 = as::mat3_rotation_y(as::radians(90.0f))
                * as::mat3_rotation_z(as::radians(90.0f));
  as::mat3 rot7 = as::mat3_rotation_y(as::radians(90.0f))
                * as::mat3_rotation_z(as::radians(180.0f));
  as::mat3 rot8 = as::mat3_rotation_y(as::radians(90.0f))
                * as::mat3_rotation_z(as::radians(270.0f));

  as::mat3 rot9 = as::mat3_rotation_y(as::radians(180.0f));
  as::mat3 rot10 = as::mat3_rotation_y(as::radians(180.0f))
                 * as::mat3_rotation_z(as::radians(90.0f));
  as::mat3 rot11 = as::mat3_rotation_y(as::radians(180.0f))
                 * as::mat3_rotation_z(as::radians(180.0f));
  as::mat3 rot12 = as::mat3_rotation_y(as::radians(180.0f))
                 * as::mat3_rotation_z(as::radians(270.0f));

  as::mat3 rot13 = as::mat3_rotation_y(as::radians(270.0f));
  as::mat3 rot14 = as::mat3_rotation_y(as::radians(270.0f))
                 * as::mat3_rotation_z(as::radians(90.0f));
  as::mat3 rot15 = as::mat3_rotation_y(as::radians(270.0f))
                 * as::mat3_rotation_z(as::radians(180.0f));
  as::mat3 rot16 = as::mat3_rotation_y(as::radians(270.0f))
                 * as::mat3_rotation_z(as::radians(270.0f));

  as::mat3 rot17 = as::mat3_rotation_x(as::radians(90.0f));
  as::mat3 rot18 = as::mat3_rotation_x(as::radians(90.0f))
                 * as::mat3_rotation_z(as::radians(90.0f));
  as::mat3 rot19 = as::mat3_rotation_x(as::radians(90.0f))
                 * as::mat3_rotation_z(as::radians(180.0f));
  as::mat3 rot20 = as::mat3_rotation_x(as::radians(90.0f))
                 * as::mat3_rotation_z(as::radians(270.0f));

  as::mat3 rot21 = as::mat3_rotation_x(as::radians(-90.0f));
  as::mat3 rot22 = as::mat3_rotation_x(as::radians(-90.0f))
                 * as::mat3_rotation_z(as::radians(90.0f));
  as::mat3 rot23 = as::mat3_rotation_x(as::radians(-90.0f))
                 * as::mat3_rotation_z(as::radians(180.0f));
  as::mat3 rot24 = as::mat3_rotation_x(as::radians(-90.0f))
                 * as::mat3_rotation_z(as::radians(270.0f));

  std::vector<as::mat3> rotations = {rot1,  rot2,  rot3,  rot4,  rot5,  rot6,
                                     rot7,  rot8,  rot9,  rot10, rot11, rot12,
                                     rot13, rot14, rot15, rot16, rot17, rot18,
                                     rot19, rot20, rot21, rot22, rot23, rot24};

  auto generate_orientated_position =
    [](orientation_state_t state, as::vec3i position) {
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
      return vec3i_from_vec4(
        as::mat4_from_mat3(as::mat3_rotation_zxy(
          as::radians(angles[state.x]), as::radians(angles[state.y]),
          as::radians(angles[state.z])))
        * vec4_from_vec3i(position));
    };

  auto vec3_from_vec3i = [](const as::vec3i& v) {
    return as::vec3(v.x, v.y, v.z);
  };
  auto vec3i_from_vec3 = [](const as::vec3& v) {
    return as::vec3i(std::lround(v.x), std::lround(v.y), std::lround(v.z));
  };

  for (const auto r : rotations) {
    auto v = vec3i_from_vec3(r * vec3_from_vec3i(as::vec3i(1, 2, 3)));
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

  auto next_scanner = as::vec3i::zero();
  const int overlap_count = 12;

  // start with scanner 0
  // go to first detected beacon
  // move to next scanner, start with first detected beacon

  // relative distances from first to all others

  // calculate all relative distances
  {
    struct pos_del
    {
      as::vec3i pos;
      as::vec3i del;
    };

    {
      // calculate delta between all positions for first set
      // calculate deltas for all of second set, keep going until 12 match
      // take first delta and subtract second delta to give position of other
      // scanner
      for (int s_lhs = 0; s_lhs < scanners.size() - 1; ++s_lhs) {
        for (int s_rhs = s_lhs + 1; s_rhs < scanners.size(); ++s_rhs) {
//          std::cout << "lhs - " << s_lhs << "rhs - " << s_rhs << '\n';
//          if (s_lhs == s_rhs) {
//            continue;
//          }
          const auto& beacons_lhs = scanners[s_lhs].beacons;
          const auto& beacons_rhs = scanners[s_rhs].beacons;
          std::vector<pos_del> relative_deltas;
          relative_deltas.reserve(beacons_lhs.size());
          for (int b_lhs = 0; b_lhs < beacons_lhs.size(); ++b_lhs) {
            relative_deltas.push_back(
              {beacons_lhs[b_lhs], beacons_lhs[b_lhs] - beacons_lhs[0]});
          }
          for (int rotation_index = 0; rotation_index < rotations.size();
               rotation_index++) {
            const auto rotation = rotations[rotation_index];
            std::vector<as::vec3i> orientated_beacons_rhs;
            orientated_beacons_rhs.reserve(beacons_rhs.size());
            for (auto beacons_rh : beacons_rhs) {
              orientated_beacons_rhs.push_back(
                vec3i_from_vec3(rotation * vec3_from_vec3i(beacons_rh)));
            }

            int chosen = 0;
            while (chosen < orientated_beacons_rhs.size()) {
              std::vector<pos_del> next_relative_deltas;
              next_relative_deltas.reserve(orientated_beacons_rhs.size());

              auto first = orientated_beacons_rhs[chosen];
              for (auto& b_rhs : orientated_beacons_rhs) {
                next_relative_deltas.push_back({b_rhs, b_rhs - first});
              }

              std::vector<as::vec3i> overlapping;
              as::vec3i position_relative_to_previous_scanner;

              auto count = 0;
              for (const auto rel_del : relative_deltas) {
                auto f = std::find_if(
                  next_relative_deltas.begin(), next_relative_deltas.end(),
                  [rel_del](const auto& rd) {
                    return rd.del.x == rel_del.del.x
                        && rd.del.y == rel_del.del.y
                        && rd.del.z == rel_del.del.z;
                  });
                if (count > 2) {
                  int k;
                  k = 0;
                }
                if (f != next_relative_deltas.end()) {
                  overlapping.push_back(rel_del.pos);
                  position_relative_to_previous_scanner = rel_del.pos - f->pos;
                  count++;
                }
              }

              if (count >= 12) {
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
              orientated_beacons_rhs.push_back(vec3i_from_vec3(
                rotations[rotation_index] * vec3_from_vec3i(beacons_rh)));
            }
            //        orientation_state.done = false;
            //        orientation_state.count = 0;
            //        while (!orientation_state.done) {
            //          std::vector<as::vec3i> orientated_beacons_rhs;
            //          for (; orientation_state.y <= 3;) {
            //            for (; orientation_state.x <= 3;) {
            //              for (; orientation_state.z <= 3;) {
            //                orientation_state.count++;
            //                for (int b_rhs = 0; b_rhs < beacons_rhs.size();
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
                  orientated_beacons_rhs.begin(), orientated_beacons_rhs.end(),
                  [next_position](const auto& beacon) {
                    return next_position.x == beacon.x
                        && next_position.y == beacon.y
                        && next_position.z == beacon.z;
                  });
                if (found != orientated_beacons_rhs.end()) {
                  coordinates.push_back(relative_delta.pos);
                  auto s0 = relative_delta.pos - orientated_beacons_rhs[b_rhs];
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
              if (found == 3) {
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