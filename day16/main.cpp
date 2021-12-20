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
  auto packet_size = packet.size();
  int64_t offset = 0;
  for (; offset < packet_size;) {
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
      int skip = bin_to_dec(packet.substr(offset, packet_size - offset));
      if (skip == 0) {
        offset += packet_size - offset;
      }
      break;
    } else { // packet
      auto length_type_id = packet[offset];
      offset += 1;
      if (length_type_id == '0') {
        auto total_length_in_bits_str = packet.substr(offset, 15);
        auto total_length_in_bits = bin_to_dec(total_length_in_bits_str);
        auto end = offset + total_length_in_bits;
        offset += 15;
        while (offset < end) {
        auto remainder = process(packet.substr(offset, total_length_in_bits), packet_versions);
        offset += remainder;
        }
        break; // maybe
      } else if (length_type_id == '1') {
        auto number_of_sub_packets_str = packet.substr(offset, 11);
        auto number_of_sub_packets = bin_to_dec(number_of_sub_packets_str);
        offset += 11;
        for (int i = 0; i < number_of_sub_packets; ++i) {
          offset += process(packet.substr(offset, 10000), packet_versions);
        }
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

  //std::cout << bin_to_dec(std::string("101001010101011111")) << '\n';

  // std::string packet = hex_to_bin("D2FE28");
  // std::string packet = hex_to_bin("38006F45291200");
  // std::string packet = hex_to_bin("EE00D40C823060");
  // std::string packet = hex_to_bin("8A004A801A8002F478");
  // std::string packet = hex_to_bin("620080001611562C8802118E34");
  // std::string packet = hex_to_bin("C0015000016115A2E0802F182340");
  // std::string packet = hex_to_bin("A0016C880162017C3686B18A3D4780");
 std::string packet = hex_to_bin("220D6448300428021F9EFE668D3F5FD6025165C00C602FC980B45002A40400B402548808A310028400C001B5CC00B10029C0096011C0003C55003C0028270025400C1002E4F19099F7600142C801098CD0761290021B19627C1D3007E33C4A8A640143CE85CB9D49144C134927100823275CC28D9C01234BD21F8144A6F90D1B2804F39B972B13D9D60939384FE29BA3B8803535E8DF04F33BC4AFCAFC9E4EE32600C4E2F4896CE079802D4012148DF5ACB9C8DF5ACB9CD821007874014B4ECE1A8FEF9D1BCC72A293A0E801C7C9CA36A5A9D6396F8FCC52D18E91E77DD9EB16649AA9EC9DA4F4600ACE7F90DFA30BA160066A200FC448EB05C401B8291F22A2002051D247856600949C3C73A009C8F0CA7FBCCF77F88B0000B905A3C1802B3F7990E8029375AC7DDE2DCA20C2C1004E4BE9F392D0E90073D31634C0090667FF8D9E667FF8D9F0C01693F8FE8024000844688FF0900010D8EB0923A9802903F80357100663DC2987C0008744F8B5138803739EB67223C00E4CC74BA46B0AD42C001DE8392C0B0DE4E8F660095006AA200EC198671A00010E87F08E184FCD7840289C1995749197295AC265B2BFC76811381880193C8EE36C324F95CA69C26D92364B66779D63EA071008C360098002191A637C7310062224108C3263A600A49334C19100A1A000864728BF0980010E8571EE188803D19A294477008A595A53BC841526BE313D6F88CE7E16A7AC60401A9E80273728D2CC53728D2CCD2AA2600A466A007CE680E5E79EFEB07360041A6B20D0F4C021982C966D9810993B9E9F3B1C7970C00B9577300526F52FCAB3DF87EC01296AFBC1F3BC9A6200109309240156CC41B38015796EABCB7540804B7C00B926BD6AC36B1338C4717E7D7A76378C85D8043F947C966593FD2BBBCB27710E57FDF6A686E00EC229B4C9247300528029393EC3BAA32C9F61DD51925AD9AB2B001F72B2EE464C0139580D680232FA129668");

  auto packet_size = packet.size();
  std::vector<int64_t> packet_versions;
  auto last = process(binary_rep, packet_versions);

  auto sum = std::accumulate(
    packet_versions.begin(), packet_versions.end(), 0,
    [](int64_t acc, int64_t v) {
      acc += v;
      return acc;
    });

  std::cout << "part 1: " << sum << '\n';
}
