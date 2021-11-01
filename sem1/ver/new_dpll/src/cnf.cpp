//
// Created by art on 11/1/21.
//
#include <list>
#import "cnf.h"
#include "dpll.h"

std::unique_ptr<CNF> CNF::load_cnf(std::istream &in) {
  return nullptr;
}

bool CNF::solve() const noexcept {
  std::list<Clause> list;

  for (auto &&[_, cl] : this->clauses_) {
    list.push_back(cl);
  }

  return dpll(list);
}
