//
// Created by qiu on 07/06/18.
//

#include "multiple_label_local_constraint.h"

namespace patternmatching {

LocalConstraint::LocalConstraint()
    : originVertex(0), originLabel(0) {
  localConstraint.clear();
  localConstraintNeighborVertex.clear();
}

LocalConstraint::LocalConstraint(
    const pvid_t &originVertex_,
    const weight_t &originLabel_,
    const std::unordered_map <weight_t, size_t> &localConstraint_,
    const std::unordered_set <vid_t> &localConstraintNeighborVertex_)
    : originVertex(originVertex_), originLabel(originLabel_) {
  localConstraint = localConstraint_;
  localConstraintNeighborVertex = localConstraintNeighborVertex_;
}

void LocalConstraint::print(std::ostream &ostream) const {
  ostream << "Vertex " << originVertex << " : Label " << originLabel << std::endl;
  for (const auto &subit : localConstraint) {
    ostream << "\t Sublabel " << subit.first << " : " << subit.second << std::endl;
  }
  for (const auto &subit : localConstraintNeighborVertex) {
    ostream << "\t Neighbor " << subit << std::endl;
  }
}
}