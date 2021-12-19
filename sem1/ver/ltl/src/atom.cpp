//
// Created by art on 12/10/21.
//

#include <atom.h>

std::optional<std::set<std::string>>
Atom::transition(const Atom &other, const std::set<std::string> &ap) const noexcept {
  for (auto f : this->until_) {
    if (f->kind() == Formula::NOT) {
      const Formula *lhs = std::addressof(f->arg().lhs());
      const Formula *rhs = std::addressof(f->arg().rhs());

      if (this->contains(rhs) || (this->contains(lhs) && !other.contains(f))) {
        return {};
      }
    } else {
      const Formula *lhs = std::addressof(f->lhs());
      const Formula *rhs = std::addressof(f->rhs());

      if (!this->contains(rhs) && (!this->contains(lhs) || !other.contains(f))) {
        return {};
      }
    }
  }

  for (auto f : this->next_) {
    if (f->kind() == Formula::NOT) {
      const Formula *arg = std::addressof(f->arg().arg());

      if (other.contains(arg)) {
        return {};
      }
    } else {
      const Formula *arg = std::addressof(f->arg());

      if (!other.contains(arg)) {
        return {};
      }
    }
  }

  return {this->ap_};
}

bool Atom::contains(const Atom::Formula *f) const noexcept {
  return this->other_.find(f) != this->other_.end() ||
      this->until_.find(f) != this->until_.end() ||
      this->next_.find(f) != this->next_.end() ||
      f->kind() == Formula::TRUE;
}

void Atom::add(const Atom::Formula *f) noexcept {
  auto kind = f->kind();

  if (kind == Formula::NOT) {
    kind = f->arg().kind();
  }

  switch (kind) {
    case Formula::U:
      this->until_.insert(f);
      break;
    case Formula::X:
      this->next_.insert(f);
      break;
    case Formula::ATOM:
      if (f->kind() == kind) {
        this->ap_.insert(f->prop());
      }
    default:
      this->other_.insert(f);
      break;
  }
}
