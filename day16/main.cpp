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

int64_t bin_to_dec(const std::string& bin)
{
  int64_t binary = 0;
  int64_t base_2 = 1;
  for (auto digit_it = bin.rbegin(); digit_it != bin.rend(); ++digit_it) {
    if (*digit_it == '1') {
      binary += base_2;
    }
    base_2 <<= 1;
  }
  return binary;
}

int64_t process(std::string packet, std::vector<int64_t>& packet_versions)
{
  int64_t offset = 0;
  auto packet_version_str = packet.substr(offset, 3);
  auto packet_version = bin_to_dec(packet_version_str);
  packet_versions.push_back(packet_version);
  offset += 3;
  auto packet_type_id_str = packet.substr(offset, 3);
  auto packet_type_id = bin_to_dec(packet_type_id_str);
  offset += 3;
  // literal
  if (packet_type_id == 4) {
    auto number_str = std::string();
    while (true) {
      bool last = packet[offset] == '0';
      offset += 1;
      auto number_part_str = packet.substr(offset, 4);
      number_str += number_part_str;
      offset += 4;
      if (last) {
        break;
      }
    }
    int literal_value = bin_to_dec(number_str);
    int j;
    j = 0;
  } else { // packet
    auto length_type_id = packet[offset];
    offset += 1;
    if (length_type_id == '0') {
      auto total_length_in_bits_str = packet.substr(offset, 15);
      auto total_length_in_bits = bin_to_dec(total_length_in_bits_str);
      offset += 15;
      auto end = offset + total_length_in_bits;
      while (offset < end) {
        auto remainder =
          process(packet.substr(offset, total_length_in_bits), packet_versions);
        offset += remainder;
      }
    } else if (length_type_id == '1') {
      auto number_of_sub_packets_str = packet.substr(offset, 11);
      auto number_of_sub_packets = bin_to_dec(number_of_sub_packets_str);
      offset += 11;
      for (int i = 0; i < number_of_sub_packets; ++i) {
        offset += process(packet.substr(offset, 10000), packet_versions);
      }
    }
  }
  return offset;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::string message;
  std::getline(reader, message);

  // check
  // std::cout << message << '\n';
  std::string binary_rep = hex_to_bin(message);

  // check
  // std::cout << binary_rep << '\n';

  // first 3 bits, packet version
  // next 3 bits, packet type id
  // type id 4 -> literal value (single number), follow packet header
  // 1 bit, keep reading, 0 bit, end
  // type id not 4, operator
  // bit after header, length type id
  // if 0, next 15 bits are number that represents total length in bits of the
  // sub packets contained by this packet
  // if 1, next 11 bits are number that represents number of sub packets
  // immediately contained

  // std::cout << bin_to_dec(std::string("101001010101011111")) << '\n';

  // std::string packet = hex_to_bin("D2FE28");
  // std::string packet = hex_to_bin("38006F45291200");
  // std::string packet = hex_to_bin("EE00D40C823060");
  // std::string packet = hex_to_bin("8A004A801A8002F478");
  // std::string packet = hex_to_bin("C0015000016115A2E0802F182340");
  // std::string packet = hex_to_bin("A0016C880162017C3686B18A3D4780");
  // std::string packet = hex_to_bin("620080001611562C8802118E34");
  std::string packet = hex_to_bin(message);

  auto packet_size = packet.size();
  std::vector<int64_t> packet_versions;
  auto last = process(packet, packet_versions);

  auto sum = std::accumulate(
    packet_versions.begin(), packet_versions.end(), 0,
    [](int64_t acc, int64_t v) {
      acc += v;
      return acc;
    });

  std::cout << "part 1: " << sum << '\n';
}
