//
// Created by art on 11/1/21.
//
#include "clause.h"

Clause &Clause::spread_one(const Literal &lit) noexcept {
  auto &&[begin, end] = this->literals_.equal_range(lit);

  while (begin != end) {
    auto next_it = std::next(begin);

    if (!lit.is_opposed(*begin)) {
      this->literals_.clear();
      break;
    }
    this->literals_.erase(begin);

    begin = next_it;
  }

  return *this;
}

bool Clause::contains(const Literal &lit) const noexcept {
  for (auto &&[begin, end] = this->literals_.equal_range(lit); begin != end; ++begin) {
    if (begin->is_opposed() == lit.is_opposed()) {
      return true;
    }
  }

  return false;
}
