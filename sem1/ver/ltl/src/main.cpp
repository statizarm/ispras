#include <iostream>
#include "ltl.h"
#include "compiler.h"

using namespace model::ltl;
using namespace model::fsm;

int main() {
  Formula formula = U(F(P("p")), !P("p") && X(G(P("q"))));

  std::cout << "Formula:\n" << formula << std::endl;

  std::cout << "Simplified Formula:\n" << Compiler::simplify(formula) << std::endl;

  auto automaton = Compiler::translate(formula);

  std::cout << "Automaton:\n" << *automaton << std::endl;
  return 0;
}
