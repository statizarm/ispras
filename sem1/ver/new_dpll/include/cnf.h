//
// Created by art on 11/1/21.
//

#ifndef YADPLL_INCLUDE_CNF_H_
#define YADPLL_INCLUDE_CNF_H_

#include <string>
#include <unordered_map>
#include <memory>
#include <istream>

#include "literal.h"

class CNF {
 public:
  static std::unique_ptr<CNF> load_cnf(std::istream &in);

  CNF() : table_(nullptr) { }
  CNF(LiteralClauses *table) : table_(table) { }

  ~CNF() {
    delete this->table_;
  }

  [[nodiscard]] bool solve() const noexcept;

 private:
  LiteralClauses *table_;
};

#endif //YADPLL_INCLUDE_CNF_H_
