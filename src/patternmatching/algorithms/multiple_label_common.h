//
// Created by qiu on 07/06/18.
//

#ifndef PROJECT_MULTIPLE_LABEL_COMMON_H
#define PROJECT_MULTIPLE_LABEL_COMMON_H

#include "totem_comdef.h"
namespace patternmatching {

typedef uint32_t pvid_t;

#define DEBUG_PRINT(X) std::cout << #X<< "\t" << X <<std::endl;
#define DEBUG_PRINT_SET(X) \
std::cout<< #X << " : ";\
for(const auto &it : X) { std::cout << it << " ";} std::cout << std::endl;

}

#endif //PROJECT_MULTIPLE_LABEL_COMMON_H
