#include <iostream>
#include <vector>
#include "cnf.h"

int main() {
  std::cout << "a || b && b || ~a" << std::endl;

  std::vector<Literal> clause_literals[] = {
      std::vector<Literal>{Literal("a"), Literal("b")},
      std::vector<Literal>{Literal("b"), Literal("a", true)},
      };
  std::vector<Clause> clauses;
  for (auto literals : clause_literals) {
    clauses.emplace_back(literals.begin(), literals.end());
  }

  CNF cnf(clauses.begin(), clauses.end());

  std::cout << "Try solve: " << cnf.solve() << std::endl;
  return 0;
}
