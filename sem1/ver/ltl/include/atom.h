//
// Created by art on 12/10/21.
//

#ifndef LTL_INCLUDE_ATOM_H_
#define LTL_INCLUDE_ATOM_H_

#include <string>
#include <set>
#include <optional>

#include "ltl.h"

class Atom {
 public:
  using Formula = model::ltl::Formula;

  explicit Atom(std::string label) : label_(std::move(label)) { }
  Atom(std::string label, const Atom &other)
    : label_(std::move(label)), until_(other.until_), next_(other.next_), ap_(other.ap_), other_(other.other_) { }

  Atom(const Atom &other) = delete;

  [[nodiscard]] bool contains(const Formula *f) const noexcept;

  void add(const Formula *f) noexcept;

  [[nodiscard]] const std::string &label() const noexcept {
    return this->label_;
  }

  [[nodiscard]] std::optional<std::set<std::string>>
  transition(const Atom &other, const std::set<std::string> &ap) const noexcept;

 private:
  std::string label_;
  std::set<const Formula *> until_;
  std::set<const Formula *> next_;
  std::set<std::string> ap_;
  std::set<const Formula *> other_;
};

#endif //LTL_INCLUDE_ATOM_H_
