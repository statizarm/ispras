//
// Created by art on 11/1/21.
//
#include <list>
#include <vector>
#include <sstream>
#include "cnf.h"
#include "dpll.h"

static std::vector<Literal> parse_clause_string(const std::string &str);

std::unique_ptr<CNF> CNF::load_cnf(std::istream &in) {
  char buf[2][16];
  int n_clauses = -1;
  int n_literals = -1;
  int lit;
  std::string line;
  while(std::getline(in, line, '\n')) {
    if (line.rfind('c', 0) == 0) {
      continue;
    } else if (line.rfind('p', 0) == 0) {
      sscanf(line.c_str(), "%s%s%d%d", buf[0], buf[1], &n_literals, &n_clauses);
      break;
    }
  }

  std::vector<Clause> clauses;
  while(std::getline(in, line, '\n') && n_clauses > 0) {
    if (line.rfind('c', 0) == 0) {
      continue;
    } else {
      auto literals = parse_clause_string(line);

      clauses.emplace_back(literals.begin(), literals.end());
    }
    --n_clauses;
  }

  return std::make_unique<CNF>(clauses.begin(), clauses.end());
}

bool CNF::solve() const noexcept {
  return dpll(this->clauses_);
}

static std::vector<Literal> parse_clause_string(const std::string &str) {
  std::istringstream in(str);
  std::vector<Literal> literals;
  int lit_id;

  while (in >> lit_id && lit_id != 0) {
    if (lit_id < 0) {
      literals.emplace_back(std::abs(lit_id), true);
    } else {
      literals.emplace_back(std::abs(lit_id), false);
    }
  }

  return literals;
}
