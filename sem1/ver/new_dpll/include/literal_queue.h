//
// Created by art on 11/4/21.
//

#ifndef YADPLL_INCLUDE_LITERAL_QUEUE_H_
#define YADPLL_INCLUDE_LITERAL_QUEUE_H_

#include <unordered_map>
#include <set>
#include <vector>

#include "clause.h"

class LiteralQueue {
 public:
  using clause_name_type = std::string;
  using clauses = std::set<clause_name_type>;

  LiteralQueue() : literal_clauses_(), literal_ids_(), literal_heap_() { }

  LiteralQueue &push(const Literal &lit, clause_name_type clause_name) noexcept;

  std::tuple<Literal, clauses, clauses> pop_literal(const Literal &lit) noexcept {
    return this->_pop_literal(lit.name(), lit.is_opposed());
  }

  std::tuple<Literal, clauses, clauses> pop_next_clauses(bool opposed) noexcept {
    return this->_pop_literal(this->_head_literal(), opposed);
  }

  [[nodiscard]] bool is_max_opposed() const noexcept {
    const auto &[same, opp] = this->literal_clauses_.find(this->_head_literal())->second;

    return same.size() < opp.size();
  }

 private:
  std::tuple<Literal, clauses, clauses> _pop_literal(Literal::name_type name, bool opposed) noexcept;
  Literal::name_type _head_literal() const noexcept;

  void _heapify_to_root(std::size_t idx) noexcept;
  void _heapify_to_leaf(std::size_t idx) noexcept;

  std::unordered_map<Literal::name_type, std::pair<clauses, clauses>> literal_clauses_;
  std::unordered_map<Literal::name_type, std::size_t> literal_ids_;
  std::vector<std::pair<std::size_t, Literal::name_type>> literal_heap_;
};
#endif //YADPLL_INCLUDE_LITERAL_QUEUE_H_
