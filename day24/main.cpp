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

namespace alu
{

enum class register_e
{
  w,
  x,
  y,
  z
};

struct variable_t
{
  register_e reg;
};

struct literal_t
{
  int64_t number;
};

struct inp_t
{
  int64_t number;
  register_e reg;
};

struct add_t
{
  register_e reg;
  std::variant<literal_t, variable_t> operand;
};

struct mul_t
{
  register_e reg;
  std::variant<literal_t, variable_t> operand;
};

struct mod_t
{
  register_e reg;
  std::variant<literal_t, variable_t> operand;
};

struct div_t
{
  register_e reg;
  std::variant<literal_t, variable_t> operand;
};

struct eql_t
{
  register_e reg;
  std::variant<literal_t, variable_t> operand;
};

using op_t = std::variant<inp_t, add_t, mul_t, div_t, mod_t, eql_t>;

} // namespace alu

alu::register_e str_to_reg(const std::string& reg_str)
{
  if (reg_str == "w") {
    return alu::register_e::w;
  }
  if (reg_str == "x") {
    return alu::register_e::x;
  }
  if (reg_str == "y") {
    return alu::register_e::y;
  }
  if (reg_str == "z") {
    return alu::register_e::z;
  }

  std::cout << "error\n";
  return alu::register_e::x;
}

std::variant<alu::literal_t, alu::variable_t> str_to_operand(
  const std::string& operand)
{
  if (operand == "w" || operand == "x" || operand == "y" || operand == "z") {
    alu::variable_t variable{};
    variable.reg = str_to_reg(operand);
    return variable;
  } else {
    alu::literal_t literal{};
    literal.number = stoi(operand);
    return literal;
  }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
  std::ifstream reader("input.txt");
  std::vector<std::string> lines;
  for (std::string line; std::getline(reader, line);) {
    lines.push_back(std::move(line));
  }

  // check
  // for (const std::string& str : lines) {
  //   std::cout << str << '\n';
  // }

  // 96

  // read all chunks in (batches of 18)
  // split in increase vs decrease pairs
  // start with increase, find z = 0 with highest number pair

  // could only match with one to the right?

  /*for (int left = 1; left < 10; left++)*/ {
    /*for (int right = 1; right < 10; right++)*/ {
      // auto global_input = std::to_string(13579246899999);
//       auto global_input = std::to_string(left) + std::to_string(right);

//      auto global_input = std::to_string(65984919997939); // part 1
      auto global_input = std::to_string(11211619541713); // part 2

      const auto start_global_input = global_input;

      // auto global_input = std::to_string(79999999999999);
      // auto global_input = std::to_string(11111111111111);

      std::vector<alu::op_t> ops;
      for (const std::string& str : lines) {
        auto space = str.find_first_of(' ');
        auto instruction = str.substr(0, space);
        if (instruction == "inp") {
          auto reg_str = str.substr(space + 1, str.size());
          alu::inp_t inp{};
          inp.number = global_input.front() - '0';
          inp.reg = str_to_reg(reg_str);
          global_input.erase(global_input.begin());
          ops.emplace_back(inp);
        } else if (instruction == "add") {
          auto reg_str = str.substr(space + 1, 1);
          auto last_space = str.find_last_of(' ');
          auto operand_str = str.substr(last_space + 1, str.size());
          alu::add_t add{};
          add.reg = str_to_reg(reg_str);
          add.operand = str_to_operand(operand_str);
          ops.emplace_back(add);
        } else if (instruction == "mul") {
          auto reg_str = str.substr(space + 1, 1);
          auto last_space = str.find_last_of(' ');
          auto operand_str = str.substr(last_space + 1, str.size());
          alu::mul_t mul{};
          mul.reg = str_to_reg(reg_str);
          mul.operand = str_to_operand(operand_str);
          ops.emplace_back(mul);
        } else if (instruction == "div") {
          auto reg_str = str.substr(space + 1, 1);
          auto last_space = str.find_last_of(' ');
          auto operand_str = str.substr(last_space + 1, str.size());
          alu::div_t div{};
          div.reg = str_to_reg(reg_str);
          div.operand = str_to_operand(operand_str);
          ops.emplace_back(div);
        } else if (instruction == "mod") {
          auto reg_str = str.substr(space + 1, 1);
          auto last_space = str.find_last_of(' ');
          auto operand_str = str.substr(last_space + 1, str.size());
          alu::mod_t mod{};
          mod.reg = str_to_reg(reg_str);
          mod.operand = str_to_operand(operand_str);
          ops.emplace_back(mod);
        } else if (instruction == "eql") {
          auto reg_str = str.substr(space + 1, 1);
          auto last_space = str.find_last_of(' ');
          auto operand_str = str.substr(last_space + 1, str.size());
          alu::eql_t eql{};
          eql.reg = str_to_reg(reg_str);
          eql.operand = str_to_operand(operand_str);
          ops.emplace_back(eql);
        }
      }

      int64_t w = 0;
      int64_t x = 0;
      int64_t y = 0;
      int64_t z = 0;

      auto write_to_register = [&w, &x, &y,
                                &z](const alu::register_e reg, int64_t number) {
        switch (reg) {
          case alu::register_e::w:
            w = number;
            break;
          case alu::register_e::x:
            x = number;
            break;
          case alu::register_e::y:
            y = number;
            break;
          case alu::register_e::z:
            z = number;
            break;
        }
      };

      auto read_from_register = [&w, &x, &y, &z](const alu::register_e reg) {
        switch (reg) {
          case alu::register_e::w:
            return w;
          case alu::register_e::x:
            return x;
          case alu::register_e::y:
            return y;
          case alu::register_e::z:
            return z;
        }
        return (int64_t)0;
      };

      for (const auto op : ops) {
        if (const auto inp = std::get_if<alu::inp_t>(&op)) {
          write_to_register(inp->reg, inp->number);
        } else if (const auto mul = std::get_if<alu::mul_t>(&op)) {
          auto lhs = read_from_register(mul->reg);
          int64_t rhs = 0;
          if (const auto lit = std::get_if<alu::literal_t>(&mul->operand)) {
            rhs = lit->number;
          } else if (
            const auto var = std::get_if<alu::variable_t>(&mul->operand)) {
            rhs = read_from_register(var->reg);
          }
          auto result = lhs * rhs;
          write_to_register(mul->reg, result);
        } else if (const auto eql = std::get_if<alu::eql_t>(&op)) {
          auto lhs = read_from_register(eql->reg);
          int64_t rhs = 0;
          if (const auto lit = std::get_if<alu::literal_t>(&eql->operand)) {
            rhs = lit->number;
          } else if (
            const auto var = std::get_if<alu::variable_t>(&eql->operand)) {
            rhs = read_from_register(var->reg);
          }
          if (lhs == rhs) {
            write_to_register(eql->reg, 1);
          } else {
            write_to_register(eql->reg, 0);
          }
        } else if (const auto add = std::get_if<alu::add_t>(&op)) {
          auto lhs = read_from_register(add->reg);
          int64_t rhs = 0;
          if (const auto lit = std::get_if<alu::literal_t>(&add->operand)) {
            rhs = lit->number;
          } else if (
            const auto var = std::get_if<alu::variable_t>(&add->operand)) {
            rhs = read_from_register(var->reg);
          }
          auto result = lhs + rhs;
          write_to_register(add->reg, result);
        } else if (const auto div = std::get_if<alu::div_t>(&op)) {
          auto lhs = read_from_register(div->reg);
          int64_t rhs = 0;
          if (const auto lit = std::get_if<alu::literal_t>(&div->operand)) {
            rhs = lit->number;
          } else if (
            const auto var = std::get_if<alu::variable_t>(&div->operand)) {
            rhs = read_from_register(var->reg);
          }
          if (rhs > 0) {
            auto result = lhs / rhs;
            write_to_register(div->reg, result);
          }
        } else if (const auto mod = std::get_if<alu::mod_t>(&op)) {
          auto lhs = read_from_register(mod->reg);
          int64_t rhs = 0;
          if (const auto lit = std::get_if<alu::literal_t>(&mod->operand)) {
            rhs = lit->number;
          } else if (
            const auto var = std::get_if<alu::variable_t>(&mod->operand)) {
            rhs = read_from_register(var->reg);
          }
          if (lhs >= 0 && rhs > 0) {
            auto result = lhs % rhs;
            write_to_register(mod->reg, result);
          }
        }

        //        std::cout << "w: " << w << '\n';
        //        std::cout << "x: " << x << '\n';
        //        std::cout << "y: " << y << '\n';
        //        std::cout << "z: " << z << '\n';
        //
        //        std::cout << "--- \n";
      }
      if (z == 0) {
        std::cout << "input: " << start_global_input << '\n';
        std::cout << "z: " << z << '\n';
      }
    }
  }

  std::cout << '\n';

  std::cout << "part 1: " << 65984919997939 << '\n';
  std::cout << "part 2: " << 11211619541713 << '\n';

  return 0;
}
