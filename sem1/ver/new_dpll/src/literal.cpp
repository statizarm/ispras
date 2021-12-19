// Created by art on 11/1/21.
//
#include <cstring>
#include "literal.h"

#define LITERAL_CHUNK_OFFSET(id) (id >> 4)
#define LITERAL_BIT_OFFSET(id) ((id & 0x0F) << 1)
#define CLAUSE_STATUS_MASK (0x03)

LiteralClauses::LiteralClauses(uint32_t n_literals, uint32_t n_clauses) noexcept
  : n_clauses_(n_clauses), n_literals_(n_literals), curr_savepoint_(0) {
  this->clause_set_after_ = new uint32_t[this->n_clauses_];
  

  this->literal_set_after_ = new uint32_t[this->n_literals_ + 1];
  for (int i = 0; i < this->n_literals_; ++i) {
    this->literal_set_after_[i] = this->n_literals_;
  }


  auto repr_size = ((this->n_clauses_ + 1) * sizeof(uint8_t *)); // in bytes
  repr_size = (((repr_size - 1) >> 6) + 1) << 6; // aligned

  auto row_size = LiteralClauses::row_size(this->n_literals_);
  auto table_size = LiteralClauses::table_size(this->n_literals_, this->n_clauses_);

  auto total_size = repr_size + table_size;

  auto table = new uint8_t[total_size];

  memset(table, 0, total_size); // set nulls

  this->repr_ = (uint32_t **) table;

  this->repr_[0] = (uint32_t *) (table + repr_size);
  for (int i = 1; i <= this->n_clauses_; ++i) {
    this->repr_[i] = (uint32_t *) ((uint8_t *) this->repr_[i - 1] + row_size);
  }
}

LiteralClauses::~LiteralClauses() noexcept {
  delete[] (uint8_t *) this->repr_;
}

void LiteralClauses::add_entry(uint32_t literal_id, uint32_t clause_id, LiteralClauses::entry_type type) noexcept {
  if (this->curr_savepoint_ != 0) {
    return;
  }

  auto chunk_id = LITERAL_CHUNK_OFFSET(literal_id);
  auto bit_offset = LITERAL_BIT_OFFSET(literal_id);
  auto mask = type << bit_offset;
  auto entry_type = this->get_type(literal_id, clause_id);

  if (!(entry_type & type)) {
    this->repr_[clause_id][chunk_id] |= mask;

    if (this->clause_sizes_table_[0][clause_id] == 0) {
      this->n_clauses_table_[0] += 1;
    } else if (this->clause_sizes_table_[0][clause_id] == CLAUSE_EMPTY) {
      this->clause_sizes_table_[0][clause_id] = 0;
    }

    this->clause_sizes_table_[0][clause_id] += 1;
  }
}

uint32_t LiteralClauses::get_literal() const noexcept {
  uint32_t *begin;
  uint32_t *end = begin;

  for (int i = 0; i < this->n_clauses_; ++i) {
    if (this->clause_sizes_table_[this->curr_savepoint_][i] > 0) {
      begin = this->repr_[i];
      end = this->repr_[i+1];
      break;
    }
  }

  value_type val;
  return get_literal_from_clause(begin, end, val);
}

LiteralClauses &LiteralClauses::set_value(uint32_t literal_id, LiteralClauses::value_type value) noexcept {
  this->set_value_and_get_next(literal_id, value, value);
  return *this;
}

LiteralClauses::entry_type LiteralClauses::get_type(uint32_t literal_id, uint32_t clause_id) const noexcept {
  auto chunk_id = LITERAL_CHUNK_OFFSET(literal_id);
  auto bit_offset = LITERAL_BIT_OFFSET(literal_id);

  return (LiteralClauses::entry_type) ((this->repr_[clause_id][chunk_id] >> bit_offset) & CLAUSE_STATUS_MASK);
}

void LiteralClauses::spread_one() noexcept {
  value_type value;
  auto lit = this->get_singleton(value);

  while (lit != 0) {
    lit = this->set_value_and_get_next(lit, value, value);
  }
}

bool LiteralClauses::is_empty() const noexcept {
  return this->n_clauses_table_[this->curr_savepoint_] == 0;
}

bool LiteralClauses::has_empty() const noexcept {
  for (auto size : this->clause_sizes_table_[this->curr_savepoint_]) {
    if (size == CLAUSE_EMPTY) {
      return true;
    }
  }
  return false;
}

std::size_t LiteralClauses::table_size(uint32_t n_literals, uint32_t n_clauses) noexcept {
  auto row_size = LiteralClauses::row_size(n_literals + 1);

  auto table_size = n_clauses * row_size; // plus memory for repr

  return table_size;
}

std::size_t LiteralClauses::row_size(uint32_t n) noexcept {
  auto row_size = ((n - 1) >> 2) + 1; // in bytes
  row_size = (((row_size - 1) >> 6) + 1) << 6; // aligned

  return row_size;
}

uint32_t LiteralClauses::get_singleton(LiteralClauses::value_type &dst) const noexcept {
  uint32_t *begin;
  uint32_t *end = begin;

  for (int i = 0; i < this->n_clauses_; ++i) {
    if (this->clause_sizes_table_[this->curr_savepoint_][i] == 1) {
      begin = this->repr_[i];
      end = this->repr_[i + 1];
      break;
    }
  }

  return get_literal_from_clause(begin, end, dst);
}

uint32_t LiteralClauses::set_value_and_get_next(
    uint32_t lit,
    LiteralClauses::value_type val,
    LiteralClauses::value_type &dst) noexcept {

  auto opposed_val = val ^ LITERAL_BOTH;
  uint32_t next_lit = 0;

  auto prev_savepoint = this->curr_savepoint_;
  this->curr_savepoint_ += 1;

  this->mark_table_[this->curr_savepoint_] = this->mark_table_[prev_savepoint];
  this->n_clauses_table_[this->curr_savepoint_] = this->n_clauses_table_[prev_savepoint];

  this->mark_table_[this->curr_savepoint_][lit] = true;
  for (int i = 0; i < this->n_clauses_; ++i) {

    auto &n = this->clause_sizes_table_[this->curr_savepoint_][i];

    n = this->clause_sizes_table_[prev_savepoint][i];

    if (n == CLAUSE_DELETED || n == CLAUSE_EMPTY) {
      continue;
    }

    auto type_bits = this->get_type(lit, i);
    if (type_bits & val) {
      n = CLAUSE_DELETED;
      this->n_clauses_table_[this->curr_savepoint_] -= 1;
    } else if (type_bits & opposed_val) {
      n = n - 1;
      if (n == 0) {
        n = CLAUSE_EMPTY;
      } else if (n == 1 && next_lit == 0) {
        next_lit = get_literal_from_clause(this->repr_[i], this->repr_[i + 1], dst);
      }
    }
  }

  return next_lit;
}

uint32_t LiteralClauses::get_literal_from_clause(
    const uint32_t *begin,
    const uint32_t *end,
    value_type &dst) const noexcept {

  uint32_t lit = 0;
  for (auto it = begin; it != end; ++it) {
    int bits = *(int *) it;
    for (auto pos = ffs(bits); pos != 0; pos = ffs(bits)) {
      --pos;
      pos >>= 1;
      bits &= ~(0x3 << (pos << 1));
      dst = LiteralClauses::value_type ((*it >> (pos << 1)) & 0x3);
      lit = ((it - begin) << 4) + pos;
      if (!this->mark_table_[this->curr_savepoint_][lit]) {
        return lit;
      }
    }

    lit = 0;
  }

  return lit;
}
