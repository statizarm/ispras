//
// Created by art on 11/1/21.
//

#ifndef DPLL_INCLUDE_CLAUSE_H_
#define DPLL_INCLUDE_CLAUSE_H_

#include <string>
#include <list>
#include <unordered_set>
#include <functional>

class LiteralClauses {
 public:
  enum entry_type {LITERAL_OPPOSED = 0x02, LITERAL_BASIC = 0x01, LITERAL_DELETED = 0x0, LITERAL_BOTH = 0x3};
  enum value_type {LITERAL_TRUE = 0x01, LITERAL_FALSE = 0x02};
  enum clause_status {CLAUSE_DELETED = 0x00, CLAUSE_EMPTY = -0x01};

  LiteralClauses(uint32_t n_literals, uint32_t n_clauses) noexcept;

  ~LiteralClauses() noexcept;

  [[nodiscard]] uint32_t n_literals() const noexcept {
    return this->n_literals_;
  }

  [[nodiscard]] uint32_t get_savepoint() const noexcept {
    return this->curr_savepoint_;
  }

  void add_entry(uint32_t literal_id, uint32_t clause_id, entry_type type) noexcept;
  LiteralClauses &set_value(uint32_t literal_id, value_type value) noexcept;

  [[nodiscard]] entry_type get_type(uint32_t literal_id, uint32_t clause_id) const noexcept;
  [[nodiscard]] uint32_t get_literal() const noexcept;

  void spread_one() noexcept;

  void rollback(uint32_t savepoint) noexcept {
    this->curr_savepoint_ = savepoint;
  }

  [[nodiscard]] bool is_empty() const noexcept;
  [[nodiscard]] bool has_empty() const noexcept;

 private:
  static std::size_t table_size(uint32_t n_literals, uint32_t n_clauses) noexcept;
  static std::size_t row_size(uint32_t n) noexcept;

  uint32_t get_singleton(value_type &dst) const noexcept;
  uint32_t set_value_and_get_next(uint32_t lit, value_type val, value_type &dst) noexcept;
  uint32_t get_literal_from_clause(const uint32_t *begin, const uint32_t *end, value_type &val) const noexcept;
  uint32_t count_clause_length(uint32_t clause_id);

  uint32_t curr_savepoint_;
  uint32_t n_clauses_;
  uint32_t n_literals_;

  uint32_t *clause_set_after_;
  uint32_t *literal_set_after_;

  uint32_t **repr_;
};

#endif //DPLL_INCLUDE_CLAUSE_H_
