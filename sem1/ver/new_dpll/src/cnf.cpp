//
// Created by art on 11/1/21.
//
#include <sstream>
#include <stack>
#include <cstring>
#include "cnf.h"

struct StackChunk {
  uint32_t literal_name;
  uint32_t save_point;
  LiteralClauses::value_type literal_value;

  StackChunk(uint32_t literal_name, uint32_t save_point, LiteralClauses::value_type literal_value)
    : literal_name(literal_name), save_point(save_point), literal_value(literal_value) { }

};

void parse_clause_string(const std::string &str, LiteralClauses *table, uint32_t clause_id) noexcept;

std::unique_ptr<CNF> CNF::load_cnf(std::istream &in) {
  char buf[2][16];
  int n_clauses = -1;
  int n_literals = -1;
  std::string line;
  while(std::getline(in, line, '\n')) {
    if (line.rfind('c', 0) == 0) {
      continue;
    } else if (line.rfind('p', 0) == 0) {
      sscanf(line.c_str(), "%s%s%d%d", buf[0], buf[1], &n_literals, &n_clauses);
      break;
    }
  }
  auto table = new LiteralClauses(n_literals, n_clauses);

  for (int i = 0; std::getline(in, line, '\n') && i < n_clauses;) {
    if (line.rfind('c', 0) == 0) {
      continue;
    }

    parse_clause_string(line, table, i);

    ++i;
  }

  return std::make_unique<CNF>(table);
}

bool CNF::solve() const noexcept {
  auto table = this->table_;
  std::stack<StackChunk> stack;

  // prepare
  table->spread_one();

  if (table->is_empty()) {
    return true;
  }

  if (table->has_empty()) {
    return false;
  }

  auto point = table->get_savepoint();
  auto lit_ = table->get_literal();
  stack.emplace(lit_, point, LiteralClauses::LITERAL_TRUE);
  stack.emplace(lit_, point, LiteralClauses::LITERAL_FALSE);

  // run
  while (!stack.empty()) {
    auto chunk = stack.top();
    stack.pop();

    if (table->set_value(chunk.literal_name, chunk.literal_value).has_empty()) {
      if (!stack.empty()) {
        table->rollback(stack.top().save_point);
      }

      continue;
    }

    table->spread_one();

    if (table->is_empty()) {
      return true;
    }

    if (table->has_empty()) {
      if (!stack.empty()) {
        table->rollback(stack.top().save_point);
      }

      continue;
    }


    chunk.literal_name = table->get_literal();
    chunk.save_point = table->get_savepoint();

    stack.emplace(chunk.literal_name, chunk.save_point, LiteralClauses::LITERAL_TRUE);
    stack.emplace(chunk.literal_name, chunk.save_point, LiteralClauses::LITERAL_FALSE);
  }

  return false;
}

void parse_clause_string(const std::string &str, LiteralClauses *table, uint32_t clause_id) noexcept {
  std::istringstream in(str);
  int lit_id;

  while (in >> lit_id && lit_id != 0) {
    auto type = LiteralClauses::LITERAL_BASIC;
    if (lit_id < 0) {
      type = LiteralClauses::LITERAL_OPPOSED;
    }

    table->add_entry(std::abs(lit_id), clause_id, type);
  }
}
