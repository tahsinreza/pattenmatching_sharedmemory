//
// Created by qiu on 21/06/18.
//

#include "algo_results.h"

AlgoResults::AlgoResults()
    : vertexEliminated(0), edgeEliminated(0), matchEliminated(0), enumeration(0) {}

void AlgoResults::clear() {
  vertexEliminated=0;
  edgeEliminated=0;
  matchEliminated=0;
  enumeration=0;
}


bool AlgoResults::isEmpty() const {
  return (vertexEliminated==0
   && edgeEliminated==0
   && matchEliminated==0
   && enumeration==0);
}

void AlgoResults::print(std::ostream &ostream) const {
  ostream << "Algo Stats :" <<std::endl;
  ostream <<"\tvertexEliminated : " << vertexEliminated<<std::endl;
  ostream <<"\tedgeEliminated : " << edgeEliminated<<std::endl;
  ostream <<"\tmatchEliminated : " << matchEliminated<<std::endl;
  ostream <<"\tenumeration : " << enumeration<<std::endl;
  ostream <<std::endl;
}
