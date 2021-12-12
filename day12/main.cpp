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

struct node_t
{
  [[nodiscard]] bool large_cave() const
  {
    return std::all_of(
      name.begin(), name.end(), [](const auto a) { return std::isupper(a); });
  }
  std::string name;
  std::vector<node_t*> connections;
};

void recurse(
  const node_t* node, std::vector<std::string> path,
  std::vector<std::vector<std::string>>& paths,
  std::vector<std::string>& visited)
{
  for (const auto* connection : node->connections) {
    if (
      std::find(path.begin(), path.end(), connection->name) != path.end()
      && !connection->large_cave()) {
      continue;
    }

    path.push_back(connection->name);

    if (connection->name == "end") {
      paths.push_back(path);
      // for (const auto& n : path) {
      //   std::cout << n << "->";
      // }
      // std::cout << '\n';
    } else {
      recurse(connection, path, paths, visited);
    }
    path.pop_back();
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> connections;
  for (std::string line; std::getline(reader, line);) {
    connections.push_back(line);
  }

  struct connection_pair_t
  {
    std::string begin;
    std::string end;
  };

  std::vector<connection_pair_t> connection_pairs;
  for (const auto& connection : connections) {
    std::istringstream connection_stream(connection);
    std::vector<std::string> connection_pair_strs;
    for (std::string line; std::getline(connection_stream, line, '-');) {
      connection_pair_strs.push_back(line);
    }
    connection_pair_t connection_pair;
    connection_pair.begin = connection_pair_strs[0];
    connection_pair.end = connection_pair_strs[1];
    connection_pairs.push_back(connection_pair);
  }

  std::vector<node_t> nodes;
  nodes.reserve(connection_pairs.size() * 2);
  for (const auto& cp : connection_pairs) {
    // std::cout << cp.begin << " - " << cp.end << '\n';
    auto begin_it =
      std::find_if(nodes.begin(), nodes.end(), [cp](const auto node) {
        return node.name == cp.begin;
      });
    auto end_it =
      std::find_if(nodes.begin(), nodes.end(), [cp](const auto node) {
        return node.name == cp.end;
      });
    if (begin_it != nodes.end() && end_it != nodes.end()) {
      begin_it->connections.push_back(&*end_it);
      end_it->connections.push_back(&*begin_it);
    } else if (begin_it != nodes.end() && end_it == nodes.end()) {
      nodes.push_back({cp.end, {}});
      nodes.back().connections.push_back(&*begin_it);
      begin_it->connections.push_back(&nodes.back());
    } else if (begin_it == nodes.end() && end_it != nodes.end()) {
      nodes.push_back({cp.begin, {}});
      nodes.back().connections.push_back(&*end_it);
      end_it->connections.push_back(&nodes.back());
    } else if (begin_it == nodes.end() && end_it == nodes.end()) {
      nodes.push_back({cp.begin, {}});
      auto& begin = nodes.back();
      nodes.push_back({cp.end, {}});
      auto& end = nodes.back();
      begin.connections.push_back(&end);
      end.connections.push_back(&begin);
    }
  }

  // explore
  auto start = std::find_if(nodes.begin(), nodes.end(), [](const auto& node) {
    return node.name == "start";
  });

  std::vector<std::string> visited;
  std::vector<std::string> path;
  std::vector<std::vector<std::string>> paths;
  path.push_back(start->name);
  visited.push_back(start->name);

  recurse(&*start, path, paths, visited);

  std::cout << "part 1: " << paths.size() << '\n';

  return 0;
}
