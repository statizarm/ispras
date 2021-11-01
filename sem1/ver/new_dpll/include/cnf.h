//
// Created by art on 11/1/21.
//

#ifndef YADPLL_INCLUDE_CNF_H_
#define YADPLL_INCLUDE_CNF_H_

#include <string>
#include <unordered_map>
#include <memory>
#include <istream>

#include "clause.h"

class CNF {
 public:
  static std::unique_ptr<CNF> load_cnf(std::istream &in);

  template<class It>
  CNF(It begin, It end) : clauses_() {
    std::string base_pref = "c";

    for (int c = 0; begin != end; ++begin, ++c) {
      this->clauses_.insert({base_pref + std::to_string(c), *begin});
    }
  }

  [[nodiscard]] bool solve() const noexcept;

 private:
  std::unordered_map<std::string, Clause> clauses_;
};

#endif //YADPLL_INCLUDE_CNF_H_
