//
// Created by art on 11/1/21.
//

#include <clause.h>
#include <queue>
#include <algorithm>
#include <unordered_set>

#include "dpll.h"

using singleton_clauses_type = std::queue<Literal>;

static cnf_repr_type &spread_one(cnf_repr_type &, singleton_clauses_type &, std::unordered_set<std::string> &);
static bool has_empty(const cnf_repr_type &cnf_repr);
static std::unordered_set<std::string> get_literals(cnf_repr_type &cnf_repr);
static cnf_repr_type set_lit_value(cnf_repr_type, const Literal &);

bool dpll(cnf_repr_type cnf_repr) {
  singleton_clauses_type singleton_clauses;
  auto literals = get_literals(cnf_repr);

  for (auto &&cl : cnf_repr) {
    if (cl.size() == 1) {
      singleton_clauses.push(*cl.literals().begin());
      break;
    }
  }

  spread_one(cnf_repr, singleton_clauses, literals);

  if (cnf_repr.empty()) {
    return true;
  }

  if (has_empty(cnf_repr)) {
    return false;
  }

  {
    auto lit = *literals.begin();

    auto left = set_lit_value(cnf_repr, Literal(lit, false));
    auto right = set_lit_value(cnf_repr, Literal(lit, true));

    return dpll(left) || dpll(right);
  }
}

static cnf_repr_type &spread_one(
    cnf_repr_type &cnf_repr,
    singleton_clauses_type &singleton_clauses,
    std::unordered_set<std::string> &literals) {
  while (!singleton_clauses.empty()) {
    auto lit = singleton_clauses.front();
    singleton_clauses.pop();

    for (auto it = cnf_repr.begin(); it != cnf_repr.end();) {
      auto next_it = std::next(it);

      if (it->contains(lit)) {
        cnf_repr.erase(it);
      } else if (it->spread_one(lit).size() == 1 && singleton_clauses.empty()) {
        singleton_clauses.push(*it->literals().begin());
      }

      it = next_it;
    }

    literals.erase(lit.name());
  }

  return cnf_repr;
}

static bool has_empty(const cnf_repr_type &cnf_repr) {
  return std::any_of(cnf_repr.begin(), cnf_repr.end(), [](const cnf_repr_type::value_type &val) {
    return val.size() == 0;
  });
}

static std::unordered_set<std::string> get_literals(cnf_repr_type &cnf_repr) {
  // TODO: add treap implementation
  std::unordered_set<std::string> literals;

  for (auto &&cl: cnf_repr) {
    for (auto &&lit : cl.literals()) {
      literals.insert(lit.name());
    }
  }

  return literals;
}

static cnf_repr_type set_lit_value(cnf_repr_type cnf_repr, const Literal &lit) {
  for (auto it = cnf_repr.begin(); it != cnf_repr.end();) {
    auto next_it = std::next(it);
    if (it->contains(lit)) {
      cnf_repr.erase(it);
    } else {
      it->spread_one(lit);
    }

    it = next_it;
  }

  return cnf_repr;
}
