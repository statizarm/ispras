//
// Created by art on 11/1/21.
//

#ifndef DPLL_INCLUDE_CLAUSE_H_
#define DPLL_INCLUDE_CLAUSE_H_

#include <string>
#include <set>

class Literal {
 public:
  explicit Literal(std::string name, bool opposed = false): name_(std::move(name)), opposed_(opposed) { }

  [[nodiscard]] bool is_opposed(const Literal &other) const noexcept {
    return this->opposed_ ^ other.opposed_;
  }

  [[nodiscard]] bool is_opposed() const noexcept {
    return this->opposed_;
  }

  [[nodiscard]] const std::string &name() const noexcept {
    return this->name_;
  }

  bool operator<(const Literal &other) const noexcept {
    return this->name_ < other.name_;
  }

 private:
  std::string name_;
  bool opposed_;
};

class Clause {
 public:
  using container_type = std::multiset<Literal>;
  template<class It>
  Clause(It begin, It end) : literals_(begin, end) { }

  Clause &spread_one(const Literal &cl) noexcept;

  [[nodiscard]] bool contains(const Literal &lit) const noexcept;

  [[nodiscard]] container_type::size_type size() const noexcept {
    return this->literals_.size();
  }

  [[nodiscard]] const container_type &literals() const noexcept {
    return this->literals_;
  }

 private:
  container_type literals_;
};

#endif //DPLL_INCLUDE_CLAUSE_H_
