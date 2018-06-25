//
// Created by qiu on 07/06/18.
//

#include "edge.h"

namespace patternmatching {

DirectedEdge::DirectedEdge()
    : vertexFromId(0), vertexToIt(0) {}
DirectedEdge::DirectedEdge(const vid_t vertexFromId_, const vid_t vertexToIt_)
    : vertexFromId(vertexFromId_), vertexToIt(vertexToIt_) {}
bool DirectedEdge::operator==(const DirectedEdge &other) const {
  return (vertexFromId == other.vertexFromId) && (vertexToIt == other.vertexToIt);
}
bool DirectedEdge::operator!=(const DirectedEdge &other) const {
  return (vertexFromId != other.vertexFromId) || (vertexToIt != other.vertexToIt);
}
void DirectedEdge::print(std::ostream &ostream) const {
  ostream << "(" << vertexFromId << "," << vertexToIt << ")" << std::endl;
}

UndirectedEdge::UndirectedEdge() : DirectedEdge() {
}
UndirectedEdge::UndirectedEdge(const vid_t vertexFromId_, const vid_t vertexToIt_)
: DirectedEdge(vertexFromId_, vertexToIt_)
{
  if (vertexFromId_ > vertexToIt_) {
    vertexFromId=vertexToIt_;
    vertexToIt=vertexFromId_;
  }
}

}