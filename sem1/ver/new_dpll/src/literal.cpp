// Created by art on 11/1/21.
//
#include <cstring>
#include "literal.h"

#define LITERAL_CHUNK_OFFSET(id) (id >> 4)
#define LITERAL_BIT_OFFSET(id) ((id & 0x0F) << 1)
#define CLAUSE_STATUS_MASK (0x03)

LiteralClauses::LiteralClauses(uint32_t n_literals, uint32_t n_clauses) noexcept
  : n_clauses_(n_clauses), n_literals_(n_literals), n_existed_clauses_(0), header_(nullptr) {

  auto repr_size = ((this->n_clauses_ + 1) * sizeof(uint8_t *)); // in bytes
  repr_size = (((repr_size - 1) >> 6) + 1) << 6; // aligned

  auto row_size = LiteralClauses::row_size(this->n_literals_);
  auto table_size = LiteralClauses::table_size(this->n_literals_, this->n_clauses_);

  auto total_size = repr_size + table_size;

  auto table = new uint8_t[total_size];

  memset(table, 0, total_size); // set nulls

  this->repr_ = (LiteralClauses::_clauseRepr **) table;

  //this->header_ = (LiteralClauses::_header *) (table + repr_size);

  this->repr_[0] = (LiteralClauses::_clauseRepr *) (table + repr_size);
  for (int i = 1; i <= this->n_clauses_; ++i) {
    this->repr_[i] = (LiteralClauses::_clauseRepr *) ((uint8_t *) this->repr_[i - 1] + row_size);
  }
}
LiteralClauses::~LiteralClauses() noexcept {
  delete[] (uint8_t *) this->repr_;
}

void LiteralClauses::add_entry(uint32_t literal_id, uint32_t clause_id, LiteralClauses::entry_type type) noexcept {
  auto chunk_id = LITERAL_CHUNK_OFFSET(literal_id);
  auto bit_offset = LITERAL_BIT_OFFSET(literal_id);
  auto mask = type << bit_offset;
  auto entry_type = this->get_type(literal_id, clause_id);

  if (!(entry_type & type)) {
    this->repr_[clause_id]->literals[chunk_id] |= mask;

    if (this->repr_[clause_id]->n_literals == 0) {
      this->n_existed_clauses_ += 1;
      this->repr_[clause_id]->n_literals += 1;
    } else if (this->repr_[clause_id]->n_literals == CLAUSE_EMPTY) {
      this->repr_[clause_id]->n_literals = 1;
    } else {
      this->repr_[clause_id]->n_literals += 1;
    }
  }
}

uint32_t LiteralClauses::get_literal() const noexcept {
  uint32_t *begin;
  uint32_t *end = begin;

  for (int i = 0; i < this->n_clauses_; ++i) {
    if (this->repr_[i]->n_literals > 0) {
      begin = this->repr_[i]->literals;
      end = (uint32_t *) this->repr_[i+1];
      break;
    }
  }

  value_type val;
  return get_literal_from_clause(begin, end, val);
}

void LiteralClauses::set_value(uint32_t literal_id, LiteralClauses::value_type value) noexcept {
  this->set_value_and_get_next(literal_id, value, value);
}

LiteralClauses::entry_type LiteralClauses::get_type(uint32_t literal_id, uint32_t clause_id) const noexcept {
  auto chunk_id = LITERAL_CHUNK_OFFSET(literal_id);
  auto bit_offset = LITERAL_BIT_OFFSET(literal_id);

  return (LiteralClauses::entry_type) ((this->repr_[clause_id]->literals[chunk_id] >> bit_offset) & CLAUSE_STATUS_MASK);
}

void LiteralClauses::spread_one() noexcept {
  value_type value;
  auto lit = this->get_singleton(value);

  while (lit != 0) {
    lit = this->set_value_and_get_next(lit, value, value);
  }
}

bool LiteralClauses::is_empty() const noexcept {
  return this->n_existed_clauses_ == 0;
}

bool LiteralClauses::has_empty() const noexcept {
  auto it = this->repr_;
  auto end = &this->repr_[this->n_clauses_];
  for (; it != end; ++it) {
    if ((*it)->n_literals == CLAUSE_EMPTY) {
      return true;
    }
  }
  return false;
}

LiteralClauses *LiteralClauses::copy() const noexcept {
  auto lc = new LiteralClauses(this->n_literals_, this->n_clauses_);

  auto table_size = LiteralClauses::table_size(this->n_literals_, this->n_clauses_);

  lc->n_existed_clauses_ = this->n_existed_clauses_;

  memcpy(lc->repr_[0], this->repr_[0], table_size);

  return lc;
}

std::size_t LiteralClauses::table_size(uint32_t n_literals, uint32_t n_clauses) noexcept {
  auto row_size = LiteralClauses::row_size(n_literals + 1);

  //auto header_size = LiteralClauses::header_size(n_literals);

  auto table_size = n_clauses * row_size; // plus memory for repr

  return table_size;
}

 std::size_t LiteralClauses::header_size(uint32_t n) noexcept {
  auto header_size = sizeof(LiteralClauses::_header) + n * sizeof(uint32_t);
  header_size = (((header_size - 1) >> 6) + 1) << 6; // aligned

  return header_size;
}

std::size_t LiteralClauses::row_size(uint32_t n) noexcept {
  auto row_size = sizeof(LiteralClauses::_clauseRepr) + (n >> 2); // in bytes
  row_size = (((row_size - 1) >> 6) + 1) << 6; // aligned

  return row_size;
}

uint32_t LiteralClauses::get_singleton(LiteralClauses::value_type &dst) const noexcept {
  uint32_t *begin;
  uint32_t *end = begin;

  for (int i = 0; i < this->n_clauses_; ++i) {
    if (this->repr_[i]->n_literals == 1) {
      begin = this->repr_[i]->literals;
      end = (uint32_t *) this->repr_[i + 1];
      break;
    }
  }

  return get_literal_from_clause(begin, end, dst);
}

uint32_t LiteralClauses::set_value_and_get_next(
    uint32_t lit,
    LiteralClauses::value_type val,
    LiteralClauses::value_type &dst) noexcept {
  auto chunk_id = LITERAL_CHUNK_OFFSET(lit);
  auto opposed_val = val ^ LITERAL_BOTH;
  auto bit_offset = LITERAL_BIT_OFFSET(lit);
  uint32_t next_lit = 0;

  auto it = this->repr_;
  auto end = &this->repr_[this->n_clauses_];
  for (; it != end; ++it) {
    auto *hp = *it;
    if (hp->n_literals == CLAUSE_DELETED || hp->n_literals == CLAUSE_EMPTY) {
      continue;
    }

    auto type_bits = hp->literals[chunk_id] >> bit_offset;
    if (type_bits & val) {
      hp->n_literals = CLAUSE_DELETED;
      this->n_existed_clauses_ -= 1;
    } else if (type_bits & opposed_val) {
      hp->n_literals -= 1;
      hp->literals[chunk_id] &= ~(0x3 << bit_offset);

      if (hp->n_literals == 0) {
        hp->n_literals = CLAUSE_EMPTY;
      } else if (hp->n_literals == 1 && next_lit == 0) {
        next_lit = get_literal_from_clause(hp->literals, (uint32_t *) *(it + 1), dst);
      }
    }
  }

  return next_lit;
}

uint32_t LiteralClauses::get_literal_from_clause(const uint32_t *begin, const uint32_t *end, value_type &dst) noexcept {
  for (auto it = begin; it != end; ++it) {
    if (auto pos = ffs(*(int *) it); pos != 0) {
      pos = (pos - 1) >> 1;
      dst = LiteralClauses::value_type ((*it >> (pos << 1)) & 0x3);
      return ((it - begin) << 4) + pos;
    }
  }
  return 0;
}
