//
// Created by art on 11/1/21.
//

#ifndef DPLL_INCLUDE_CLAUSE_H_
#define DPLL_INCLUDE_CLAUSE_H_

#include <string>
#include <set>
#include <unordered_set>
#include <functional>

class LiteralClauses {
 public:
  enum entry_type {LITERAL_OPPOSED = 0x02, LITERAL_BASIC = 0x01, LITERAL_DELETED = 0x0, LITERAL_BOTH = 0x3};
  enum value_type {LITERAL_TRUE = 0x01, LITERAL_FALSE = 0x02};
  enum clause_status {CLAUSE_DELETED = 0x00, CLAUSE_EMPTY = -0x01};

  LiteralClauses(uint32_t n_literals, uint32_t n_clauses) noexcept;

  ~LiteralClauses() noexcept;

  void add_entry(uint32_t literal_id, uint32_t clause_id, entry_type type) noexcept;
  void set_value(uint32_t literal_id, value_type value) noexcept;

  [[nodiscard]] entry_type get_type(uint32_t literal_id, uint32_t clause_id) const noexcept;
  [[nodiscard]] uint32_t get_literal() const noexcept;

  void spread_one() noexcept;

  [[nodiscard]] bool is_empty() const noexcept;
  [[nodiscard]] bool has_empty() const noexcept;

  [[nodiscard]] LiteralClauses *copy() const noexcept;

 private:
  static std::size_t table_size(uint32_t n_literals, uint32_t n_clauses) noexcept;
  [[maybe_unused, deprecated]] static std::size_t header_size(uint32_t n) noexcept;
  static std::size_t row_size(uint32_t n) noexcept;
  static uint32_t get_literal_from_clause(const uint32_t *begin, const uint32_t *end, value_type &val) noexcept;

  LiteralClauses() : n_clauses_(), n_literals_(), n_existed_clauses_(), header_(nullptr), repr_(nullptr) { }

  uint32_t get_singleton(value_type &dst) const noexcept;
  uint32_t set_value_and_get_next(uint32_t lit, value_type val, value_type &dst) noexcept;

  uint32_t n_clauses_;
  uint32_t n_literals_;
  uint32_t n_existed_clauses_;

  [[maybe_unused, deprecated]] struct _header {
    std::size_t n_clauses;
    int32_t clause_sizes[];
  } *header_;

  struct _clauseRepr {
    std::int32_t n_literals;
    uint32_t literals[];
  } **repr_;
};

#endif //DPLL_INCLUDE_CLAUSE_H_
