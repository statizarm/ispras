//
// Created by art on 11/1/21.
//

#ifndef DPLL_INCLUDE_CLAUSE_H_
#define DPLL_INCLUDE_CLAUSE_H_

#include <string>
#include <set>
#include <unordered_set>
#include <functional>

class Literal {
 public:
  using name_type = int;

  Literal() = default;

  explicit Literal(name_type name, bool opposed = false): name_(name), opposed_(opposed) { }

  [[nodiscard]] bool is_opposed() const noexcept {
    return this->opposed_;
  }

  [[nodiscard]] Literal opposed() const noexcept {
    return Literal(this->name_, !this->opposed_);
  }

  [[nodiscard]] name_type name() const noexcept {
    return this->name_;
  }

  bool operator==(const Literal &other) const noexcept {
    return this->name_ == other.name_ && this->opposed_ == other.opposed_;
  }

 private:
  name_type name_;
  bool opposed_;
};

template<>
struct std::hash<Literal> {
  std::size_t operator()(Literal const &lit) const noexcept {
    std::size_t h1 = std::hash<Literal::name_type>{}(lit.name());
    std::size_t h2 = std::hash<bool>{}(lit.is_opposed());

    return h1 ^ (h2 << 1);
  }
};

class Clause {
 public:
  using container_type = std::unordered_set<Literal>;

  Clause() = default;

  template<class It>
  Clause(It begin, It end) : literals_(begin, end) { }

  Clause &delete_opposed(const Literal &lit) noexcept {
    this->literals_.erase(lit.opposed());
    return *this;
  }

  [[nodiscard]] bool contains(const Literal &lit) const noexcept {
    return this->literals_.find(lit) != this->literals_.end();
  }

  [[nodiscard]] container_type::size_type size() const noexcept {
    return this->literals_.size();
  }

  [[nodiscard]] const container_type &literals() const noexcept {
    return this->literals_;
  }

  [[nodiscard]] Literal get_literal() const noexcept {
    return *this->literals_.begin();
  }

 private:
  container_type literals_;
};

#endif //DPLL_INCLUDE_CLAUSE_H_
