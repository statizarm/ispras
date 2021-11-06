//
// Created by art on 11/1/21.
//
#include <sstream>
#include <stack>
#include <cstring>
#include "cnf.h"

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
  std::stack<std::shared_ptr<CNF>> stack;

  stack.push(this->copy());

  while (!stack.empty()) {
    auto cnf = stack.top();

    cnf->table_->spread_one();

    if (cnf->table_->is_empty()) {
      return true;
    }

    if (cnf->table_->has_empty()) {
      stack.pop();
      continue;
    }

    stack.push(cnf->copy());

    auto lit = cnf->table_->get_literal();

    stack.top()->table_->set_value(lit, LiteralClauses::LITERAL_FALSE);
    cnf->table_->set_value(lit, LiteralClauses::LITERAL_TRUE);
  }

  return false;
}

std::shared_ptr<CNF> CNF::copy() const noexcept {
  auto new_table = this->table_->copy();
  return std::make_shared<CNF>(new_table);
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
