//
// Created by art on 11/1/21.
//

#ifndef YADPLL_INCLUDE_DPLL_H_
#define YADPLL_INCLUDE_DPLL_H_

#include <unordered_map>
#include <string>
#include <list>

using cnf_repr_type = std::unordered_map<std::string, Clause>;

bool dpll(const cnf_repr_type &);

#endif //YADPLL_INCLUDE_DPLL_H_
