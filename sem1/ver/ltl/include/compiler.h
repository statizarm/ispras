//
// Created by art on 12/10/21.
//

#ifndef LTL_INCLUDE_COMPILER_H_
#define LTL_INCLUDE_COMPILER_H_

#include <memory>

#include "fsm.h"
#include "ltl.h"

class Compiler {
 public:
  using Automaton = model::fsm::Automaton;
  using Formula = model::ltl::Formula;
  Compiler() = default;

  static std::shared_ptr<Automaton> translate(const Formula &formula) noexcept;
  static const Formula &simplify(const Formula &formula) noexcept;
 private:
};

#endif //LTL_INCLUDE_COMPILER_H_
