//
// Created by art on 12/10/21.
//
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <stack>

#include "compiler.h"
#include "atom.h"

using model::ltl::Formula;
using model::fsm::Automaton;

static void calculate_closure(
    const Formula *formula,
    std::unordered_map<const Formula *, const Formula *> &closure,
    std::set<std::string> &ap,
    std::vector<const Formula *> &temporal_operators,
    std::vector<const Formula *> &variables) {

  std::queue<const Formula *> queue;
  std::unordered_map<const Formula *, const Formula *> not_operators;
  std::unordered_set<const Formula *> unadded;

  queue.push(formula);
  while(!queue.empty()) {
    formula = queue.front();
    queue.pop();

    switch (formula->kind()) {
      case Formula::U:
        temporal_operators.push_back(formula);
      case Formula::AND: case Formula::OR:
        queue.push(std::addressof(formula->lhs()));
        queue.push(std::addressof(formula->rhs()));
        break;
      case Formula::NOT:
        closure.insert({std::addressof(formula->arg()), formula});
        closure.insert({formula, std::addressof(formula->arg())});
      case Formula::X:
        queue.push(std::addressof(formula->arg()));
        break;
      case Formula::ATOM:
        ap.insert(formula->prop());
        break;
      default:
        break;
    }
    if (closure.find(formula) == closure.end()) {
      unadded.insert(formula);
    }
  }

  for (auto f : unadded) {
    if (closure.find(f) == closure.end()) {
      auto neg = std::addressof(!*f);
      closure.insert({f, neg});
      closure.insert({neg, f});
    }
  }

  for (auto &[pos, neg] : closure) {
    if (pos->kind() == Formula::ATOM || pos->kind() == Formula::X) {
      variables.push_back(neg);
    }
  }
}

static std::list<std::shared_ptr<Atom>> generate_atoms(const std::vector<const Formula *> &variables) {
  std::vector<std::shared_ptr<Atom>> atoms(1 << variables.size());

  for (int i = 0; i < atoms.size(); ++i) {
    atoms[i] = std::make_shared<Atom>("s" + std::to_string(i));
  }

  for (auto i = 0; i < variables.size(); ++i) {
    auto n = 1 << i;

    const Formula *val[2] {
      std::addressof(variables[i]->arg()),
      variables[i]
    };
    int val_id = 0;

    for (auto j = 0; j < atoms.size();) {
      auto end = j + n;
      while (j < end) {
        atoms[j]->add(val[val_id]);
        ++j;
      }
      val_id ^= 1;
    }
  }

  return {atoms.begin(), atoms.end()};
}

static std::list<std::shared_ptr<Atom>> calculate_atoms(
    const Formula *formula,
    std::set<std::string> &ap,
    std::vector<const Formula *> &temporal_operators) noexcept {
  std::unordered_map<const Formula *, const Formula *> closure;
  std::vector<const Formula *> variables;

  calculate_closure(formula, closure, ap, temporal_operators, variables);

  std::list<std::shared_ptr<Atom>> atoms = generate_atoms(variables);

  for (auto &atom : atoms) {
    bool added = true;
    while (added) {
      added = false;
      for (auto [pos, neg] : closure) {
        if (atom->contains(pos) || atom->contains((neg))) {
          continue;
        }

        switch (pos->kind()) {
          case Formula::OR:
            if (atom->contains(std::addressof(pos->lhs())) || atom->contains(std::addressof(pos->rhs()))) {
              atom->add(pos);
              added = true;
            } else if (atom->contains(closure[std::addressof(pos->lhs())]) && atom->contains(closure[std::addressof(pos->rhs())])) {
              atom->add(neg);
              added = true;
            }
            break;
          case Formula::AND:
            if (atom->contains(std::addressof(pos->lhs())) && atom->contains(std::addressof(pos->rhs()))) {
              atom->add(pos);
              added = true;
            } else if (atom->contains(closure[std::addressof(pos->lhs())]) || atom->contains(closure[std::addressof(pos->rhs())])) {
              atom->add(neg);
              added = true;
            }
            break;
          case Formula::U:
            if (atom->contains(std::addressof(pos->rhs()))) {
              atom->add(pos);
              added = true;
            } else if (atom->contains(closure[std::addressof(pos->rhs())]) && atom->contains(std::addressof(pos->lhs()))) {
              auto copied = std::make_shared<Atom>("s" + std::to_string(atoms.size()), *atom);
              atoms.push_back(copied);
              copied->add(pos);

              atom->add(neg);

              added = true;
            } else if (atom->contains(closure[std::addressof(pos->lhs())]) && atom->contains(closure[std::addressof(pos->rhs())])) {
              atom->add(neg);
              added = true;
            }
            break;
          default:
            break;
        }
      }
    }
  }

  return atoms;
}

std::shared_ptr<Automaton> Compiler::translate(const Formula &formula) noexcept {
  std::vector<const Formula *> temporal_operators;
  std::set<std::string> ap;
  const Formula *base_formula = std::addressof(Compiler::simplify(formula));

  auto atoms = calculate_atoms(base_formula, ap, temporal_operators);

  auto automaton = std::make_shared<Automaton>();

  for (auto &atom : atoms) {
    automaton->add_state(atom->label());
  }

  for (auto &source : atoms) {
    for (auto &destination : atoms) {
      auto syms = source->transition(*destination, ap);
      if (syms) {
        automaton->add_trans(source->label(), syms.value(), destination->label());
      }
    }
  }

  for (auto &atom : atoms) {
    if (atom->contains(base_formula)) {
      automaton->set_initial(atom->label());
    }
  }

  for (int i = 0; i < temporal_operators.size(); ++i) {
    for (auto &atom : atoms) {
      if (atom->contains(std::addressof(temporal_operators[i]->rhs())) || !atom->contains(temporal_operators[i])) {
        automaton->set_final(atom->label(), i);
      }
    }
  }

  return automaton;
}

static size_t hash_combine(size_t l, size_t r) {
  return l ^ (r << 1);
}

struct FormulaHash {
  size_t operator()(const Formula *formula) const noexcept {
    switch (formula->kind()) {
      case Formula::R: case Formula::U: case Formula::OR: case Formula::AND:
      case Formula::IMPL:
        return hash_combine(
            std::hash<const Formula *>{}(std::addressof(formula->lhs())),
            hash_combine(
                std::hash<const Formula *>{}(std::addressof(formula->rhs())),
                std::hash<Formula::Kind>{}(formula->kind())));
      case Formula::NOT: case Formula::G: case Formula::X:
      case Formula::F:
        return hash_combine(
            std::hash<const Formula *>{}(std::addressof(formula->arg())),
            std::hash<Formula::Kind>{}(formula->kind())
        );
      case Formula::ATOM:
        return hash_combine(
            std::hash<std::string>{}(formula->prop()),
            std::hash<Formula::Kind>{}(formula->kind())
        );
      case Formula::TRUE:
        return std::hash<Formula::Kind>{}(formula->kind());
    }
    return 0;
  }
};

struct FormulaEqual {
  bool operator()(const Formula *left, const Formula *right) const noexcept {
    if (left->kind() != right->kind()) {
      return false;
    }

    switch (left->kind()) {
      case Formula::R: case Formula::U: case Formula::OR: case Formula::AND:
      case Formula::IMPL:
        return std::addressof(left->lhs()) == std::addressof(right->lhs()) &&
            std::addressof(left->rhs()) == std::addressof(right->rhs());
      case Formula::NOT: case Formula::G: case Formula::X:
      case Formula::F:
        return std::addressof(left->arg()) == std::addressof(right->arg());
      case Formula::ATOM:
        return left->prop() == right->prop();
      case Formula::TRUE:
        return true;
      default:
        return false;
    }
  }
};

static const Formula &simplify_with_cache(
    const Formula &formula,
    std::unordered_set<const Formula *, FormulaHash, FormulaEqual> &cache) {
  using namespace model::ltl;
  const Formula *tmp_lhs;
  const Formula *tmp_rhs;
  const Formula *res;

  if (auto it = cache.find(std::addressof(formula)); it != cache.end()) {
    return **it;
  }

  switch(formula.kind()) {
    case Formula::R:
      res = std::addressof(!U(simplify_with_cache(!formula.lhs(), cache), simplify_with_cache(!formula.rhs(), cache)));
      break;
    case Formula::F:
      res = std::addressof(U(T(), simplify_with_cache(formula.arg(), cache)));
      break;
    case Formula::G:
      res = std::addressof(!U(T(), (simplify_with_cache(!formula.arg(), cache))));
      break;
    case Formula::IMPL:
      res = std::addressof(simplify_with_cache(!formula.lhs(), cache) || simplify_with_cache(formula.rhs(), cache));
      break;
    case Formula::X:
      tmp_lhs = std::addressof(simplify_with_cache(formula.arg(), cache));
      if (tmp_lhs->kind() == Formula::OR) {
        res = std::addressof(X(tmp_lhs->lhs()) || X(tmp_lhs->rhs()));
      } else if (tmp_lhs->kind() == Formula::AND) {
        res = std::addressof(X(tmp_lhs->lhs()) && X(tmp_lhs->lhs()));
      } else if (tmp_lhs->kind() == Formula::NOT) {
        res = std::addressof(!X(tmp_lhs->arg()));
      } else if (tmp_lhs == std::addressof(formula.arg())) {
        res = std::addressof(formula);
      } else {
        res = std::addressof(X(*tmp_lhs));
      }
      break;
    case Formula::NOT:
      tmp_lhs = std::addressof(simplify_with_cache(formula.arg(), cache));
      if (tmp_lhs->kind() == Formula::OR) {
        res = std::addressof(
            simplify_with_cache(!tmp_lhs->lhs(), cache) && simplify_with_cache(!tmp_lhs->rhs(), cache)
        );
      } else if (tmp_lhs->kind() == Formula::AND) {
        res = std::addressof(
            simplify_with_cache(!tmp_lhs->lhs(), cache) || simplify_with_cache(!tmp_lhs->rhs(), cache)
        );
      } else if (tmp_lhs->kind() == Formula::NOT) {
        res = std::addressof(tmp_lhs->arg());
      } else if (tmp_lhs == std::addressof(formula.arg())) {
        res = std::addressof(formula);
      } else {
        res = std::addressof(!*tmp_lhs);
      }
      break;
    case Formula::OR:
      tmp_lhs = std::addressof(simplify_with_cache(formula.lhs(), cache));
      tmp_rhs = std::addressof(simplify_with_cache(formula.rhs(), cache));
      if (tmp_lhs == std::addressof(formula.lhs()) && tmp_rhs == std::addressof(formula.rhs())) {
        res = std::addressof(formula);
      } else {
        res = std::addressof(*tmp_lhs || *tmp_rhs);
      }
      break;
    case Formula::AND:
      tmp_lhs = std::addressof(simplify_with_cache(formula.lhs(), cache));
      tmp_rhs = std::addressof(simplify_with_cache(formula.rhs(), cache));
      if (tmp_lhs == std::addressof(formula.lhs()) && tmp_rhs == std::addressof(formula.rhs())) {
        res = std::addressof(formula);
      } else {
        res = std::addressof(*tmp_lhs && *tmp_rhs);
      }
      break;
    case Formula::U:
      tmp_lhs = std::addressof(simplify_with_cache(formula.lhs(), cache));
      tmp_rhs = std::addressof(simplify_with_cache(formula.rhs(), cache));
      if (tmp_lhs == std::addressof(formula.lhs()) && tmp_rhs == std::addressof(formula.rhs())) {
        res = std::addressof(formula);
      } else {
        res = std::addressof(U(*tmp_lhs, *tmp_rhs));
      }
      break;
    default:
      res = std::addressof(formula);
      break;
  }

  if (auto it = cache.find(res); it == cache.end()) {
    cache.insert(res);
  }

  return *res;
}

const Formula &Compiler::simplify(const Formula &formula) noexcept {
  std::unordered_set<const Formula *, FormulaHash, FormulaEqual> cache;
  return simplify_with_cache(formula, cache);
}
