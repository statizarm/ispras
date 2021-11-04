//
// Created by art on 11/4/21.
//
#include <queue>
#include "literal_queue.h"

#define LEFT(x) ((x * 2 + 1))
#define RIGHT(x) ((x + 1) * 2)
#define PARENT(x) ((x - 1) / 2)

template <typename T>
std::size_t set_subtract(std::set<T> &left, const std::set<T> &right);

LiteralQueue &LiteralQueue::push(const Literal &lit, clause_name_type clause_name) noexcept {
  auto lit_name = lit.name();
  auto it = this->literal_ids_.find(lit_name);
  std::size_t idx = this->literal_heap_.size();

  if (it == this->literal_ids_.end()) {
    this->literal_ids_.emplace(lit_name, idx);
    this->literal_heap_.emplace_back(1, lit_name);
  } else {
    idx = it->second;
    this->literal_heap_[idx].first += 1;
  }

  this->_heapify_to_root(idx);

  if (lit.is_opposed()) {
    this->literal_clauses_[lit_name].second.insert(std::move(clause_name));
  } else {
    this->literal_clauses_[lit_name].first.insert(std::move(clause_name));
  }

  return *this;
}

std::tuple<Literal, LiteralQueue::clauses, LiteralQueue::clauses>
LiteralQueue::_pop_literal(Literal::name_type lit_name, bool opposed) noexcept {
  auto it = this->literal_clauses_.find(lit_name);
  if (it == this->literal_clauses_.end()) {
    return {};
  }

  auto res = (opposed) ?
      std::tuple(Literal(it->first, opposed), std::move(it->second.second), std::move(it->second.first)):
      std::tuple(Literal(it->first, opposed), std::move(it->second.first), std::move(it->second.second));

  auto &clauses_to_delete = std::get<1>(res);
  auto id_it = this->literal_ids_.find(lit_name);
  std::swap(this->literal_heap_[id_it->second], this->literal_heap_.back());

  this->literal_ids_.erase(id_it);
  this->literal_heap_.pop_back();
  this->literal_clauses_.erase(it);

  std::queue<int> hnode_queue;

  // change prior
  for (auto i = this->literal_heap_.size(); i > 0;) {
    --i;
    auto &p = this->literal_heap_[i];

    p.first -= set_subtract(this->literal_clauses_[p.second].second, clauses_to_delete);
    p.first -= set_subtract(this->literal_clauses_[p.second].first, clauses_to_delete);

    this->_heapify_to_leaf(i);
  }


  return res;
}

void LiteralQueue::_heapify_to_root(std::size_t idx) noexcept {
  auto p = PARENT(idx);
  auto elem = this->literal_heap_[idx];

  while (idx > 0 && this->literal_heap_[p] < elem) {
    auto &l = this->literal_heap_[p];
    this->literal_ids_[l.second] = idx;

    this->literal_heap_[idx] = l;
    idx = p;
    p = PARENT(idx);
  }

  this->literal_heap_[idx] = elem;
  this->literal_ids_[elem.second] = idx;
}

void LiteralQueue::_heapify_to_leaf(std::size_t idx) noexcept {
  auto n = this->literal_heap_.size();
  auto max = LEFT(idx);
  auto elem = this->literal_heap_[idx];

  while (max < this->literal_heap_.size() && this->literal_heap_[max] > elem) {
    auto right = RIGHT(idx);

    if (right < n && this->literal_heap_[right] > this->literal_heap_[max]) {
      max = right;
    }

    auto &l = literal_heap_[max];
    this->literal_ids_[l.second] = idx;

    this->literal_heap_[idx] = l;
    idx = max;
    max = LEFT(idx);
  }

  if (elem.first == 0) {
    this->literal_clauses_.erase(elem.second);
    this->literal_ids_.erase(elem.second);
    this->literal_heap_.pop_back();
  } else {
    this->literal_ids_[elem.second] = idx;
    this->literal_heap_[idx] = elem;
  }
}

Literal::name_type LiteralQueue::_head_literal() const noexcept {
  return this->literal_heap_[0].second;
}

template<class T>
std::size_t set_subtract(std::set<T> &left, const std::set<T> &right) {
  if (left.size() == 0 || right.size() == 0) {
    return 0;
  } else if (
      *left.begin() > *std::prev(right.end()) ||
      *std::prev(left.end()) < *right.begin()) {
    return 0;
  }

  auto lit = left.begin();
  auto lend = left.end();
  auto rit = right.begin();
  auto rend = right.end();

  std::size_t count = 0;
  while (lit != lend && rit != rend) {
    if (*lit < *rit) {
      ++lit;
    } else if (*lit > *rit) {
      ++rit;
    } else {
      auto next_lit = std::next(lit);
      left.erase(lit);
      ++count;
      lit = next_lit;
      ++rit;
    }
  }

  return count;
}

