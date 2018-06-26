//
// Created by qiu on 21/06/18.
//

#include "graph_stat.h"

GraphStat::GraphStat()
    : vertexTotalNumber(0), edgeTotalNumber(0) {}

GraphStat &GraphStat::operator+=(const GraphStat &other) {
  edgeTotalNumber+=other.edgeTotalNumber;
  vertexTotalNumber+=other.vertexTotalNumber;
  for (const auto &it1 : other.vertexLabelTotalNumberMap) {
    vertexLabelTotalNumberMap[it1.first] += it1.second;
  }
  for (const auto &it1 : other.edgeLabelTotalNumberMap) {
    for (const auto &it2 : it1.second) {
      edgeLabelTotalNumberMap[it1.first][it2.first] += it2.second;
    }
  }
  for (const auto &it1 : other.edgeOutboundLabelTotalNumberMap) {
    edgeOutboundLabelTotalNumberMap[it1.first] += it1.second;
  }
  return *this;
}

GraphStat operator+(GraphStat left,
                    const GraphStat &right) {
  left += right;
  return left;
}

void GraphStat::print(std::ostream &ostream) const {
  ostream << "Global Stats :" <<std::endl;
  ostream <<"\tvertexTotalNumber : " << vertexTotalNumber<<std::endl;
  ostream <<"\tedgeTotalNumber : " << edgeTotalNumber<<std::endl;

  ostream << "vertexLabelTotalNumberMap :" <<std::endl;
  for (const auto &it : vertexLabelTotalNumberMap) {
    ostream << "\t" << it.first << " : " << it.second<< std::endl;
  }
  ostream << "vertexLabelAverageNumberMap :" <<std::endl;
  for (const auto &it : vertexLabelAverageNumberMap) {
    ostream << "\t" << it.first << " : " << it.second<< std::endl;
  }

  ostream << "edgeLabelTotalNumberMap :" <<std::endl;
  for (const auto &it1 : edgeLabelTotalNumberMap) {
    for (const auto &it2 : it1.second) {
      ostream << "\t(" << it1.first <<","<< it2.first <<") : " << it2.second<< std::endl;
    }
  }
  ostream << "edgeLabelAverageNumberMap :" <<std::endl;
  for (const auto &it1 : edgeLabelAverageNumberMap) {
    for (const auto &it2 : it1.second) {
      ostream << "\t(" << it1.first <<","<< it2.first <<") : " << it2.second<< std::endl;
    }
  }

  ostream << "edgeOutboundLabelTotalNumberMap :" <<std::endl;
  for (const auto &it : edgeOutboundLabelTotalNumberMap) {
    ostream << "\t" << it.first << " : " << it.second<< std::endl;
  }
  ostream << "edgeOutboundLabelAverageNumberMap :" <<std::endl;
  for (const auto &it : edgeOutboundLabelAverageNumberMap) {
    ostream << "\t" << it.first << " : " << it.second<< std::endl;
  }

  ostream <<std::endl;
}

void GraphStat::addVertex(const weight_t &vertexLabel) {
  vertexTotalNumber += 1;
  vertexLabelTotalNumberMap[vertexLabel] += 1;

}
void GraphStat::addEdge(const weight_t &vertexFromLabel, const weight_t &vertexToLabel) {
  edgeTotalNumber += 1;
  edgeLabelTotalNumberMap[vertexFromLabel][vertexToLabel] += 1;
  edgeOutboundLabelTotalNumberMap[vertexFromLabel] += 1;
}

void GraphStat::computeStats() {
  for (const auto &it : vertexLabelTotalNumberMap) {
    vertexLabelAverageNumberMap[it.first] = static_cast<double>(it.second) / vertexTotalNumber;
  }

  for (const auto &it1 : edgeLabelTotalNumberMap) {
    for (const auto &it2 : it1.second) {
      edgeLabelAverageNumberMap[it1.first][it2.first] = static_cast<double>(it2.second) / edgeTotalNumber;
    }
  }

  for (const auto &it : edgeOutboundLabelTotalNumberMap) {
    edgeOutboundLabelAverageNumberMap[it.first] = static_cast<double>(it.second) / vertexLabelTotalNumberMap[it.first];
  }
};