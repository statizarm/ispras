//
// Created by art on 11/1/21.
//

#include <clause.h>
#include <queue>
#include <algorithm>
#include <stack>

#include "dpll.h"
#include "literal_queue.h"

static cnf_repr_type &spread_one(cnf_repr_type &, LiteralQueue &);
static bool has_empty(const cnf_repr_type &cnf_repr);
static LiteralQueue get_literal_queue(const cnf_repr_type &cnf_repr);
static cnf_repr_type &set_lit_value(cnf_repr_type &, LiteralQueue &, bool opposed);

bool dpll(const cnf_repr_type &cnf_repr) {
  std::stack<std::pair<cnf_repr_type, LiteralQueue>> stack;

  stack.emplace(cnf_repr, get_literal_queue(cnf_repr));

  while (!stack.empty()) {
    auto &[repr, queue] = stack.top();

    spread_one(repr, queue);

    if (repr.empty()) {
      return true;
    }

    if (has_empty(repr)) {
      stack.pop();
      continue;
    }

    // Memory usage optimization
    stack.emplace(repr, queue);

    if (queue.is_max_opposed()) {
      set_lit_value(stack.top().first, stack.top().second, true);
      set_lit_value(repr, queue, false);
    } else {
      set_lit_value(stack.top().first, stack.top().second, false);
      set_lit_value(repr, queue, true);
    }
  }

  return false;
}

static cnf_repr_type &spread_one(cnf_repr_type &cnf_repr, LiteralQueue &queue) {
  bool has_singleton = false;
  Literal single_literal(0);
  for (auto &[_, cl] : cnf_repr) {
    if (cl.size() == 1) {
      has_singleton = true;
      single_literal = cl.get_literal();
      break;
    }
  }

  while (has_singleton) {
    has_singleton = false;
    auto [lit, same, opp] = queue.pop_literal(single_literal);

    for (auto &&name : same) {
      cnf_repr.erase(name);
    }

    for (auto &&name : opp) {
      if (auto it = cnf_repr.find(name); it == cnf_repr.end()){
        continue;
      } else if (auto &cl = it->second.delete_opposed(lit); cl.size() == 1) {
        has_singleton = true;
        single_literal = cl.get_literal();
      }
    }
  }

  return cnf_repr;
}

static bool has_empty(const cnf_repr_type &cnf_repr) {
  return std::any_of(cnf_repr.begin(), cnf_repr.end(), [](const cnf_repr_type::value_type &val) {
    return val.second.size() == 0;
  });
}

static LiteralQueue get_literal_queue(const cnf_repr_type &cnf_repr) {
  LiteralQueue queue;

  for (auto &&[name, cl]: cnf_repr) {
    for (auto &&lit : cl.literals()) {
      queue.push(lit, name);
    }
  }

  return queue;
}

static cnf_repr_type &set_lit_value(cnf_repr_type &cnf_repr, LiteralQueue &queue, bool opposed) {
  auto [lit, same, opp] = queue.pop_next_clauses(opposed);

  for (auto &&name : same) {
    cnf_repr.erase(name);
  }

  for (auto &&name : opp) {
    if (auto it = cnf_repr.find(name); it != cnf_repr.end()) {
      it->second.delete_opposed(lit);
    }
  }

  return cnf_repr;
}
